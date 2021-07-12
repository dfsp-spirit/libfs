
// Demo program that reads a FreeSurfer 4D volume from an MGH file.
// To compile this with g++ 9.3:
// 
//    g++ -I../../include/ read_mgh.cpp -o read_mgh
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
    std::cout << "The RAS part of the header is valid: " << (mgh.header.ras_good_flag ? "yes" : "no" ) << ".\n";

    // Optional: Put the data into an Array4D for more convenient access to the voxel indices.
    fs::Array4D<uint8_t> ar(&mgh.header);
    ar.data = mgh.data.data_mri_uchar;
    std::cout << "The value at voxel (99,99,99,0) is: " << (unsigned int)ar.at(99,99,99,0) << ".\n";


    std::cout << "=== Writing and re-reading ===.\n";

    // Write and re-read
    std::string write_filename = "tmp.brain.mgh";
    fs::write_mgh(mgh, write_filename);
    fs::Mgh mgh2;
    fs::read_mgh(&mgh2, write_filename);
    std::cout << "Received MGH with size " << mgh2.header.dim1length << "*" << mgh2.header.dim2length <<  "*" <<  mgh2.header.dim3length << "*" << mgh2.header.dim4length << " voxels.\n"; 
    std::cout << "The data type is " << mgh2.header.dtype << " and the length of mgh.data.data_mri_uchar is " << mgh2.data.data_mri_uchar.size() << ".\n";
    std::cout << "The RAS part of the header is valid: " << (mgh2.header.ras_good_flag ? "yes" : "no" ) << ".\n";
    // Optional: Put the data into an Array4D for more convenient access to the voxel indices.
    fs::Array4D<uint8_t> ar2(&mgh2.header);
    ar2.data = mgh2.data.data_mri_uchar;
    std::cout << "The value at voxel (99,99,99,0) is: " << (unsigned int)ar2.at(99,99,99,0) << ".\n";


    std::cout << "=== Reading original file stream-based. ===.\n";

    // Read original MGH file stream-based
    std::ifstream is;
    is.open(mgh_fname, std::ios_base::in | std::ios::binary);
    fs::Mgh mgh3;
    fs::read_mgh(&mgh3, &is);
    std::cout << "Received MGH with size " << mgh3.header.dim1length << "*" << mgh3.header.dim2length <<  "*" <<  mgh3.header.dim3length << "*" << mgh3.header.dim4length << " voxels.\n"; 
    std::cout << "The data type is " << mgh3.header.dtype << " and the length of mgh.data.data_mri_uchar is " << mgh3.data.data_mri_uchar.size() << ".\n";
    std::cout << "The RAS part of the header is valid: " << (mgh3.header.ras_good_flag ? "yes" : "no" ) << ".\n";
    // Optional: Put the data into an Array4D for more convenient access to the voxel indices.
    fs::Array4D<uint8_t> ar3(&mgh3.header);
    ar3.data = mgh3.data.data_mri_uchar;
    std::cout << "The value at voxel (99,99,99,0) is: " << (unsigned int)ar3.at(99,99,99,0) << ".\n";

    exit(0);
}
