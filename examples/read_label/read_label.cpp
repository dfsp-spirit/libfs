
// Demo program that reads label data from an ASCII label file.
// To compile this with g++ 9.3:
//
//    g++ -I../../include/ read_label.cpp -o read_label
//
// or with clang 10:
//
//    clang++ -I../../include/ read_label.cpp -o read_label
//

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

int main(int argc, char** argv) {
    std::string label_fname = "lh.cortex.label";
    if(argc == 2) {
        label_fname = argv[1];
    }
    std::cout << "Reading input label file '" << label_fname << "'.\n";
    fs::Label label;
    fs::read_label(&label, label_fname);
    float min_entry = *std::min_element(label.value.begin(), label.value.end());
    float max_entry = *std::max_element(label.value.begin(), label.value.end());
    if(label.value.size() > 0) {
        std::cout << "Received " << label.value.size() << " label values in range " << min_entry  << " to " << max_entry << ".\n";
    } else {
        std::cout << "Received empty label value vector.\n";
    }

    // Write to file and re-read
    std::string write_filename = "tmp.lh.cortex.label";
    fs::write_label(label, write_filename);
    fs::Label label2;
    fs::read_label(&label2, write_filename);
    float min_entry2 = *std::min_element(label2.value.begin(), label2.value.end());
    float max_entry2 = *std::max_element(label2.value.begin(), label2.value.end());
    if(label2.value.size() > 0) {
        std::cout << "Received " << label2.value.size() << " label values in range " << min_entry2  << " to " << max_entry2 << ".\n";
    } else {
        std::cout << "Received empty label value vector.\n";
    }

    exit(0);
}
