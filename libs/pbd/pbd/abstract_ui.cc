/*
    Copyright (C) 2012 Paul Davis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <unistd.h>
#include <iostream>
#include <algorithm>

#include "pbd/stacktrace.h"
#include "pbd/abstract_ui.h"
#include "pbd/pthread_utils.h"
#include "pbd/failed_constructor.h"
#include "pbd/logging.h"

#include "pbd/i18n.h"

#ifdef COMPILER_MSVC
#include <ardourext/misc.h>  // Needed for 'DECLARE_DEFAULT_COMPARISONS'. Objects in an STL container can be
                             // searched and sorted. Thus, when instantiating the container, MSVC complains
                             // if the type of object being contained has no appropriate comparison operators
                             // defined (specifically, if operators '<' and '==' are undefined). This seems
                             // to be the case with ptw32 'pthread_t' which is a simple struct.
DECLARE_DEFAULT_COMPARISONS(ptw32_handle_t)
#endif

using namespace std;

template<typename RequestBuffer> void
cleanup_request_buffer (void* ptr)
{
	RequestBuffer* rb = (RequestBuffer*) ptr;

	/* this is called when the thread for which this request buffer was
	 * allocated dies. That could be before or after the end of the UI
	 * event loop for which this request buffer provides communication.
	 *
	 * We are not modifying the UI's thread/buffer map, just marking it
	 * dead. If the UI is currently processing the buffers and misses
	 * this "dead" signal, it will find it the next time it receives
	 * a request. If the UI has finished processing requests, then
	 * we will leak this buffer object.
	 */
	A_LOG_DATA1 (PBD::LOG::AbstractUI, rb);

	rb->dead = true;
}

template<typename R>
Glib::Threads::Private<typename AbstractUI<R>::RequestBuffer> AbstractUI<R>::per_thread_request_buffer (cleanup_request_buffer<AbstractUI<R>::RequestBuffer>);

template <typename RequestObject>
AbstractUI<RequestObject>::AbstractUI (const string& name)
	: BaseUI (name)
{
	A_LOG_CLASS_CALL1 (PBD::LOG::AbstractUI, name);

	void (AbstractUI<RequestObject>::*pmf)(pthread_t,string,uint32_t) = &AbstractUI<RequestObject>::register_thread;

	/* better to make this connect a handler that runs in the UI event loop but the syntax seems hard, and
	   register_thread() is thread safe anyway.
	*/

	PBD::ThreadCreatedWithRequestSize.connect_same_thread (new_thread_connection, boost::bind (pmf, this, _1, _2, _3));

	/* find pre-registerer threads */

	vector<EventLoop::ThreadBufferMapping> tbm = EventLoop::get_request_buffers_for_target_thread (event_loop_name());

	{
		Glib::Threads::Mutex::Lock rbml (request_buffer_map_lock);
		for (vector<EventLoop::ThreadBufferMapping>::iterator t = tbm.begin(); t != tbm.end(); ++t) {
			request_buffers[t->emitting_thread] = static_cast<RequestBuffer*> (t->request_buffer);
		}
	}
}

template <typename RequestObject>
AbstractUI<RequestObject>::~AbstractUI ()
{
	for (RequestBufferMapIterator i = request_buffers.begin(); i != request_buffers.end(); ++i) {
		if ((*i).second->dead) {
			EventLoop::remove_request_buffer_from_map ((*i).second);
			delete (*i).second;
		}
	}
}

