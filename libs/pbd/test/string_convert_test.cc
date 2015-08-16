#include "string_convert_test.h"

#include <stdio.h>
#include <string.h>

#include <limits>
#include <cassert>

#include <pthread.h>

#include <glib.h>

#include "pbd/string_convert.h"

using namespace PBD;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION (StringConvertTest);

static const double s_test_double = 31459.265359;
static const int s_iter_count = 500000;

#ifdef PLATFORM_WINDOWS
static const std::string french_locale_name ("French_France.1252");
static const std::string italian_locale_name ("Italian_Italy.1252");
#else
static const std::string french_locale_name ("fr_FR");
static const std::string italian_locale_name ("it_IT");
#endif

namespace {

bool
check_fr_printf ()
{
	char buf[32];
	snprintf (buf, sizeof(buf), "%.12g", s_test_double);
	bool found = (strchr (buf, ',') != NULL);
	return found;
}

void*
check_fr_printf_thread (void*)
{
	for (int n = 0; n < s_iter_count; n++) {
		assert (check_fr_printf ());
	}

	return NULL;
}

// RAII class that sets the global C locale to fr_FR and then resets it
class LocaleGuard {
public:
	LocaleGuard (const std::string& locale)
	{
		m_previous_locale = setlocale (LC_ALL, NULL);

		std::cerr << std::endl;
		std::cerr << "Previous C locale was: " << m_previous_locale << std::endl;

		CPPUNIT_ASSERT (m_previous_locale != NULL);

		const char* new_locale = setlocale (LC_ALL, locale.c_str ());

		CPPUNIT_ASSERT (new_locale != NULL);

		CPPUNIT_ASSERT (locale == new_locale);

		std::cerr << "Current C locale is: " << new_locale << std::endl;
	}

	~LocaleGuard ()
	{
		CPPUNIT_ASSERT (setlocale (LC_ALL, m_previous_locale) != NULL);
	}

private:
	const char* m_previous_locale;
};

} // anon namespace

static const std::string MAX_INT16_STR ("32767");
static const std::string MIN_INT16_STR ("-32768");

// Execute the test in the Italian locale as the format for integers uses
// thousands separators in the number format. Test that this has no impact on
// the string conversions which are expected to be the same as in the C locale.
void
StringConvertTest::test_int16_conversion ()
{
	LocaleGuard guard (italian_locale_name);

	string str;
	CPPUNIT_ASSERT (int16_to_string (numeric_limits<int16_t>::max (), str));
	CPPUNIT_ASSERT_EQUAL (MAX_INT16_STR, str);

	int16_t val = 0;
	CPPUNIT_ASSERT (string_to_int16 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<int16_t>::max (), val);

	CPPUNIT_ASSERT (int16_to_string (numeric_limits<int16_t>::min (), str));
	CPPUNIT_ASSERT_EQUAL (MIN_INT16_STR, str);

	CPPUNIT_ASSERT (string_to_int16 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<int16_t>::min (), val);

	// test the string_to/to_string templates
	int16_t max = numeric_limits<int16_t>::max ();
	CPPUNIT_ASSERT_EQUAL (max, string_to<int16_t>(to_string (max)));

	int16_t min = numeric_limits<int16_t>::min ();
	CPPUNIT_ASSERT_EQUAL (min, string_to<int16_t>(to_string (min)));
}

static const std::string MAX_UINT16_STR("65535");
static const std::string MIN_UINT16_STR("0");

