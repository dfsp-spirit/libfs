
// Demo program that reads brain mesh from a surf file.
// To compile this with g++ 9.3:
// 
//    g++ -I../../include/ read_surf.cpp -o read_surf
//

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

int main(int argc, char** argv) {
    std::string surface_fname = "lh.white";
    if(argc == 2) {
        surface_fname = argv[1];
    }
    std::cout << "Reading input surface file '" << surface_fname << "'.\n";
    fs::Mesh surface;
    fs::read_fssurface(&surface, surface_fname);
    std::cout << "Received surface with " << surface.vertices.size() << " vertices and " << surface.faces.size()  << " faces.\n"; 
    exit(0);
}
