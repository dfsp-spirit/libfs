
#include "libfs.h"
#include "catch.hpp"

TEST_CASE( "test function" ) {

    SECTION( "twotimes with float input" ) {
        float res = fs::twotimes(5.0);
        REQUIRE( res >= 9.0);
    }
}
