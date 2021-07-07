# libfs
A header-only, zero-dependency, C++20 library for accessing FreeSurfer neuroimaging file formats.


## Features

* read FreeSurfer per-vertex data from binary curv format files (like `$SUBJECTS_DIR/surf/lh.thickness`).
* read FreeSurfer brain surface meshes from binary surf format files (like `$SUBJECTS_DIR/surf/lh.white`).
* read FreeSurfer 4D volume files (3D voxels + a fourth time/subject dimension) from binary MGH format files (like `$SUBJECTS_DIR/mri/brain.mgh`). 
 
Note that the MGZ format is not supported yet, but you can extract it (it's simply gzipped MGH) or convert it using the FreeSurfer `mri_convert` command line program: `mri_convert file.mgz file.mgh`.


## Running the tests

If you have not cloned yet:

```
git clone https://github.com/dfsp-spirit/libfs
cd libfs/
```

Then build and run the tests:

```
cmake .
make
./LibFsTests
```

## Usage Examples

See the [examples directory](./examples/).


## License

This is free software, published under the [FreeSurfer software license](https://surfer.nmr.mgh.harvard.edu/fswiki/FreeSurferSoftwareLicense) ([copy in this repo](./LICENSE)). 

Please [register at the FreeSurfer website](https://surfer.nmr.mgh.harvard.edu/fswiki/License) to obtain a free license file for FreeSurfer when using this library. This is not required and the file is not used or checked in any way by this library, but it helps the FreeSurfer developers. Most likely you have already done this if you want to use `fslib` in your programs.

Note that this library is not a part of FreeSurfer, and it is in no way endorsed by the FreeSurfer developers.

