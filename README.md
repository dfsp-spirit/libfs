# libfs
A header-only, zero-dependency, C++11 library for accessing [FreeSurfer](https://freesurfer.net/) neuroimaging file formats.


## Features

* read and write FreeSurfer per-vertex data from and to binary curv format files (like `$SUBJECTS_DIR/surf/lh.thickness`).
* read FreeSurfer brain surface meshes from binary surf format files (like `$SUBJECTS_DIR/surf/lh.white`).
* read and write FreeSurfer 4D volume files (typically 3D voxels + a fourth time/subject dimension) from binary MGH format files (like `$SUBJECTS_DIR/mri/brain.mgh` or `$SUBJECTS_DIR/surf/lh.thickness.fwhm5.fsaverage.mgh`). 


Supported data types for the MGH format include:
* `MRI_INT`: 32 bit signed int
* `MRI_FLOAT`: 32 bit signed float and
* `MRI_UCHAR`: 8 bit unsigned int.
 
Note that the MGZ format is not supported yet, but you can extract it manually (it's simply gzipped MGH) or convert it using the FreeSurfer `mri_convert` command line program: `mri_convert file.mgz file.mgh`.

## Roadmap

Next TODOs:

* Add support for reading label files, so we can properly mask non-cortical areas (based on files like `$SUBJECTS_DIR/label/lh.cortex.label`).
* Refactor `read_*` functions to have a version of each function that works on a stream, so its easy to support GZIPPED versions.
* Refactoring: use more templates for `fwrite*` and `fread*` functions.

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
./run_libfs_tests
```
Note that the only thing that's being built is the test binary.

## Usage Examples

Just download the file [include/libfs.h](./include/libfs.h) and drop it whereever you like. Make sure your compiler knows about that place. Then use the functions:

```cpp
#include "libfs.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    std::string curv_fname = "lh.thickness";
    std::vector<float> data = fs::read_curv(curv_fname);
    std::cout << "Received " << data.size() << " per-vertex values.\n"; 
    exit(0);
}
```

See the [examples directory](./examples/) for some programs which use the library. The example above is a minimal version of the [read_curv example](./examples/read_curv/read_curv.cpp). 

Compilation instructions for g++ 9.3 and clang are at the top of each example source file, it should be easy to adapt them for your favorite C++ compiler. If you prefer to build with cmake, have a look at the [CMakeLists.txt file](./CMakeLists.txt) we use to build the unit tests.


## Author and Getting help

The `libfs` library was written by [Tim Schäfer](http://rcmd.org/ts).

Note that this library is **not** a part of FreeSurfer, and it is **in no way** endorsed by the FreeSurfer developers. Please do not contact them regarding this library, especially not for support. Open [an issue](https://github.com/dfsp-spirit/libfs/issues) in this repo instead.

