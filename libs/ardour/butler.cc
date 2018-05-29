/*
    Copyright (C) 1999-2009 Paul Davis

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

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef PLATFORM_WINDOWS
#include <poll.h>
#endif

#include "pbd/error.h"
#include "pbd/pthread_utils.h"

#include "ardour/butler.h"
#include "ardour/debug.h"
#include "ardour/disk_io.h"
#include "ardour/disk_reader.h"
#include "ardour/io.h"
#include "ardour/logging.h"
#include "ardour/session.h"
#include "ardour/track.h"
#include "ardour/auditioner.h"

#include "pbd/i18n.h"

using namespace PBD;

namespace ARDOUR {

A_DEFINE_CLASS_MEMBERS(ARDOUR::Butler);

Butler::Butler(Session& s)
	: SessionHandleRef (s)
	, thread()
	, have_thread (false)
	, audio_dstream_capture_buffer_size(0)
	, audio_dstream_playback_buffer_size(0)
	, midi_dstream_buffer_size(0)
	, pool_trash(16)
	, _xthread (true)
{
	g_atomic_int_set(&should_do_transport_work, 0);
	SessionEvent::pool->set_trash (&pool_trash);

        /* catch future changes to parameters */
        Config->ParameterChanged.connect_same_thread (*this, boost::bind (&Butler::config_changed, this, _1));
}

Butler::~Butler()
{
	terminate_thread ();
}

void
Butler::map_parameters ()
{
        /* use any current ones that we care about */
        boost::function<void (std::string)> ff (boost::bind (&Butler::config_changed, this, _1));
        Config->map_parameters (ff);
}

void
Butler::config_changed (std::string p)
{
	if (p == "playback-buffer-seconds") {
		_session.adjust_playback_buffering ();
		if (Config->get_buffering_preset() == Custom) {
			/* size is in Samples, not bytes */
			audio_dstream_playback_buffer_size = (uint32_t) floor (Config->get_audio_playback_buffer_seconds() * _session.sample_rate());
			_session.adjust_playback_buffering ();
		} else {
#ifndef NDEBUG
			std::cerr << "Skip explicit buffer seconds, preset in use\n";
#endif
		}
	} else if (p == "capture-buffer-seconds") {
		if (Config->get_buffering_preset() == Custom) {
			audio_dstream_capture_buffer_size = (uint32_t) floor (Config->get_audio_capture_buffer_seconds() * _session.sample_rate());
			_session.adjust_capture_buffering ();
		} else {
#ifndef NDEBUG
			std::cerr << "Skip explicit buffer seconds, preset in use\n";
#endif
		}
	} else if (p == "buffering-preset") {
		DiskIOProcessor::set_buffering_parameters (Config->get_buffering_preset());
		audio_dstream_capture_buffer_size = (uint32_t) floor (Config->get_audio_capture_buffer_seconds() * _session.sample_rate());
		audio_dstream_playback_buffer_size = (uint32_t) floor (Config->get_audio_playback_buffer_seconds() * _session.sample_rate());
		_session.adjust_capture_buffering ();
		_session.adjust_playback_buffering ();
	} else if (p == "midi-readahead") {
		DiskReader::set_midi_readahead_samples ((samplecnt_t) (Config->get_midi_readahead() * _session.sample_rate()));
	}
}

int
Butler::start_thread()
{
	// set up capture and playback buffering
	DiskIOProcessor::set_buffering_parameters (Config->get_buffering_preset());

	/* size is in Samples, not bytes */
	const float rate = (float)_session.sample_rate();
	audio_dstream_capture_buffer_size = (uint32_t) floor (Config->get_audio_capture_buffer_seconds() * rate);
	audio_dstream_playback_buffer_size = (uint32_t) floor (Config->get_audio_playback_buffer_seconds() * rate);

	/* size is in bytes
	 * XXX: AudioEngine needs to tell us the MIDI buffer size
	 * (i.e. how many MIDI bytes we might see in a cycle)
	 */
	midi_dstream_buffer_size = (uint32_t) floor (Config->get_midi_track_buffer_seconds() * rate);

	DiskReader::set_midi_readahead_samples ((samplecnt_t) (Config->get_midi_readahead() * rate));

	should_run = false;

	if (pthread_create_and_store ("disk butler", &thread, _thread_work, this)) {
		error << _("Session: could not create butler thread") << endmsg;
		return -1;
	}

	//pthread_detach (thread);
	have_thread = true;

	// we are ready to request buffer adjustments
	_session.adjust_capture_buffering ();
	_session.adjust_playback_buffering ();

	return 0;
}

void
Butler::terminate_thread ()
{
	A_CLASS_CALL ();

	if (have_thread) {
		void* status;
		queue_request (Request::Quit);
		pthread_join (thread, &status);
	}
}

void *
Butler::_thread_work (void* arg)
{
	SessionEvent::create_per_thread_pool ("butler events", 4096);
	pthread_set_name (X_("butler"));
	return ((Butler *) arg)->thread_work ();
}