void
StringConvertTest::test_uint16_conversion ()
{
	LocaleGuard guard (italian_locale_name);

	string str;
	CPPUNIT_ASSERT (uint16_to_string (numeric_limits<uint16_t>::max (), str));
	CPPUNIT_ASSERT_EQUAL (MAX_UINT16_STR, str);

	uint16_t val = 0;
	CPPUNIT_ASSERT (string_to_uint16 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<uint16_t>::max (), val);

	CPPUNIT_ASSERT (uint16_to_string (numeric_limits<uint16_t>::min (), str));
	CPPUNIT_ASSERT_EQUAL (MIN_UINT16_STR, str);

	CPPUNIT_ASSERT (string_to_uint16 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<uint16_t>::min (), val);

	// test the string_to/to_string templates
	uint16_t max = numeric_limits<uint16_t>::max ();
	CPPUNIT_ASSERT_EQUAL (max, string_to<uint16_t>(to_string (max)));

	uint16_t min = numeric_limits<uint16_t>::min ();
	CPPUNIT_ASSERT_EQUAL (min, string_to<uint16_t>(to_string (min)));
}

static const std::string MAX_INT32_STR ("2147483647");
static const std::string MIN_INT32_STR ("-2147483648");

// Execute the test in the Italian locale as the format for integers uses
// thousands separators in the number format. Test that this has no impact on
// the string conversions which are expected to be the same as in the C locale.
void
StringConvertTest::test_int32_conversion ()
{
	LocaleGuard guard (italian_locale_name);

	string str;
	CPPUNIT_ASSERT (int32_to_string (numeric_limits<int32_t>::max (), str));
	CPPUNIT_ASSERT_EQUAL (MAX_INT32_STR, str);

	int32_t val = 0;
	CPPUNIT_ASSERT (string_to_int32 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<int32_t>::max (), val);

	CPPUNIT_ASSERT (int32_to_string (numeric_limits<int32_t>::min (), str));
	CPPUNIT_ASSERT_EQUAL (MIN_INT32_STR, str);

	CPPUNIT_ASSERT (string_to_int32 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<int32_t>::min (), val);

	// test the string_to/to_string templates
	int32_t max = numeric_limits<int32_t>::max ();
	CPPUNIT_ASSERT_EQUAL (max, string_to<int32_t>(to_string (max)));

	int32_t min = numeric_limits<int32_t>::min ();
	CPPUNIT_ASSERT_EQUAL (min, string_to<int32_t>(to_string (min)));
}

static const std::string MAX_UINT32_STR("4294967295");
static const std::string MIN_UINT32_STR("0");

void
StringConvertTest::test_uint32_conversion ()
{
	LocaleGuard guard (italian_locale_name);

	string str;
	CPPUNIT_ASSERT (uint32_to_string (numeric_limits<uint32_t>::max (), str));
	CPPUNIT_ASSERT_EQUAL (MAX_UINT32_STR, str);

	uint32_t val = 0;
	CPPUNIT_ASSERT (string_to_uint32 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<uint32_t>::max (), val);

	CPPUNIT_ASSERT (uint32_to_string (numeric_limits<uint32_t>::min (), str));
	CPPUNIT_ASSERT_EQUAL (MIN_UINT32_STR, str);

	CPPUNIT_ASSERT (string_to_uint32 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<uint32_t>::min (), val);

	// test the string_to/to_string templates
	uint32_t max = numeric_limits<uint32_t>::max ();
	CPPUNIT_ASSERT_EQUAL (max, string_to<uint32_t>(to_string (max)));

	uint32_t min = numeric_limits<uint32_t>::min ();
	CPPUNIT_ASSERT_EQUAL (min, string_to<uint32_t>(to_string (min)));
}

static const std::string MAX_INT64_STR ("9223372036854775807");
static const std::string MIN_INT64_STR ("-9223372036854775808");

void
StringConvertTest::test_int64_conversion ()
{
	LocaleGuard guard (italian_locale_name);

	string str;
	CPPUNIT_ASSERT (int64_to_string (numeric_limits<int64_t>::max (), str));
	CPPUNIT_ASSERT_EQUAL (MAX_INT64_STR, str);

	int64_t val = 0;
	CPPUNIT_ASSERT (string_to_int64 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<int64_t>::max (), val);

	CPPUNIT_ASSERT (int64_to_string (numeric_limits<int64_t>::min (), str));
	CPPUNIT_ASSERT_EQUAL (MIN_INT64_STR, str);

	CPPUNIT_ASSERT (string_to_int64 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<int64_t>::min (), val);

	// test the string_to/to_string templates
	int64_t max = numeric_limits<int64_t>::max ();
	CPPUNIT_ASSERT_EQUAL (max, string_to<int64_t>(to_string (max)));

	int64_t min = numeric_limits<int64_t>::min ();
	CPPUNIT_ASSERT_EQUAL (min, string_to<int64_t>(to_string (min)));
}

