
/*
    Copyright (C) 2015 Tim Mayberry

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

#ifndef __libpbd_stack_trace_h__
#define __libpbd_stack_trace_h__

#include <iosfwd>
#include <string>

#include "pbd/libpbd_visibility.h"

namespace PBD {

class LIBPBD_API StackTrace {
public:
	/**
	 * Creates a stacktrace at the current location
	 */
	StackTrace();

	/**
	 * Get addresses contained in stacktrace
	 * @return addresses or NULL
	 * @param count number of elements/addresses in returned array
	 */
	const void* const* get_addresses(size_t& count) const;

	/**
	 * print stacktrace to stderr
	 * @param start_offset offset the start of the trace by start_offset frames
	 */
	void print(size_t start_offset = 0) const;

	/**
	 * print stacktrace to ostream
	 */
	void to_stream (std::ostream& ostream);

	// overload operator<<?
	
	/**
	 * @return string containing stacktrace
	 */
	std::string to_string () const;

	/**
	 * @return true if stacktraces are equivalent
	 */
	bool operator==(const StackTrace& other)
	{
		if (m_count != other.m_count) return false;

		for (size_t i = 0; i < m_count; ++i) {
			if (m_trace[i] != other.m_trace[i]) {
				return false;
			}
		}
		return true;
	}

private:
	static const int s_max_traces = 62;

	void* m_trace[s_max_traces];

	size_t m_count;
};

} // namespace PBD

#endif // __libpbd_stack_trace_h__