template <typename RequestObject> void
AbstractUI<RequestObject>::register_thread (pthread_t thread_id, string thread_name, uint32_t num_requests)
{
	/* the calling thread wants to register with the thread that runs this
	 * UI's event loop, so that it will have its own per-thread queue of
	 * requests. this means that when it makes a request to this UI it can
	 * do so in a realtime-safe manner (no locks).
	 */

	A_LOG_CLASS_CALL2 (PBD::LOG::AbstractUI, thread_name, event_loop_name ());

	/* the per_thread_request_buffer is a thread-private variable.
	   See pthreads documentation for more on these, but the key
	   thing is that it is a variable that as unique value for
	   each thread, guaranteed. Note that the thread in question
	   is the caller of this function, which is assumed to be the
	   thread from which signals will be emitted that this UI's
	   event loop will catch.
	*/

	RequestBuffer* b = per_thread_request_buffer.get();

	if (!b) {

		/* create a new request queue/ringbuffer */

		A_LOG_CLASS_MSG (PBD::LOG::AbstractUI, A_FMT ("Create new request buffer for {} in {}",
		                                              thread_name, event_loop_name ()));

		b = new RequestBuffer (num_requests); // XXX leaks
		/* set this thread's per_thread_request_buffer to this new
		   queue/ringbuffer. remember that only this thread will
		   get this queue when it calls per_thread_request_buffer.get()

		   the second argument is a function that will be called
		   when the thread exits, and ensures that the buffer is marked
		   dead. it will then be deleted during a call to handle_ui_requests()
		*/

		per_thread_request_buffer.set (b);
	} else {
		A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
		                 A_FMT ("Thread {} is is already registered with Event Loop: {}", thread_name,
		                        event_loop_name ()));
	}

	{
		/* add the new request queue (ringbuffer) to our map
		   so that we can iterate over it when the time is right.
		   This step is not RT-safe, but is assumed to be called
		   only at thread initialization time, not repeatedly,
		   and so this is of little consequence.
		*/
		Glib::Threads::Mutex::Lock rbml (request_buffer_map_lock);
		request_buffers[thread_id] = b;
	}
}

template <typename RequestObject> RequestObject*
AbstractUI<RequestObject>::get_request (RequestType rt)
{
	A_LOG_CLASS_CALL1 (PBD::LOG::AbstractUI, rt);

	RequestBuffer* rbuf = per_thread_request_buffer.get ();
	RequestBufferVector vec;

	/* see comments in ::register_thread() above for an explanation of
	   the per_thread_request_buffer variable
	*/

	if (rbuf != 0) {

		/* the calling thread has registered with this UI and therefore
		 * we have a per-thread request queue/ringbuffer. use it. this
		 * "allocation" of a request is RT-safe.
		 */

		rbuf->get_write_vector (&vec);

		if (vec.len[0] == 0) {
			A_LOG_CLASS_MSG (
			    PBD::LOG::AbstractUI,
			    A_FMT ("{}: no space in per thread pool for request of type {}", event_loop_name (), rt));
			return 0;
		}

		A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
		                 A_FMT ("{}: allocated per-thread request of type {}", event_loop_name (), rt));

		vec.buf[0]->type = rt;
		return vec.buf[0];
	}

	/* calling thread has not registered, so just allocate a new request on
	 * the heap. the lack of registration implies that realtime constraints
	 * are not at work.
	 */

	A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
	                 A_FMT ("{}: allocated normal heap request of type {}", event_loop_name (), rt));

	RequestObject* req = new RequestObject;
	req->type = rt;

	return req;
}

template <typename RequestObject> void
AbstractUI<RequestObject>::handle_ui_requests ()
{
	A_LOG_CLASS_CALL (PBD::LOG::AbstractUI);

	RequestBufferMapIterator i;
	RequestBufferVector vec;

	/* check all registered per-thread buffers first */
	Glib::Threads::Mutex::Lock rbml (request_buffer_map_lock);

	/* clean up any dead invalidation records (object was deleted) */
	trash.sort();
	trash.unique();
	for (std::list<InvalidationRecord*>::iterator r = trash.begin(); r != trash.end();) {
		if (!(*r)->in_use ()) {
			assert (!(*r)->valid ());
			A_LOG_CLASS_MSG (PBD::LOG::AbstractUI, A_FMT ("{}: drop invalidation trash {}",
			                                                    event_loop_name (), adt::void_cast (*r)));
			std::list<InvalidationRecord*>::iterator tmp = r;
			++tmp;
			delete *r;
			trash.erase (r);
			r = tmp;
		} else {
			++r;
		}
	}
#ifndef NDEBUG
	if (trash.size() > 0) {
		A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
		                 A_FMT ("{}: Items in trash: {}", event_loop_name (), trash.size ()));
	}
