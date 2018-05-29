#include <getopt.h>
#include <glibmm/thread.h>

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>

#include "pbd/pbd.h"
#include "ardour/ardour.h"
#include "test_ui.h"

static const char* localedir = LOCALEDIR;

int
main(int argc, char* argv[])
{
	CPPUNIT_ASSERT (PBD::init (&argc, &argv));

	CPPUNIT_ASSERT (ARDOUR::init (false, true, localedir));

	TestUI* test_ui = new TestUI();

	CppUnit::TestResult testresult;

	CppUnit::TestResultCollector collectedresults;
	testresult.addListener (&collectedresults);

	CppUnit::BriefTestProgressListener progress;
	testresult.addListener (&progress);

	CppUnit::TestRunner testrunner;
	testrunner.addTest (CppUnit::TestFactoryRegistry::getRegistry ().makeTest ());
	testrunner.run (testresult);

	CppUnit::CompilerOutputter compileroutputter (&collectedresults, std::cerr);
	compileroutputter.write ();

	delete test_ui;

	ARDOUR::cleanup ();

	return collectedresults.wasSuccessful () ? 0 : 1;
}
