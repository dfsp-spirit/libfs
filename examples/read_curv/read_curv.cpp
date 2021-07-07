
// Demo program that reads per-vertex data from a curv file.
// To compile this:
// 
//    g++ -std=c++2a -I../../include/ read_curv.cpp -o read_curv
//
// or:
//
//    clang++ -std=c++2a -I../../include/ read_curv.cpp -o read_curv
//

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

int main(int argc, char** argv) {
    std::string curv_fname = "lh.thickness";
    if(argc == 2) {
        curv_fname = argv[1];
    }
    std::cout << "Reading input curv file '" << curv_fname << "'.\n";
    std::cout << "2 times 5 is " << fs::twotimes(5.0) << ".\n";
    std::cout << "System endianness is: " << (fs::is_bigendian() ? "big" : "little") << ".\n";
    std::vector<float> data = fs::read_curv(curv_fname);
    float min_entry = *std::min_element(data.begin(), data.end());
    float max_entry = *std::max_element(data.begin(), data.end());
    if(data.size() > 0) {
        std::cout << "Received " << data.size() << " values in range " << min_entry  << " to " << max_entry << ".\n"; 
    } else {
        std::cout << "Received empty vector.\n";
    }
    exit(0);
}
