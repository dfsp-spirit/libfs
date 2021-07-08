
#include "libfs.h"
#include "catch.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>


TEST_CASE( "Reading the demo curv file works" ) {

    std::vector<float> data = fs::read_curv("examples/read_curv/lh.thickness");

    SECTION("The number of values read is correct" ) {        
        REQUIRE( data.size() == 149244);
    }

    SECTION("The range of the values read is correct" ) {    
        float min_entry = *std::min_element(data.begin(), data.end());
        float max_entry = *std::max_element(data.begin(), data.end());    
        REQUIRE(min_entry == Approx(0.0));
        REQUIRE(max_entry == Approx(5.0));
    }

    SECTION("Some values are as expected") {
        REQUIRE(data[0] == Approx(2.561705));
        REQUIRE(data[100] == Approx(2.579938));
        REQUIRE(data[100000] == Approx(0.0));
    }
}


TEST_CASE( "Reading the demo MGH file works" ) {

    fs::Mgh mgh;
    fs::read_mgh(&mgh, "examples/read_mgh/brain.mgh");

    SECTION("The MRI_DTYPE is correct" ) {        
        REQUIRE( mgh.header.dtype == fs::MRI_UCHAR);
    }

    SECTION("The number of values read is correct" ) {        
        REQUIRE( mgh.data.data_mri_uchar.size() == 256*256*256);
    }

    SECTION("The range of the values read is correct" ) {    
        uint8_t min_entry = *std::min_element(mgh.data.data_mri_uchar.begin(), mgh.data.data_mri_uchar.end());
        uint8_t max_entry = *std::max_element(mgh.data.data_mri_uchar.begin(), mgh.data.data_mri_uchar.end());    
        REQUIRE(min_entry == 0);
        REQUIRE(max_entry == 156);
    }

    SECTION("The sum of the values is as expected") {
        int dsum = std::accumulate(mgh.data.data_mri_uchar.begin(), mgh.data.data_mri_uchar.end(), 0);
        REQUIRE(dsum == 121035479);
    }
}


TEST_CASE( "Reading the demo surface file works" ) {

    fs::Mesh surface;
    fs::read_fssurface(&surface, "examples/read_surf/lh.white");

    SECTION("The number of vertices and faces is correct" ) {        
        REQUIRE( surface.vertices.size() == 149244 * 3);
        REQUIRE( surface.faces.size() == 298484 * 3);
    }

    SECTION("The range of vertex indices is correct" ) {
        int min_entry = *std::min_element(surface.vertices.begin(), surface.vertices.end()); // could use minmax for single call
        int max_entry = *std::max_element(surface.vertices.begin(), surface.vertices.end());    
        REQUIRE(min_entry == 0);
        REQUIRE(max_entry == 149243);
    }

}
