/* selftest.hpp - unit testing support

Copyright © 2013-2017 Brian Bray

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

/*

Selftest unit testing
=====================

These are several related facilities here:
	selftest::trace A 'trace' debug ostream that can be selectively activated
	ASSERT          Self testing that can be disabled for performance
	BAD_ARG()...    Macro's for throwing exceptions that are debugger
                    friendly and provide extra context.
	TEST_FUNCTION( function )
            		Registers and defines a unit test function.
                    The unit test function uses the following two
                    macros to verify results.
	CHECKIF( predicate )
                    Tests that predicate is true
    CHECKIFTHROWS( expr, except )
                    Checks that expression 'expr' throws an expected exception.
                    Used for unit testing error conditions

These facilities have options enabled or disabled by setting preprocessor 
variables before the '#include "selftest.hpp"'


Using selftest::trace
---------------------

To use it:
    #define TRACING 1
    #include "selftest.hpp"
    selftest::trace << "I'm here, let the bells ring and the banners fly\n";

Options before #include "selftest.hpp"
	#define TRACING 1		<-- trace is std::cerr
	#define TRACING 0		<-- trace output is discarded
	TRACING not #defined    <-- trace not defined



Using ASSERT and IFASSERTING
----------------------------

This version of ASSERT provides context and is debugger friendly (ie: you can
set a breakpoint that traps all asserts before the call stack is gone)

To use it:
    #include "selftest.hpp"
	ASSERT( predicate );        // Throws if predicate is false
	IFASSERTING( statement );   // Evaluates statement if ASSERTS are enabled

The preprocessor options for assert are:
	#define GOFASTERSTRIPE  <-- ASSERT disabled to save execution time
	#define OBFUSCATETESTS  <-- Messages without details to save space


Using THROW macros
------------------

Self test support macros:

    #include "selftest.hpp"
	BAD_ARG( message )
		Throws std::invalid_argument
	TEST_FAIL( message )
		Throws selftest::selftest_error
	OVER_LIMIT( message )
		Throws selftest::over_reasonable_limit

For use in a debugger, set a breakpoint on the function 
    selftest::thrower<0>() 
to catch all these errors as they happen and before throwing.

Options before #include "selftest.hpp"
	$define TRACETHROWS
		Will print messages to cerr prior to the throw


Using Unit Test macros
----------------------

In unit test source files:
	TEST_FUNCTION( function )
            		Registers and defines a unit test function.
	CHECKIF( predicate )
		            Prints a message on std::cerr if predicate is false.
    CHECKIFTHROWS( stmt, except )
                    Test fails if stmt does not throw expected exception type

It is expected that unit tests are in separate source files from regular code
which are linked only if unit tests are to be run by the executable. Call
        selftest::UnitTest::run_unit_tests<0>() 
to invoke all of the registered unit test functions.

A unit test source file consists of a sequence of routines mainly containing
CHECKIF()'s. Each routine is defined by the macro TEST_FUNCTION(function). For
example:

    #include "selftest.hpp"
	TEST_FUNCTION( check_sqrt )
	{
		CHECKIF( sqrt(4)==2 );
		CHECKIF( sqrt(100)==10 );
	}

Unit test routines will be executed in the order of appearance in the source.
Both the name of the test function and the text of the CHECKIF will be visible
if the test fails, so verbose names are useful.


An example follows

file main.cc
-------------
int main()
{
    #ifdef DEBUG
	    auto fails = selftest::run_unit_tests<0>();
    	trace << fails.numFailedTest << "/" << fails.numTests << " unit tests failed\n";
    #endif
}



TEST_FUNCTION( test_sqrt )
{
	float foo;
	CHECKIF( sqrt(foo)==2. );
	CHECKIF( isnan(sqrt(-foo)) );
}

TEST_FUNCTION( error_handler_testing )
{
    ...
	CHECKIFTHROWS( a_math_function(bad_argument), std::exception )
}
*/

#include <iostream>
#include <chrono>
#include <string>

