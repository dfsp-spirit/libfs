# libfs
A portable, header-only, single file, no-dependency, mildly templated, C++11 library for accessing [FreeSurfer](https://freesurfer.net/) neuroimaging file formats.


[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.8090828.svg)](https://doi.org/10.5281/zenodo.8090828)
![main](https://github.com/dfsp-spirit/libfs/actions/workflows/unittests.yml/badge.svg?branch=main)


## Features

* read and write FreeSurfer per-vertex data from and to binary curv format files (like `$SUBJECTS_DIR/surf/lh.thickness`).
* read and write FreeSurfer brain surface meshes from binary surf format files (like `$SUBJECTS_DIR/surf/lh.white`).
  - can also import triangular meshes from the following standard mesh file formats:
    * Wavefront object format (.obj)
    * Stanford PLY format (.ply, ascii version)
    * Object File Format (.off, both the plain version and the COFF variant including per-vertex colors are supported).
  - can export meshes to the following standard mesh file formats: Wavefront object, Stanford PLY (ascii version).
* read FreeSurfer brain surface parcellations, i.e., the result of applying a brain atlas, from binary annot format files (like `$SUBJECTS_DIR/label/lh.aparc.annot`).
* read and write FreeSurfer ASCII label files (like `$SUBJECTS_DIR/label/lh.cortex.label`).
* read and write FreeSurfer 4D volume files (typically 3D voxels + a fourth time/subject dimension) from binary MGH format files (like `$SUBJECTS_DIR/mri/brain.mgh` or `$SUBJECTS_DIR/surf/lh.thickness.fwhm5.fsaverage.mgh`).

Supported data types for the MGH format include:
* `MRI_INT`: 32 bit signed int
* `MRI_FLOAT`: 32 bit signed float and
* `MRI_UCHAR`: 8 bit unsigned int.
* `MRI_SHORT`: 16 bit signed int.


#### A note on the MGZ format

The MGZ format is just a gzipped version of the MGH format. While the MGZ format is not supported directly by `libfs`, you have two options to read and write MGZ files:

* You can use `zlib` and the [zstr](https://github.com/mateidavid/zstr/) header-only C++ library (a stream wrapper around `zlib`) in combination with `libfs` to read MGZ files. It's easy and a complete example program that does it can be found in [examples/read_mgz/](./examples/read_mgz/). The program also contains an example for writing an MGZ file. While `zlib` itself is not header-only, it should be available *everywhere anyways*, so it should not drag you into dependency hell.
* You can extract the MGZ files manually on the command line before running your program or convert them using the FreeSurfer `mri_convert` command line program: `mri_convert file.mgz file.mgh`.


#### What `libfs` is **not**

This library was written from scratch in C++. It is **not** based on the FreeSurfer C code and does not use the same data structures that are used in FreeSurfer. Note that libfs also does **not** allow you to call FreeSurfer functions from your programs.

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

#### Full example programs

See the [examples directory](./examples/) for some full demo programs which use the library. The example above is a minimal version of the [read_curv example](./examples/read_curv/read_curv.cpp). Other examples include:

* [examples/read_annot/read_annot.cpp](./examples/read_annot/read_annot.cpp): demo program that reads a FreeSurfer cortical parcellation file (atlas mapped to a subject)
* [examples/read_curv/read_curv.cpp](./examples/read_curv/read_curv.cpp): demo program that reads a FreeSurfer per-vertex data file, containing one value for every vertex of a matching surface, in surface vertex order (e.g., cortical thickness at that vertex)
* [examples/read_label/read_label.cpp](./examples/read_label/read_label.cpp): demo program that reads a FreeSurfer label file, assigning one value to a subset of vertices (e.g., 1 to all vertices in a certain region, and 0 to all others).
* [examples/read_mgh/read_mgh.cpp](./examples/read_mgh/read_mgh.cpp): demo program that reads a FreeSurfer MGH file, containing a 3D or 4D image.
* [examples/read_mgz/read_mgz.cpp](./examples/read_mgz/read_mgz.cpp): demo program that reads a FreeSurfer MGZ file, containing a compressed 3D or 4D image.
* [examples/read_surf/read_surf.cpp](./examples/read_surf/read_surf.cpp): demo program that reads a FreeSurfer cortical mesh (a.k.a. brain surface) file.


You can run the script [examples/run_all_examples.bash](./examples/run_all_examples.bash) to run all example files.


### Building your programs

Just add the directory into which you saved `libfs.h` to the include path during compilation. This is done by adding an `-I<path>` flag for most compilers, check the manual of your compiler if in doubt.

Compilation instructions for g++ and clang are at the top of each example source file, it should be easy to adapt them for your favorite C++ compiler. If you prefer to build with cmake, have a look at the [CMakeLists.txt file](./CMakeLists.txt) we use to build the unit tests.


### Full API documentation

The API docs can be browsed online at [codedocs.xyz/dfsp-spirit/libfs/](https://codedocs.xyz/dfsp-spirit/libfs/). The API docs contain usage examples for many important functions.


### Conventions

* Everything from `libfs` is in the `fs::` namespace.
* Internal functions in `libfs` are prefixed with an underscore, e.g., `_do_internal_stuff`. You should never call these functions from client code, as they may change without notice between versions. Relying on them is an application bug. (Please [open an issue](https://github.com/dfsp-spirit/libfs/issues) if you feel that you need to use an internal function from your client code, and explain your use case.) Internal functions are not listed in the API docs.
* Function naming:
  - Functions that read data are called `read_*`, e.g., `read_curv` and `read_mgh`.
  - Functions that write data to files are called `write_*`, e.g., `write_curv` and `write_mgh`.
* Most `read_*`/`write_*` functions are overloaded and accept either a `const std::string& filename` argument or a `std::istream *is`/`std::ostream *os` as a source/sink. This allows you to pass streams and bring your own gunzip (see [examples/read_mgz/](./examples/read_mgz/)).
* You can control the output of libfs by defining a log level for libfs before importing the libfs header file, see the API docs for details. An example can be seen at the very top of the [demo app](./src/demo_main.cpp).


## Development


### Running the tests

You need git, cmake and some C++ compiler. Under Debian-based Linux distributions `sudo apt-get install build-essential cmake git` should do it.

If you have not cloned yet:

```shell
git clone https://github.com/dfsp-spirit/libfs
cd libfs/
```

Then build and run the tests:

```shell
cmake .
make
./run_libfs_tests
```
Note that the only things that are being built are the test binary `run_libfs_tests` and the demo application, `demo_libfs`.

Note: If you do not have cmake, you can compile the tests manually, e.g., for `g++`:

```shell
# in the libfs repo root:
g++ -Iinclude -Ithird_party src/main.cpp src/libfs_tests.cpp -o run_libfs_tests
```

Please check your compiler's manual if you are using a different compiler.


### Running all mini examples

The examples are small, stand-alone programs in the `examples/` directory. Each example demonstrates how to interact with a certain file type.

In the repo root, just run `./examples/run_all_examples.bash` from your system shell. This script will also compile them (requires `g++`).

Note that the `read_mgz` example requires `zlib`.


### Running the demo app

The demo app `demo_libfs` is a slightly larger app that is build using cmake, like you would build a larger project that uses libfs.

To build it, see the instructions in the `Running the tests` section above, which will also build the demo app, `demo_libfs`.


### Building the documentation locally

If you have `doxygen` installed (`sudo apt install doxygen graphviz` under Debian-based Linux distros), you can generate the full API documentation like this:

```
cmake .
make doc
```

or with:

```
doxygen
```

The documentation will be built and can be found in `doc_built/` afterwards. The recommended way to browse it is to open `doc_built/html/index.html` with your favorite webbrowser, e.g.:

```
firefox doc_built/html/index.html
```


## Author, License and Getting help

The `libfs` library was written by [Tim Schäfer](https://ts.rcmd.org).

It is available under the [MIT license](./LICENSE).

Note that this library is **not** a part of FreeSurfer, and it is **in no way** endorsed by the FreeSurfer developers. Please do not contact them regarding this library, especially not for support. [Open an issue](https://github.com/dfsp-spirit/libfs/issues) in this repo instead.


## Related software packages

There are a number of software tools to read FreeSurfer files for different programming languages, e.g.:

* [NIBabel for Python](https://nipy.org/nibabel/)
* [freesurferformats for R](https://github.com/dfsp-spirit/freesurferformats)
* [neuroformats-rs for Rust](https://github.com/dfsp-spirit/neuroformats-rs)
* [NeuroFormats.jl for Julia](https://github.com/dfsp-spirit/NeuroFormats.jl)
* [neuro for Go](https://github.com/dfsp-spirit/neuro)
* SurfStat for Matlab

There most likely are a lot more to be found on the internet, and many neuro-imaging software packages can directly read FreeSurfer data.