#endif

	A_LOG_CLASS_MSG (PBD::LOG::AbstractUI, A_FMT ("{}: check {} request buffers for requests",
	                                                    event_loop_name (), request_buffers.size ()));

	for (i = request_buffers.begin(); i != request_buffers.end(); ++i) {

		while (!(*i).second->dead) {

			/* we must process requests 1 by 1 because
			 * the request may run a recursive main
			 * event loop that will itself call
			 * handle_ui_requests. when we return
			 * from the request handler, we cannot
			 * expect that the state of queued requests
			 * is even remotely consistent with
			 * the condition before we called it.
			 */

			i->second->get_read_vector (&vec);

			A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
			                 A_FMT ("{}: reading requests from RB[{}] @ {}, requests = {} + {}",
			                        event_loop_name (), std::distance (request_buffers.begin (), i),
			                        adt::void_cast (i->second), vec.len[1], vec.len[0]));

			if (vec.len[0] == 0) {
				break;
			} else {
				if (vec.buf[0]->invalidation && !vec.buf[0]->invalidation->valid ()) {
					A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
					                 A_FMT ("{}: skipping invalidated request", event_loop_name ()));
					rbml.release ();
				} else {

					A_LOG_CLASS_MSG (
					    PBD::LOG::AbstractUI,
					    A_FMT ("{}: valid request, unlocking before calling", event_loop_name ()));
					rbml.release ();

					A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
					                 A_FMT ("{}: valid request, calling ::do_request()", event_loop_name ()));
					do_request (vec.buf[0]);
				}

				/* if the request was CallSlot, then we need to ensure that we reset the functor in the request, in case it
				 * held a shared_ptr<>. Failure to do so can lead to dangling references to objects passed to PBD::Signals.
				 *
				 * Note that this method (::handle_ui_requests()) is by definition called from the event loop thread, so
				 * caller_is_self() is true, which means that the execution of the functor has definitely happened after
				 * do_request() returns and we no longer need the functor for any reason.
				 */

				if (vec.buf[0]->type == CallSlot) {
					vec.buf[0]->the_slot = 0;
				}

				rbml.acquire ();
				if (vec.buf[0]->invalidation) {
					vec.buf[0]->invalidation->unref ();
				}
				vec.buf[0]->invalidation = NULL;
				i->second->increment_read_ptr (1);
			}
		}
	}

	assert (rbml.locked ());
	for (i = request_buffers.begin(); i != request_buffers.end(); ) {
		if ((*i).second->dead) {
			A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
			                 A_FMT ("{}: deleting dead per-thread request buffer @ {} ({} requests)",
			                        event_loop_name (), adt::void_cast (i->second),
			                        (*i).second->read_space ()));
			RequestBufferMapIterator tmp = i;
			++tmp;
			/* remove it from the EventLoop static map of all request buffers */
			EventLoop::remove_request_buffer_from_map ((*i).second);
			/* delete it
			 *
			 * Deleting the ringbuffer destroys all RequestObjects
			 * and thereby drops any InvalidationRecord references of
			 * requests that have not been processed.
			 */
			delete (*i).second;
			/* remove it from this thread's list of request buffers */
			request_buffers.erase (i);
			i = tmp;
		} else {
			++i;
		}
	}

	/* and now, the generic request buffer. same rules as above apply */

	while (!request_list.empty()) {
		assert (rbml.locked ());
		RequestObject* req = request_list.front ();
		request_list.pop_front ();

		/* we're about to execute this request, so its
		 * too late for any invalidation. mark
		 * the request as "done" before we start.
		 */

		if (req->invalidation && !req->invalidation->valid()) {
			A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
			                 A_FMT ("{}: Handling invalid heap request, type {}, deleting",
			                        event_loop_name (), req->type));
			delete req;
			continue;
		}

		/* at this point, an object involved in a functor could be
		 * deleted before we actually execute the functor. so there is
		 * a race condition that makes the invalidation architecture
		 * somewhat pointless.
		 *
		 * really, we should only allow functors containing shared_ptr
		 * references to objects to enter into the request queue.
		 */

		/* unlock the request lock while we execute the request, so
		 * that we don't needlessly block other threads (note: not RT
		 * threads since they have their own queue) from making requests.
		 */

		/* also the request may destroy the object itself resulting in a direct
		 * path to EventLoop::invalidate_request () from here
		 * which takes the lock */

		rbml.release ();

		A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
		                 A_FMT ("{}: Execute request type {}", event_loop_name (), req->type));

		/* and lets do it ... this is a virtual call so that each
		 * specific type of UI can have its own set of requests without
		 * some kind of central request type registration logic
		 */

		do_request (req);

		A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
		                 A_FMT ("{}: Delete heap request type {}", event_loop_name (), req->type));

		delete req;

		/* re-acquire the list lock so that we check again */

		rbml.acquire();
	}

	rbml.release ();
}