// Tracing support classes and typedefs
#ifdef TRACING

    namespace selftest {

        #if TRACING == 1
            static std::ostream &trace = std::cerr;
        #else
            class NullOstream {};

            template <typename T> 
            NullOstream& 
            operator<<(NullOstream& tr, T)
            {
                return tr;
            }

            inline NullOstream& 
            operator<<(NullOstream& tr, std::ostream& (*)(std::ostream&))
            {
                return tr;
            }

            inline NullOstream& 
            operator<<(NullOstream& tr, std::ios& (*)(std::ios&))
            {
                return tr;
            }

            static selftest::NullOstream trace;
        #endif

    }	// namespace selftest

#endif      // ifdef TRACING


// Macro ASSERT
#ifdef GOFASTERSTRIPE
    #define ASSERT( X )
    #define IFASSERTING( X )
#else

    #ifdef OBFUSCATETESTS
        #define ASSERT( X ) {if(!(X)) \
            {selftest::thrower<0>( selftest::failType::badassert, 0, 0, 0, 0 );} }
    #else
        #define ASSERT( X ) {if(!(X)) \
            {selftest::thrower<0>( selftest::failType::badassert, \
                                #X, __func__, __FILE__, __LINE__ );} }
    #endif

    #define IFASSERTING( X ) {X;}

#endif


// Macros THROWxxx
#ifdef OBFUSCATETESTS
    #define BAD_ARG( X ) \
            selftest::thrower<0>( selftest::failType::badarg, 0, 0, 0, 0 )
    #define TEST_FAIL( X ) \
            selftest::thrower<0>( selftest::failType::badselftest, 0, 0, 0, 0 )
    #define OVER_LIMIT( X ) \
            selftest::thrower<0>( selftest::failType::overlimit, 0, 0, 0, 0 )
#else
    #define BAD_ARG( X ) \
            selftest::thrower<0>( selftest::failType::badarg, \
                               X, __func__, __FILE__, __LINE__ )
    #define TEST_FAIL( X ) \
            selftest::thrower<0>( selftest::failType::badselftest, \
                               X, __func__, __FILE__, __LINE__ )
    #define OVER_LIMIT( X ) \
            selftest::thrower<0>( selftest::failType::overlimit, \
                               X, __func__, __FILE__, __LINE__ )
#endif


// Unit testing Macros
#define TEST_FUNCTION( X ) selftest::TestFunc X; \
                           selftest::UnitTest<0> unittester ## X ( X,#X ); \
                           void X()
#define UNITTEST_FAIL( X ) \
        selftest::thrower<0>( selftest::failType::badunittest, \
                              X, __func__, __FILE__, __LINE__ )

#define CHECKIF( X ) {if(!(X)) UNITTEST_FAIL( #X ); }
#define CHECKIFTHROWS( X,E ) {bool caught_expected=false; \
	try {X;} \
	catch(const E &e) {caught_expected=true;} \
	if(!caught_expected) UNITTEST_FAIL( #X " should throw " #E ); \
	}


// Declaration of support classes, types, and routines
namespace selftest {

class selftest_error : public std::logic_error {
public:
	explicit selftest_error(const std::string& arg)
		: logic_error(arg) {};
	virtual ~selftest_error() noexcept {};
};

class over_reasonable_limit : public std::runtime_error {
public:
	explicit over_reasonable_limit(const std::string& arg)
		: runtime_error(arg) {};
	virtual ~over_reasonable_limit() noexcept {};
};

class terminate_unittest {};

enum class failType {
    first, 
    badarg=first, 
    badassert, 
    badselftest, 
    badunittest, 
    overlimit, 
    last=overlimit
};

typedef void TestFunc();

struct FailRatio {
	int numFailedTests;
	int numTests;
};

template<int>
class UnitTest {
public:
	UnitTest( TestFunc *tf, const char* tfName );
	static FailRatio runUnitTestsImpl();

private:
	bool callUnitTest();

	TestFunc *testfunc_;
	UnitTest *next_;
	const char *tfname_;
};


template<int>
void thrower(
    const failType ft,
    const char* failedPredicate,
    const char* function,
    const char* fileName,
    int lineNum );


template<>
void thrower<0>(
    const failType ft,
    const char* failedPredicate,
    const char* function,
    const char* fileName,
    int lineNum )
{
    #ifdef DEBUG
		#ifdef TRACETHROWS
        	const bool debugging = true;
		#else
        	const bool debugging = false;
		#endif
    #else
        const bool debugging = false;    
    #endif
    
	/* Compose message texts:
	<file>:<line>:0: error: <ft text> '<failedPredicate>' failed in <function>.
	*/
	std::string message;
	if (fileName && lineNum) {
		message = std::string(fileName) + ":" + 
                std::to_string(lineNum) + ":0: error: ";
    }
	switch (ft) {
	case failType::badarg:
		message += "Argument test";
		break;
	case failType::badassert:
		message += "Assertion";
		break;
	case failType::badselftest:
		message += "Self test";
		break;
	case failType::badunittest:
		message += "Unit test";
		break;
	case failType::overlimit:
		message += "Reasonable limit";
		break;
	}

	if (failedPredicate)
		message += std::string(" '") + failedPredicate + "'";
	message += " failed";
	if (function)
		message += std::string(" in ") + function;
	message += '.';

	switch (ft) {
	case failType::badarg:
        if ( debugging ) {
		    std::cerr << message << std::endl;
        }
		throw std::invalid_argument(message);
		break;
	case failType::badassert:
        if ( debugging ) {
		    std::cerr << message << std::endl;
        }
		throw selftest::selftest_error(message);
		break;
	case failType::badselftest:
		throw selftest::selftest_error(message);
		break;
	case failType::badunittest:
		std::cerr << message << std::endl;
		throw terminate_unittest();
		break;
	case failType::overlimit:
        if ( debugging ) {
		    std::cerr << message << std::endl;
        }
		throw selftest::over_reasonable_limit(message);
		break;
	}

	return;
}


template<>
UnitTest<0>::UnitTest( TestFunc *tf, const char* tfName )
	: testfunc_( tf ),
	  tfname_( tfName )
{
	static UnitTest *head = nullptr;
	next_ = head;
	if (tf)
		head = this;
	else
		head = nullptr;			// Last test registered, reset list
}

template<>
bool UnitTest<0>::callUnitTest()
{
    // Maximum duration for a single unit test
    const int time_limit_seconds = 2;
    bool failedTest = false;

	try {
		auto testStart = std::chrono::high_resolution_clock::now();

		testfunc_();

		auto duration = std::chrono::high_resolution_clock::now()-testStart;
		if ( duration > std::chrono::seconds(time_limit_seconds) ) {
			std::cerr << "Unit test " << tfname_ << " not complete within "
			     << time_limit_seconds << " seconds." << std::endl;
			failedTest = true;
		}
		return failedTest;
	}

	catch( const terminate_unittest& e ) {
		// Message, already written
        failedTest = true;
		return failedTest;
	}

	catch( const char* e ) {
		std::cerr << "Exception thrown during unit test '" << tfname_
		     <<  "': \"" << e << "\"." << std::endl;
	}

	catch( const std::exception& e ) {
		std::cerr << "Exception thrown during unit test '" << tfname_
		     << "': " << e.what() << "." << std::endl;
	}

	catch( ... ) {
		std::cerr << "Exception of unknown type thrown during unit test '"
		     << tfname_ << "'." << std::endl;
	}

	failedTest = true;
    return failedTest;
}

template<>
FailRatio UnitTest<0>::runUnitTestsImpl()
{
    bool failedTest = false;
	UnitTest lastTest( nullptr, "Tests complete" );
	// Reverse list so that they are run in the order given
	UnitTest *newHead = nullptr;
	UnitTest *oldHead = lastTest.next_;
	UnitTest *tptr = nullptr;

	while (oldHead) {
		// Unlink from old
		tptr = oldHead;
		oldHead = oldHead->next_;
		// Link to end of new
		tptr->next_ = newHead;
		newHead = tptr;
	}

	// Now call them
    // sttrace << "Starting unit tests...\n";
	//runningUnitTests = true;
	FailRatio rc {0,0};
	while (newHead) {
		failedTest = newHead->callUnitTest();
		++rc.numTests;
		if (failedTest) {
			++rc.numFailedTests;
		}
		newHead = newHead->next_;
	}
	//runningUnitTests = false;

	return rc;
}


template<int>
FailRatio runUnitTests();

template<>
FailRatio runUnitTests<0>()
{
    return UnitTest<0>::runUnitTestsImpl();
}


}	// namespace st

