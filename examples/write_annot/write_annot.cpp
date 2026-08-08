// Demo program that reads a FreeSurfer annotation (brain parcellation), writes
// it back to a new file, and re-reads it to verify the round-trip.
//
// To compile:
//    g++ -std=c++11 -I../../include/ write_annot.cpp -o write_annot
//

#include "libfs.h"

#include <string>
#include <iostream>

int main(int argc, char** argv) {
    std::string annot_fname = "../read_annot/lh.aparc.annot";
    std::string out_fname = "tmp.aparc.annot";
    if(argc >= 2) {
        annot_fname = argv[1];
    }
    if(argc >= 3) {
        out_fname = argv[2];
    }

    std::cout << "Reading annot file '" << annot_fname << "'.\n";
    fs::Annot annot;
    fs::read_annot(&annot, annot_fname);
    std::cout << "Read annotation for " << annot.num_vertices() << " vertices with "
              << annot.colortable.num_entries() << " regions.\n";

    std::cout << "Writing annot to '" << out_fname << "'.\n";
    fs::write_annot(annot, out_fname);

    std::cout << "Re-reading written file to verify round-trip.\n";
    fs::Annot annot2;
    fs::read_annot(&annot2, out_fname);
    std::cout << "Re-read annotation for " << annot2.num_vertices() << " vertices with "
              << annot2.colortable.num_entries() << " regions.\n";

    // Verify.
    if (annot.vertex_indices != annot2.vertex_indices ||
        annot.vertex_labels  != annot2.vertex_labels) {
        std::cerr << "Round-trip FAILED: vertex data mismatch.\n";
        exit(1);
    }
    if (annot.colortable.num_entries() != annot2.colortable.num_entries()) {
        std::cerr << "Round-trip FAILED: colortable entry count mismatch.\n";
        exit(1);
    }
    for (size_t i = 0; i < annot.colortable.num_entries(); i++) {
        if (annot.colortable.name[i]  != annot2.colortable.name[i] ||
            annot.colortable.r[i]     != annot2.colortable.r[i] ||
            annot.colortable.g[i]     != annot2.colortable.g[i] ||
            annot.colortable.b[i]     != annot2.colortable.b[i] ||
            annot.colortable.a[i]     != annot2.colortable.a[i] ||
            annot.colortable.label[i] != annot2.colortable.label[i]) {
            std::cerr << "Round-trip FAILED: colortable entry " << i << " mismatch.\n";
            exit(1);
        }
    }
    std::cout << "Round-trip OK: annotation data is identical.\n";
    exit(0);
}
