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

#include "pbd/class_tracker.h"

#include <set>

#include <boost/tokenizer.hpp>

struct ClassTrackerManagerData {
	Glib::Threads::Mutex m_class_trackers_mutex;
	std::set<PBD::ClassTracker*> m_class_trackers;
	std::set<std::string> m_classes_to_enable;
	bool m_enable_all_classes;

	ClassTrackerManagerData()
	    : m_enable_all_classes(false)
	{
	}
};

static ClassTrackerManagerData* s_manager_data = 0;

namespace PBD {

void
ClassTrackerManager::initialize()
{
	if (s_manager_data) {
		return;
	}
	s_manager_data = new ClassTrackerManagerData;
}

void
ClassTrackerManager::deinitialize()
{
	delete s_manager_data;
	s_manager_data = 0;
}

bool
ClassTrackerManager::tracking_enabled()
{
#if defined(PBD_ENABLE_CLASS_TRACKER)
	return true;
#else
	return false;
#endif
}

ClassTracker*
ClassTrackerManager::get_class_tracker(const std::string& name)
{
	if (!s_manager_data) {
		return 0;
	}

	Glib::Threads::Mutex::Lock guard(s_manager_data->m_class_trackers_mutex);

	for (std::set<PBD::ClassTracker*>::const_iterator i =
	         s_manager_data->m_class_trackers.begin();
	     i != s_manager_data->m_class_trackers.end();
	     ++i) {
		if ((*i)->get_tracked_class_name() == std::string(name)) {
			return *i;
		}
	}
	return 0; // nullptr;
}

void
ClassTrackerManager::get_class_trackers(std::vector<ClassTracker*>& class_trackers)
{
	if (!s_manager_data) {
		return;
	}

	Glib::Threads::Mutex::Lock guard(s_manager_data->m_class_trackers_mutex);

	std::copy(s_manager_data->m_class_trackers.begin(),
	          s_manager_data->m_class_trackers.end(),
	          std::back_inserter(class_trackers));
}

void
ClassTrackerManager::set_classes_enabled_from_string(const std::string& class_list)
{
	if (!s_manager_data) {
		return;
	}

	DEBUG_TRACE(DEBUG::ClassTracker,
	            string_compose(
	                "Setting ClassTrackerManager class list : %1\n", class_list));

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep (",");
	tokenizer tokens (class_list, sep);

	Glib::Threads::Mutex::Lock guard(s_manager_data->m_class_trackers_mutex);

	for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end();
	     ++tok_iter) {
		if (*tok_iter == "all") {
			s_manager_data->m_enable_all_classes = true;
			return;
		}

		bool enabled = false;
		for (std::set<PBD::ClassTracker*>::iterator j =
		         s_manager_data->m_class_trackers.begin();
		     j != s_manager_data->m_class_trackers.end();
		     ++j) {
			ClassTracker* class_tracker = *j;
			if (class_tracker->get_tracked_class_name() == *tok_iter) {
				DEBUG_TRACE(
				    DEBUG::ClassTracker,
				    string_compose("Enabling class tracking for class : %1\n", *tok_iter));
				class_tracker->set_counting_enabled(true);
				class_tracker->set_tracing_enabled(true);
				enabled = true;
			}
		}
		if (!enabled) {
			s_manager_data->m_classes_to_enable.insert(*tok_iter);
		}
	}
}

void
ClassTrackerManager::add_class_tracker(ClassTracker* class_tracker)
{
	if (!s_manager_data) {
		return;
	}

	DEBUG_TRACE(DEBUG::ClassTracker,
	            string_compose("Adding ClassTracker for class : %1\n",
	                           class_tracker->get_tracked_class_name()));

	Glib::Threads::Mutex::Lock guard(s_manager_data->m_class_trackers_mutex);

	s_manager_data->m_class_trackers.insert(class_tracker);

	std::set<std::string>::const_iterator i;
	i = s_manager_data->m_classes_to_enable.find(class_tracker->get_tracked_class_name());

	if (i != s_manager_data->m_classes_to_enable.end()) {
		DEBUG_TRACE(DEBUG::ClassTracker,
		            string_compose("Enabling ClassTracker for class : %1\n",
		                           class_tracker->get_tracked_class_name()));
		class_tracker->set_counting_enabled(true);
		class_tracker->set_tracing_enabled(true);
		s_manager_data->m_classes_to_enable.erase(i);
		return;
	}

	if (s_manager_data->m_enable_all_classes) {
		class_tracker->set_counting_enabled(true);
		class_tracker->set_tracing_enabled(true);
	}
}

void
ClassTrackerManager::remove_class_tracker(ClassTracker* class_tracker)
{
	if (!s_manager_data) {
		return;
	}

	Glib::Threads::Mutex::Lock guard(s_manager_data->m_class_trackers_mutex);

	if (s_manager_data->m_class_trackers.erase(class_tracker) != 1) {
		DEBUG_TRACE(DEBUG::ClassTracker,
		            string_compose("Error, ClassTracker : %1 not found in Manager\n",
		            class_tracker->get_tracked_class_name()));
		return;
	}
}

} // namespace PBD