static const std::string MAX_UINT64_STR ("18446744073709551615");
static const std::string MIN_UINT64_STR ("0");

void
StringConvertTest::test_uint64_conversion ()
{
	LocaleGuard guard (italian_locale_name);

	string str;
	CPPUNIT_ASSERT (uint64_to_string (numeric_limits<uint64_t>::max (), str));
	CPPUNIT_ASSERT_EQUAL (MAX_UINT64_STR, str);

	uint64_t val = 0;
	CPPUNIT_ASSERT (string_to_uint64 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<uint64_t>::max (), val);

	CPPUNIT_ASSERT (uint64_to_string (numeric_limits<uint64_t>::min (), str));
	CPPUNIT_ASSERT_EQUAL (MIN_UINT64_STR, str);

	CPPUNIT_ASSERT (string_to_uint64 (str, val));
	CPPUNIT_ASSERT_EQUAL (numeric_limits<uint64_t>::min (), val);

	// test the string_to/to_string templates
	uint64_t max = numeric_limits<uint64_t>::max ();
	CPPUNIT_ASSERT_EQUAL (max, string_to<uint64_t>(to_string (max)));

	uint64_t min = numeric_limits<uint64_t>::min ();
	CPPUNIT_ASSERT_EQUAL (min, string_to<uint64_t>(to_string (min)));
}

static const std::string MAX_FLOAT_WIN ("3.4028234663852886e+038");
static const std::string MIN_FLOAT_WIN ("1.1754943508222875e-038");
static const std::string MAX_FLOAT_STR ("3.4028234663852886e+38");
static const std::string MIN_FLOAT_STR ("1.1754943508222875e-38");

void
StringConvertTest::test_float_conversion ()
{
	LocaleGuard guard (french_locale_name);

	// check float to string and back again for min and max float values
	string str;
	CPPUNIT_ASSERT (float_to_string (numeric_limits<float>::max (), str));
#ifdef PLATFORM_WINDOWS
	CPPUNIT_ASSERT_EQUAL (MAX_FLOAT_WIN, str);
#else
	CPPUNIT_ASSERT_EQUAL (MAX_FLOAT_STR, str);
#endif

	float val = 0.0f;
	CPPUNIT_ASSERT (string_to_float (str, val));
	CPPUNIT_ASSERT_DOUBLES_EQUAL (
	    numeric_limits<float>::max (), val, numeric_limits<float>::epsilon ());

	CPPUNIT_ASSERT (float_to_string (numeric_limits<float>::min (), str));
#ifdef PLATFORM_WINDOWS
	CPPUNIT_ASSERT_EQUAL (MIN_FLOAT_WIN, str);
#else
	CPPUNIT_ASSERT_EQUAL (MIN_FLOAT_STR, str);
#endif

	CPPUNIT_ASSERT (string_to_float (str, val));
	CPPUNIT_ASSERT_DOUBLES_EQUAL (
	    numeric_limits<float>::min (), val, numeric_limits<float>::epsilon ());

	// test the string_to/to_string templates
	float max = numeric_limits<float>::max ();
	CPPUNIT_ASSERT_EQUAL (max, string_to<float>(to_string (max)));

	float min = numeric_limits<float>::min ();
	CPPUNIT_ASSERT_EQUAL (min, string_to<float>(to_string (min)));

// check that parsing the windows float string representation with the
// difference in the exponent part parses correctly on other platforms
// and vice versa
#ifdef PLATFORM_WINDOWS
	CPPUNIT_ASSERT (string_to_float (MAX_FLOAT_STR, val));
	CPPUNIT_ASSERT_DOUBLES_EQUAL (
	    numeric_limits<float>::max (), val, numeric_limits<float>::epsilon ());

	CPPUNIT_ASSERT (string_to_float (MIN_FLOAT_STR, val));
	CPPUNIT_ASSERT_DOUBLES_EQUAL (
	    numeric_limits<float>::min (), val, numeric_limits<float>::epsilon ());
#else
	CPPUNIT_ASSERT (string_to_float (MAX_FLOAT_WIN, val));
	CPPUNIT_ASSERT_DOUBLES_EQUAL (
	    numeric_limits<float>::max (), val, numeric_limits<float>::epsilon ());

	CPPUNIT_ASSERT (string_to_float (MIN_FLOAT_WIN, val));
	CPPUNIT_ASSERT_DOUBLES_EQUAL (
	    numeric_limits<float>::min (), val, numeric_limits<float>::epsilon ());
#endif
}

