
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

    // Print info on Colortable
    std::cout << "The names of the first 3 regions are: " << annot.colortable.name[0] << ", " << annot.colortable.name[1] << ", " << annot.colortable.name[2] << ".\n";
    std::cout << "The lengths of their region name strings are: " << annot.colortable.name[0].length() << ", " << annot.colortable.name[1].length() << ", " << annot.colortable.name[2].length() << ".\n";
    std::cout << "Their region label integers are: " << annot.colortable.label[0] << ", " << annot.colortable.label[1] << ", " << annot.colortable.label[2] << ".\n";    
    std::cout << "Their region color channel R values are: " << annot.colortable.r[0] << ", " << annot.colortable.r[1] << ", " << annot.colortable.r[2] << ".\n";
    std::cout << "Their region color channel G values are: " << annot.colortable.g[0] << ", " << annot.colortable.g[1] << ", " << annot.colortable.g[2] << ".\n";
    std::cout << "Their region color channel B values are: " << annot.colortable.b[0] << ", " << annot.colortable.b[1] << ", " << annot.colortable.b[2] << ".\n";
    std::cout << "Their region color channel A values are: " << annot.colortable.a[0] << ", " << annot.colortable.a[1] << ", " << annot.colortable.a[2] << ".\n";
    
    std::string region_name = "bankssts";
    std::cout << "Region " << region_name << " has index " << annot.colortable.get_region_idx(region_name) << " in the Colortable.\n";
    region_name = "caudalanteriorcingulate";
    std::cout << "Region " << region_name << " has index " << annot.colortable.get_region_idx(region_name) << " in the Colortable.\n";
    
    // Print information on the parcellation.
    std::cout << "The first 3 vertex indices in the parcellation are: " << annot.vertex_indices[0] << ", " << annot.vertex_indices[1] << ", "<< annot.vertex_indices[2] << ".\n";
    std::cout << "Their labels are: " << annot.vertex_labels[0] << ", " << annot.vertex_labels[1] << ", "<< annot.vertex_labels[2] << ".\n";

    region_name = annot.colortable.name[0];
    std::cout << "The region " << region_name << " contains " << annot.region_vertices(region_name).size() << " vertices.\n";
    region_name = annot.colortable.name[1];
    std::cout << "The region " << region_name << " contains " << annot.region_vertices(region_name).size() << " vertices.\n";
    region_name = annot.colortable.name[2];
    std::cout << "The region " << region_name << " contains " << annot.region_vertices(region_name).size() << " vertices.\n";

    std::vector<std::string> vertex_reg_names = annot.vertex_region_names();
    std::cout << "The regions for the first 3 vertices are " << vertex_reg_names[0] << ", " << vertex_reg_names[1] << ", " << vertex_reg_names[2] << ".\n";

    // Export a vertex-color version of a brain mesh in PLY format. You can open it in Meshlab and inspect the colors.
    // Note that in Blender, you will have to manually configure the software to display vertex colors. Simply opening the mesh does NOT display them.
    std::string surface_fname = "../read_surf/lh.white";
    fs::Mesh surface;
    fs::read_surf(&surface, surface_fname);
    surface.to_ply_file("aparc_brain.ply", annot.vertex_colors());

    exit(0);
}
