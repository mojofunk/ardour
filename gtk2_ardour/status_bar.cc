/*
    Copyright (C) 20002-2004 Paul Davis

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

#include "status_bar.h"

#include <sstream>

#include "ardour/audioengine.h"
#include "ardour/profile.h"
#include "ardour/session.h"
#include "ardour/session_route.h"
#include "ardour/slave.h"
#include "ardour/track.h"

#include "gtkmm2ext/cairo_packer.h"
#include "gtkmm2ext/gui_thread.h"
#include "gtkmm2ext/utils.h"

#include "global_signals.h"

#include "i18n.h"

using namespace std;
using namespace ARDOUR;
using namespace ARDOUR_UI_UTILS;
using namespace Gtk;
using namespace Gtkmm2ext;

StatusBar::StatusBar ()
	: _status_bar_visibility (X_("status-bar"))
{
	show ();
	CairoHPacker* hbox = manage (new CairoHPacker);
	hbox->set_name (X_("StatusBarBox"));
	hbox->show ();
	hbox->set_border_width (3);

	VBox* vbox = manage (new VBox);
	vbox->pack_start (*hbox, true, false);
	vbox->show();

	add (*vbox);

	wall_clock_label.set_name ("WallClock");
	wall_clock_label.set_use_markup ();
	disk_space_label.set_name ("WallClock");
	disk_space_label.set_use_markup ();
	timecode_format_label.set_name ("WallClock");
	timecode_format_label.set_use_markup ();
	cpu_load_label.set_name ("CPULoad");
	cpu_load_label.set_use_markup ();
	buffer_load_label.set_name ("BufferLoad");
	buffer_load_label.set_use_markup ();
	sample_rate_label.set_name ("SampleRate");
	sample_rate_label.set_use_markup ();
	format_label.set_name ("Format");
	format_label.set_use_markup ();

	bool wall_clock = false;
	bool disk_space = false;

	if (!Profile->get_small_screen()) {
#ifndef GTKOSX
		// OSX provides its own wallclock, thank you very much
		wall_clock = true;
#endif
		disk_space = true;
	}

	hbox->pack_end (wall_clock_label, false, false, 2);
	hbox->pack_end (disk_space_label, false, false, 4);
	hbox->pack_end (cpu_load_label, false, false, 4);
	hbox->pack_end (buffer_load_label, false, false, 4);
	hbox->pack_end (sample_rate_label, false, false, 4);
	hbox->pack_end (timecode_format_label, false, false, 4);
	hbox->pack_end (format_label, false, false, 4);

	_status_bar_visibility.add (&wall_clock_label,      X_("WallClock"), _("Wall Clock"), wall_clock);
	_status_bar_visibility.add (&disk_space_label,      X_("Disk"),      _("Disk Space"), disk_space);
	_status_bar_visibility.add (&cpu_load_label,        X_("DSP"),       _("DSP"), true);
	_status_bar_visibility.add (&buffer_load_label,     X_("Buffers"),   _("Buffers"), true);
	_status_bar_visibility.add (&sample_rate_label,     X_("Audio"),     _("Audio"), true);
	_status_bar_visibility.add (&timecode_format_label, X_("TCFormat"),  _("Timecode Format"), true);
	_status_bar_visibility.add (&format_label,          X_("Format"),    _("File Format"), true);

	signal_button_press_event().connect (sigc::mem_fun (_status_bar_visibility, &VisibilityGroup::button_press_event));

	DPIReset.connect (sigc::mem_fun (*this, &StatusBar::resize_text_widgets));

}

void
StatusBar::post_engine ()
{
	AudioEngine::instance()->SampleRateChanged.connect (forever_connections, MISSING_INVALIDATOR, boost::bind (&StatusBar::update_sample_rate, this, _1), gui_context());

	/* start the time-of-day-clock */

#ifndef GTKOSX
	/* OS X provides a nearly-always visible wallclock, so don't be stupid */
	update_wall_clock ();
	Glib::signal_timeout().connect_seconds (sigc::mem_fun(*this, &StatusBar::update_wall_clock), 1);
#endif

}

void
StatusBar::set_session (Session* s)
{
	SessionHandlePtr::set_session (s);

	update_format ();
}

void
StatusBar::resize_text_widgets ()
{
	set_size_request_to_display_given_text (cpu_load_label, "DSP: 100.0%", 2, 2);
	set_size_request_to_display_given_text (buffer_load_label, "Buffers: p:100% c:100%", 2, 2);
}


void
StatusBar::update_sample_rate (framecnt_t)
{
	char buf[64];

	ENSURE_GUI_THREAD (*this, &StatusBar::update_sample_rate, ignored)

	if (!AudioEngine::instance()->connected()) {

		snprintf (buf, sizeof (buf), _("Audio: <span foreground=\"red\">none</span>"));

	} else {

		framecnt_t rate = AudioEngine::instance()->sample_rate();

		if (rate == 0) {
			/* no sample rate available */
			snprintf (buf, sizeof (buf), _("Audio: <span foreground=\"red\">none</span>"));
		} else {

			if (fmod (rate, 1000.0) != 0.0) {
				snprintf (buf, sizeof (buf), _("Audio: <span foreground=\"green\">%.1f kHz / %4.1f ms</span>"),
					  (float) rate / 1000.0f,
					  (AudioEngine::instance()->usecs_per_cycle() / 1000.0f));
			} else {
				snprintf (buf, sizeof (buf), _("Audio: <span foreground=\"green\">%" PRId64 " kHz / %4.1f ms</span>"),
					  rate/1000,
					  (AudioEngine::instance()->usecs_per_cycle() / 1000.0f));
			}
		}
	}
	sample_rate_label.set_markup (buf);
}

