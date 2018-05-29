/*
    Copyright (C) 2017 Tim Mayberry

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

#ifndef __libpbd_dev_tools_h__
#define __libpbd_dev_tools_h__

#ifdef WITH_DEV_TOOLS
#include "adt.hpp"
#else
#include "pbd/dev_tools_noop.h"
#endif

#endif // __libpbd_dev_tools_h__
