/*
    Copyright (C) 2014 Tim Mayberry

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

#include "timers.h"

#include <glibmm/main.h>

static sigc::signal<void,bool> Blink;

static sigc::signal<void>      Second;

static sigc::signal<void>      Rapid;

static sigc::signal<void>      SuperRapid;


static bool
blink_handler ()
{
	static bool blink_on = false;

	if (Blink.empty()) {
		return false;
	}

	Blink (blink_on = !blink_on); /* EMIT_SIGNAL */
	return true;
}

static bool
second_handler ()
{
	if (Second.empty()) {
		return false;
	}

	Second (); /* EMIT_SIGNAL */
	return true;
}

static bool
rapid_handler ()
{
	if (Rapid.empty()) {
		return false;
	}

	Rapid (); /* EMIT_SIGNAL */
	return true;
}

static bool
super_rapid_handler ()
{
	if (SuperRapid.empty()) {
		return false;
	}

	SuperRapid (); /* EMIT_SIGNAL */
	return true;
}

sigc::connection
Timers::blink_connect (sigc::slot<void, bool> s)
{
	if (Blink.empty()) {
		Glib::signal_timeout().connect (&blink_handler, 240);
	}

	return Blink.connect (s);
}

sigc::connection
Timers::second_connect (sigc::slot<void> s)
{
	if (Second.empty()) {
		Glib::signal_timeout().connect_seconds (&second_handler, 1);
	}

	return Second.connect (s);
}

sigc::connection
Timers::rapid_connect (sigc::slot<void> s)
{
	if (Rapid.empty()) {
		Glib::signal_timeout().connect (&rapid_handler, 100);
	}

	return Rapid.connect (s);
}

sigc::connection
Timers::super_rapid_connect (sigc::slot<void> s)
{
	if (SuperRapid.empty()) {
		Glib::signal_timeout().connect (&super_rapid_handler, 40);
	}

	return SuperRapid.connect (s);
}