template <typename RequestObject> void
AbstractUI<RequestObject>::send_request (RequestObject *req)
{
	A_LOG_CLASS_CALL1 (PBD::LOG::AbstractUI, req);

	/* This is called to ask a given UI to carry out a request. It may be
	 * called from the same thread that runs the UI's event loop (see the
	 * caller_is_self() case below), or from any other thread.
	 */

	if (base_instance() == 0) {
		delete req;
		return; /* XXX is this the right thing to do ? */
	}

	if (caller_is_self ()) {
		/* the thread that runs this UI's event loop is sending itself
		   a request: we dispatch it immediately and inline.
		*/
		A_LOG_CLASS_MSG (PBD::LOG::AbstractUI, A_FMT ("{}: Direct dispatch of request type {}",
		                                              event_loop_name (), req->type));

		do_request (req);
		delete req;
	} else {

		/* If called from a different thread, we first check to see if
		 * the calling thread is registered with this UI. If so, there
		 * is a per-thread ringbuffer of requests that ::get_request()
		 * just set up a new request in. If so, all we need do here is
		 * to advance the write ptr in that ringbuffer so that the next
		 * request by this calling thread will use the next slot in
		 * the ringbuffer. The ringbuffer has
		 * single-reader/single-writer semantics because the calling
		 * thread is the only writer, and the UI event loop is the only
		 * reader.
		 */

		RequestBuffer* rbuf = per_thread_request_buffer.get ();

		if (rbuf != 0) {
			A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
			                 A_FMT ("{}: Send per-thread request type {} using ringbuffer @ {}, IR: {}",
			                        event_loop_name (), req->type, (void*)rbuf,
			                        (void*)req->invalidation));
			rbuf->increment_write_ptr (1);
		} else {
			/* no per-thread buffer, so just use a list with a lock so that it remains
			 * single-reader/single-writer semantics
			 */
			A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
			                 A_FMT ("{}: Send per-thread request type {}, IR: {}", event_loop_name (),
			                        req->type, (void*)req->invalidation));
			Glib::Threads::Mutex::Lock lm (request_buffer_map_lock);
			request_list.push_back (req);
		}

		/* send the UI event loop thread a wakeup so that it will look
		   at the per-thread and generic request lists.
		*/

		signal_new_request ();
	}
}

template<typename RequestObject> void
AbstractUI<RequestObject>::call_slot (InvalidationRecord* invalidation, const boost::function<void()>& f)
{
	A_LOG_CLASS_CALL (PBD::LOG::AbstractUI);

	if (caller_is_self()) {
		A_LOG_CLASS_DURATION (
		    PBD::LOG::AbstractUI,
		    A_FMT ("{}: Direct dispatch of call slot via functor @ {}, invalidation record {}",
		           event_loop_name (), adt::void_cast (&f), adt::void_cast (invalidation)));
		f ();
		return;
	}

	/* object destruction may race with realtime signal emission.
	 *
	 * There may be a concurrent event-loop in progress of deleting
	 * the slot-object. That's perfectly fine. But we need to mark
	 * the invalidation record itself as being used by the request.
	 *
	 * The IR needs to be kept around until the last signal using
	 * it is disconnected and then it can be deleted in the event-loop
	 * (GUI thread).
	 */
	if (invalidation) {
		if (!invalidation->valid()) {
			A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
			                 A_FMT ("{}: Ignoring call-slot using functor: {}, invalidation: {}",
			                        event_loop_name (), adt::void_cast (&f),
			                        adt::void_cast (invalidation)));
			return;
		}
		invalidation->ref ();
		invalidation->event_loop = this;
	}

	RequestObject *req = get_request (BaseUI::CallSlot);

	if (req == 0) {
		if (invalidation) {
			invalidation->unref ();
		}
		return;
	}

	A_LOG_CLASS_MSG (PBD::LOG::AbstractUI,
	                 A_FMT ("{}: Queue call-slot using functor @ {}, invalidation record {}",
	                        event_loop_name (), adt::void_cast (&f), adt::void_cast (invalidation)));

	/* copy semantics: copy the functor into the request object */

	req->the_slot = f;

	/* the invalidation record is an object which will carry out
	 * invalidation of any requests associated with it when it is
	 * destroyed. it can be null. if its not null, associate this
	 * request with the invalidation record. this allows us to
	 * "cancel" requests submitted to the UI because they involved
	 * a functor that uses an object that is being deleted.
	 */

	req->invalidation = invalidation;

	send_request (req);
}

template<typename RequestObject> void*
AbstractUI<RequestObject>::request_buffer_factory (uint32_t num_requests)
{
	RequestBuffer*  mcr = new RequestBuffer (num_requests); // XXX leaks
	per_thread_request_buffer.set (mcr);
	return mcr;
}
