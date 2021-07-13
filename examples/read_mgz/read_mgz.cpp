
// Demo program that reads a FreeSurfer 4D volume from an MGZ file.
// Requires zlib and the stream-based wrapper 'zstr' from https://github.com/mateidavid/zstr/.
// 
// To compile this with g++:
// 
//    g++ -I../../include/ -I./include_zstr/ read_mgz.cpp -o read_mgz -lz
//

#include "libfs.h"
#include "zstr.hpp" // This is from https://github.com/mateidavid/zstr/ and contained in ./include_zstr/.

#include <string>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    std::string mgz_fname = "brain.mgz";
    if(argc == 2) {
        mgz_fname = argv[1];
    }
    std::cout << "Reading input MGZ file '" << mgz_fname << "'.\n";
    fs::Mgh mgh;

    // Create zstr wrapper around file input and get inner istream.
    std::unique_ptr< std::istream > ifs_p = std::unique_ptr< std::istream >(new zstr::ifstream(mgz_fname));
    std::istream * is_p = ifs_p.get();

    fs::read_mgh(&mgh, is_p);   // Use stream version of overloaded function fs::read_mgh.
    std::cout << "Received MGH with size " << mgh.header.dim1length << "*" << mgh.header.dim2length <<  "*" <<  mgh.header.dim3length << "*" << mgh.header.dim4length << " voxels.\n"; 
    std::cout << "The data type is " << mgh.header.dtype << " and the length of mgh.data.data_mri_uchar is " << mgh.data.data_mri_uchar.size() << ".\n";
    std::cout << "The RAS part of the header is valid: " << (mgh.header.ras_good_flag ? "yes" : "no" ) << ".\n";

    // Optional: Put the data into an Array4D for more convenient access to the voxel indices.
    fs::Array4D<uint8_t> ar(&mgh.header);
    ar.data = mgh.data.data_mri_uchar;
    std::cout << "The value at voxel (99,99,99,0) is: " << (unsigned int)ar.at(99,99,99,0) << ".\n";

    exit(0);
}
