
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
#include <numeric>

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


template <typename T>
T median(std::vector<T> scores) {
  size_t size = scores.size();

  if (size == 0) {
    throw std::invalid_argument("The input vector must not be empty.");
  }
  else {
    std::sort(scores.begin(), scores.end());
    if (size % 2 == 0) {
      return (scores[size / 2 - 1] + scores[size / 2]) / 2;
    }
    else {
      return scores[size / 2];
    }
  }
}


int main(int argc, char** argv) {
    std::string curv_fname = "lh.thickness";
    if(argc >= 2) {
        curv_fname = argv[1];
    } else {
        std::cout << "[INFO] No command line option supplied for curv/MGH file, assuming '" << curv_fname << "'.\n";
    }
    std::cout << "[INFO] Reading input file '" << curv_fname << "' (must be in curv or MGH format, MGH filenames must end with '.mgh').\n";
    std::vector<float> data = fs::read_desc_data(curv_fname);
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
        std::cout << "[INFO] The label file '" << label_fname << "' contains " << data_filtered.size() << " of the " << data.size() << " mesh vertices (" << float(data_filtered.size()) / data.size() * 100.0 << " percent).\n";
    } else {
        std::cout << "[INFO] Not Filtering descriptor data, no label supplied.\n";
        data_filtered = data;
    }

    if(data.size() > 0) {

        int num_nan_full = 0;
        for(int i = 0; i < data.size(); i++) {
            if(std::isnan(data[i])) {
                num_nan_full++;
            }
        }

        std::vector<float> data_filtered_nonan = std::vector<float>();
        for(int i = 0; i < data_filtered.size(); i++) {
            if(! std::isnan(data_filtered[i])) {
                data_filtered_nonan.push_back(data_filtered[i]);
            }
        }
        int num_nan_filtered = data_filtered.size() - data_filtered_nonan.size();
        if(num_nan_filtered > 0) {
            if(data_filtered_nonan.size() == 0) {
                std::cout << "[INFO] All values are NaN, exiting.\n";
                exit(1);
            }
            std::cout << "[INFO] Continuing stats computation with the " << data_filtered_nonan.size() << " values which are not NaN.\n";
        }


        std::cout << "Number of values total in descriptor file: " << data.size() << "\n";
        std::cout << "Number of NaN in full descriptor data: " << num_nan_full << "\n";
        std::cout << "Used label for filtering values: " << (has_label ? "yes" : "no")  << "\n";
        std::cout << "Number of values after filtering by label: " << data_filtered.size() << "\n";
        std::cout << "Number of NaN in data filtered by label: " << num_nan_filtered << "\n";
        std::cout << "Number of non-NaN values used for stats computation: " << data_filtered_nonan.size() << "\n";

        float min_entry = *std::min_element(data_filtered_nonan.begin(), data_filtered_nonan.end());
        float max_entry = *std::max_element(data_filtered_nonan.begin(), data_filtered_nonan.end());
        std::cout << "The minimum is: " << min_entry << "\n";
        std::cout << "The maximum is: " << max_entry << "\n";

        double sum = std::accumulate(data_filtered_nonan.begin(), data_filtered_nonan.end(), 0.0);
        double mean = sum / data_filtered_nonan.size();
        std::cout << "The sum is: " << sum << "\n";
        std::cout << "The mean value is: " << mean << "\n";

        std::vector<double> diff(data_filtered_nonan.size());
        std::transform(data_filtered_nonan.begin(), data_filtered_nonan.end(), diff.begin(), std::bind2nd(std::minus<double>(), mean));
        double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        double stdev = std::sqrt(sq_sum / data_filtered_nonan.size());
        std::cout << "The standard deviation is: " << stdev << "\n";

        float m = median<float>(data_filtered_nonan);
        std::cout << "The median is: " << m << "\n";

    } else {
        std::cout << "Received empty vector.\n";
        exit(1);
    }


    exit(0);
}
