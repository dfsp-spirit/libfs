# libfs
A portable, header-only, single file, no-dependency, mildly templated, C++11 library for accessing [FreeSurfer](https://freesurfer.net/) neuroimaging file formats.


## Features

* read and write FreeSurfer per-vertex data from and to binary curv format files (like `$SUBJECTS_DIR/surf/lh.thickness`).
* read FreeSurfer brain surface meshes from binary surf format files (like `$SUBJECTS_DIR/surf/lh.white`).
  - can also import triangular meshes from the following standard mesh file formats: Wavefront object format, Stanford PLY format (ascii version).
  - can export meshes to the following standard mesh file formats: Wavefront object, Standord PLY (ascii version).
* read FreeSurfer brain surface parcellations, i.e., the result of applying a brain atlas, from binary annot format files (like `$SUBJECTS_DIR/label/lh.aparc.annot`).
* read and write FreeSurfer ASCII label files (like `$SUBJECTS_DIR/label/lh.cortex.label`).
* read and write FreeSurfer 4D volume files (typically 3D voxels + a fourth time/subject dimension) from binary MGH format files (like `$SUBJECTS_DIR/mri/brain.mgh` or `$SUBJECTS_DIR/surf/lh.thickness.fwhm5.fsaverage.mgh`).

Supported data types for the MGH format include:
* `MRI_INT`: 32 bit signed int
* `MRI_FLOAT`: 32 bit signed float and
* `MRI_UCHAR`: 8 bit unsigned int.
 

#### A note on the MGZ format

The MGZ format is just a gzipped version of the MGH format. While the MGZ format is not supported directly by `libfs`, you have two options to read and write MGZ files:

* You can use `zlib` and the [zstr](https://github.com/mateidavid/zstr/) header-only C++ library (a stream wrapper around `zlib`) in combination with `libfs` to read MGZ files. It's easy and a complete example program that does it can be found in [examples/read_mgz/](./examples/read_mgz/). The program also contains an example for writing an MGZ file. While `zlib` itself is not header-only, it should be available *everywhere anyways*, so it should not drag you into dependency hell.
* You can extract the MGZ files manually on the command line before running your program or convert them using the FreeSurfer `mri_convert` command line program: `mri_convert file.mgz file.mgh`.


#### What `libfs` is **not**

 This library was written from scratch in C++. It is **not** based on the FreeSurfer C code and does not use the same data structures that are used in FreeSurfer.

## Usage 

### Examples

Just download the file [include/libfs.h](./include/libfs.h) and drop it whereever you like. Make sure your compiler knows about that place. Then use the functions:

```cpp
// filename: main.cpp. To compile with g++ run: 
//     g++ -I<path_to_directory_containing_libfs.h> main.cpp -o read_curv_data
#include "libfs.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    std::string curv_fname = "lh.thickness";
    std::vector<float> data = fs::read_curv_data(curv_fname);
    std::cout << "Received " << data.size() << " per-vertex values.\n"; 
    exit(0);
}
```

See the [examples directory](./examples/) for some programs which use the library. The example above is a minimal version of the [read_curv example](./examples/read_curv/read_curv.cpp). 


### Building your programs

Just add the directory into which you saved `libfs.h` to the include path during compilation. This is done by adding an `-I<path>` flag for most compilers, check the manual of your compiler if in doubt.

Compilation instructions for g++ and clang are at the top of each example source file, it should be easy to adapt them for your favorite C++ compiler. If you prefer to build with cmake, have a look at the [CMakeLists.txt file](./CMakeLists.txt) we use to build the unit tests.


### Full API documentation

The API docs can be browsed online at [codedocs.xyz/dfsp-spirit/libfs/](https://codedocs.xyz/dfsp-spirit/libfs/).


### Conventions

* Everything from `libfs` is in the `fs::` namespace.
* Internal functions in `libfs` are prefixed with an underscore, e.g., `_do_internal_stuff`. You should never call these functions from client code, as they may change without notice between versions. Relying on them is an application bug. (Please [open an issue](https://github.com/dfsp-spirit/libfs/issues) if you feel that you need to use an internal function from your client code, and explain your use case.)
* Function naming:
  - Functions that read data are called `read_*`, e.g., `read_curv` and `read_mgh`.
  - Functions that write data to files are called `write_*`, e.g., `write_curv` and `write_mgh`.
* Most `read_*`/`write_*` functions are overloaded and accept either a `const std::string& filename` argument or a `std::istream *is`/`std::ostream *os` as a source/sink. This allows you to pass streams and bring your own gunzip (see [examples/read_mgz/](./examples/read_mgz/)).


## Development


### Running the tests

You need git, cmake and some C++ compiler. Under Debian-based Linux distributions `sudo apt-get install build-essential cmake git` should do it.

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


### Running all examples

In the repo root, just run `./examples/run_all_examples.bash` from your system shell.

Note that the `read_mgz` example requires `zlib`.


### Building the documentation locally

If you have `doxygen` installed (`sudo apt install doxygen graphviz` under Debian-based Linux distros), you can generate the full API documentation like this:

```
cmake .
make doc
```

The documentation will be built and can be found in `doc_built/` afterwards. The recommended way to browse it is to open `doc_built/html/index.html` with your favorite webbrowser.


## Author and Getting help

The `libfs` library was written by [Tim Schäfer](http://rcmd.org/ts).

Note that this library is **not** a part of FreeSurfer, and it is **in no way** endorsed by the FreeSurfer developers. Please do not contact them regarding this library, especially not for support. [Open an issue](https://github.com/dfsp-spirit/libfs/issues) in this repo instead.

