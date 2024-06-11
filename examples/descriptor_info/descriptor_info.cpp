
// Demo program that reads per-vertex data from a curv file.
// To compile this with g++ 9.3:
//
//    g++ -I../../include/ descriptor_info.cpp -o descriptor_info
//
// or with clang 10:
//
//    clang++ -I../../include/ descriptor_info.cpp -o descriptor_info
//

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>


// from Knuth, 1968. The Art of Computer Programming.
bool approximatelyEqual(float a, float b, float epsilon) {
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

// Convert a float vector (with all values equal to either 0.0 or 1.0) to a boolean vector.
std::vector<bool> label_to_bool(std::vector<float> label, float epsilon = 0.001) {
    std::vector<bool> l = std::vector<bool>(label.size(), false);
    int num_concerning = 0;
    for(int i = 0; i < label.size(); i++) {
        if(approximatelyEqual(label[i], 0.0, epsilon)) {
            l[i] = false;
        } else if(approximatelyEqual(label[i], 1.0, epsilon)) {
            l[i] = true;
        }
        else {
            num_concerning++;
        }
    }
    if(num_concerning > 0) {
        throw std::invalid_argument("Encountered " + std::to_string(num_concerning) + " values which were neither 0 nor 1 during label conversion to bool. Not a binary label?\n");
    }
    return l;
}


int main(int argc, char** argv) {
    std::string curv_fname = "lh.thickness";
    if(argc >= 2) {
        curv_fname = argv[1];
    } else {
        std::cout << "[INFO] No command line option supplied for curv file, assuming '" << curv_fname << "'.\n";
    }
    std::cout << "[INFO] Reading input curv file '" << curv_fname << "'.\n";
    std::vector<float> data = fs::read_curv_data(curv_fname);
    std::cout << "[INFO] The input file contains values for " << data.size() << " vertices.\n";

    std::string label_fname = "lh.cortex.label";
    fs::Label label;
    bool has_label = false;
    if(argc >= 3) {
        label_fname = argv[2];
        if (label_fname != "none") {
            fs::read_label(&label, label_fname);
            has_label = true;
        } else {
            std::cout << "[INFO] Not using any label.\n";
        }
    } else {
        std::cout << "[INFO] No command line option supplied for label file, assuming '" << label_fname << "'. Explicitely set it to 'none' if you don't want any.\n";
        fs::read_label(&label, label_fname);
        has_label = true;
    }


    std::vector<bool> is_in = std::vector<bool>(data.size(), true);

    if(has_label) {
        if(data.size() < label.value.size()) {
            throw std::invalid_argument("Mismatch between data size " + std::to_string(data.size()) + " and label size " + std::to_string(label.value.size()) + ".\n");
        }
        is_in = label.vert_in_label(data.size());
        std::cout << "[INFO] Received label data, " + std::to_string(label.value.size()) + " vertices are part of the label.\n";
    }

    std::vector<float> data_filtered;
    if(has_label) {
        std::cout << "[INFO] Filtering descriptor data: using only the values from vertices which are part of the label.\n";
        for(int i = 0; i < data.size(); i++) {
            if(is_in[i]) {
                data_filtered.push_back(data[i]);
            }
        }
    } else {
        std::cout << "[INFO] Not Filtering descriptor data, no label supplied.\n";
        data_filtered = data;
    }

    if(data.size() > 0) {
        float min_entry = *std::min_element(data_filtered.begin(), data_filtered.end());
        float max_entry = *std::max_element(data_filtered.begin(), data_filtered.end());
        std::cout << "Received " << data_filtered.size() << " values in range " << min_entry  << " to " << max_entry << ".\n";

        int num_nan = 0;
        for(int i = 0; i < data_filtered.size(); i++) {
            if(std::isnan(data_filtered[i])) {
                num_nan++;
            }
        }
        std::cout << "Out of " << data_filtered.size() << " values, " << num_nan << " are NaN.\n";

    } else {
        std::cout << "Received empty vector.\n";
    }


    exit(0);
}
