/*
    Copyright (C) 2015 Tim Mayberry

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

#ifndef __libpbd_class_tracker_h__
#define __libpbd_class_tracker_h__

#include <map>
#include <vector>
#include <iostream>
#include <stdint.h> // TODO replace with cstdint with CPP11

#include <glibmm/threads.h>

#include "pbd/atomic_counter.h"
#include "pbd/stack_trace.h"
#include "pbd/debug.h"
#include "pbd/compose.h"

namespace PBD  {

class ClassTracker {
protected:
	virtual ~ClassTracker() {}

public: // interface

	/**
	 * @return name of class being tracked.
	 */
	virtual const char* const get_tracked_class_name() const = 0;

	/**
	 * @return true if tracking class creation/copy/destruction counts
	 */
	virtual bool get_counting_enabled() const = 0;

	/**
	 * @param enable Enable tracking class creation/copy/destruction counts
	 */
	virtual void set_counting_enabled(bool enable) = 0;

	/**
	 * @return true if all tracing is enabled
	 */
	virtual bool get_tracing_enabled() const = 0;

	/**
	 * @param enable Enable capturing backtraces when a class type is created,
	 * copied or destroyed.
	 */
	virtual void set_tracing_enabled(bool enable) = 0;

	/**
	 * Reset all the counters for a class so that any existing class instances
	 * will no longer be tracked so that only instances created after reset will
	 * be tracked.
	 */
	virtual void reset_tracking() = 0;

	/**
	 * Until there is a better callback based method of tracking lifetime data
	 * just force a tracker to print all current tracking data to stderr
	 */
	virtual void print_current_data() = 0;

	/**
	 * @return the current instance count for the class being tracked
	 */
	virtual uint32_t get_instance_count() const = 0;

	/**
	 * @return the amount of times all classes of the type being tracked have been
	 * copied.
	 */
	virtual uint32_t get_copied_count() const = 0;

	// TODO set tracing/counting enabled for a specific thread

	// TODO observer/callbacks for counts/traces

};

class ClassTrackerManager {
public:
	static void initialize();

	static void deinitialize();

	static bool tracking_enabled();

public: // methods
	/**
	 * A comma separated list of class names to enable tracking for. If the
	 * ClassTracker for a name in the list has not been registered yet the
	 * ClassTracker will be enabled when it is added.
	 */
	static void set_classes_enabled_from_string(const std::string& str);

	static void get_class_trackers(std::vector<ClassTracker*>& class_trackers);

	static ClassTracker* get_class_tracker(const std::string& class_name);

public: // Called only by ClassTracker derived classes

	static void add_class_tracker(ClassTracker* class_tracker);

	static void remove_class_tracker(ClassTracker* class_tracker);

};

template <class TrackedClassType>
class ClassTrackerMember {
public:
	ClassTrackerMember() {
		get_collector().class_created(this);
	}

	ClassTrackerMember(const ClassTrackerMember&) {
		get_collector().class_copied(this);
	}

	~ClassTrackerMember()
	{
		get_collector().class_destroyed(this);
	}

private:

	class ClassTrackerCollector : public ClassTracker {
	public: // ctors
		ClassTrackerCollector()
		    : m_counting_enabled(false)
		    , m_tracing_enabled(false)
		{
			ClassTrackerManager::add_class_tracker(this);
		}

		~ClassTrackerCollector()
		{
			if (get_counting_enabled()) {
				print_instance_count();
			}

			if (get_tracing_enabled()) {
				print_creation_traces();
			}
			reset_stack_traces();

			// ClassTrackerManager::remove_class_tracker(this);
		}

	public: // ClassTracker interface
		virtual const char* const get_tracked_class_name() const
		{
			return TrackedClassType::get_class_tracker_name();
		}

		virtual bool get_counting_enabled() const
		{ return m_counting_enabled; }

		virtual void set_counting_enabled(bool enable)
		{
			if (m_counting_enabled && enable) {
				return;
			}
			// set the counter to zero whether counting is being enabled or
			// disabled as the value has no meaning once the state of enabled
			// is changed.
			m_instance_count.set(0);

			m_counting_enabled = enable;
		}

		virtual bool get_tracing_enabled () const
		{ return m_tracing_enabled; }

		virtual void set_tracing_enabled (bool enable)
		{
			if (m_tracing_enabled && enable) {
				return;
			}

			reset_stack_traces ();

			m_tracing_enabled = enable;
		}

		virtual uint32_t get_instance_count () const {
			return m_instance_count.get();
		}

		virtual uint32_t get_copied_count () const {
			return m_copied_count.get();
		}

		virtual void reset_tracking()
		{
			m_instance_count.set(0);
			m_copied_count.set(0);
			reset_stack_traces ();
		}

		virtual void print_current_data()
		{
			print_instance_count();
			print_copied_count();
			print_creation_traces();
		}

	public: // methods
		void class_created (ClassTrackerMember* ptr)
		{
			class_instance_created(ptr);

			if (m_counting_enabled) {
				DEBUG_TRACE(DEBUG::ClassTracker,
				            string_compose(
				                "class created of type: %1, current instance count = %2\n",
				                get_tracked_class_name(),
				                m_instance_count.get()));
			}
		}

		void class_copied(ClassTrackerMember* ptr)
		{
			class_instance_created(ptr);

			if (m_counting_enabled) {
				++m_copied_count;
				DEBUG_TRACE(DEBUG::ClassTracker,
				            string_compose("class copied of type: %1, current instance "
				                           "count = %2, copied count = %3\n",
				                           get_tracked_class_name(),
				                           m_instance_count.get(),
				                           m_copied_count.get()));
			}
		}

