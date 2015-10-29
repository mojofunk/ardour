#include "stack_trace_test.h"

#include "pbd/stack_trace.h"

CPPUNIT_TEST_SUITE_REGISTRATION (StackTraceTest);

using namespace std;
using namespace PBD;

StackTraceTest::StackTraceTest ()
{
}

void
StackTraceTest::testPrint ()
{
	StackTrace trace;

	trace.print();
}
