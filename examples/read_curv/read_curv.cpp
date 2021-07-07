
// Demo program that reads per-vertex data from a curv file.
// To compile this:
// 
//    g++ -std=c++2a -I../../include/ read_curv.cpp -o read_curv
//

#include "libfs.h"

#include <string>
#include <iostream>


int main(int argc, char** argv) {
    std::string curv_fname = "lh.thickness";
    if(argc == 2) {
        curv_fname = argv[1];
    }
    std::cout << "Reading input curv file '" << curv_fname << "'.\n";
    std::cout << "2 times 5 is " << fs::twotimes(5.0) << ".\n";
    std::cout << "System endianness is: " << (fs::is_bigendian() ? "big" : "little") << ".\n";
    exit(0);
}
