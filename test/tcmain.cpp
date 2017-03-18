/*
    main program of testception.

    The main program is separated out in a separate compilation unit in order
    to test the ability of the library to be linked in a larger program
*/

#include <iostream>

#define TRACING 1
#define SELFTEST_IMPLEMENTATION
#include "selftest.hpp"

int main( int argc, char *argv[] )
{
    selftest::trace << "Starting test sequence. "
             "5 failures expected during this test.\n\n\n";
    auto fails = selftest::runUnitTests();

    if ( 5==fails.numFailedTests ) {
        selftest::trace << "\n\n\nTestception completed successfully\n";
        return 0;
    } else {
        std::cerr << "\n\n\nUnit testing of selftest.hpp failed\n";
        return 1;
    }
}
