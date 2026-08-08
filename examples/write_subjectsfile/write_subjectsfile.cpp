// Demo program that creates and writes a FreeSurfer subjects file,
// then reads it back to verify.
//
// To compile:
//    g++ -std=c++11 -I../../include/ write_subjectsfile.cpp -o write_subjectsfile
//

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    std::string out_fname = "tmp.subjects.txt";
    if(argc >= 2) {
        out_fname = argv[1];
    }

    std::vector<std::string> subjects = {
        "subject1",
        "subject2",
        "subject3_with_longer_name"
    };

    std::cout << "Writing " << subjects.size() << " subjects to '" << out_fname << "'.\n";
    fs::write_subjectsfile(out_fname, subjects);

    std::cout << "Reading subjects file back.\n";
    std::vector<std::string> subjects2 = fs::read_subjectsfile(out_fname);
    std::cout << "Read " << subjects2.size() << " subjects:\n";
    for (size_t i = 0; i < subjects2.size(); i++) {
        std::cout << "  [" << i << "] " << subjects2[i] << "\n";
    }

    if (subjects != subjects2) {
        std::cerr << "Round-trip FAILED: subjects data mismatch.\n";
        exit(1);
    }
    std::cout << "Round-trip OK: subjects data is identical.\n";
    exit(0);
}