void
StatusBar::update_format ()
{
	if (!_session) {
		format_label.set_text ("");
		return;
	}

	stringstream s;
	s << _("File:") << X_(" <span foreground=\"green\">");

	switch (_session->config.get_native_file_header_format ()) {
	case BWF:
		s << _("BWF");
		break;
	case WAVE:
		s << _("WAV");
		break;
	case WAVE64:
		s << _("WAV64");
		break;
	case CAF:
		s << _("CAF");
		break;
	case AIFF:
		s << _("AIFF");
		break;
	case iXML:
		s << _("iXML");
		break;
	case RF64:
		s << _("RF64");
		break;
	}

	s << " ";

	switch (_session->config.get_native_file_data_format ()) {
	case FormatFloat:
		s << _("32-float");
		break;
	case FormatInt24:
		s << _("24-int");
		break;
	case FormatInt16:
		s << _("16-int");
		break;
	}

	s << X_("</span>");

	format_label.set_markup (s.str ());
}

void
StatusBar::update_cpu_load ()
{
	char buf[64];

	/* If this text is changed, the set_size_request_to_display_given_text call in StatusBar::resize_text_widgets
	   should also be changed.
	*/

	float const c = AudioEngine::instance()->get_dsp_load ();
	snprintf (buf, sizeof (buf), _("DSP: <span foreground=\"%s\">%5.1f%%</span>"), c >= 90 ? X_("red") : X_("green"), c);
	cpu_load_label.set_markup (buf);
}

void
StatusBar::update_buffer_load ()
{
	char buf[256];

	uint32_t const playback = _session ? _session->playback_load () : 100;
	uint32_t const capture = _session ? _session->capture_load () : 100;

	/* If this text is changed, the set_size_request_to_display_given_text call in StatusBar::resize_text_widgets
	   should also be changed.
	*/

	if (_session) {
		snprintf (
			buf, sizeof (buf),
			_("Buffers: <span foreground=\"green\">p:</span><span foreground=\"%s\">%" PRIu32 "%%</span> "
			           "<span foreground=\"green\">c:</span><span foreground=\"%s\">%" PRIu32 "%%</span>"),
			playback <= 5 ? X_("red") : X_("green"),
			playback,
			capture <= 5 ? X_("red") : X_("green"),
			capture
			);

		buffer_load_label.set_markup (buf);
	} else {
		buffer_load_label.set_text ("");
	}
}

void
StatusBar::count_recenabled_streams (Route& route)
{
	Track* track = dynamic_cast<Track*>(&route);
	if (track && track->record_enabled()) {
		rec_enabled_streams += track->n_inputs().n_total();
	}
}

void
StatusBar::update_disk_space()
{
	if (_session == 0) {
		return;
	}

	boost::optional<framecnt_t> opt_frames = _session->available_capture_duration();
	char buf[64];
	framecnt_t fr = _session->frame_rate();

	if (fr == 0) {
		/* skip update - no SR available */
		return;
	}

	if (!opt_frames) {
		/* Available space is unknown */
		snprintf (buf, sizeof (buf), "%s", _("Disk: <span foreground=\"green\">Unknown</span>"));
	} else if (opt_frames.get_value_or (0) == max_framecnt) {
		snprintf (buf, sizeof (buf), "%s", _("Disk: <span foreground=\"green\">24hrs+</span>"));
	} else {
		rec_enabled_streams = 0;
		_session->foreach_route (this, &StatusBar::count_recenabled_streams);

		framecnt_t frames = opt_frames.get_value_or (0);

		if (rec_enabled_streams) {
			frames /= rec_enabled_streams;
		}

		int hrs;
		int mins;
		int secs;

		hrs  = frames / (fr * 3600);

		if (hrs > 24) {
			snprintf (buf, sizeof (buf), "%s", _("Disk: <span foreground=\"green\">&gt;24 hrs</span>"));
		} else {
			frames -= hrs * fr * 3600;
			mins = frames / (fr * 60);
			frames -= mins * fr * 60;
			secs = frames / fr;

			bool const low = (hrs == 0 && mins <= 30);

			snprintf (
				buf, sizeof(buf),
				_("Disk: <span foreground=\"%s\">%02dh:%02dm:%02ds</span>"),
				low ? X_("red") : X_("green"),
				hrs, mins, secs
				);
		}
	}

	disk_space_label.set_markup (buf);
}

void
StatusBar::update_timecode_format ()
{
	char buf[64];

	if (_session) {
		bool matching;
		TimecodeSlave* tcslave;
		SyncSource sync_src = Config->get_sync_source();

		if ((sync_src == LTC || sync_src == MTC) && (tcslave = dynamic_cast<TimecodeSlave*>(_session->slave())) != 0) {
			matching = (tcslave->apparent_timecode_format() == _session->config.get_timecode_format());
		} else {
			matching = true;
		}

		snprintf (buf, sizeof (buf), S_("Timecode|TC: <span foreground=\"%s\">%s</span>"),
			  matching ? X_("green") : X_("red"),
			  Timecode::timecode_format_name (_session->config.get_timecode_format()).c_str());
	} else {
		snprintf (buf, sizeof (buf), "TC: n/a");
	}

	timecode_format_label.set_markup (buf);
}

gint
StatusBar::update_wall_clock ()
{
	time_t now;
	struct tm *tm_now;
	static int last_min = -1;

	time (&now);
	tm_now = localtime (&now);
	if (last_min != tm_now->tm_min) {
		char buf[16];
		sprintf (buf, "%02d:%02d", tm_now->tm_hour, tm_now->tm_min);
		wall_clock_label.set_text (buf);
		last_min = tm_now->tm_min;
	}

	return TRUE;
}

