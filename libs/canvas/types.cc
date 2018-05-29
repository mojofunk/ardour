/*
    Copyright (C) 2011-2013 Paul Davis
    Author: Carl Hetherington <cth@carlh.net>

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

#include <cfloat>
#include <cassert>

#include <cairomm/context.h>

#include "pbd/string_convert.h"

#include "canvas/types.h"

using namespace std;
using namespace ArdourCanvas;

Coord const ArdourCanvas::COORD_MAX = 1.7e307;

std::string
Duple::to_string () const
{
	return std::string ("(") + PBD::to_string (x) + ", " + PBD::to_string (y) + ")";
}

std::string
Rect::to_string () const
{
	return std::string ("[(") + PBD::to_string (x0) + ", " + PBD::to_string (y0) + "), (" +
	       PBD::to_string (x1) + ", " + PBD::to_string (y1) + ") " + PBD::to_string (width ()) +
	       " x " + PBD::to_string (height ()) + "]";
}

ostream &
ArdourCanvas::operator<< (ostream & s, Duple const & r)
{
	s << r.to_string ();
	return s;
}

ostream &
ArdourCanvas::operator<< (ostream & s, Rect const & r)
{
	s << r.to_string ();
	return s;
}

