


#include "libfs.h"

#include <string>
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv) {
    std::string curv_fname = "lh.thickness";
    if(argc == 2) {
        curv_fname = argv[1];
    }
    std::cout << "Reading input curv file '" << curv_fname << "'.\n";
    std::cout << "2 times 5 is " << twotimes(5.0) << ".\n";
    exit(0);
}
