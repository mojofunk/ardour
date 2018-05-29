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

#include <iostream>

#include <glibmm/optioncontext.h>

#include "pbd/library_options.h"
#include "pbd/i18n.h"

namespace {

static bool libpbd_options_parsed = false;

} // anonymous

namespace PBD {

LibraryOptions::LibraryOptions()
	: Glib::OptionGroup("libpbd", _("libpbd options"), _("Command-line options for libpbd"))
{
	Glib::OptionEntry entry;

	entry.set_long_name ("debug");
	entry.set_short_name ('D');
	entry.set_description (_("Set debug flags. Use \"-D list\" to see available options."));
	add_entry (entry, _debug_categories);
}

LibraryOptions& LibraryOptions::get_instance ()
{
	static LibraryOptions options;
	return options;
}


void
LibraryOptions::set_debug_categories (const std::string& categories)
{
	get_instance ()._debug_categories = categories;
}

std::string
LibraryOptions::get_debug_categories ()
{
	return get_instance ()._debug_categories;
}

bool
LibraryOptions::parse_command_line (int *argc, char ***argv)
{
	if (libpbd_options_parsed) {
		// already been parsed by another OptionContext
		return true;
	}

	Glib::OptionContext context;

	context.set_main_group (get_instance ());

	try {
		context.parse (*argc, *argv);
	}

	catch (const Glib::OptionError& ex) {
		std::cout << _("Error while parsing command-line options: ") << std::endl
		          << ex.what () << std::endl;
		std::cout << _("Use --help to see a list of available command-line options.") << std::endl;
		return false;
	}

	catch(const Glib::Error& ex) {
		std::cout << "Error: " << ex.what () << std::endl;
		return false;
	}

	libpbd_options_parsed = true;

	return true;
}

} // namespace PBD
