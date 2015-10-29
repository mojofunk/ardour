#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class ClassTrackerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE (ClassTrackerTest);
	CPPUNIT_TEST (testClassTracker);
	CPPUNIT_TEST (testClassTrackerManager);
	CPPUNIT_TEST (testScopedClassTracker);
	CPPUNIT_TEST (testClassTrackerThreadSafety);
	CPPUNIT_TEST_SUITE_END ();

public:
	ClassTrackerTest ();
	void testClassTracker ();
	void testClassTrackerManager ();
	void testScopedClassTracker ();
	void testClassTrackerThreadSafety ();
};
