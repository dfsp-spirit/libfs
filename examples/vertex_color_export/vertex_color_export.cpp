// Demo program that reads a brain mesh, per-vertex data (cortical thickness),
// and a cortex label, then maps the thickness data to vertex colors using the
// viridis colormap and exports the colored mesh to a PLY file.
//
// The cortex label is used to mask out medial wall vertices: their thickness
// value is set to NaN before applying the colormap, and the viridis function
// maps NaN values to white (the standard convention in neuroimaging) by default.
//
// To compile this with g++ 9.3:
//
//    g++ -I../../include/ vertex_color_export.cpp -o vertex_color_export
//
// or with clang 10:
//
//    clang++ -I../../include/ vertex_color_export.cpp -o vertex_color_export
//
// Then run it from this directory:
//
//    ./vertex_color_export
//
// This will read the following input files (and you can override all 3 via
// command line arguments):
//    1) a brain mesh in FreeSurfer surf format.  Default: '../read_surf/lh.white'
//    2) per-vertex data for that mesh in curv format. Default: '../read_curv/lh.thickness'
//    3) a cortex label file, a binary map over the vertices. Default: '../read_label/lh.cortex.label'
//
// It then writes the output file 'lh.white.thickness.colored.ply' to the
// current directory, which you can open in Meshlab or other viewers to inspect
// the vertex colors. Note that in Blender, you will have to manually configure
// the software to display vertex colors, simply opening the mesh does NOT
// display them.

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>

int main(int argc, char** argv) {
    std::string surface_fname = "../read_surf/lh.white";
    std::string curv_fname = "../read_curv/lh.thickness";
    std::string label_fname = "../read_label/lh.cortex.label";
    std::string output_fname = "lh.white.thickness.colored.ply";

    if (argc == 4) {
        surface_fname = argv[1];
        curv_fname = argv[2];
        label_fname = argv[3];
    } else if (argc != 1) {
        std::cout << "===" << argv[0] << " -- export a brain mesh with per-vertex colors to PLY ===\n";
        std::cout << "Usage: " << argv[0] << " [surface_file curv_file label_file]\n";
        std::cout << "   surface_file : str, path to a FreeSurfer surf file.     Default: '../read_surf/lh.white'\n";
        std::cout << "   curv_file    : str, path to a FreeSurfer curv file.     Default: '../read_curv/lh.thickness'\n";
        std::cout << "   label_file   : str, path to a FreeSurfer label file.    Default: '../read_label/lh.cortex.label'\n";
        std::cout << "Note: call with no arguments to use the defaults listed above.\n";
        std::cout << "The output PLY file is written to '" << output_fname << "' in the current directory.\n";
        exit(1);
    }

    // ---- Step a) Read the left hemisphere brain mesh from a surf file. ----
    std::cout << "Reading input surface file '" << surface_fname << "'.\n";
    if (!fs::util::file_exists(surface_fname)) {
        std::cerr << "Surface file '" << surface_fname << "' not found. Run from the 'examples/vertex_color_export' directory or pass file paths as arguments.\n";
        exit(1);
    }
    fs::Mesh surface;
    fs::read_surf(&surface, surface_fname);
    size_t num_vertices = surface.num_vertices();
    std::cout << "Received surface with " << num_vertices << " vertices and " << surface.num_faces() << " faces.\n";

    // ---- Step b) Read the per-vertex cortical thickness data from a curv file. ----
    std::cout << "Reading input curv file '" << curv_fname << "'.\n";
    if (!fs::util::file_exists(curv_fname)) {
        std::cerr << "Curv file '" << curv_fname << "' not found.\n";
        exit(1);
    }
    std::vector<float> thickness = fs::read_curv_data(curv_fname);
    if (thickness.size() != num_vertices) {
        std::cerr << "Curv file contains " << thickness.size() << " values, but the surface has " << num_vertices << " vertices. They must match.\n";
        exit(1);
    }
    std::vector<float> finite_thickness;
    for (size_t i = 0; i < thickness.size(); i++) {
        if (!std::isnan(thickness[i])) {
            finite_thickness.push_back(thickness[i]);
        }
    }
    if (!finite_thickness.empty()) {
        float min_entry = *std::min_element(finite_thickness.begin(), finite_thickness.end());
        float max_entry = *std::max_element(finite_thickness.begin(), finite_thickness.end());
        std::cout << "Received " << thickness.size() << " thickness values in range " << min_entry << " to " << max_entry << " (ignoring NaN).\n";
    } else {
        std::cout << "Received " << thickness.size() << " thickness values, all NaN.\n";
    }

    // ---- Step c) Read the lh.cortex.label file, a binary map over the vertices. ----
    std::cout << "Reading input label file '" << label_fname << "'.\n";
    if (!fs::util::file_exists(label_fname)) {
        std::cerr << "Label file '" << label_fname << "' not found.\n";
        exit(1);
    }
    fs::Label cortex_label;
    fs::read_label(&cortex_label, label_fname);
    std::cout << "Received cortex label with " << cortex_label.num_entries() << " entries (vertices marked as cortex).\n";

    // Compute a boolean mask: which vertices are inside the cortex label?
    std::vector<bool> in_cortex = cortex_label.vert_in_label(num_vertices);
    size_t num_in_cortex = 0;
    size_t num_outside_cortex = 0;
    for (size_t i = 0; i < in_cortex.size(); i++) {
        if (in_cortex[i]) {
            num_in_cortex++;
        } else {
            num_outside_cortex++;
        }
    }
    std::cout << "Cortex label contains " << num_in_cortex << " of " << num_vertices << " vertices; " << num_outside_cortex << " vertices are outside the cortex (medial wall).\n";

    // ---- Step d) Set thickness to NaN for vertices which are not part of the cortex label (the medial wall vertices). ----
    for (size_t i = 0; i < num_vertices; i++) {
        if (!in_cortex[i]) {
            thickness[i] = NAN;
        }
    }
    std::cout << "Set thickness to NaN for the " << num_outside_cortex << " non-cortex (medial wall) vertices. These will be drawn in white.\n";

    // ---- Step e) Map the per-vertex thickness data to RGB colors using the viridis colormap. ----
    // NaN input values are mapped to white (255, 255, 255) by default, which is the
    // standard convention in neuroimaging for missing data (e.g. the medial wall).
    // The data range is auto-normalized to the finite min/max of the input.
    std::cout << "Mapping thickness data to RGB colors using the viridis colormap (NaN -> white).\n";
    std::vector<uint8_t> colors = fs::util::viridis(thickness);
    std::cout << "Generated " << colors.size() << " color values (" << colors.size() / 3 << " vertices * 3 channels).\n";

    // ---- Step f) Export the mesh with per-vertex colors to a PLY file. ----
    std::cout << "Writing colored mesh to PLY file '" << output_fname << "'.\n";
    surface.to_ply_file(output_fname, colors);
    std::cout << "Done. Open '" << output_fname << "' in Meshlab (or another PLY viewer that supports vertex colors) to inspect it.\n";

    exit(0);
}