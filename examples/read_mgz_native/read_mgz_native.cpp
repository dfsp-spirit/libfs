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

    // ---- Round-trip: write and re-read an existing MGZ ------------------------
    std::string write_filename = "tmp.native.mgz";
    std::cout << "\n===== Round-trip: writing '" << write_filename << "' with write_mgz().\n";
    fs::write_mgz(mgh, write_filename);

    std::cout << "Re-reading written MGZ file to verify round-trip.\n";
    fs::Mgh mgh2;
    fs::read_mgz(&mgh2, write_filename);

    std::cout << "Re-read MGH with size " << mgh2.header.dim1length << "*"
              << mgh2.header.dim2length << "*" << mgh2.header.dim3length << "*"
              << mgh2.header.dim4length << " voxels.\n";
    std::cout << "The data type is " << mgh2.header.dtype
              << " and the length of mgh2.data.data_mri_uchar is "
              << mgh2.data.data_mri_uchar.size() << ".\n";

    fs::Array4D<uint8_t> ar2(&mgh2.header);
    ar2.data = mgh2.data.data_mri_uchar;
    std::cout << "The value at voxel (99,99,99,0) is: "
              << (unsigned int)ar2.at(99,99,99,0) << ".\n";

    if (mgh.data.data_mri_uchar == mgh2.data.data_mri_uchar) {
        std::cout << "Round-trip OK: data is identical.\n";
    } else {
        std::cerr << "Round-trip MISMATCH: data differs after write+read!\n";
        exit(1);
    }

    // ---- Write an MGZ from scratch (synthetic data) --------------------------
    std::cout << "\n===== Writing synthetic MGZ from scratch. =====\n";

    // Create a small 3D float volume (4×3×2 voxels, 1 frame).
    std::vector<float> synth_data = {
        1.0f, 2.0f, 3.0f, 4.0f,   // slice 0, row 0
        5.0f, 6.0f, 7.0f, 8.0f,   // slice 0, row 1
        9.0f,10.0f,11.0f,12.0f,   // slice 0, row 2
       13.0f,14.0f,15.0f,16.0f,   // slice 1, row 0
       17.0f,18.0f,19.0f,20.0f,   // slice 1, row 1
       21.0f,22.0f,23.0f,24.0f    // slice 1, row 2
    };

    fs::Mgh synth;
    synth.header.dim1length = 4;   // x (fastest varying)
    synth.header.dim2length = 3;   // y
    synth.header.dim3length = 2;   // z
    synth.header.dim4length = 1;   // t (frame)
    synth.header.dtype = fs::MRI_FLOAT;
    synth.header.ras_good_flag = 1;
    synth.header.xsize = 1.0f;
    synth.header.ysize = 1.0f;
    synth.header.zsize = 1.0f;
    // Identity Mdc (no rotation) and zero origin.
    synth.header.Mdc = {1.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,0.0f,1.0f};
    synth.header.Pxyz_c = {0.0f, 0.0f, 0.0f};
    synth.data.data_mri_float = synth_data;

    std::string synth_filename = "synth.mgz";
    std::cout << "Writing synthetic " << synth.header.dim1length << "*"
              << synth.header.dim2length << "*" << synth.header.dim3length << "*"
              << synth.header.dim4length << " float volume to '" << synth_filename << "'.\n";
    fs::write_mgz(synth, synth_filename);

    // Re-read and verify.
    fs::Mgh synth2;
    fs::read_mgz(&synth2, synth_filename);
    std::cout << "Re-read synthetic MGZ: " << synth2.header.dim1length << "*"
              << synth2.header.dim2length << "*" << synth2.header.dim3length << "*"
              << synth2.header.dim4length << " voxels, dtype=" << synth2.header.dtype << ".\n";

    if (synth2.data.data_mri_float.size() == synth_data.size()) {
        bool ok = true;
        for (size_t i = 0; i < synth_data.size(); i++) {
            if (synth2.data.data_mri_float[i] != synth_data[i]) { ok = false; break; }
        }
        if (ok) {
            std::cout << "Synthetic round-trip OK: data is identical.\n";
        } else {
            std::cerr << "Synthetic round-trip MISMATCH!\n";
            exit(1);
        }
    } else {
        std::cerr << "Synthetic round-trip size mismatch!\n";
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
