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

#include "pbd/stack_trace.h"

#include <cstdio>

#include "libpbd-config.h"

#include "pbd/demangle.h"

#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif

namespace PBD {

StackTrace::StackTrace()
	: m_count(0)
{
#ifdef HAVE_EXECINFO
	m_count = backtrace (m_trace, s_max_traces);
#endif
}

const void* const*
StackTrace::get_addresses(size_t& count) const
{
	count = m_count;
	if (m_count) {
		return m_trace;
	}
	return NULL;
}

void
StackTrace::print(size_t start_offset) const
{
#ifdef HAVE_EXECINFO
	// TODO use scoped_ptr
	char** trace_lines = backtrace_symbols(m_trace, m_count);
	
	for (size_t i = start_offset; i < m_count ; ++i) {
		const char* const trace_line = trace_lines[i];
		std::string demangled_line = demangle(trace_line);
		fprintf(stderr, "%s\n", demangled_line.c_str());
	}

	free(trace_lines);
#endif
}

} // namespace PBD
