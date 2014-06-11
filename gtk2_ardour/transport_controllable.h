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

#ifndef TRANSPORT_CONTROLLABLE_H
#define TRANSPORT_CONTROLLABLE_H

#include "pbd/controllable.h"

class ARDOUR_UI;

struct TransportControllable : public PBD::Controllable {
	enum ToggleType {
		Roll = 0,
		Stop,
		RecordEnable,
		GotoStart,
		GotoEnd,
		AutoLoop,
		PlaySelection,
	};

	TransportControllable (std::string name, ARDOUR_UI&, ToggleType);
	void set_value (double);
	double get_value (void) const;

	ARDOUR_UI& ui;
	ToggleType type;
};

#endif