		void class_destroyed(ClassTrackerMember* ptr)
		{
			if (get_counting_enabled()) {
				--m_instance_count;
				DEBUG_TRACE(DEBUG::ClassTracker,
				            string_compose(
				                "class destroyed of type: %1, current instance count = %2\n",
				                get_tracked_class_name(),
				                m_instance_count.get()));
			}

			if (get_tracing_enabled()) {
				remove_trace(ptr);
			}
		}

	private: // methods
		void class_instance_created(ClassTrackerMember* ptr)
		{
			if (get_counting_enabled()) {
				++m_instance_count;
			}

			if (get_tracing_enabled()) {
				add_trace(ptr);
			}
		}

		void add_trace (ClassTrackerMember* ptr)
		{
			Glib::Threads::Mutex::Lock guard(m_creation_trace_map_mutex);
			StackTrace* trace = new StackTrace;
			m_creation_trace_map.insert(std::make_pair(ptr, trace));
		}

		void remove_trace (ClassTrackerMember* ptr)
		{
			Glib::Threads::Mutex::Lock guard(m_creation_trace_map_mutex);
			typename TraceMap::iterator i = m_creation_trace_map.find(ptr);

			if (i == m_creation_trace_map.end()) {
				// error
				return;
			}
			delete i->second;
			m_creation_trace_map.erase(i);
		}

		void reset_stack_traces ()
		{
			 Glib::Threads::Mutex::Lock guard(m_creation_trace_map_mutex);
			 for (typename TraceMap::const_iterator i = m_creation_trace_map.begin();
			      i != m_creation_trace_map.end();
			      ++i) {
				 StackTrace* stack_trace = i->second;
				 delete stack_trace;
			 }
			 m_creation_trace_map.clear();
		}

		void print_instance_count()
		{
			if (m_instance_count.get() > 0) {
				std::cerr << "Current instance count for class: " << get_tracked_class_name()
				          << " : " << m_instance_count.get() << std::endl;
			}
		}

		void print_copied_count()
		{
			if (m_copied_count.get() > 0) {
				std::cerr << "Current copied count for class: " << get_tracked_class_name()
				          << " : " << m_copied_count.get() << std::endl;
			}
		}

		void print_creation_traces()
		{
			if (m_creation_trace_map.empty()) {
				return;
			}
			std::cerr << "Printing instance creation traces for class "
			          << get_tracked_class_name() << std::endl;

			std::map<StackTrace*, int> unique_trace_count_map;

			for (typename TraceMap::const_iterator i = m_creation_trace_map.begin();
			     i != m_creation_trace_map.end();
			     ++i) {
				StackTrace* stack_trace = i->second;
				bool unique = true;
				for (std::map<StackTrace*, int>::iterator j =
				         unique_trace_count_map.begin();
				     j != unique_trace_count_map.end();
				     ++j) {

					StackTrace* unique_trace = j->first;
					// StackTrace::operator==()
					if (*stack_trace == *unique_trace) {
						// increase the unique trace count
						j->second++;
						unique = false;
						break;
					}
				}
				if (unique) {
					unique_trace_count_map.insert(std::make_pair(stack_trace, 1));
				}
			}

			for (std::map<StackTrace*, int>::const_iterator i =
			         unique_trace_count_map.begin();
			     i != unique_trace_count_map.end();
			     ++i) {
				std::cerr << "class: " << get_tracked_class_name() << " leaked "
				          << i->second << " times, stack trace: " << std::endl;
				i->first->print(6);
			}
		}

	private: // data
		bool m_counting_enabled;
		atomic_counter m_instance_count;
		atomic_counter m_copied_count;

		typedef std::map<ClassTrackerMember*, StackTrace*> TraceMap;

		bool m_tracing_enabled;

		Glib::Threads::Mutex m_creation_trace_map_mutex;
		TraceMap m_creation_trace_map;
	};

	static ClassTrackerCollector& get_collector()
	{
		static ClassTrackerCollector collector;
		return collector;
	}
};

/**
 * This class would be better if rather than resetting the tracking totally for
 * a list of classes would just capture or request on destruction all the
 * allocations etc that occurred in the current/or all threads between the
 * point in time that the ScopedClassTracker was created and the time it was
 * destroyed/went out of scope.
 */
class ScopedClassTracker
{
public:
	ScopedClassTracker(const std::string& class_name)
		: m_class_name(class_name)
	{
		ClassTracker* class_tracker =
		    ClassTrackerManager::get_class_tracker(m_class_name);

		if (!class_tracker) {
			// enable tracking when the first instance is created
			ClassTrackerManager::set_classes_enabled_from_string(class_name);
		} else {
			class_tracker->set_counting_enabled(true);
			class_tracker->set_tracing_enabled(true);
			class_tracker->reset_tracking();
		}
	}

	~ScopedClassTracker()
	{
		ClassTracker* class_tracker =
		    ClassTrackerManager::get_class_tracker(m_class_name);

		if (class_tracker) {
			class_tracker->print_current_data();
			class_tracker->set_counting_enabled(false);
			class_tracker->set_tracing_enabled(false);
		}
	}

private:
	const std::string m_class_name;
};


} // namespace PBD

#if defined(PBD_ENABLE_CLASS_TRACKER)

#define PBD_CLASS_TRACKER(TrackedClassType)                                          \
	friend class PBD::ClassTrackerMember<TrackedClassType>;                             \
	static const char* const get_class_tracker_name() { return #TrackedClassType; }     \
	PBD::ClassTrackerMember<TrackedClassType> class_tracker##__LINE__;
#else
#define PBD_CLASS_TRACKER(TrackedClassType)
#endif // PBD_ENABLE_CLASS_TRACKER

#endif // __libpbd_class_tracker_h__
