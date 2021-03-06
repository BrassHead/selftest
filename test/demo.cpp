/* selftest demo of assert and friends

Copyright © 2013-2017 Brian Bray

See the attached 'LICENSE-MIT.txt' file for a licence to use this software and
IMPORTANT DISCLAIMERS.
*/


#define TRACING 1
#define SELFTEST_IMPLEMENTATION
#include "selftest.hpp"
using selftest::trace;

#include <string>
using std::string;
using std::to_string;

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;


string fizzbuzz( int index ) {
    if ( index<=0 ) {
        BAD_ARG( "Can't Fizz or Buzz with less than nothing!" );
    }
    IFASSERTING( if ( index>1000 ) OVER_LIMIT( "Too much Fizz" ) );

    bool isFizz = (0 == index%3);
    bool isBuzz = (0 == index%5);
    string res;

    if ( isFizz ) {
        res = "Fizz";
        if ( isBuzz ) {
            res += " Buzz";
        }
    } else if ( isBuzz ) {
        res = "Buzz";
    } else {
        res = to_string( index );
    }

    ASSERT( res.length()>0 );
    return res;
}

TEST_FUNCTION( FizzTests )
{
    CHECKSTREQ( "2",fizzbuzz( 2 ) );
    CHECKIF( "Fizz" == fizzbuzz( 33 ) );
}

TEST_FUNCTION( BuzzTests )
{
    CHECKIF( "Buzz" == fizzbuzz( 55 ) );
    CHECKSTREQ( "Fizz Buzz",fizzbuzz( 90 ) );
    // uncomment below to see what happens with a failed test
    // CHECKIF( "99" == fizzbuzz( 99 ) );
    // CHECKSTREQ( "99",fizzbuzz( 99 ) );
}

TEST_FUNCTION( FizzBuzzErrors )
{
    CHECKIFTHROWS( fizzbuzz( 43000 ), selftest::over_reasonable_limit );
    CHECKIFTHROWS( fizzbuzz( -3 ), std::invalid_argument );
}

int main( int argc, char *argv[] )
{
    trace << "Main started\n";

    #ifdef DEBUG
        auto fails = selftest::runUnitTests();
        if ( fails.numFailedTests>0 ) {
            cerr << fails.numFailedTests 
                 << "/" 
                 << fails.numTests 
                 << " unit tests failed"
                 << endl;
            exit( 1 );
        } else {
            cerr << "All unit tests passed." << endl;
        }
    #endif

    for ( int i=1; i<20; ++i ) {
        cout << fizzbuzz( i ) << "\n";
    }

    trace << "Main ending\n";
    return 0;
}
