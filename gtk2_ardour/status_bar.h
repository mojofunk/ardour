/*
    Copyright (C) 1999-2002 Paul Davis

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

#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include "pbd/signals.h"

#include "ardour/session_handle.h"
#include "ardour/types.h"

#include <gtkmm/eventbox.h>
#include <gtkmm/label.h>

#include "visibility_group.h"

class StatusBar : public Gtk::EventBox, public ARDOUR::SessionHandlePtr
{
public:

	StatusBar ();

	void set_session(ARDOUR::Session*);

	void post_engine ();

	gint update_wall_clock ();
	void update_disk_space ();
	void update_timecode_format ();
	void update_cpu_load ();
	void update_buffer_load ();
	void update_sample_rate (ARDOUR::framecnt_t);
	void update_format ();

	VisibilityGroup& visibility_group () { return _status_bar_visibility; }

private:

	void resize_text_widgets ();

	Gtk::Label   wall_clock_label;
	Gtk::Label   disk_space_label;
	Gtk::Label   timecode_format_label;
	Gtk::Label   cpu_load_label;
	Gtk::Label   buffer_load_label;
	Gtk::Label   sample_rate_label;
	Gtk::Label   format_label;

	VisibilityGroup _status_bar_visibility;

	PBD::ScopedConnectionList forever_connections;

	uint32_t rec_enabled_streams;
	void count_recenabled_streams (ARDOUR::Route&);

};

#endif
