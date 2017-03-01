/* ut-selftest.cc - Selftest all the selftests!! Test-ception is here

Copyright © 2013-2017 Brian Bray

See the attached 'LICENSE-MIT.txt' file for a licence to use this software and
IMPORTANT DISCLAIMERS.
*/

//#define TRACETHROWS
#define TRACING 1
#include "selftest.hpp"

#include <iostream>
#include <chrono>
#include <thread>

namespace {

using std::this_thread::sleep_for;
using std::chrono::seconds;
using selftest::selftest_error;
using selftest::over_reasonable_limit;
using std::cerr;
using selftest::trace;


TEST_FUNCTION( simple_pass )
{
	CHECKIF( true );
}

TEST_FUNCTION( assert_throws )
{
	CHECKIFTHROWS( ASSERT( false ), selftest_error );
}

TEST_FUNCTION( try_throws )
{
	CHECKIFTHROWS( BAD_ARG( "BAD_ARG" ), std::invalid_argument );
	CHECKIFTHROWS( OVER_LIMIT( "OVER_LIMIT" ), over_reasonable_limit );
	CHECKIFTHROWS( TEST_FAIL( "TEST_FAIL" ), selftest_error );
}

TEST_FUNCTION( multiple_failures )
{
	bool first_intentional_failure = false;
	bool continues_after_failure = false;
	CHECKIF( first_intentional_failure );
	CHECKIF( continues_after_failure );	// Should not appear
}

TEST_FUNCTION( second_intentional_failure )
{
	OVER_LIMIT( "Test message" );
}

TEST_FUNCTION( third_intentional_failure )
{
	throw "Visible message";
}

TEST_FUNCTION( fourth_intentional_failure )
{
	throw 42;
}


TEST_FUNCTION( fifth_and_final_intentional_failure )
{
	sleep_for( seconds(3) );
}

} // anon namespace


int main( int argc, char *argv[] )
{
	trace << "Starting test sequence. "
		     "5 failures expected during this test.\n\n\n";
    auto fails = selftest::runUnitTests<0>();

	if ( 5==fails.numFailedTests ) {
		trace << "\n\n\nTestception completed successfully\n";
		return 0;
	} else {
		cerr << "\n\n\nUnit testing of selftest.hpp failed\n";
		return 1;
	}
}
