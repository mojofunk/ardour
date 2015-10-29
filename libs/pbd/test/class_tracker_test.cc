#include "class_tracker_test.h"

#include "pbd/class_tracker.h"

#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION (ClassTrackerTest);

using namespace std;
using namespace PBD;

ClassTrackerTest::ClassTrackerTest ()
{
}

class Foo {
public:

	Foo(int id) : m_id(id) { }

	int get_id() const { return m_id; }

private:
	int m_id;

	PBD_CLASS_TRACKER(Foo);
};

static int count = 0;

void
create_foo_on_stack ()
{
	Foo foo(++count);
	foo.get_id();
}

void
copy_foo_on_stack ()
{
	Foo foo(++count);
	foo.get_id();
	Foo foo2(foo);
	foo2.get_id();
}

void
leak_one_foo ()
{
	Foo* foo = new Foo(++count);
	CPPUNIT_ASSERT(foo);
	foo->get_id();
}

void
leak_many_foo ()
{
	for (int i = 0; i < 32; ++i) {
		Foo* foo = new Foo(++count);
		CPPUNIT_ASSERT(foo);
		foo->get_id();
	}
}

void
ClassTrackerTest::testClassTracker ()
{
	CPPUNIT_ASSERT(ClassTrackerManager::tracking_enabled());
	ClassTrackerManager::set_classes_enabled_from_string("Foo");

	// should not yet be registered
	CPPUNIT_ASSERT(!ClassTrackerManager::get_class_tracker("Foo"));
	create_foo_on_stack();
	// should now be registered and enabled
	ClassTracker* class_tracker = ClassTrackerManager::get_class_tracker("Foo");
	CPPUNIT_ASSERT(class_tracker);
	CPPUNIT_ASSERT(class_tracker->get_counting_enabled());
	CPPUNIT_ASSERT(class_tracker->get_tracing_enabled());

	copy_foo_on_stack();
	leak_one_foo();
	leak_many_foo();
}

class Bar {
public:

	Bar(int id) : m_id(id) { }

	int get_id() const { return m_id; }

private:
	int m_id;

	PBD_CLASS_TRACKER(Bar);
};

void
ClassTrackerTest::testClassTrackerManager ()
{
	// ClassTrackerManager should have already been initialized by PBD::init

	// If class tracking is disabled then this test case will fail
	CPPUNIT_ASSERT(ClassTrackerManager::tracking_enabled());

	ClassTrackerManager::set_classes_enabled_from_string("Bar");
	std::vector<ClassTracker*> m_class_trackers;

	CPPUNIT_ASSERT(!ClassTrackerManager::get_class_tracker("Bar"));
	{
		Bar bar(1); // black sheep?
	}
	// check that the class tracker for Bar is now registered
	ClassTracker* bar_class_tracker =
	    ClassTrackerManager::get_class_tracker("Bar");
	CPPUNIT_ASSERT(bar_class_tracker);

	CPPUNIT_ASSERT(bar_class_tracker->get_instance_count() == 0);
	{
		Bar bar(1);
		CPPUNIT_ASSERT(bar_class_tracker->get_instance_count() == 1);
	}
	CPPUNIT_ASSERT(bar_class_tracker->get_instance_count() == 0);

	ClassTrackerManager::get_class_trackers (m_class_trackers);
	CPPUNIT_ASSERT(!m_class_trackers.empty());
}

class TestClass {
public:

	TestClass(int id) : m_id(id) { }

	int get_id() const { return m_id; }

private:
	int m_id;

	PBD_CLASS_TRACKER(TestClass);
};

void
ClassTrackerTest::testScopedClassTracker ()
{
	{
		TestClass tc1(1);
		tc1.get_id();
	}

	ScopedClassTracker test_class_tracker("TestClass");

	{
		TestClass tc1(1);
		tc1.get_id();
	}

	{
		TestClass* tc2 = new TestClass(2);
		tc2->get_id();
	}
}

void
ClassTrackerTest::testClassTrackerThreadSafety ()
{
	// TODO
}
