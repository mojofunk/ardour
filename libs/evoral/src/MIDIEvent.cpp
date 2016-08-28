/* This file is part of Evoral.
 * Copyright (C) 2008 David Robillard <http://drobilla.net>
 * Copyright (C) 2000-2008 Paul Davis
 *
 * Evoral is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * Evoral is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string>
#include "evoral/Beats.hpp"
#include "evoral/MIDIEvent.hpp"
#ifdef EVORAL_MIDI_XML
	#include "pbd/xml++.h"
#endif

using namespace std;

namespace Evoral {

#ifdef EVORAL_MIDI_XML

template<typename Time>
MIDIEvent<Time>::MIDIEvent(const XMLNode& event)
  : Event<Time>()
{
	string name = event.name();

	if (name == "ControlChange") {
		this->_buf = (uint8_t*) ::malloc(3);
		this->_owns_buf = true;
		set_type(MIDI_CMD_CONTROL);

		uint8_t cc_num;
		uint8_t cc_val;
		event.get_property("Control", cc_num);
		event.get_property("Value", cc_val);
		set_cc_number(cc_num);
		set_cc_value (cc_val);
	} else if (name == "ProgramChange") {
		this->_buf = (uint8_t*) ::malloc(2);
		this->_owns_buf = true;
		set_type(MIDI_CMD_PGM_CHANGE);

		set_pgm_number(atoi(event.property("Number")->value().c_str()));
	}
}


template<typename Time>
boost::shared_ptr<XMLNode>
MIDIEvent<Time>::to_xml() const
{
	XMLNode *result = 0;

	switch (type()) {
	case MIDI_CMD_CONTROL:
		result = new XMLNode("ControlChange");
		result->set_property("Channel", channel());
		result->set_property("Control", cc_number());
		result->set_property("Value", cc_value());
		break;

	case MIDI_CMD_PGM_CHANGE:
		result = new XMLNode("ProgramChange");
		result->set_property("Channel", channel());
		result->set_property("Number", pgm_number());
		break;

	case MIDI_CMD_NOTE_ON:
		result = new XMLNode("NoteOn");
		result->set_property("Channel", channel());
		result->set_property("Note", note());
		result->set_property("Velocity", velocity());
		break;

	case MIDI_CMD_NOTE_OFF:
		result = new XMLNode("NoteOff");
		result->set_property("Channel", channel());
		result->set_property("Note", note());
		result->set_property("Velocity", velocity());
		break;

	case MIDI_CMD_BENDER:
		result = new XMLNode("PitchBendChange");
		result->set_property("Channel", channel());
		result->set_property("Value",  pitch_bender_value());
		break;

	default:
		// The implementation is continued as needed
		result = new XMLNode("NotImplemented");
		break;
	}

	return boost::shared_ptr<XMLNode>(result);
}

#endif // EVORAL_MIDI_XML

template class MIDIEvent<Evoral::Beats>;

} // namespace Evoral

