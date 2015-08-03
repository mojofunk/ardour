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

#ifndef PBD_STRING_CONVERT_H
#define PBD_STRING_CONVERT_H

#include <string>
#include <stdint.h>

/**
 * Locale independent and thread-safe string conversion utility functions.
 *
 * All conversions are done as if they were performed in the C locale without
 * actually changing the current locale.
 */
namespace PBD {

bool bool_to_string (bool val, std::string& str);

bool int16_to_string (int16_t val, std::string& str);

bool uint16_to_string (uint16_t val, std::string& str);

bool int32_to_string (int32_t val, std::string& str);

bool uint32_to_string (uint32_t val, std::string& str);

bool int64_to_string (int64_t val, std::string& str);

bool uint64_to_string (uint64_t val, std::string& str);

bool float_to_string (float val, std::string& str);

bool double_to_string (double val, std::string& str);

bool string_to_bool (const std::string& str, bool& val);

bool string_to_int16 (const std::string& str, int16_t& val);

bool string_to_uint16 (const std::string& str, uint16_t& val);

bool string_to_int32 (const std::string& str, int32_t& val);

bool string_to_uint32 (const std::string& str, uint32_t& val);

bool string_to_int64 (const std::string& str, int64_t& val);

bool string_to_uint64 (const std::string& str, uint64_t& val);

bool string_to_float (const std::string& str, float& val);

bool string_to_double (const std::string& str, double& val);

template <class T>
inline
std::string
to_string (T val)
{
	// This will cause a compile time error if this function is ever
	// instantiated, which is useful to catch unintended conversions
	T::THIS_FUNCTION_IS_UNIMPLEMENTED;
	return std::string();
}

template <>
inline
std::string
to_string (bool val)
{
	std::string tmp;
	bool_to_string (val, tmp);
	return tmp;
}

template <>
inline
std::string
to_string (int16_t val)
{
	std::string tmp;
	int16_to_string (val, tmp);
	return tmp;
}

template <>
inline
std::string
to_string (uint16_t val)
{
	std::string tmp;
	uint16_to_string (val, tmp);
	return tmp;
}

template <>
inline
std::string
to_string (int32_t val)
{
	std::string tmp;
	int32_to_string (val, tmp);
	return tmp;
}

template <>
inline
std::string
to_string (uint32_t val)
{
	std::string tmp;
	uint32_to_string (val, tmp);
	return tmp;
}

template <>
inline
std::string
to_string (int64_t val)
{
	std::string tmp;
	int64_to_string (val, tmp);
	return tmp;
}

template <>
inline
std::string
to_string (uint64_t val)
{
	std::string tmp;
	uint64_to_string (val, tmp);
	return tmp;
}

template <>
inline
std::string
to_string (float val)
{
	std::string tmp;
	float_to_string (val, tmp);
	return tmp;
}

template <>
inline
std::string
to_string (double val)
{
	std::string tmp;
	double_to_string (val, tmp);
	return tmp;
}

template <class T>
inline
T
string_to (const std::string& str)
{
	// This will cause a compile time error if this function is ever
	// instantiated, which is useful to catch unintended conversions
	T::THIS_FUNCTION_IS_UNIMPLEMENTED;
	return T();
}

template <>
inline
bool
string_to (const std::string& str)
{
	bool tmp;
	string_to_bool (str, tmp);
	return tmp;
}

template <>
inline
int16_t
string_to (const std::string& str)
{
	int16_t tmp;
	string_to_int16 (str, tmp);
	return tmp;
}

template <>
inline
uint16_t
string_to (const std::string& str)
{
	uint16_t tmp;
	string_to_uint16 (str, tmp);
	return tmp;
}

template <>
inline
int32_t
string_to (const std::string& str)
{
	int32_t tmp;
	string_to_int32 (str, tmp);
	return tmp;
}

template <>
inline
uint32_t
string_to (const std::string& str)
{
	uint32_t tmp;
	string_to_uint32 (str, tmp);
	return tmp;
}

template <>
inline
int64_t
string_to (const std::string& str)
{
	int64_t tmp;
	string_to_int64 (str, tmp);
	return tmp;
}

template <>
inline
uint64_t
string_to (const std::string& str)
{
	uint64_t tmp;
	string_to_uint64 (str, tmp);
	return tmp;
}

template <>
inline
float
string_to (const std::string& str)
{
	float tmp;
	string_to_float (str, tmp);
	return tmp;
}

template <>
inline
double
string_to (const std::string& str)
{
	double tmp;
	string_to_double (str, tmp);
	return tmp;
}

} // namespace PBD

#endif // PBD_STRING_CONVERT_H
