
// Demo program that reads per-vertex data from a curv file.
// To compile this witg g++ 9.3:
// 
//    g++ -I../../include/ read_annot.cpp -o read_annot
//
// or with clang 10:
//
//    clang++ -I../../include/ read_annot.cpp -o read_annot
//

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

int main(int argc, char** argv) {
    std::string annot_fname = "lh.aparc.annot";
    if(argc == 2) {
        annot_fname = argv[1];
    }
    std::cout << "Reading input annot file '" << annot_fname << "'.\n";    

    fs::Annot annot;
    fs::read_annot(&annot, annot_fname);

    std::cout << "Read annotation for " << annot.num_vertices() << " surface vertices containing " << annot.colortable.num_entries() << " regions.\n";
    std::cout << "The names of the first 3 regions are: " << annot.colortable.name[0] << ", " << annot.colortable.name[1] << ", " << annot.colortable.name[2] << ".\n";

    std::string region_name;
    region_name = annot.colortable.name[0];
    std::cout << "The region " << region_name << " contains " << annot.region_vertices(region_name).size() << " vertices.\n";
    region_name = annot.colortable.name[1];
    std::cout << "The region " << region_name << " contains " << annot.region_vertices(region_name).size() << " vertices.\n";
    region_name = annot.colortable.name[2];
    std::cout << "The region " << region_name << " contains " << annot.region_vertices(region_name).size() << " vertices.\n";

    std::vector<std::string> vertex_reg_names = annot.vertex_region_names();
    std::cout << "The regions for the first 3 vertices are " << vertex_reg_names[0] << ", " << vertex_reg_names[1] << ", " << vertex_reg_names[2] << ".\n";

    exit(0);
}
