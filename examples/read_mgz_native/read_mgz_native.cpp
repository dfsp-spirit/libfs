// Demo program that reads and writes FreeSurfer MGZ files using the native
// read_mgz() / write_mgz() functions (available when zlib is present).
//
// To compile:
//    g++ -std=c++11 -I../../include/ read_mgz_native.cpp -o read_mgz_native -lz
//

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    std::string mgz_fname = "../read_mgz/brain.mgz";
    if(argc == 2) {
        mgz_fname = argv[1];
    }
    std::cout << "Reading input MGZ file '" << mgz_fname << "' with read_mgz().\n";

#ifdef LIBFS_HAS_ZLIB
    fs::Mgh mgh;
    fs::read_mgz(&mgh, mgz_fname);

    std::cout << "Received MGH with size " << mgh.header.dim1length << "*"
              << mgh.header.dim2length << "*" << mgh.header.dim3length << "*"
              << mgh.header.dim4length << " voxels.\n";
    std::cout << "The data type is " << mgh.header.dtype
              << " and the length of mgh.data.data_mri_uchar is "
              << mgh.data.data_mri_uchar.size() << ".\n";
    std::cout << "The RAS part of the header is valid: "
              << (mgh.header.ras_good_flag ? "yes" : "no") << ".\n";

    // Access data via Array4D for convenient 4D indexing.
    fs::Array4D<uint8_t> ar(&mgh.header);
    ar.data = mgh.data.data_mri_uchar;
    std::cout << "The value at voxel (99,99,99,0) is: "
              << (unsigned int)ar.at(99,99,99,0) << ".\n";

    // Write it back as a new MGZ file using write_mgz().
    std::string write_filename = "tmp.native.mgz";
    std::cout << "Writing MGZ file '" << write_filename << "' with write_mgz().\n";
    fs::write_mgz(mgh, write_filename);

    // Re-read the file we just wrote and verify it.
    std::cout << "Re-reading written MGZ file to verify round-trip.\n";
    fs::Mgh mgh2;
    fs::read_mgz(&mgh2, write_filename);

    std::cout << "Re-read MGH with size " << mgh2.header.dim1length << "*"
              << mgh2.header.dim2length << "*" << mgh2.header.dim3length << "*"
              << mgh2.header.dim4length << " voxels.\n";
    std::cout << "The data type is " << mgh2.header.dtype
              << " and the length of mgh.data.data_mri_uchar is "
              << mgh2.data.data_mri_uchar.size() << ".\n";

    fs::Array4D<uint8_t> ar2(&mgh2.header);
    ar2.data = mgh2.data.data_mri_uchar;
    std::cout << "The value at voxel (99,99,99,0) is: "
              << (unsigned int)ar2.at(99,99,99,0) << ".\n";

    // Quick sanity check: data should be identical after round-trip.
    if (mgh.data.data_mri_uchar == mgh2.data.data_mri_uchar) {
        std::cout << "Round-trip OK: data is identical.\n";
    } else {
        std::cerr << "Round-trip MISMATCH: data differs after write+read!\n";
        exit(1);
    }
#else
    std::cerr << "MGZ support is not available: zlib was not detected at compile time.\n"
              << "Link with -lz and, if using an older compiler, #define LIBFS_HAS_ZLIB\n"
              << "before including libfs.h.\n";
    exit(1);
#endif

    exit(0);
}
