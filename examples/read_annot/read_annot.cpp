
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
    exit(0);
}
