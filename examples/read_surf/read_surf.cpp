
// Demo program that reads brain mesh from a surf file.
// To compile this with g++ 9.3:
// 
//    g++ -I../../include/ read_surf.cpp -o read_surf
//

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

int main(int argc, char** argv) {
    std::string surface_fname = "lh.white";
    if(argc == 2) {
        surface_fname = argv[1];
    }
    std::cout << "Reading input surface file '" << surface_fname << "'.\n";
    fs::Mesh surface;
    fs::read_surf(&surface, surface_fname);
    std::cout << "Received surface with " << surface.num_vertices() << " vertices and " << surface.num_faces()  << " faces.\n";

    // Write the mesh to files in OBJ and PLY formats.
    std::ofstream outobj("lh_white.obj");
    if(outobj.is_open()) {
        outobj << surface.to_obj();
        outobj.close();
    } else {
        std::cerr << "Failed to open OBJ output file.\n";
    }
    std::ofstream outply("lh_white.ply");
    if(outply.is_open()) {
        outply << surface.to_ply();
        outply.close();
    } else {
        std::cerr << "Failed to open PLY output file.\n";
    }

    exit(0);
}