void *
Butler::thread_work ()
{
	uint32_t err = 0;

	bool disk_work_outstanding = false;
	RouteList::iterator i;

	while (true) {
		A_CLASS_MSG (A_FMT ("Butler main loop, disk work outstanding ? {}", disk_work_outstanding));

		if (!disk_work_outstanding) {
			A_CLASS_MSG ("Butler waits for requests");

			char msg;
			/* empty the pipe of all current requests */
			if (_xthread.receive (msg, true) >= 0) {
				Request::Type req = (Request::Type) msg;
				switch (req) {

					case Request::Run:
					  A_CLASS_MSG ("Butler asked to run");
					  should_run = true;
						break;

					case Request::Pause:
					  A_CLASS_MSG ("Butler asked to pause");
					  should_run = false;
						break;

					case Request::Quit:
					  A_CLASS_MSG ("Butler asked to quit");
					  return 0;
						abort(); /*NOTREACHED*/
						break;

					default:
						break;
				}
			}
		}


	  restart:
		A_CLASS_MSG ("At restart for disk work");
		disk_work_outstanding = false;

		if (transport_work_requested()) {
			A_CLASS_DURATION ("Do transport work");
			_session.butler_transport_work ();
		}

		sampleoffset_t audition_seek;
		if (should_run && _session.is_auditioning() && (audition_seek = _session.the_auditioner()->seek_sample()) >= 0) {
			boost::shared_ptr<Track> tr = boost::dynamic_pointer_cast<Track> (_session.the_auditioner());
			A_CLASS_DURATION ("Seek the auditioner");
			tr->seek(audition_seek);
			tr->do_refill ();
			_session.the_auditioner()->seek_response(audition_seek);
		}

		boost::shared_ptr<RouteList> rl = _session.get_routes();

		RouteList rl_with_auditioner = *rl;
		rl_with_auditioner.push_back (_session.the_auditioner());

		DEBUG_TRACE (DEBUG::Butler, string_compose ("butler starts refill loop, twr = %1\n", transport_work_requested()));

		for (i = rl_with_auditioner.begin(); !transport_work_requested() && should_run && i != rl_with_auditioner.end(); ++i) {

			boost::shared_ptr<Track> tr = boost::dynamic_pointer_cast<Track> (*i);

			if (!tr) {
				continue;
			}

			boost::shared_ptr<IO> io = tr->input ();

			if (io && !io->active()) {
				/* don't read inactive tracks */
				A_CLASS_MSG (A_FMT ("Butler skips inactive track {}", tr->name ()));
				continue;
			}

			A_CLASS_DURATION (A_FMT ("Butler refills {}, playback load = {}", tr->name (),
			                         tr->playback_buffer_load ()));

			switch (tr->do_refill ()) {
			case 0:
				A_CLASS_MSG (A_FMT ("Track refill done {}", tr->name ()));
				break;

			case 1:
				A_CLASS_MSG (A_FMT ("Track refill unfinished {}", tr->name ()));
				disk_work_outstanding = true;
				break;

			default:
				error << string_compose(_("Butler read ahead failure on dstream %1"), (*i)->name()) << endmsg;
                                std::cerr << string_compose(_("Butler read ahead failure on dstream %1"), (*i)->name()) << std::endl;
				break;
			}

		}

		if (i != rl_with_auditioner.begin() && i != rl_with_auditioner.end()) {
			/* we didn't get to all the streams */
			disk_work_outstanding = true;
		}

		if (!err && transport_work_requested()) {
			A_CLASS_MSG ("transport work requested during refill, back to restart");
			goto restart;
		}

		disk_work_outstanding = disk_work_outstanding || flush_tracks_to_disk_normal (rl, err);

		if (err && _session.actively_recording()) {
			/* stop the transport and try to catch as much possible
			   captured state as we can.
			*/
			A_CLASS_MSG ("error occurred during recording - stop transport");
			_session.request_stop ();
		}

		if (!err && transport_work_requested()) {
			A_CLASS_MSG ("transport work requested during flush, back to restart");
			goto restart;
		}

		if (!disk_work_outstanding) {
			_session.refresh_disk_space ();
		}

		{
			Glib::Threads::Mutex::Lock lm (request_lock);

			if (should_run && (disk_work_outstanding || transport_work_requested())) {
				A_CLASS_MSG (A_FMT ("at end, should run {} disk work {} transport "
				                    "work {} ... goto restart",
				                    should_run, disk_work_outstanding, transport_work_requested ()));
				goto restart;
			}

			A_CLASS_MSG ("Butler signals pause @ %2");

			paused.signal();
		}

		A_CLASS_MSG ("Butler emptying pool trash");
		empty_pool_trash ();
	}

	return (0);
}

