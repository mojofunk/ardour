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

#include "pbd/timer.h"

#include "logging.h"

namespace {

class StandardTimer : public PBD::StandardTimer
{
public:
	StandardTimer (unsigned int interval)
		: PBD::StandardTimer(interval)
	{ }

	virtual bool on_elapsed () {
		A_LOG_CALL (LOG::GUITiming);
		return PBD::StandardTimer::on_elapsed ();
	}
};

class BlinkTimer : public PBD::BlinkTimer
{
public:
	BlinkTimer (unsigned int interval)
		: PBD::BlinkTimer(interval)
	{ }

	virtual bool on_elapsed () {
		A_LOG_CALL (LOG::GUITiming);
		return PBD::BlinkTimer::on_elapsed ();
	}
};

class UITimers
{

public:

	UITimers ()
		: blink(240)
		, second(1000)
		, rapid(100)
		, super_rapid(40)
		, fps(40)
		, _suspend_counter(0)
	{
	}

	BlinkTimer      blink;
	StandardTimer   second;
	StandardTimer   rapid;
	StandardTimer   super_rapid;
	StandardTimer   fps;

	gint            _suspend_counter;
};

UITimers&
get_timers ()
{
	static UITimers timers;
	return timers;
}

} // anon namespace

namespace Timers {

sigc::connection
blink_connect(const sigc::slot<void,bool>& slot)
{
	return get_timers().blink.connect (slot);
}

sigc::connection
second_connect(const sigc::slot<void>& slot)
{
	return get_timers().second.connect (slot);
}

sigc::connection
rapid_connect(const sigc::slot<void>& slot)
{
	return get_timers().rapid.connect (slot);
}

sigc::connection
super_rapid_connect(const sigc::slot<void>& slot)
{
#ifdef PLATFORM_WINDOWS
	return get_timers().fps.connect (slot);
#else
	return get_timers().super_rapid.connect (slot);
#endif
}

void
set_fps_interval (unsigned int interval)
{
	get_timers().fps.set_interval (interval);
}

sigc::connection
fps_connect(const sigc::slot<void>& slot)
{
	return get_timers().fps.connect (slot);
}

TimerSuspender::TimerSuspender ()
{
	if (g_atomic_int_add(&get_timers()._suspend_counter, 1) == 0) {
		get_timers().rapid.suspend();
		get_timers().super_rapid.suspend();
		get_timers().fps.suspend();
	}
}

TimerSuspender::~TimerSuspender ()
{
	if (g_atomic_int_dec_and_test (&get_timers()._suspend_counter)) {
		get_timers().rapid.resume();
		get_timers().super_rapid.resume();
		get_timers().fps.resume();
	}
}

} // namespace Timers
