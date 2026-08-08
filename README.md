# libfs
A portable, header-only, single file, no-dependency, mildly templated, C++11 library for accessing [FreeSurfer](https://freesurfer.net/) neuroimaging file formats.

<!-- badges: start -->
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.8090828.svg)](https://doi.org/10.5281/zenodo.8090828)
![unittests](https://github.com/dfsp-spirit/libfs/actions/workflows/unittests.yml/badge.svg?branch=main)
![examples](https://github.com/dfsp-spirit/libfs/actions/workflows/examples.yml/badge.svg?branch=main)
![docs](https://github.com/dfsp-spirit/libfs/actions/workflows/docs.yml/badge.svg?branch=main)
<!-- badges: end -->

## Features

* read and write FreeSurfer per-vertex data from and to binary curv format files (like `$SUBJECTS_DIR/surf/lh.thickness`).
* read and write FreeSurfer brain surface meshes from binary surf format files (like `$SUBJECTS_DIR/surf/lh.white`).
  - can also import triangular meshes from the following standard mesh file formats:
    * Wavefront object format (.obj)
    * Stanford PLY format (.ply, ascii version)
    * Object File Format (.off, both the plain version and the COFF variant including per-vertex colors are supported).
  - can export meshes to the following standard mesh file formats: Wavefront object, Stanford PLY (ascii version).
* read and write FreeSurfer brain surface parcellations, i.e., the result of applying a brain atlas, from/to binary annot format files (like `$SUBJECTS_DIR/label/lh.aparc.annot`).
* read and write FreeSurfer ASCII label files (like `$SUBJECTS_DIR/label/lh.cortex.label`).
* read and write FreeSurfer 4D volume files (typically 3D voxels + a fourth time/subject dimension) from binary MGH format files (like `$SUBJECTS_DIR/mri/brain.mgh` or `$SUBJECTS_DIR/surf/lh.thickness.fwhm5.fsaverage.mgh`).
  - also reads and writes the gzip-compressed MGZ variant (`read_mgz` / `write_mgz`) and NIfTI `.nii.gz` files (via `read_nifti` / `write_nifti` / `read_desc_data`). These functions require an explicit opt-in: `#define LIBFS_HAS_ZLIB` before including `libfs.h` and link with `-lz`.
* map per-vertex data to RGBA colors using viridis colormap, with NAN handling
* security measures to protect against invalid / malformed / malicious input files

Supported data types for the MGH format include:
* `MRI_INT`: 32 bit signed int
* `MRI_FLOAT`: 32 bit signed float and
* `MRI_UCHAR`: 8 bit unsigned int.
* `MRI_SHORT`: 16 bit signed int.


#### A note on zlib support (MGZ and .nii.gz)

The MGZ format is a gzipped MGH file. NIfTI `.nii.gz` files are gzipped `.nii` files.
`libfs` provides native functions for both:
* `read_mgz()` / `write_mgz()` -- for MGZ files.
* `read_nifti()` / `write_nifti()` / `read_desc_data()` -- these functions can also
  handle `.nii.gz` files (in addition to uncompressed `.nii`).

**zlib is an opt-in dependency.** To enable compressed format support, you must
`#define LIBFS_HAS_ZLIB` **before** including `libfs.h` and link with `-lz`:

```cpp
#define LIBFS_HAS_ZLIB
#include "libfs.h"
```

If you use CMake, calling `find_package(ZLIB)` and linking the target against
`ZLIB::ZLIB` is enough (the `LIBFS_HAS_ZLIB` define is added automatically -- see
the project's `CMakeLists.txt`).

No other dependencies are needed.

If you cannot use zlib at all, you can still extract MGZ / .nii.gz files manually
on the command line before running your program, or convert them using the
FreeSurfer `mri_convert` command line program: `mri_convert file.mgz file.mgh`.

If you prefer using the stream-based `read_mgh()`/`write_mgh()` overloads with a
C++ wrapper around zlib, the
[read_mgz example](./examples/read_mgz/) shows how to do this with the `zstr`
library (note: `zstr` is header-only, but you still need `-lz` for the
underlying zlib).


#### What `libfs` is **not**

This library was written from scratch in C++. It is **not** based on the FreeSurfer C code and does not use the same data structures that are used in FreeSurfer. Note that libfs also does **not** allow you to call FreeSurfer functions from your programs.

## Usage

### Quick Example

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

### API Docs

See [dfsp-spirit.github.io/libfs/](https://dfsp-spirit.github.io/libfs/).


### Full example programs

See the [examples directory](./examples/) for some full demo programs which use the library. The example above is a minimal version of the [read_curv example](./examples/read_curv/read_curv.cpp). Other examples include:

* [examples/read_annot/read_annot.cpp](./examples/read_annot/read_annot.cpp): demo program that reads a FreeSurfer cortical parcellation file (atlas mapped to a subject)
* [examples/write_annot/write_annot.cpp](./examples/write_annot/write_annot.cpp): demo program that reads an annot file, writes it back, and verifies the round-trip.
* [examples/read_curv/read_curv.cpp](./examples/read_curv/read_curv.cpp): demo program that reads a FreeSurfer per-vertex data file, containing one value for every vertex of a matching surface, in surface vertex order (e.g., cortical thickness at that vertex)
* [examples/read_label/read_label.cpp](./examples/read_label/read_label.cpp): demo program that reads a FreeSurfer label file, assigning one value to a subset of vertices (e.g., 1 to all vertices in a certain region, and 0 to all others).
* [examples/read_mgh/read_mgh.cpp](./examples/read_mgh/read_mgh.cpp): demo program that reads a FreeSurfer MGH file, containing a 3D or 4D image.
* [examples/read_mgz/read_mgz.cpp](./examples/read_mgz/read_mgz.cpp): demo program that reads and writes a FreeSurfer MGZ file using the `zstr` stream wrapper around zlib (the pre-v0.5 approach).
* [examples/read_mgz_native/read_mgz_native.cpp](./examples/read_mgz_native/read_mgz_native.cpp): demo program that reads and writes a FreeSurfer MGZ file using the native `read_mgz()` / `write_mgz()` functions. Requires zlib (`-lz`).
* [examples/read_surf/read_surf.cpp](./examples/read_surf/read_surf.cpp): demo program that reads a FreeSurfer cortical mesh (a.k.a. brain surface) file.
* [examples/write_subjectsfile/write_subjectsfile.cpp](./examples/write_subjectsfile/write_subjectsfile.cpp): demo program that creates a subjects file, writes it, and reads it back to verify the round-trip.
* [examples/vertex_color_export/vertex_color_export.cpp](./examples/vertex_color_export/vertex_color_export.cpp): demo program that reads a brain mesh, per-vertex morphometry data (cortical thickness), and a cortex label, masks the medial wall vertices to NaN, maps the thickness to vertex colors using the viridis colormap (NaN -> white by default), and exports the colored mesh to a PLY file.


You can run the script [examples/run_all_examples.bash](./examples/run_all_examples.bash) to run all example files.


### Building your programs

libfs is a header-only library — just `#include "libfs.h"`. You can integrate it into your project in several ways:

#### Option 1: CMake FetchContent (recommended)

No download, no install. Add this to your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
    libfs
    GIT_REPOSITORY https://github.com/dfsp-spirit/libfs.git
    GIT_TAG        main   # or a specific version tag
)
FetchContent_MakeAvailable(libfs)
target_link_libraries(my_app PRIVATE libfs::libfs)
```

#### Option 2: Install system-wide

```bash
git clone https://github.com/dfsp-spirit/libfs.git
cd libfs
cmake -S. -Bbuild
cmake --install build   # may need sudo for /usr/local
```

Then in your `CMakeLists.txt`:

```cmake
find_package(libfs REQUIRED)
target_link_libraries(my_app PRIVATE libfs::libfs)
```

#### Option 3: Vendored copy

```cmake
add_subdirectory(third_party/libfs)
target_link_libraries(my_app PRIVATE libfs::libfs)
```

#### Option 4: Manual include path

Just drop `include/libfs.h` anywhere and add its directory to your compiler's include path with `-I<path>`. No CMake required.


### Full API documentation

The API docs can be browsed online at [dfsp-spirit.github.io/libfs/](https://dfsp-spirit.github.io/libfs/). The API docs contain usage examples for many important functions.


### Conventions

* Everything from `libfs` is in the `fs::` namespace.
* Internal functions in `libfs` are prefixed with an underscore, e.g., `_do_internal_stuff`. You should never call these functions from client code, as they may change without notice between versions. Relying on them is an application bug. (Please [open an issue](https://github.com/dfsp-spirit/libfs/issues) if you feel that you need to use an internal function from your client code, and explain your use case.) Internal functions are not listed in the API docs.
* Function naming:
  - Functions that read data are called `read_*`, e.g., `read_curv` and `read_mgh`.
  - Functions that write data to files are called `write_*`, e.g., `write_curv` and `write_mgh`.
* Most `read_*`/`write_*` functions are overloaded and accept either a `const std::string& filename` argument or a `std::istream *is`/`std::ostream *os` as a source/sink. This allows you to pass streams for custom decompression (e.g., if you want to use a different zlib wrapper).
* You can control the output of libfs by defining a log level for libfs before importing the libfs header file, see the API docs for details. An example can be seen at the very top of the [demo app](./src/demo_main.cpp).


### Security & Defensive Validation

libfs validates binary file headers before allocating memory or reading data, protecting against malformed or malicious files that could cause integer overflows, excessive memory allocations, or out-of-bounds reads. The following limits are configurable by `#define`-ing them **before** `#include "libfs.h"`:

| Setting | Default | Description |
|---|---|---|
| `LIBFS_MAX_ALLOC_BYTES` | 2 GiB | Maximum memory a single file may claim to need. Files whose header-claimed payload exceeds this are rejected. |
| `LIBFS_MAX_STRING_LENGTH` | 4096 | Maximum length for fixed-length strings embedded in binary headers (e.g., filenames in annotation colortables). |
| `LIBFS_MAX_COLORTABLE_ENTRIES` | 10000 | Maximum number of entries in an annotation colortable. |

**Example: overriding the defaults**

```cpp
#define LIBFS_MAX_ALLOC_BYTES (8ULL * 1024ULL * 1024ULL * 1024ULL)  // raise to 8 GiB
#include "libfs.h"
```

**Validation performed on every file read:**

- **Header dimensions** must be positive; negative `int32_t` values (which would wrap to huge `size_t`) are rejected.
- **Allocation sizes** are computed with overflow-safe multiplication (`size_t` × `size_t`); files that would cause integer overflow are rejected **before** any allocation is attempted.
- **Memory cap**: the computed payload size is checked against `LIBFS_MAX_ALLOC_BYTES`.
- **Stream integrity**: every `istream::read()` call is checked (`gcount`). Truncated or corrupted files that deliver fewer bytes than expected throw an error instead of silently producing garbage.
- **File-size cross-check** (file-based overloads only): the header-claimed data size is compared against the actual file size on disk. Files whose headers claim more data than the file physically contains are rejected immediately.


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

Note that the `read_mgz` example requires `zlib` (link with `-lz`).


### Running the demo app

The demo app `demo_libfs` is a slightly larger app that is build using cmake, like you would build a larger project that uses libfs.

You can find the code in [src/](./src/).

To build it, see the instructions in the `Running the tests` section above, which will also build the demo app, `demo_libfs`.


### Building the documentation locally

If you have `doxygen` installed (`sudo apt install doxygen graphviz` under Debian-based Linux distros), you can generate the full API documentation like this:

```
doxygen Doxyfile
```

The documentation will be built and can be found in `doc_built/` afterwards. The recommended way to browse it is to open `doc_built/html/index.html` with your favorite webbrowser, e.g.:

```
firefox doc_built/html/index.html
```

### Making a release

* make sure you have logged all changes in the CHANGES file
* build & run tests: `cmake . && make && ./run_libfs_tests`
* build & run mini examples: `./examples/run_all_examples.bash`
* run demo app (it is build with the tests automatically): `./demo_libfs subject1 examples/subjects_dir/`
* build the docs (`doxygen Doxyfile`) and check for warnings
* bump version in include/libfs.h (the `#define` lines near top)
* tag the commit with the version
* create release from tag in github web UI



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