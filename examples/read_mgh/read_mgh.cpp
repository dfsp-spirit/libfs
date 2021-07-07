
// Demo program that reads a FreeSurfer 4D volume from an MGH file.
// To compile this:
// 
//    g++ -std=c++2a -I../../include/ read_mgh.cpp -o read_mgh
//

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    std::string mgh_fname = "brain.mgh";
    if(argc == 2) {
        mgh_fname = argv[1];
    }
    std::cout << "Reading input MGH file '" << mgh_fname << "'.\n";
    fs::Mgh mgh;
    fs::read_mgh(&mgh, mgh_fname);
    std::cout << "Received MGH with size " << mgh.header.dim1length << "*" << mgh.header.dim2length <<  "*" <<  mgh.header.dim3length << "*" << mgh.header.dim4length << " voxels.\n"; 
    std::cout << "The data type is " << mgh.header.dtype << " and the length of mgh.data.data_mri_uchar is " << mgh.data.data_mri_uchar.size() << ".\n";
    exit(0);
}
