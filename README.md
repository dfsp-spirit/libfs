# libfs
A header-only, zero-dependency, C++20 library for accessing FreeSurfer neuroimaging file formats.


## Features

* read FreeSurfer per-vertex data from binary curv format files (like `$SUBJECTS_DIR/surf/lh.thickness`).
* read FreeSurfer brain surface meshes from binary surf format files (like `$SUBJECTS_DIR/surf/lh.white`).
* read FreeSurfer 4D volume files (3D voxels + a fourth time/subject dimension) from binary MGH format files (like `$SUBJECTS_DIR/mri/brain.mgh`). Note that MGZ is not supported yet.


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

This is free software, published under the [FreeSurfer software license](https://surfer.nmr.mgh.harvard.edu/fswiki/FreeSurferSoftwareLicense) ([copy in this repo](./LICENSE)). Please [register at the FreeSurfer website to obtain a free license file](https://surfer.nmr.mgh.harvard.edu/fswiki/License) when using this library.