static const std::string MAX_DOUBLE_STR ("1.7976931348623157e+308");
static const std::string MIN_DOUBLE_STR ("2.2250738585072014e-308");

void
StringConvertTest::test_double_conversion ()
{
	LocaleGuard guard (french_locale_name);

	string str;
	CPPUNIT_ASSERT (double_to_string (numeric_limits<double>::max (), str));
	CPPUNIT_ASSERT_EQUAL (MAX_DOUBLE_STR, str);

	double val = 0.0;
	CPPUNIT_ASSERT (string_to_double (str, val));
	CPPUNIT_ASSERT_DOUBLES_EQUAL (
	    numeric_limits<double>::max (), val, numeric_limits<double>::epsilon ());

	CPPUNIT_ASSERT (double_to_string (numeric_limits<double>::min (), str));
	CPPUNIT_ASSERT_EQUAL (MIN_DOUBLE_STR, str);

	CPPUNIT_ASSERT (string_to_double (str, val));
	CPPUNIT_ASSERT_DOUBLES_EQUAL (
	    numeric_limits<double>::min (), val, numeric_limits<double>::epsilon ());

	// test that overflow fails
	CPPUNIT_ASSERT (!string_to_double ("1.8e+308", val));
	// test that underflow fails
	CPPUNIT_ASSERT (!string_to_double ("2.4e-310", val));

	// test the string_to/to_string templates
	double max = numeric_limits<double>::max ();
	CPPUNIT_ASSERT_EQUAL (max, string_to<double>(to_string (max)));

	double min = numeric_limits<double>::min ();
	CPPUNIT_ASSERT_EQUAL (min, string_to<double>(to_string (min)));
}

// we have to use these as CPPUNIT_ASSERT_EQUAL won't accept char arrays
static const std::string BOOL_TRUE_STR ("yes");
static const std::string BOOL_FALSE_STR ("no");