bool
Butler::flush_tracks_to_disk_normal (boost::shared_ptr<RouteList> rl, uint32_t& errors)
{
	bool disk_work_outstanding = false;

	for (RouteList::iterator i = rl->begin(); !transport_work_requested() && should_run && i != rl->end(); ++i) {

		// cerr << "write behind for " << (*i)->name () << endl;

		boost::shared_ptr<Track> tr = boost::dynamic_pointer_cast<Track> (*i);

		if (!tr) {
			continue;
		}

		/* note that we still try to flush diskstreams attached to inactive routes
		 */

		int ret;

		A_CLASS_MSG (
		    A_FMT ("Butler flushes track {} capture load {}", tr->name (), tr->capture_buffer_load ()));

		ret = tr->do_flush (ButlerContext, false);
		switch (ret) {
		case 0:
			A_CLASS_MSG (A_FMT ("flush complete for {}", tr->name ()));
			break;

		case 1:
			A_CLASS_MSG (A_FMT ("flush not finished for {}", tr->name ()));
			disk_work_outstanding = true;
			break;

		default:
			errors++;
			error << string_compose(_("Butler write-behind failure on dstream %1"), (*i)->name()) << endmsg;
			std::cerr << string_compose(_("Butler write-behind failure on dstream %1"), (*i)->name()) << std::endl;
			/* don't break - try to flush all streams in case they
			   are split across disks.
			*/
		}
	}

	return disk_work_outstanding;
}

bool
Butler::flush_tracks_to_disk_after_locate (boost::shared_ptr<RouteList> rl, uint32_t& errors)
{
	A_CLASS_CALL ();

	bool disk_work_outstanding = false;

	/* almost the same as the "normal" version except that we do not test
	 * for transport_work_requested() and we force flushes.
	 */

	for (RouteList::iterator i = rl->begin(); i != rl->end(); ++i) {

		// cerr << "write behind for " << (*i)->name () << endl;

		boost::shared_ptr<Track> tr = boost::dynamic_pointer_cast<Track> (*i);

		if (!tr) {
			continue;
		}

		/* note that we still try to flush diskstreams attached to inactive routes
		 */

		int ret;

		A_CLASS_MSG (
		    A_FMT ("Butler flushes track {} capture load {}", tr->name (), tr->capture_buffer_load ()));

		ret = tr->do_flush (ButlerContext, true);
		switch (ret) {
		case 0:
			A_CLASS_MSG (A_FMT ("flush complete for {}", tr->name ()));
			break;

		case 1:
			A_CLASS_MSG (A_FMT ("flush not finished for {}", tr->name ()));
			disk_work_outstanding = true;
			break;

		default:
			errors++;
			error << string_compose(_("Butler write-behind failure on dstream %1"), (*i)->name()) << endmsg;
			std::cerr << string_compose(_("Butler write-behind failure on dstream %1"), (*i)->name()) << std::endl;
			/* don't break - try to flush all streams in case they
			   are split across disks.
			*/
		}
	}

	return disk_work_outstanding;
}

void
Butler::schedule_transport_work ()
{
	DEBUG_TRACE (DEBUG::Butler, "requesting more transport work\n");
	g_atomic_int_inc (&should_do_transport_work);
	summon ();
}

void
Butler::queue_request (Request::Type r)
{
	char c = r;
	if (_xthread.deliver (c) != 1) {
		/* the x-thread channel is non-blocking
		 * write may fail, but we really don't want to wait
		 * under normal circumstances.
		 *
		 * a lost "run" requests under normal RT operation
		 * is mostly harmless.
		 *
		 * TODO if ardour is freehweeling, wait & retry.
		 * ditto for Request::Type Quit
		 */
		assert(1); // we're screwd
	}
}

void
Butler::summon ()
{
	A_CLASS_CALL ();
	queue_request (Request::Run);
}

void
Butler::stop ()
{
	A_CLASS_CALL ();
	Glib::Threads::Mutex::Lock lm (request_lock);
	queue_request (Request::Pause);
	paused.wait(request_lock);
}

void
Butler::wait_until_finished ()
{
	A_CLASS_CALL ();
	Glib::Threads::Mutex::Lock lm (request_lock);
	queue_request (Request::Pause);
	paused.wait(request_lock);
}

bool
Butler::transport_work_requested () const
{
	return g_atomic_int_get(&should_do_transport_work);
}

void
Butler::empty_pool_trash ()
{
	/* look in the trash, deleting empty pools until we come to one that is not empty */

	RingBuffer<CrossThreadPool*>::rw_vector vec;
	pool_trash.get_read_vector (&vec);

	guint deleted = 0;

	for (int i = 0; i < 2; ++i) {
		for (guint j = 0; j < vec.len[i]; ++j) {
			if (vec.buf[i][j]->empty()) {
				delete vec.buf[i][j];
				++deleted;
			} else {
				/* found a non-empty pool, so stop deleting */
				if (deleted) {
					pool_trash.increment_read_idx (deleted);
				}
				return;
			}
		}
	}

	if (deleted) {
		pool_trash.increment_read_idx (deleted);
	}
}

void
Butler::drop_references ()
{
	std::cerr << "Butler drops pool trash\n";
	SessionEvent::pool->set_trash (0);
}


} // namespace ARDOUR

