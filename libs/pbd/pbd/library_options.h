/*
    Copyright (C) 2013 Tim Mayberry

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

#ifndef __libpbd_library_options_h__
#define __libpbd_library_options_h__

#include <glibmm/optiongroup.h>

#include "pbd/libpbd_visibility.h"

namespace PBD {

class LIBPBD_API LibraryOptions : public Glib::OptionGroup
{
private: // Ctors
	LibraryOptions();

public:
	/**
	 * parse command line, removing libpbd relevant arguments
	 *
	 * @return true if successfully parsed.
	 */
	static bool parse_command_line (int *argc, char ***argv);

	/**
	 * @return LibraryOptions Singleton
	 */
	static LibraryOptions& get_instance ();

	/**
	 * A comma separated list of debug categories to enable
	 */
	static void set_debug_categories (const std::string& categories);

	/**
	 * @return debug categories string, empty if unset
	 */
	static std::string get_debug_categories ();

private: // Data
	Glib::ustring _debug_categories;
};

} // namespace PBD

#endif // __libpbd_library_options_h__
