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

#ifndef TIMERS_H
#define TIMERS_H

#include <sigc++/sigc++.h>

class Timers
{
public:

static sigc::connection blink_connect (sigc::slot<void, bool> s);

static sigc::connection second_connect (sigc::slot<void> s);

/** point_zero_one_seconds -- 10Hz ^= 100ms */
static sigc::connection rapid_connect (sigc::slot<void> s);

/** point_zero_something_seconds -- currently 25Hz ^= 40ms */
static sigc::connection super_rapid_connect (sigc::slot<void> s);

};

#endif