void
StringConvertTest::test_bool_conversion ()
{
	string str;

	// check the normal case for true/false
	CPPUNIT_ASSERT (bool_to_string (true, str));
	CPPUNIT_ASSERT_EQUAL (BOOL_TRUE_STR, str);

	bool val = false;
	CPPUNIT_ASSERT (string_to_bool (str, val));
	CPPUNIT_ASSERT_EQUAL (val, true);

	CPPUNIT_ASSERT (bool_to_string (false, str));
	CPPUNIT_ASSERT_EQUAL (BOOL_FALSE_STR, str);

	val = true;
	CPPUNIT_ASSERT (string_to_bool (str, val));
	CPPUNIT_ASSERT_EQUAL (val, false);

	// now check the other accepted values for true and false
	// when converting from a string to bool

	val = false;
	CPPUNIT_ASSERT (string_to_bool ("1", val));
	CPPUNIT_ASSERT_EQUAL (val, true);

	val = true;
	CPPUNIT_ASSERT (string_to_bool ("0", val));
	CPPUNIT_ASSERT_EQUAL (val, false);

	val = false;
	CPPUNIT_ASSERT (string_to_bool ("Y", val));
	CPPUNIT_ASSERT_EQUAL (val, true);

	val = true;
	CPPUNIT_ASSERT (string_to_bool ("N", val));
	CPPUNIT_ASSERT_EQUAL (val, false);

	val = false;
	CPPUNIT_ASSERT (string_to_bool ("y", val));
	CPPUNIT_ASSERT_EQUAL (val, true);

	val = true;
	CPPUNIT_ASSERT (string_to_bool ("n", val));
	CPPUNIT_ASSERT_EQUAL (val, false);

	// test some junk
	CPPUNIT_ASSERT (!string_to_bool ("01234someYNtrueyesno junk0123", val));

	// test the string_to/to_string templates
	CPPUNIT_ASSERT_EQUAL (true, string_to<bool>(to_string (true)));

	CPPUNIT_ASSERT_EQUAL (false, string_to<bool>(to_string (false)));
}

namespace {

bool
check_int_convert ()
{
	int32_t num = g_random_int ();
	return (num == string_to<int32_t>(to_string (num)));
}

bool
check_float_convert ()
{
	float num = (float)g_random_double ();
	return (num == string_to<float>(to_string (num)));
}

bool
check_double_convert ()
{
	double num = g_random_double ();
	return (num == string_to<double>(to_string (num)));
}

void*
check_int_convert_thread(void*)
{
	for (int n = 0; n < s_iter_count; n++) {
		assert (check_int_convert ());
	}
	return NULL;
}

void*
check_float_convert_thread(void*)
{
	for (int n = 0; n < s_iter_count; n++) {
		assert (check_float_convert ());
	}
	return NULL;
}

void*
check_double_convert_thread(void*)
{
	for (int n = 0; n < s_iter_count; n++) {
		assert (check_double_convert ());
	}
	return NULL;
}

} // anon namespace

// Perform the test in the French locale as the format for decimals is
// different and a comma is used as a decimal point. Test that this has no
// impact on the string conversions which are expected to be the same as in the
// C locale.
void
StringConvertTest::test_convert_thread_safety ()
{
	LocaleGuard guard (french_locale_name);

	std::cerr << "Checking conversions" << std::endl;

	CPPUNIT_ASSERT (check_int_convert ());
	CPPUNIT_ASSERT (check_float_convert ());
	CPPUNIT_ASSERT (check_double_convert ());
	CPPUNIT_ASSERT (check_fr_printf ());

	pthread_t convert_int_thread;
	pthread_t convert_float_thread;
	pthread_t convert_double_thread;
	pthread_t fr_printf_thread;

	std::cerr << "Starting conversion threads" << std::endl;

	CPPUNIT_ASSERT (
	    pthread_create (
	        &convert_int_thread, NULL, check_int_convert_thread, NULL) == 0);
	CPPUNIT_ASSERT (
	    pthread_create (
	        &convert_float_thread, NULL, check_float_convert_thread, NULL) == 0);
	CPPUNIT_ASSERT (
	    pthread_create (
	        &convert_double_thread, NULL, check_double_convert_thread, NULL) == 0);
	CPPUNIT_ASSERT (
	    pthread_create (&fr_printf_thread, NULL, check_fr_printf_thread, NULL) ==
	    0);

	std::cerr << "Joining conversion threads" << std::endl;

	void* return_value;

	CPPUNIT_ASSERT (pthread_join (convert_int_thread, &return_value) == 0);
	CPPUNIT_ASSERT (pthread_join (convert_float_thread, &return_value) == 0);
	CPPUNIT_ASSERT (pthread_join (convert_double_thread, &return_value) == 0);
	CPPUNIT_ASSERT (pthread_join (fr_printf_thread, &return_value) == 0);
}
