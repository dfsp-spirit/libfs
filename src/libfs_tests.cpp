
#include "libfs.h"
#include "catch.hpp"
#include <vector>

TEST_CASE( "test function" ) {

    SECTION( "twotimes with float input" ) {
        float res = fs::twotimes(5.0);
        REQUIRE( res >= 9.0);
    }

    SECTION("Reading curv file works" ) {
        std::vector<float> data = fs::read_curv("examples/read_curv/lh.thickness");
        REQUIRE( data.size() == 149244);
    }
}
