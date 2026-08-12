#pragma once

#include <iostream>
#include <climits>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <map>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>

// -- Optional MGZ / NIfTI-gz support via zlib -------------------------------------
// When LIBFS_HAS_ZLIB is #defined before including this header, the following
// become available:
//   - read_mgz() / write_mgz()
//   - read_nifti_gz() / write_nifti_gz()  (and .nii.gz support in read_nifti() /
//     write_nifti() / read_desc_data())
// Just link with -lz.
//
// There is NO auto-detection — you must explicitly opt in:
//     #define LIBFS_HAS_ZLIB
//     #include "libfs.h"
//
// If LIBFS_HAS_ZLIB is not defined, the MGZ / NIfTI-gz functions are simply
// absent.  Attempting to use them results in a compile error, and attempting
// to read/write a .gz file via the generic read_nifti() / write_nifti() /
// read_desc_data() functions throws a runtime_error at runtime.
//
// This is all compile-time; there is zero runtime overhead when zlib support
// is not enabled.
#ifdef LIBFS_HAS_ZLIB
#include <zlib.h>
#endif
// -- End optional MGZ support -----------------------------------------------------

/// @brief Full libfs version string, in semver format.
/// @details A `"MAJOR.MINOR.PATCH"` string, e.g. `"0.4.2"`.  Use this when
///          displaying the library version to users or writing it into output
///          file headers.  For programmatic version checks, prefer the
///          individual `LIBFS_VERSION_MAJOR`, `LIBFS_VERSION_MINOR`, and
///          `LIBFS_VERSION_PATCH` integer macros.
#define LIBFS_VERSION "0.6.0"

/// @brief Major version number (incompatible API changes).
/// @details Incremented when the public API changes in a way that breaks
///          source or binary compatibility.  For example, bumping from 0 to 1
///          signals a stable first release.  Combine with
///          `LIBFS_VERSION_MINOR` and `LIBFS_VERSION_PATCH` to construct a
///          complete semver identifier.
#define LIBFS_VERSION_MAJOR 0

/// @brief Minor version number (backward-compatible feature additions).
/// @details Incremented when new functionality is added without breaking
///          existing API contracts.  Together with `LIBFS_VERSION_MAJOR` this
///          forms the `MAJOR.MINOR` prefix used in release tags.
#define LIBFS_VERSION_MINOR 6

/// @brief Patch version number (backward-compatible bug fixes).
/// @details Incremented for bug-fix releases that do not add new features or
///          change the public API.  The full version string is
///          `LIBFS_VERSION_MAJOR.LIBFS_VERSION_MINOR.LIBFS_VERSION_PATCH`.
#define LIBFS_VERSION_PATCH 0

// -- Security / defensive hardening configuration -------------------------------------
// Users can #define any of these BEFORE including libfs.h to override the defaults.

/// Default maximum memory allocation limit in bytes (2 GiB).
/// A header claiming to need more memory than this will be rejected.
#define LIBFS_MAX_ALLOC_BYTES_DEFAULT (2ULL * 1024ULL * 1024ULL * 1024ULL)

/// Maximum memory allocation limit.
#ifndef LIBFS_MAX_ALLOC_BYTES
#define LIBFS_MAX_ALLOC_BYTES LIBFS_MAX_ALLOC_BYTES_DEFAULT
#endif

/// Maximum length for fixed-length strings read from binary headers (e.g., filenames in annot colortables).
#ifndef LIBFS_MAX_STRING_LENGTH
#define LIBFS_MAX_STRING_LENGTH 4096
#endif

/// Maximum number of entries in an annotation colortable.
#ifndef LIBFS_MAX_COLORTABLE_ENTRIES
#define LIBFS_MAX_COLORTABLE_ENTRIES 10000
#endif

/// Maximum line length when reading OBJ files (prevents single-line memory-exhaustion DoS).
/// A single line longer than this causes a parse error.
#ifndef LIBFS_MAX_OBJ_LINE_LENGTH
#define LIBFS_MAX_OBJ_LINE_LENGTH 1048576  // 1 MiB
#endif

/// Maximum number of lines parsed from an OBJ file (prevents many-tiny-lines CPU-exhaustion DoS).
#ifndef LIBFS_MAX_OBJ_LINES
#define LIBFS_MAX_OBJ_LINES 100000000  // 100M lines
#endif

/// Maximum file size for an OBJ file checked before parsing (prevents reading huge files).
/// Defaults to the same value as LIBFS_MAX_ALLOC_BYTES.
#ifndef LIBFS_MAX_OBJ_FILE_SIZE
#define LIBFS_MAX_OBJ_FILE_SIZE LIBFS_MAX_ALLOC_BYTES
#endif

/// Maximum line length when reading OFF files (prevents single-line memory-exhaustion DoS).
#ifndef LIBFS_MAX_OFF_LINE_LENGTH
#define LIBFS_MAX_OFF_LINE_LENGTH 1048576  // 1 MiB
#endif

/// Maximum number of lines parsed from an OFF file (prevents many-tiny-lines CPU-exhaustion DoS).
#ifndef LIBFS_MAX_OFF_LINES
#define LIBFS_MAX_OFF_LINES 100000000  // 100M lines
#endif

/// Maximum file size for an OFF file checked before parsing.
#ifndef LIBFS_MAX_OFF_FILE_SIZE
#define LIBFS_MAX_OFF_FILE_SIZE LIBFS_MAX_ALLOC_BYTES
#endif

/// Maximum line length when reading PLY files (prevents single-line memory-exhaustion DoS).
#ifndef LIBFS_MAX_PLY_LINE_LENGTH
#define LIBFS_MAX_PLY_LINE_LENGTH 1048576  // 1 MiB
#endif

/// Maximum number of lines parsed from a PLY file (prevents many-tiny-lines CPU-exhaustion DoS).
#ifndef LIBFS_MAX_PLY_LINES
#define LIBFS_MAX_PLY_LINES 100000000  // 100M lines
#endif

/// Maximum file size for a PLY file checked before parsing.
#ifndef LIBFS_MAX_PLY_FILE_SIZE
#define LIBFS_MAX_PLY_FILE_SIZE LIBFS_MAX_ALLOC_BYTES
#endif
// -- End security configuration --------------------------------------------------------

/// @file
///
/*! \mainpage The libfs API documentation
 *
 * \section intro_sec Introduction
 *
 * Welcome to the API documentation for libfs, a header-only C++11 library to read and write FreeSurfer neuroimaging data.
 *
 * All relevant functions are in the file include/libfs.h and only a few utility functions are class
 * members, so the best place to start is to open the documentation for libfs.h in the Files section above.
 *
 * \subsection intro-examples A note on the API doc examples
 *
 * The examples in the doc strings of the libfs.h functions usually only show data preparation and the function call itself. Typically you
 * will get a working program out of them by wrapping them into something like:
 *
 * @code
 * #include "libfs.h"
 * #include <string>
 * #include <iostream>
 * #include <vector>
 * // maybe more includes for some examples here.
 *
 * int main(int argc, char** argv) {
 *      // Demo code goes here
 * }
 * @endcode
 *
 * To see full demo programs and compilation instructions, check the <a href="https://github.com/dfsp-spirit/libfs/tree/main/examples">examples/ directory</a> in the GitHub repository linked below.
 *
 * \subsection logging Logging with libfs
 *
 * You can define the output produced by libfs from your application. To do so,
 * `#define` _one_ of the following debug levels in your application, *before* including 'libfs.h':
 *
 *  - `LIBFS_DBG_CRITICAL`     // print only critical errors that will raise an expection and most likely cause application to stop (unless caught).
 *  - `LIBFS_DBG_ERROR`        // prints errors (and more severe things).
 *  - `LIBFS_DBG_WARNING`      // the default, prints warnings (and more severe things).
 *  - `LIBFS_DBG_INFO`         // prints info messages, like what is currently being done.
 *  - `LIBFS_DBG_VERBOSE`      // prints info messages inside loops, may considerable slow down apps and litter stdout.
 *  - `LIBFS_DBG_EXCESSIVE`    // prints info messages in nested loops, will considerable slow down apps and quickly litter stdout.
 *
 *
 * Things you should know about logging and controlling libfs output:
 *
 *   - The debug levels are ordered in the list above, and defining a single one will automatically enable
 * all levels of higher importance (e.g., defining `LIBFS_DBG_WARNING` also enables `LIBFS_DBG_ERROR` and `LIBFS_DBG_CRITICAL`).
 *   - If you define nothing at all, libfs defaults to `LIBFS_DBG_WARNING`.
 *   - If you do not want any ouput from libfs, define `LIBFS_DBG_NONE`. This is not recommended though, as it completely disables
 *     all output, including critical error messages. This means that your application may terminate without any message,
 *     and is only advisable if you are very sure that you catch all possible exceptions and then produce an error message
 *     for users in your application code.
 *   - Currently all debug output goes to `stdout`, i.e., typically to the terminal.
 *
 *
 * \subsection intro-website The libfs project website
 *
 * The project page for libfs can be found at https://github.com/dfsp-spirit/libfs. It contains information on all documentation available for libfs.
 *
 *
 */

/// @brief Application tag prepended to every debug message from libfs.
/// @details Each line of output produced by the debug macros is prefixed with
///          this string so users can identify which component emitted the
///          message.  Define `LIBFS_APPTAG` **before** including `libfs.h` to
///          supply your own tag, e.g.:
///          @code
///          #define LIBFS_APPTAG "[my-app] "
///          #include "libfs.h"
///          @endcode
///          If left undefined, the default `"[libfs] "` is used.
#ifndef LIBFS_APPTAG
#define LIBFS_APPTAG "[libfs] "
#endif

/// @brief Default debug level: emit warnings and everything more severe.
/// @details This is the built-in fallback when no `LIBFS_DBG_*` macro is
///          defined by the user.  Enabling `LIBFS_DBG_WARNING` also
///          automatically activates `LIBFS_DBG_ERROR` and
///          `LIBFS_DBG_CRITICAL` (see the cascading `#ifdef` chain below).
///          Typical output includes recoverable problems such as unknown file
///          metadata keys or deprecated API usage.
#define LIBFS_DBG_WARNING

// If the user wants something below our default, remove our default.
#ifdef LIBFS_DBG_NONE
#undef LIBFS_DBG_WARNING
#endif

#ifdef LIBFS_DBG_CRITICAL
#undef LIBFS_DBG_WARNING
#endif

#ifdef LIBFS_DBG_ERROR
#undef LIBFS_DBG_WARNING
#endif

// Ensure that the user does not have to define all debug levels
// up to the one they actually want, by defining all lower ones for them.
#ifdef LIBFS_DBG_EXCESSIVE
#define LIBFS_DBG_VERBOSE
#endif

#ifdef LIBFS_DBG_VERBOSE
#define LIBFS_DBG_INFO
#endif

#ifdef LIBFS_DBG_INFO
#define LIBFS_DBG_WARNING
#endif

/// @brief Debug level: emit errors and everything more severe.
/// @details When defined (directly by the user or through the cascading
///          `#ifdef` chain), messages tagged as errors, as well as critical
///          errors, are printed.  Error-level messages indicate a problem
///          that prevents the current operation from completing successfully
///          (e.g., a file could not be parsed), but the application may
///          still continue.
#ifdef LIBFS_DBG_WARNING
#define LIBFS_DBG_ERROR
#endif

/// @brief Debug level: emit only critical errors.
/// @details When defined, only the most severe messages are printed.
///          Critical errors typically precede an exception that will
///          terminate the application unless caught.  Use this level in
///          production to keep output minimal while still reporting fatal
///          conditions.
#ifdef LIBFS_DBG_ERROR
#define LIBFS_DBG_CRITICAL
#endif

// End of debug handling.

namespace fs
{

  namespace util
  {

    /// @brief  Cross-platform wrapper for localtime_r and localtime_s.
    /// @param time the time to convert to an std::tm struct.
    /// @return the std::tm struct.
    tm _localtime(const std::time_t &time)
    {
      std::tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
      ::localtime_s(&tm_snapshot, &time);
#else
      ::localtime_r(&time, &tm_snapshot); // POSIX
#endif
      return tm_snapshot;
    }

    /// @brief Get current time as string, e.g. for log messages.
    /// @param t the timepoint to format as a string, typically `std::system_clock::now()`.
    /// @return the formatted time string.
    /// #### Examples
    ///
    /// @code
    /// std::string time_rep = fs::util::time_tag(std::chrono::system_clock::now());
    /// @endcode
    std::string time_tag(std::chrono::system_clock::time_point t)
    {
      auto as_time_t = std::chrono::system_clock::to_time_t(t);
      struct tm tm;
      char time_buffer[64];
      // if (::gmtime_r(&as_time_t, &tm)) {
      tm = _localtime(as_time_t);
      if (std::strftime(time_buffer, sizeof(time_buffer), "%F %T", &tm))
      {
        return std::string{time_buffer};
      }
      throw std::runtime_error("Failed to get current date as string");
    }

    /// Logging threshold for critical messages.
    const std::string LOGTAG_CRITICAL = "CRITICAL";

    /// Logging threshold for error messages.
    const std::string LOGTAG_ERROR = "ERROR";

    /// Logging threshold for warning messages.
    const std::string LOGTAG_WARNING = "WARNING";

    /// Logging threshold for warning messages.
    const std::string LOGTAG_INFO = "INFO";

    /// Logging threshold for warning messages.
    const std::string LOGTAG_VERBOSE = "VERBOSE";

    /// Logging threshold for warning messages.
    const std::string LOGTAG_EXCESSIVE = "EXCESSIVE";

    /// @brief Log a message, goes to stdout.
    /// @param message the message to be logged.
    /// @param loglevel the log level, one of `fs::util::LOGTAG_*`.
    inline void log(std::string const &message, std::string const loglevel = "INFO")
    {
#ifdef LIBFS_DBG_ERROR
      std::cout << LIBFS_APPTAG << "[" << loglevel << "] [" << fs::util::time_tag(std::chrono::system_clock::now()) << "] " << message << "\n";
#endif
    }

    // -- Security / defensive hardening helpers -----------------------------------------

    /// @brief Safe multiplication for size_t: returns false on overflow.
    /// @private
    inline bool safe_multiply(size_t a, size_t b, size_t &result)
    {
      if (a == 0 || b == 0)
      {
        result = 0;
        return true;
      }
      if (a > std::numeric_limits<size_t>::max() / b)
      {
        return false;
      }
      result = a * b;
      return true;
    }

    /// @brief Validate that allocating num_elements of size bytes_per_element does not
    ///        overflow or exceed LIBFS_MAX_ALLOC_BYTES.
    /// @returns true if the allocation is safe, false otherwise.
    /// @private
    inline bool check_alloc(size_t num_elements, size_t bytes_per_element)
    {
      size_t total_bytes = 0;
      if (!safe_multiply(num_elements, bytes_per_element, total_bytes))
      {
        return false;
      }
      if (total_bytes > LIBFS_MAX_ALLOC_BYTES)
      {
        return false;
      }
      return true;
    }

    /// @brief Get the size of a file in bytes. Returns 0 on error.
    /// @private
    inline size_t get_file_size(const std::string &filename)
    {
      std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
      if (!ifs.is_open())
      {
        return 0;
      }
      std::streampos end = ifs.tellg();
      if (end < 0)
      {
        return 0;
      }
      return static_cast<size_t>(end);
    }

    /// @brief Check whether a float is finite (not NaN, not Inf). C++11-compatible.
    /// @private
    inline bool is_finite_float(float value)
    {
      return !std::isnan(value) && !std::isinf(value);
    }

    // -- End security helpers ---------------------------------------------------------

    /// @brief Check whether a string ends with the given suffix.
    /// @private
    ///
    /// #### Examples
    ///
    /// @code
    /// bool ev = fs::util::ends_with("freesurfer", "surfer"); // true
    /// @endcode
    inline bool ends_with(std::string const &value, std::string const &suffix)
    {
      if (suffix.size() > value.size())
        return false;
      return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
    }

    /// @brief Check whether a string ends with one of the given suffixes.
    /// @private
    ///
    /// #### Examples
    ///
    /// @code
    /// bool ev = fs::util::ends_with("freesurfer", {"surfer", "not"}); // true
    /// @endcode
    inline bool ends_with(std::string const &value, std::initializer_list<std::string> suffixes)
    {
      for (auto suffix : suffixes)
      {
        if (ends_with(value, suffix))
        {
          return true;
        }
      }
      return false;
    }

    /// @brief Turn 1D vector into 2D vector.
    /// @param values the input 1D vector.
    /// @param num_cols number of columns for the returned 2D vector.
    /// @return 2D vector with `num_cols` columns.
    /// @private
    ///
    /// #### Examples
    ///
    /// @code
    /// std::vector<float> input = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };
    /// std::vector<std::vector<float>> res = fs::util::v2d(input, 2);
    /// @endcode
    template <typename T>
    std::vector<std::vector<T>> v2d(std::vector<T> values, size_t num_cols)
    {
      std::vector<std::vector<T>> result;
      for (std::size_t i = 0; i < values.size(); ++i)
      {
        if (i % num_cols == 0)
        {
          result.resize(result.size() + 1);
        }
        result[i / num_cols].push_back(values[i]);
      }
      return result;
    }

    /// @brief Flatten 2D vector.
    /// @param values the input 2D vector.
    /// @return 1D vector.
    ///
    /// #### Examples
    ///
    /// @code
    /// std::vector<float> input = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };
    /// std::vector<std::vector<float>> res = fs::util::v2d(input, 2);
    /// @endcode
    template <typename T>
    std::vector<T> vflatten(std::vector<std::vector<T>> values)
    {
      size_t total_size = 0;
      for (std::size_t i = 0; i < values.size(); i++)
      {
        total_size += values[i].size();
      }

      std::vector<T> result = std::vector<T>(total_size);
      size_t cur_idx = 0;
      for (std::size_t i = 0; i < values.size(); i++)
      {
        for (std::size_t j = 0; j < values[i].size(); j++)
        {
          result[cur_idx] = values[i][j];
          cur_idx++;
        }
      }
      return result;
    }

    /// @brief Check whether a string starts with the given prefix.
    /// @private
    /// @note This is a private function, users should call the overloaded version that accepts
    ///       a vector of prefixes instead.
    ///
    /// #### Examples
    ///
    /// @code
    /// bool ev = fs::util::starts_with("freesurfer", "free"); // true
    /// @endcode
    inline bool starts_with(std::string const &value, std::string const &prefix)
    {
      if (prefix.length() > value.length())
        return false;
      return value.rfind(prefix, 0) == 0;
    }

    /// @brief Check whether a string starts with one of the given prefixes.
    /// @param value the string for which to check whether it starts with any of the prefixes
    /// @param prefixes the prefixes to consider
    /// @returns whether the string starts with one of the prefixes.
    ///
    /// #### Examples
    ///
    /// @code
    /// bool ev = fs::util::starts_with("freesurfer", {"free", "not"}); // true
    /// @endcode
    inline bool starts_with(std::string const &value, std::initializer_list<std::string> prefixes)
    {
      for (auto prefix : prefixes)
      {
        if (starts_with(value, prefix))
        {
          return true;
        }
      }
      return false;
    }

    /// @brief Check whether a file exists (can be read) at given path.
    /// @details You should not rely on this as a pre-check when considering to open a file due
    ///          to race conditions, just try-catch open in that case. This is intended to check
    ///          whether a certain software run succeeded, by checking whether the key expected
    ///          output files exist.
    /// @param name the filename that should be checked.
    /// #### Examples
    ///
    /// @code
    /// bool exists = fs::util::file_exists("./study1/subject1/label/lh.aparc.annot");
    /// @endcode
    inline bool file_exists(const std::string &name)
    {
      if (FILE *file = fopen(name.c_str(), "r"))
      {
        fclose(file);
        return true;
      }
      else
      {
        return false;
      }
    }

    /// @brief Construct a UNIX file system path from the given path_components.
    /// @details Any trailing or leading slash (path_sep) will be stripped from the individual components and replaced with a single one between two components. If the first path component started with a slash, that slash will be kept (absolute paths are left intact).
    /// @param path_components init list of strings, the path components
    /// @param path_sep path separator to use, typically `/` on Unix-based system.
    /// @throws std::invalid_argument on empty
    /// @returns string representation of the path, using the `path_sep`.
    ///
    /// #### Examples
    ///
    /// @code
    /// std::string p = fs::util::fullpath({"path", "to", "file.txt"});
    /// // Gives: "path/to/file.txt"
    /// std::string p = fs::util::fullpath({"/path", "to", "file.txt"});
    /// // Gives: "/path/to/file.txt"
    /// @endcode
    std::string fullpath(std::initializer_list<std::string> path_components, std::string path_sep = std::string("/"))
    {
      std::string fp;
      if (path_components.size() == 0)
      {
        throw std::invalid_argument("The 'path_components' must not be empty.");
      }

      std::string comp;
      std::string comp_mod;
      size_t idx = 0;
      for (auto comp : path_components)
      {
        comp_mod = comp;
        if (idx != 0)
        { // We keep a leading slash intact for the first element (absolute path).
          if (starts_with(comp, path_sep))
          {
            comp_mod = comp.substr(1, comp.size() - 1);
          }
        }

        if (ends_with(comp_mod, path_sep))
        {
          comp_mod = comp_mod.substr(0, comp_mod.size() - 1);
        }

        fp += comp_mod;
        if (idx < path_components.size() - 1)
        {
          fp += path_sep;
        }
        idx++;
      }
      return fp;
    }

    /// @brief Write the given text representation (any string) to a file.
    /// @param filename the file to which to write, will be overwritten if exists
    /// @param rep the string to write to the file
    /// @throws std::runtime_error if the file cannot be opened.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::util::str_to_file("thoughts.txt", "blah, blah, blah");
    /// @endcode
    void str_to_file(const std::string &filename, const std::string rep)
    {
      std::ofstream ofs;
      ofs.open(filename, std::ofstream::out);
#ifdef LIBFS_DBG_VERBOSE
      std::cout << LIBFS_APPTAG << "Opening file '" << filename << "' for writing.\n";
#endif
      if (ofs.is_open())
      {
        ofs << rep;
        ofs.close();
      }
      else
      {
        throw std::runtime_error("Unable to open file '" + filename + "' for writing.\n");
      }
    }

    /// @brief Map per-vertex numeric data to RGB colors using the Viridis perceptually-uniform colormap.
    /// @details This converts the output of fs::read_curv_data (or any per-vertex float vector) into the
    ///          RGB color vector expected by fs::Mesh::to_ply() / fs::Mesh::to_ply_file() (and the OFF variants).
    ///          The returned vector contains 3 consecutive uint8_t values per vertex (red, green, blue), in the
    ///          exact interleave order used by the mesh exporters (v0_red, v0_green, v0_blue, v1_red, ...).
    ///          The input data is normalized to the [0, 1] range before sampling the colormap. By default the
    ///          normalization range is computed from the finite (non-NaN) min and max of the input, and you can
    ///          override either bound by passing a finite value for @p vmin and/or @p vmax (pass NAN for the
    ///          bound(s) you want to be derived from the data). NaN input values are mapped to a configurable
    ///          color, which defaults to white (255, 255, 255) following the standard convention in neuroimaging
    ///          where missing data (e.g. the medial wall) is shown in white. Use the @p nan_r / @p nan_g /
    ///          @p nan_b parameters to change this color.
    ///          The Viridis colormap is implemented from its official 256-sample lookup table (the same data used
    ///          by matplotlib), with linear interpolation between samples, so there are no external dependencies.
    /// @param data the per-vertex scalar values to convert (one value per vertex).
    /// @param vmin the value mapped to the bottom of the colormap. Pass NAN (the default) to auto-compute it as the finite minimum of @p data.
    /// @param vmax the value mapped to the top of the colormap. Pass NAN (the default) to auto-compute it as the finite maximum of @p data.
    /// @param nan_r the red component (0..255) of the color used for NaN input values. Defaults to 255 (white).
    /// @param nan_g the green component (0..255) of the color used for NaN input values. Defaults to 255 (white).
    /// @param nan_b the blue component (0..255) of the color used for NaN input values. Defaults to 255 (white).
    /// @return a vector of 3 * data.size() uint8_t RGB color values, interleaved by vertex, ready for fs::Mesh::to_ply().
    /// @throws std::invalid_argument if @p vmin and @p vmax are both given but @p vmin is greater than @p vmax.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::vector<float> data = fs::read_curv_data("lh.thickness");
    /// // NaN values map to white by default, override the value range via vmin/vmax:
    /// std::vector<uint8_t> col = fs::util::viridis(data);
    /// surface.to_ply_file("lh.thickness.ply", col);
    /// // Optionally clip the value range, e.g. to [0.5, 4.0]:
    /// std::vector<uint8_t> col2 = fs::util::viridis(data, 0.5f, 4.0f);
    /// surface.to_ply_file("lh.thickness_clipped.ply", col2);
    /// // Optionally change the NaN color, e.g. to black:
    /// std::vector<uint8_t> col3 = fs::util::viridis(data, NAN, NAN, 0, 0, 0);
    /// surface.to_ply_file("lh.thickness_black_nan.ply", col3);
    /// @endcode
    std::vector<uint8_t> viridis(const std::vector<float> &data, float vmin = NAN, float vmax = NAN, uint8_t nan_r = 255, uint8_t nan_g = 255, uint8_t nan_b = 255)
    {
      std::vector<uint8_t> colors;
      if (data.empty())
      {
        return colors;
      }
      colors.reserve(data.size() * 3);

      // The official 256-entry Viridis colormap (RGB, floats in [0, 1]), identical to the
      // matplotlib viridis lookup table. Linearly interpolated between samples below.
      static const float lut[768] = {
      0.267004, 0.004874, 0.329415, 0.26851, 0.009605, 0.335427, 0.269944, 0.014625,
      0.341379, 0.271305, 0.019942, 0.347269, 0.272594, 0.025563, 0.353093, 0.273809,
      0.031497, 0.358853, 0.274952, 0.037752, 0.364543, 0.276022, 0.044167, 0.370164,
      0.277018, 0.050344, 0.375715, 0.277941, 0.056324, 0.381191, 0.278791, 0.062145,
      0.386592, 0.279566, 0.067836, 0.391917, 0.280267, 0.073417, 0.397163, 0.280894,
      0.078907, 0.402329, 0.281446, 0.08432, 0.407414, 0.281924, 0.089666, 0.412415,
      0.282327, 0.094955, 0.417331, 0.282656, 0.100196, 0.42216, 0.28291, 0.105393,
      0.426902, 0.283091, 0.110553, 0.431554, 0.283197, 0.11568, 0.436115, 0.283229,
      0.120777, 0.440584, 0.283187, 0.125848, 0.44496, 0.283072, 0.130895, 0.449241,
      0.282884, 0.13592, 0.453427, 0.282623, 0.140926, 0.457517, 0.28229, 0.145912,
      0.46151, 0.281887, 0.150881, 0.465405, 0.281412, 0.155834, 0.469201, 0.280868,
      0.160771, 0.472899, 0.280255, 0.165693, 0.476498, 0.279574, 0.170599, 0.479997,
      0.278826, 0.17549, 0.483397, 0.278012, 0.180367, 0.486697, 0.277134, 0.185228,
      0.489898, 0.276194, 0.190074, 0.493001, 0.275191, 0.194905, 0.496005, 0.274128,
      0.199721, 0.498911, 0.273006, 0.20452, 0.501721, 0.271828, 0.209303, 0.504434,
      0.270595, 0.214069, 0.507052, 0.269308, 0.218818, 0.509577, 0.267968, 0.223549,
      0.512008, 0.26658, 0.228262, 0.514349, 0.265145, 0.232956, 0.516599, 0.263663,
      0.237631, 0.518762, 0.262138, 0.242286, 0.520837, 0.260571, 0.246922, 0.522828,
      0.258965, 0.251537, 0.524736, 0.257322, 0.25613, 0.526563, 0.255645, 0.260703,
      0.528312, 0.253935, 0.265254, 0.529983, 0.252194, 0.269783, 0.531579, 0.250425,
      0.27429, 0.533103, 0.248629, 0.278775, 0.534556, 0.246811, 0.283237, 0.535941,
      0.244972, 0.287675, 0.53726, 0.243113, 0.292092, 0.538516, 0.241237, 0.296485,
      0.539709, 0.239346, 0.300855, 0.540844, 0.237441, 0.305202, 0.541921, 0.235526,
      0.309527, 0.542944, 0.233603, 0.313828, 0.543914, 0.231674, 0.318106, 0.544834,
      0.229739, 0.322361, 0.545706, 0.227802, 0.326594, 0.546532, 0.225863, 0.330805,
      0.547314, 0.223925, 0.334994, 0.548053, 0.221989, 0.339161, 0.548752, 0.220057,
      0.343307, 0.549413, 0.21813, 0.347432, 0.550038, 0.21621, 0.351535, 0.550627,
      0.214298, 0.355619, 0.551184, 0.212395, 0.359683, 0.55171, 0.210503, 0.363727,
      0.552206, 0.208623, 0.367752, 0.552675, 0.206756, 0.371758, 0.553117, 0.204903,
      0.375746, 0.553533, 0.203063, 0.379716, 0.553925, 0.201239, 0.38367, 0.554294,
      0.19943, 0.387607, 0.554642, 0.197636, 0.391528, 0.554969, 0.19586, 0.395433,
      0.555276, 0.1941, 0.399323, 0.555565, 0.192357, 0.403199, 0.555836, 0.190631,
      0.407061, 0.556089, 0.188923, 0.41091, 0.556326, 0.187231, 0.414746, 0.556547,
      0.185556, 0.41857, 0.556753, 0.183898, 0.422383, 0.556944, 0.182256, 0.426184,
      0.55712, 0.180629, 0.429975, 0.557282, 0.179019, 0.433756, 0.55743, 0.177423,
      0.437527, 0.557565, 0.175841, 0.44129, 0.557685, 0.174274, 0.445044, 0.557792,
      0.172719, 0.448791, 0.557885, 0.171176, 0.45253, 0.557965, 0.169646, 0.456262,
      0.55803, 0.168126, 0.459988, 0.558082, 0.166617, 0.463708, 0.558119, 0.165117,
      0.467423, 0.558141, 0.163625, 0.471133, 0.558148, 0.162142, 0.474838, 0.55814,
      0.160665, 0.47854, 0.558115, 0.159194, 0.482237, 0.558073, 0.157729, 0.485932,
      0.558013, 0.15627, 0.489624, 0.557936, 0.154815, 0.493313, 0.55784, 0.153364,
      0.497, 0.557724, 0.151918, 0.500685, 0.557587, 0.150476, 0.504369, 0.55743,
      0.149039, 0.508051, 0.55725, 0.147607, 0.511733, 0.557049, 0.14618, 0.515413,
      0.556823, 0.144759, 0.519093, 0.556572, 0.143343, 0.522773, 0.556295, 0.141935,
      0.526453, 0.555991, 0.140536, 0.530132, 0.555659, 0.139147, 0.533812, 0.555298,
      0.13777, 0.537492, 0.554906, 0.136408, 0.541173, 0.554483, 0.135066, 0.544853,
      0.554029, 0.133743, 0.548535, 0.553541, 0.132444, 0.552216, 0.553018, 0.131172,
      0.555899, 0.552459, 0.129933, 0.559582, 0.551864, 0.128729, 0.563265, 0.551229,
      0.127568, 0.566949, 0.550556, 0.126453, 0.570633, 0.549841, 0.125394, 0.574318,
      0.549086, 0.124395, 0.578002, 0.548287, 0.123463, 0.581687, 0.547445, 0.122606,
      0.585371, 0.546557, 0.121831, 0.589055, 0.545623, 0.121148, 0.592739, 0.544641,
      0.120565, 0.596422, 0.543611, 0.120092, 0.600104, 0.54253, 0.119738, 0.603785,
      0.5414, 0.119512, 0.607464, 0.540218, 0.119423, 0.611141, 0.538982, 0.119483,
      0.614817, 0.537692, 0.119699, 0.61849, 0.536347, 0.120081, 0.622161, 0.534946,
      0.120638, 0.625828, 0.533488, 0.12138, 0.629492, 0.531973, 0.122312, 0.633153,
      0.530398, 0.123444, 0.636809, 0.528763, 0.12478, 0.640461, 0.527068, 0.126326,
      0.644107, 0.525311, 0.128087, 0.647749, 0.523491, 0.130067, 0.651384, 0.521608,
      0.132268, 0.655014, 0.519661, 0.134692, 0.658636, 0.517649, 0.137339, 0.662252,
      0.515571, 0.14021, 0.665859, 0.513427, 0.143303, 0.669459, 0.511215, 0.146616,
      0.67305, 0.508936, 0.150148, 0.676631, 0.506589, 0.153894, 0.680203, 0.504172,
      0.157851, 0.683765, 0.501686, 0.162016, 0.687316, 0.499129, 0.166383, 0.690856,
      0.496502, 0.170948, 0.694384, 0.493803, 0.175707, 0.6979, 0.491033, 0.180653,
      0.701402, 0.488189, 0.185783, 0.704891, 0.485273, 0.19109, 0.708366, 0.482284,
      0.196571, 0.711827, 0.479221, 0.202219, 0.715272, 0.476084, 0.20803, 0.718701,
      0.472873, 0.214, 0.722114, 0.469588, 0.220124, 0.725509, 0.466226, 0.226397,
      0.728888, 0.462789, 0.232815, 0.732247, 0.459277, 0.239374, 0.735588, 0.455688,
      0.24607, 0.73891, 0.452024, 0.252899, 0.742211, 0.448284, 0.259857, 0.745492,
      0.444467, 0.266941, 0.748751, 0.440573, 0.274149, 0.751988, 0.436601, 0.281477,
      0.755203, 0.432552, 0.288921, 0.758394, 0.428426, 0.296479, 0.761561, 0.424223,
      0.304148, 0.764704, 0.419943, 0.311925, 0.767822, 0.415586, 0.319809, 0.770914,
      0.411152, 0.327796, 0.77398, 0.40664, 0.335885, 0.777018, 0.402049, 0.344074,
      0.780029, 0.397381, 0.35236, 0.783011, 0.392636, 0.360741, 0.785964, 0.387814,
      0.369214, 0.788888, 0.382914, 0.377779, 0.791781, 0.377939, 0.386433, 0.794644,
      0.372886, 0.395174, 0.797475, 0.367757, 0.404001, 0.800275, 0.362552, 0.412913,
      0.803041, 0.357269, 0.421908, 0.805774, 0.35191, 0.430983, 0.808473, 0.346476,
      0.440137, 0.811138, 0.340967, 0.449368, 0.813768, 0.335384, 0.458674, 0.816363,
      0.329727, 0.468053, 0.818921, 0.323998, 0.477504, 0.821444, 0.318195, 0.487026,
      0.823929, 0.312321, 0.496615, 0.826376, 0.306377, 0.506271, 0.828786, 0.300362,
      0.515992, 0.831158, 0.294279, 0.525776, 0.833491, 0.288127, 0.535621, 0.835785,
      0.281908, 0.545524, 0.838039, 0.275626, 0.555484, 0.840254, 0.269281, 0.565498,
      0.84243, 0.262877, 0.575563, 0.844566, 0.256415, 0.585678, 0.846661, 0.249897,
      0.595839, 0.848717, 0.243329, 0.606045, 0.850733, 0.236712, 0.616293, 0.852709,
      0.230052, 0.626579, 0.854645, 0.223353, 0.636902, 0.856542, 0.21662, 0.647257,
      0.8584, 0.209861, 0.657642, 0.860219, 0.203082, 0.668054, 0.861999, 0.196293,
      0.678489, 0.863742, 0.189503, 0.688944, 0.865448, 0.182725, 0.699415, 0.867117,
      0.175971, 0.709898, 0.868751, 0.169257, 0.720391, 0.87035, 0.162603, 0.730889,
      0.871916, 0.156029, 0.741388, 0.873449, 0.149561, 0.751884, 0.874951, 0.143228,
      0.762373, 0.876424, 0.137064, 0.772852, 0.877868, 0.131109, 0.783315, 0.879285,
      0.125405, 0.79376, 0.880678, 0.120005, 0.804182, 0.882046, 0.114965, 0.814576,
      0.883393, 0.110347, 0.82494, 0.88472, 0.106217, 0.83527, 0.886029, 0.102646,
      0.845561, 0.887322, 0.099702, 0.85581, 0.888601, 0.097452, 0.866013, 0.889868,
      0.095953, 0.876168, 0.891125, 0.09525, 0.886271, 0.892374, 0.095374, 0.89632,
      0.893616, 0.096335, 0.906311, 0.894855, 0.098125, 0.916242, 0.896091, 0.100717,
      0.926106, 0.89733, 0.104071, 0.935904, 0.89857, 0.108131, 0.945636, 0.899815,
      0.112838, 0.9553, 0.901065, 0.118128, 0.964894, 0.902323, 0.123941, 0.974417,
      0.90359, 0.130215, 0.983868, 0.904867, 0.136897, 0.993248, 0.906157, 0.143936,
    };

      const int n = 256;

      bool auto_min = std::isnan(vmin);
      bool auto_max = std::isnan(vmax);

      // Determine the finite (non-NaN) min/max of the data, used for auto range.
      float data_min = NAN;
      float data_max = NAN;
      bool have_finite = false;
      for (size_t i = 0; i < data.size(); i++)
      {
        if (std::isnan(data[i]))
        {
          continue;
        }
        if (!have_finite)
        {
          data_min = data[i];
          data_max = data[i];
          have_finite = true;
        }
        else
        {
          if (data[i] < data_min)
          {
            data_min = data[i];
          }
          if (data[i] > data_max)
          {
            data_max = data[i];
          }
        }
      }

      float lo = auto_min ? data_min : vmin;
      float hi = auto_max ? data_max : vmax;

      if (!auto_min && !auto_max)
      {
        if (vmin > vmax)
        {
          throw std::invalid_argument("In viridis(): 'vmin' must not be greater than 'vmax'.");
        }
      }

      if (!have_finite)
      {
        // All input values are NaN: map the whole vector to the configured NaN color.
        for (size_t i = 0; i < data.size(); i++)
        {
          colors.push_back(nan_r);
          colors.push_back(nan_g);
          colors.push_back(nan_b);
        }
        return colors;
      }

      bool constant = (hi <= lo);

      for (size_t i = 0; i < data.size(); i++)
      {
        if (std::isnan(data[i]))
        {
          colors.push_back(nan_r);
          colors.push_back(nan_g);
          colors.push_back(nan_b);
          continue;
        }

        float t;
        if (constant)
        {
          t = 0.5f;
        }
        else
        {
          t = (data[i] - lo) / (hi - lo);
          if (t < 0.0f) { t = 0.0f; }
          if (t > 1.0f) { t = 1.0f; }
        }

        float pos = t * (n - 1);
        int idx0 = static_cast<int>(pos);
        if (idx0 < 0) { idx0 = 0; }
        if (idx0 > n - 2) { idx0 = n - 2; }
        int idx1 = idx0 + 1;
        float frac = pos - static_cast<float>(idx0);

        for (int c = 0; c < 3; c++)
        {
          float val = lut[idx0 * 3 + c] * (1.0f - frac) + lut[idx1 * 3 + c] * frac;
          int iv = static_cast<int>(val * 255.0f + 0.5f);
          if (iv < 0) { iv = 0; }
          if (iv > 255) { iv = 255; }
          colors.push_back(static_cast<uint8_t>(iv));
        }
      }
      return colors;
    }
  } // End namespace util.

  // MRI data types, used by the MGH functions.

  /// MRI data type representing an 8 bit unsigned integer.
  const int MRI_UCHAR = 0;

  /// MRI data type representing a 32 bit signed integer.
  const int MRI_INT = 1;

  /// MRI data type representing a 32 bit float.
  const int MRI_FLOAT = 3;

  /// MRI data type representing a 16 bit signed integer.
  const int MRI_SHORT = 4;

  // Forward declarations.
  int _fread3(std::istream &);
  template <typename T>
  T _freadt(std::istream &);
  std::string _freadstringnewline(std::istream &);
  std::string _freadfixedlengthstring(std::istream &, size_t, bool, size_t);
  bool _ends_with(std::string const &fullString, std::string const &ending);
  size_t _vidx_2d(size_t, size_t, size_t);
  struct MghHeader;
  struct Mgh;

  // NIfTI-1 forward declarations (needed by read_desc_data).
  void read_nifti(Mgh *, std::istream *, bool force_standard = false);
  void read_nifti(Mgh *, const std::string &, bool force_standard = false);
#ifdef LIBFS_HAS_ZLIB
  inline void read_nifti_gz(Mgh *, const std::string &, bool force_standard = false);
  inline void write_nifti_gz(const Mgh &, const std::string &);
#endif

  /// @brief Models a triangular mesh, used for brain surface meshes.
  ///
  /// @details Represents a vertex-indexed mesh. The `n` vertices are stored as 3D point coordinates (x,y,z) in a vector
  /// of length `3n`, in which 3 consecutive values represent the x, y and z coordinate of the same vertex.
  /// The `m` faces are stored as a vector of `3m` integers, where 3 consecutive values represent the 3 vertices (by index)
  /// making up the respective face. Vertex indices are 0-based.
  /// Optional per-vertex data (colors, normals, texture coordinates) use the same interleaved layout:
  /// all values for vertex 0 appear first, then vertex 1, etc. Empty vectors indicate absent data.
  /// #### Examples
  ///
  /// @code
  /// fs::Mesh surface = fs::Mesh::construct_cube();
  /// size_t nv = surface.num_vertices(); // 8
  /// auto first_face_verts = surface.face_vertices(0);
  /// int first_face_third_vert = surface.fm_at(0, 2);
  /// size_t nf = surface.num_faces();
  /// size_t nv = surface.num_vertices();
  /// surface.to_obj("cube_out.obj");
  /// @endcode
  struct Mesh
  {

    /// Construct a Mesh from the given vertices and faces.
    Mesh(std::vector<float> cvertices, std::vector<int32_t> cfaces)
    {
      vertices = cvertices;
      faces = cfaces;
    }

    /// @brief Construct a Mesh from 2-D vertex and face lists.
    /// @details Each inner vector represents one vertex (3 floats: x, y, z)
    ///          or one face (3 int32_t indices).  The input is flattened
    ///          internally into the 1-D `vertices` and `faces` member vectors
    ///          that the rest of the API expects.  This is a convenience
    ///          constructor for code that already works with per-vertex or
    ///          per-face arrays.
    /// @param cvertices Outer vector of per-vertex `{x, y, z}` triplets.
    /// @param cfaces Outer vector of per-face `{v0, v1, v2}` index triplets.
    Mesh(std::vector<std::vector<float>> cvertices, std::vector<std::vector<int32_t>> cfaces)
    {
      vertices = util::vflatten(cvertices);
      faces = util::vflatten(cfaces);
    }

    /// Construct an empty Mesh.
    Mesh() {}

    std::vector<float> vertices;   ///< *n x 3* vector of the *x*,*y*,*z* coordinates for the *n* vertices. The x,y,z coordinates for a single vertex form consecutive entries.
    std::vector<int32_t> faces;    ///< *n x 3* vector of the 3 vertex indices for the *n* triangles or faces. The 3 vertices of a single face form consecutive entries.
    std::vector<uint8_t> vertex_colors; ///< *n x 3* vector of RGB color values, 3 per vertex (v0_r, v0_g, v0_b, v1_r, ...). Empty if no vertex colors are available. Populated by from_ply() and from_off() when the file contains colors. Same interleave format as used by to_ply(col) / to_off(col).
    std::vector<float> vertex_normals;  ///< *n x 3* vector of normal vectors (nx,ny,nz), one per vertex in the same order as `vertices`. Empty if no normals are available. Populated by from_obj() when the OBJ file contains `vn` lines.
    std::vector<float> vertex_texcoords; ///< *n x 2* vector of texture coordinates (u,v), one per vertex in the same order as `vertices`. Empty if no texcoords are available. Populated by from_obj() when the OBJ file contains `vt` lines.

    /// @brief Return whether this mesh has per-vertex normals.
    /// @return true if `vertex_normals` is non-empty (and thus its size matches `vertices.size()`).
    bool has_normals() const { return !vertex_normals.empty(); }

    /// @brief Return whether this mesh has per-vertex texture coordinates.
    /// @return true if `vertex_texcoords` is non-empty (and thus its size matches `vertices.size() / 3 * 2`).
    bool has_texcoords() const { return !vertex_texcoords.empty(); }

    /// @brief Construct and return a simple cube mesh.
    /// @return fs::Mesh instance representing a cube.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// size_t nv = surface.num_vertices();  // 8
    /// size_t nf = surface.num_faces();     // 12
    /// @endcode
    static fs::Mesh construct_cube()
    {
      fs::Mesh mesh;
      mesh.vertices = {1.0, 1.0, 1.0,
                       1.0, 1.0, -1.0,
                       1.0, -1.0, 1.0,
                       1.0, -1.0, -1.0,
                       -1.0, 1.0, 1.0,
                       -1.0, 1.0, -1.0,
                       -1.0, -1.0, 1.0,
                       -1.0, -1.0, -1.0};
      mesh.faces = {0, 2, 3,
                    3, 1, 0,
                    4, 7, 6,
                    7, 4, 5,
                    0, 5, 4,
                    5, 0, 1,
                    2, 6, 7,
                    7, 3, 2,
                    0, 4, 6,
                    6, 2, 0,
                    1, 7, 5,
                    7, 1, 3};
      return mesh;
    }

    /// @brief Construct and return a simple pyramidal mesh.
    /// @details This constructs a right square pyramid with base edge length 1 and height 1. Think of the Great Pyramid of Giza.
    /// @return fs::Mesh instance representing a 4-sided pyramid.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_pyramid();
    /// size_t nv = surface.num_vertices();  // 5
    /// size_t nf = surface.num_faces();     // 6
    /// @endcode
    static fs::Mesh construct_pyramid()
    {
      fs::Mesh mesh;
      mesh.vertices = {0.0, 0.0, 0.0, // start with 4x base
                       0.0, 1.0, 0.0,
                       1.0, 1.0, 0.0,
                       1.0, 0.0, 0.0,
                       0.5, 0.5, 1.0}; // apex
      mesh.faces = {0, 1, 2,           // start with 2 base faces
                    0, 2, 3,
                    0, 4, 1, // now the 4 wall faces
                    1, 4, 2,
                    3, 2, 4,
                    0, 3, 4};
      return mesh;
    }

    /// @brief Construct and return a simple planar grid mesh.
    /// @details This is a 2D rectangular grid embedded in 3D. Each rectangular cell consists of 2 triangular faces. The height (z coordinate) for all vertices is `0.0`.
    /// @param nx number of vertices in x direction
    /// @param ny number of vertices in y direction
    /// @param distx distance between vertices in x direction
    /// @param disty distance between vertices in y direction
    /// @return fs::Mesh instance representing a flat grid.
    /// @throws std::invalid_argument error if `nx` or `ny` are `< 2`.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_grid(4, 5);
    /// size_t nv = surface.num_vertices();  // 4*5 = 20
    /// size_t nf = surface.num_faces();     // (4-1)*(5-1)*2 = 24;
    /// @endcode
    static fs::Mesh construct_grid(const size_t nx = 4, const size_t ny = 5, const float distx = 1.0, const float disty = 1.0)
    {
      if (nx < 2 || ny < 2)
      {
        throw std::runtime_error("Parameters nx and ny must be at least 2.");
      }
      fs::Mesh mesh;
      size_t num_vertices = nx * ny;
      size_t num_faces = ((nx - 1) * (ny - 1)) * 2;
      std::vector<float> vertices;
      vertices.reserve(num_vertices * 3);
      std::vector<int> faces;
      faces.reserve(num_faces * 3);

      // Create vertices.
      float cur_x, cur_y, cur_z;
      cur_x = cur_y = cur_z = 0.0;
      for (size_t i = 0; i < nx; i++)
      {
        cur_y = 0.0;
        for (size_t j = 0; j < ny; j++)
        {
          vertices.push_back(cur_x);
          vertices.push_back(cur_y);
          vertices.push_back(cur_z);
          cur_y += disty;
        }
        cur_x += distx;
      }

      // Create faces.
      for (size_t i = 0; i < num_vertices; i++)
      {
        if ((i + 1) % ny == 0 || i >= num_vertices - ny)
        {
          // Do not use the last ones in row or column as source.
          continue;
        }
        // Add the upper left triangle of this grid cell.
        faces.push_back(int(i));
        faces.push_back(int(i + ny + 1));
        faces.push_back(int(i + 1));
        // Add the lower right triangle of this grid cell.
        faces.push_back(int(i));
        faces.push_back(int(i + ny + 1));
        faces.push_back(int(i + ny));
      }

      mesh.vertices = vertices;
      mesh.faces = faces;
      return mesh;
    }

    /// @brief Return string representing the mesh in Wavefront Object (.obj) format.
    /// @return Wavefront Object string representation of the mesh, including vertices and faces.
    /// @see fs::Mesh::to_obj_file is a shortcut if you want to export the string representation to a file.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::str mesh_repr_off = surface.to_obj();
    /// @endcode
    std::string to_obj() const
    {
      std::vector<uint8_t> empty_col;
      return (this->to_obj(empty_col));
    }

    /// @brief Return string representing the mesh in Wavefront Object (.obj) format with vertex colors.
    /// @param col u_char vector of RGB color values, 3 per vertex. They must appear by vertex, i.e. in order v0_red, v0_green, v0_blue, v1_red, v1_green, v1_blue. Leave empty if you do not want colors.
    /// @details Colors are written using the widely-supported convention of 6 floats per vertex line: `v x y z r g b`, where RGB are floating-point values in [0, 1].
    /// If the mesh has vertex normals (`vertex_normals` non-empty), `vn` lines are emitted and faces use `v//vn` notation.
    /// If the mesh has texture coordinates (`vertex_texcoords` non-empty), `vt` lines are emitted and faces use `v/vt` or `v/vt/vn` notation.
    /// @throws std::invalid_argument if the number of vertex colors does not match the number of vertices.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::vector<uint8_t> col = surface.vertex_colors;
    /// std::string obj_rep = surface.to_obj(col);
    /// @endcode
    std::string to_obj(const std::vector<uint8_t> col) const
    {
      bool use_vertex_colors = col.size() != 0;
      bool use_normals = this->has_normals();
      bool use_texcoords = this->has_texcoords();

      std::stringstream objs;
      for (size_t vidx = 0; vidx < this->vertices.size(); vidx += 3)
      { // vertex coords
        objs << "v " << vertices[vidx] << " " << vertices[vidx + 1] << " " << vertices[vidx + 2];
        if (use_vertex_colors)
        {
          if (col.size() != this->vertices.size())
          {
            throw std::invalid_argument("Number of vertex coordinates and vertex colors must match when writing OBJ file, but got " + std::to_string(this->vertices.size()) + " and " + std::to_string(col.size()) + ".");
          }
          objs << " " << (col[vidx] / 255.0f) << " " << (col[vidx + 1] / 255.0f) << " " << (col[vidx + 2] / 255.0f);
        }
        objs << "\n";
      }

      // Emit texture coordinates if present.
      if (use_texcoords)
      {
        for (size_t vidx = 0; vidx < this->vertex_texcoords.size(); vidx += 2)
        {
          objs << "vt " << vertex_texcoords[vidx] << " " << vertex_texcoords[vidx + 1] << "\n";
        }
      }

      // Emit vertex normals if present.
      if (use_normals)
      {
        for (size_t vidx = 0; vidx < this->vertex_normals.size(); vidx += 3)
        {
          objs << "vn " << vertex_normals[vidx] << " " << vertex_normals[vidx + 1] << " " << vertex_normals[vidx + 2] << "\n";
        }
      }

      for (size_t fidx = 0; fidx < this->faces.size(); fidx += 3)
      { // faces: vertex indices, 1-based
        int v0 = faces[fidx] + 1;
        int v1 = faces[fidx + 1] + 1;
        int v2 = faces[fidx + 2] + 1;

        objs << "f ";
        if (use_texcoords && use_normals)
        {
          objs << v0 << "/" << v0 << "/" << v0 << " "
               << v1 << "/" << v1 << "/" << v1 << " "
               << v2 << "/" << v2 << "/" << v2;
        }
        else if (use_texcoords)
        {
          objs << v0 << "/" << v0 << " "
               << v1 << "/" << v1 << " "
               << v2 << "/" << v2;
        }
        else if (use_normals)
        {
          objs << v0 << "//" << v0 << " "
               << v1 << "//" << v1 << " "
               << v2 << "//" << v2;
        }
        else
        {
          objs << v0 << " " << v1 << " " << v2;
        }
        objs << "\n";
      }
      return (objs.str());
    }

    /// @brief Return adjacency matrix representation of this mesh.
    /// @return boolean 2D matrix, where true means an edge between the respective vertex pair exists, and false mean it does not.
    /// @see fs::Mesh::to_rep_adjlist gives you an adjacency list instead.
    /// @note This requires a lot of memory for large meshes.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::vector<std::vector<bool>> adjm = surface.as_adjmatrix();
    /// @endcode
    std::vector<std::vector<bool>> as_adjmatrix() const
    {
      std::vector<std::vector<bool>> adjm = std::vector<std::vector<bool>>(this->num_vertices(), std::vector<bool>(this->num_vertices(), false));
      for (size_t fidx = 0; fidx < this->faces.size(); fidx += 3)
      { // faces: vertex indices
        adjm[faces[fidx]][faces[fidx + 1]] = true;
        adjm[faces[fidx + 1]][faces[fidx]] = true;
        adjm[faces[fidx + 1]][faces[fidx + 2]] = true;
        adjm[faces[fidx + 2]][faces[fidx + 1]] = true;
        adjm[faces[fidx + 2]][faces[fidx]] = true;
        adjm[faces[fidx]][faces[fidx + 2]] = true;
      }
      return adjm;
    }

    /// @brief Hash function for 2-tuples of `<size_t, sizt_t>`, used to hash an edge of a graph or mesh.
    struct _tupleHashFunction
    {
      size_t operator()(const std::tuple<size_t, size_t> &x) const
      {
        size_t a = std::get<0>(x);
        size_t b = std::get<1>(x);
        return a ^ (b << 1) ^ (b >> (sizeof(size_t) * 8 - 1));
      }
    };

    /// @brief Datastructure for storing, and quickly querying the existence of, mesh edges.
    /// @details This is an unordered set of 2-tuples, where each tuple represents an edge, given as a pair of vertex indices. Each edge occurs twice in the list, once as `make_tuple(i,j)` and once as `make_tuple(j,i)`. Use the API of `std::unordered_set` to interact with it.
    typedef std::unordered_set<std::tuple<size_t, size_t>, _tupleHashFunction> edge_set;

    /// @brief Return edge list representation of this mesh.
    /// @note While this mesh or graph representation is typically known as an edge list, this function actually returns a set.
    /// @return an `fs::Mesh::edge_set`, i.e., an unordered set of 2-tuples, where each tuple represents an edge, given as a pair of vertex indices. Each edge occurs twice in the list, once as `make_tuple(i,j)` and once as `make_tuple(j,i)`.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// edge_set edges = surface.as_edgelist();
    /// size_t num_undirected_edges = edg es.size() / 2;
    /// @endcode
    edge_set as_edgelist() const
    {
      edge_set edges;
      for (size_t fidx = 0; fidx < this->faces.size(); fidx += 3)
      { // faces: vertex indices
        edges.insert(std::make_tuple(faces[fidx], faces[fidx + 1]));
        edges.insert(std::make_tuple(faces[fidx + 1], faces[fidx]));

        edges.insert(std::make_tuple(faces[fidx + 1], faces[fidx + 2]));
        edges.insert(std::make_tuple(faces[fidx + 2], faces[fidx + 1]));

        edges.insert(std::make_tuple(faces[fidx], faces[fidx + 2]));
        edges.insert(std::make_tuple(faces[fidx + 2], faces[fidx]));
      }
      return edges;
    }

    /// @brief Return adjacency list representation of this mesh.
    /// @param via_matrix whether the computation should be done via an  step involving an adjacency matrix, or via an edge set. Leaving this at `true` temporarily requires a lot of memory for large meshes, but is faster.
    /// @return vector of vectors, where the outer vector has size this->num_vertices. The inner vector at index N contains the M neighbors of vertex n, as vertex indices.
    /// @see fs::Mesh::to_rep_adjmatrix gives you an adjacency matrix instead.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::vector<std::vector<size_t>> adjl = surface.as_adjlist();
    /// std::vector<std::vector<size_t>> adjl1 = surface.as_adjlist(true);
    /// @endcode
    std::vector<std::vector<size_t>> as_adjlist(const bool via_matrix = true) const
    {
      if (!via_matrix)
      {
        return (this->_as_adjlist_via_edgeset());
      }
      std::vector<std::vector<bool>> adjm = this->as_adjmatrix();
      std::vector<std::vector<size_t>> adjl = std::vector<std::vector<size_t>>(this->num_vertices(), std::vector<size_t>());
      size_t nv = adjm.size();
      for (size_t i = 0; i < nv; i++)
      {
        for (size_t j = i + 1; j < nv; j++)
        {
          if (adjm[i][j] == true)
          {
            adjl[i].push_back(j);
            adjl[j].push_back(i);
          }
        }
      }
      return adjl;
    }

    /// @brief Return adjacency list representation of this mesh via edge list.
    /// @return vector of vectors, where the outer vector has size this->num_vertices. The inner vector at index N contains the M neighbors of vertex n, as vertex indices.
    /// @see fs::Mesh::to_rep_adjmatrix gives you an adjacency matrix instead.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::vector<std::vector<size_t>> adjl = surface.as_adjlist();
    /// @endcode
    std::vector<std::vector<size_t>> _as_adjlist_via_edgeset() const
    {
      edge_set edges = this->as_edgelist();
      std::vector<std::vector<size_t>> adjl = std::vector<std::vector<size_t>>(this->num_vertices(), std::vector<size_t>());
      for (const std::tuple<size_t, size_t> &e : edges)
      {
        adjl[std::get<0>(e)].push_back(std::get<1>(e));
      }
      return adjl;
    }

    /// @brief Smooth given per-vertex data using nearest neighbor smoothing.
    /// @param pvd vector of per-vertex data values, one value per mesh vertex.
    /// @param num_iter number of iterations of smoothing to perform.
    /// @param via_matrix passed on to `this->as_asjlist()`, whether to construct the adjacency list of the mesh using an intermediate step involving an adjacency matrix, as opposed to using an edge set. The latter is slower but requires less memory.
    /// @param with_nan whether you need support for NAN values in `pvd`. A bit slower if active. Ignored if `detectnan` is `true`.
    /// @param detect_nan whether to auto-detect presence of NAN values, ignoring the setting of `with_nan`.
    /// @return vector of smoothed per-vertex data values, same length as `pvd` param.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::vector<float> pvd = {1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7};
    /// std::vector<float> pvd_smooth = surface.smooth_pvd_nn(pvd, 2);
    /// @endcode
    std::vector<float> smooth_pvd_nn(const std::vector<float> pvd, const size_t num_iter = 1, const bool via_matrix = true, const bool with_nan = true, const bool detect_nan = true) const
    {

      const std::vector<std::vector<size_t>> adjlist = this->as_adjlist(via_matrix);
      return fs::Mesh::smooth_pvd_nn(adjlist, pvd, num_iter, with_nan, detect_nan);
    }

    /// @brief Smooth given per-vertex data using nearest neighbor smoothing based on adjacency list mesh represenation.
    /// @param mesh_adj the mesh, given as an adjacency list. The outer vector has size num_vertices, and the inner vectors sizes are the number of neighbors of the respective vertex.
    /// @param pvd vector of per-vertex data values, one value per mesh vertex. Must not include NAN values. See `smooth_pvd_nn_nan` if you need support for NAN values.
    /// @param num_iter number of iterations of smoothing to perform.
    /// @param with_nan whether you need support for NAN values in `pvd`. A bit slower if active. Ignored if `detectnan` is `true`.
    /// @param detect_nan whether to auto-detect presence of NAN values, ignoring the setting of `with_nan`.
    /// @return vector of smoothed per-vertex data values, same length as `pvd` param.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::vector<std::vector<size_t>> mesh_adj = surface.as_adjlist();
    /// std::vector<float> pvd = {1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7};
    /// std::vector<float> pvd_smooth = fs::Mesh::smooth_pvd_nn(mesh_adj, pvd, 2);
    /// @endcode
    static std::vector<float> smooth_pvd_nn(const std::vector<std::vector<size_t>> mesh_adj, const std::vector<float> pvd, const size_t num_iter = 1, const bool with_nan = true, const bool detect_nan = true)
    {
      assert(pvd.size() == mesh_adj.size());
      bool final_with_nan = with_nan;
      if (detect_nan)
      {
        final_with_nan = false;
        for (size_t i = 0; i < pvd.size(); i++)
        {
          if (std::isnan(pvd[i]))
          {
            final_with_nan = true;
            break;
          }
        }
      }
      if (final_with_nan)
      {
        return fs::Mesh::_smooth_pvd_nn_nan(mesh_adj, pvd, num_iter);
      }
      std::vector<float> current_pvd_source;
      std::vector<float> current_pvd_smoothed = std::vector<float>(pvd.size());

      float val_sum;
      size_t num_neigh;
      for (size_t i = 0; i < num_iter; i++)
      {
        if (i == 0)
        {
          current_pvd_source = pvd;
        }
        else
        {
          current_pvd_source = current_pvd_smoothed;
        }
        for (size_t v_idx = 0; v_idx < mesh_adj.size(); v_idx++)
        {
          num_neigh = mesh_adj[v_idx].size();
          val_sum = current_pvd_source[v_idx] / (num_neigh + 1);
          for (size_t neigh_rel_idx = 0; neigh_rel_idx < num_neigh; neigh_rel_idx++)
          {
            val_sum += current_pvd_source[mesh_adj[v_idx][neigh_rel_idx]] / (num_neigh + 1);
          }
          current_pvd_smoothed[v_idx] = val_sum;
        }
      }
      return current_pvd_smoothed;
    }

    /// @brief Smooth given per-vertex data including NAN values using nearest neighbor smoothing based on adjacency list mesh representation.
    /// @private
    /// @param mesh_adj the mesh, given as an adjacency list. The outer vector has size num_vertices, and the inner vectors sizes are the number of neighbors of the respective vertex.
    /// @param pvd vector of per-vertex data values, one value per mesh vertex. Must not include NAN values. See `smooth_pvd_nn_nan` if you need support for NAN values.
    /// @param num_iter number of iterations of smoothing to perform.
    /// @return vector of smoothed per-vertex data values, same length as `pvd` param.
    /// @note This function is private, users should call `fs::Mesh::smooth_pvd_nn` instead.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::vector<std::vector<size_t>> mesh_adj = surface.as_adjlist();
    /// std::vector<float> pvd = {1.0, 1.1, 1.2, NAN, 1.4, 1.5, 1.6, 1.7};
    /// std::vector<float> pvd_smooth = fs::Mesh::smooth_pvd_nn(mesh_adj, pvd, 2);
    /// @endcode
    static std::vector<float> _smooth_pvd_nn_nan(const std::vector<std::vector<size_t>> mesh_adj, const std::vector<float> pvd, const size_t num_iter = 1)
    {
      std::vector<float> current_pvd_source;
      std::vector<float> current_pvd_smoothed = std::vector<float>(pvd.size());

      float val_sum;
      size_t num_neigh;
      size_t num_non_nan_values;
      float neigh_val;
      for (size_t i = 0; i < num_iter; i++)
      {

        if (i == 0)
        {
          current_pvd_source = pvd;
        }
        else
        {
          current_pvd_source = current_pvd_smoothed;
        }

        for (size_t v_idx = 0; v_idx < mesh_adj.size(); v_idx++)
        {
          if (std::isnan(current_pvd_source[v_idx]))
          {
            current_pvd_smoothed[v_idx] = NAN;
            continue;
          }
          val_sum = current_pvd_source[v_idx];
          num_non_nan_values = 1; // If we get here, the source vertex value is not NAN.
          num_neigh = mesh_adj[v_idx].size();
          for (size_t neigh_rel_idx = 0; neigh_rel_idx < num_neigh; neigh_rel_idx++)
          {
            neigh_val = current_pvd_source[mesh_adj[v_idx][neigh_rel_idx]];
            if (std::isnan(neigh_val))
            {
              continue;
            }
            else
            {
              val_sum += neigh_val;
              num_non_nan_values++;
            }
          }
          current_pvd_smoothed[v_idx] = val_sum / (float)num_non_nan_values;
        }
      }
      return current_pvd_smoothed;
    }

    /// @brief Extend mesh neighborhoods based on mesh adjacency representation.
    /// @details This function is mainly extended to extend a source neighborhood representation (typically the mesh's `k=1` neighborhood, i.e., the adjacency list of the mesh) to a higher `k`. In a `k=3` neighborhood, the neighorhood around a source vertex includes all vertices in edge distance up to 3 from the source vertex (but not the source vertex itself).
    /// @param mesh_adj The adjacency list representation of the underlying mesh, the outer vector must have size `N` for a mesh with `N` vertices.
    /// @param extend_by the number of edges to hop to extend the neighborhoods.
    /// @param mesh_adj_ext the starting neighborhoods to extend, same representation as `mesh_adj`. The outer vector must have size `N` or `0`. If passed as an empty vector, this will be ignored and a copy of the `mesh_adj` is used as the `start_neighborhoods`.
    /// @return extended neighborhoods
    static std::vector<std::vector<size_t>> extend_adj(const std::vector<std::vector<size_t>> mesh_adj, const size_t extend_by = 1, std::vector<std::vector<size_t>> mesh_adj_ext = std::vector<std::vector<size_t>>())
    {
      size_t num_vertices = mesh_adj.size();
      if (mesh_adj_ext.size() == 0)
      {
        mesh_adj_ext = mesh_adj;
      }
      std::vector<size_t> neighborhood;
      std::vector<size_t> ext_neighborhood;
      for (size_t ext_idx = 0; ext_idx < extend_by; ext_idx++)
      {
        for (size_t source_vert_idx = 0; source_vert_idx < num_vertices; source_vert_idx++)
        {
          neighborhood = mesh_adj_ext[source_vert_idx]; // copy needed so we do not modify during iteration.
          // Extension: add all neighbors in distance one for all vertices in the neighborhood.
          for (size_t neigh_vert_rel_idx = 0; neigh_vert_rel_idx < neighborhood.size(); neigh_vert_rel_idx++)
          {
            for (size_t canidate_rel_idx = 0; canidate_rel_idx < mesh_adj[neighborhood[neigh_vert_rel_idx]].size(); canidate_rel_idx++)
            {
              if (mesh_adj[neighborhood[neigh_vert_rel_idx]][canidate_rel_idx] != source_vert_idx)
              {
                mesh_adj_ext[source_vert_idx].push_back(mesh_adj[neighborhood[neigh_vert_rel_idx]][canidate_rel_idx]);
              }
            }
          }
          // We need to remove duplicates.
          std::sort(mesh_adj_ext[source_vert_idx].begin(), mesh_adj_ext[source_vert_idx].end());
          mesh_adj_ext[source_vert_idx].erase(std::unique(mesh_adj_ext[source_vert_idx].begin(), mesh_adj_ext[source_vert_idx].end()), mesh_adj_ext[source_vert_idx].end());
        }
      }
      return mesh_adj_ext;
    }

    /// @brief Export this mesh to a file in Wavefront OBJ format.
    /// @param filename path to the output file, will be overwritten if existing.
    /// @throws std::runtime_error if the target file cannot be opened.
    /// @see fs::Mesh::to_obj if you want the string representation (without writing it to a file).
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// const std::string out_path = fs::util::fullpath({"/tmp", "mesh.obj"});
    /// surface.to_obj_file(out_path);
    /// @endcode
    void to_obj_file(const std::string &filename) const
    {
      fs::util::str_to_file(filename, this->to_obj());
    }

    /// @brief Export this mesh to a file in Wavefront OBJ format with vertex colors.
    /// @throws std::runtime_error if the target file cannot be opened, std::invalid_argument if the number of vertex colors does not match the number of vertices.
    void to_obj_file(const std::string &filename, const std::vector<uint8_t> col) const
    {
      fs::util::str_to_file(filename, this->to_obj(col));
    }

    /// @brief Compute a new mesh that is a submesh of this mesh, based on a subset of the vertices of this mesh.
    /// @param old_vertex_indices vector of vertex indices of this mesh, which should be included in the submesh.
    /// @param mapdir_fulltosubmesh whether to return a map from the old (full mesh) to the new (submesh)  vertex indices (`true`), or the other way around (`false`, the default) as the first element of the returned pair.
    /// @return a pair of the vertex index map (direction 'fullmesh to submesh' by default, but see 'mapdir_fulltosubmesh' parameter) and the submesh.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface;
    /// fs::read_surf(&surface, "examples/read_surf/lh.white");
    /// fs::Label label;
    /// fs::read_label(&label, "examples/read_label/lh.cortex.label");
    /// std::pair <std::unordered_map<int32_t, int32_t>, fs::Mesh> result = surface.submesh_vertex(label.vertex);
    /// fs::Mesh patch = result.second;
    /// auto vertexindexmap_submesh2full = result.first; // or '<std::unordered_map<int32_t, int32_t>' instead of 'auto'.
    /// @endcode
    std::pair<std::unordered_map<int32_t, int32_t>, fs::Mesh> submesh_vertex(const std::vector<int32_t> &old_vertex_indices, const bool mapdir_fulltosubmesh = false) const
    {
      fs::Mesh submesh;
      std::vector<float> new_vertices;
      std::vector<int> new_faces;
      std::unordered_map<int32_t, int32_t> vertex_index_map_full2submesh;
      int32_t new_vertex_idx = 0;
      for (size_t i = 0; i < old_vertex_indices.size(); i++)
      {
        vertex_index_map_full2submesh[old_vertex_indices[i]] = new_vertex_idx;
        new_vertices.push_back(this->vertices[size_t(old_vertex_indices[i]) * 3]);
        new_vertices.push_back(this->vertices[size_t(old_vertex_indices[i]) * 3 + 1]);
        new_vertices.push_back(this->vertices[size_t(old_vertex_indices[i]) * 3 + 2]);
        new_vertex_idx++;
      }
      int face_v0;
      int face_v1;
      int face_v2;
      for (size_t i = 0; i < this->num_faces(); i++)
      {
        face_v0 = this->faces[i * 3];
        face_v1 = this->faces[i * 3 + 1];
        face_v2 = this->faces[i * 3 + 2];
        if ((vertex_index_map_full2submesh.find(face_v0) != vertex_index_map_full2submesh.end()) && (vertex_index_map_full2submesh.find(face_v1) != vertex_index_map_full2submesh.end()) && (vertex_index_map_full2submesh.find(face_v2) != vertex_index_map_full2submesh.end()))
        {
          new_faces.push_back(vertex_index_map_full2submesh[face_v0]);
          new_faces.push_back(vertex_index_map_full2submesh[face_v1]);
          new_faces.push_back(vertex_index_map_full2submesh[face_v2]);
        }
      }
      submesh.vertices = new_vertices;
      submesh.faces = new_faces;

      std::pair<std::unordered_map<int32_t, int32_t>, fs::Mesh> result;
      if (!mapdir_fulltosubmesh)
      { // Compute the new2old (reverse) vertex index map:
        std::unordered_map<int32_t, int32_t> vertex_index_map_submesh2full;
        for (auto const &pair : vertex_index_map_full2submesh)
        {
          vertex_index_map_submesh2full[pair.second] = pair.first;
        }
        result = std::pair<std::unordered_map<int32_t, int32_t>, fs::Mesh>(vertex_index_map_submesh2full, submesh);
      }
      else
      {
        result = std::pair<std::unordered_map<int32_t, int32_t>, fs::Mesh>(vertex_index_map_full2submesh, submesh);
      }

      return result;
    }

    /// @brief Given per-vertex data for a submesh, expand it back to full mesh size.
    /// @details Takes per-vertex scalar data that was computed only on a
    ///          submesh and maps it back onto the original, full-resolution
    ///          mesh.  Vertices that exist in the original mesh but are
    ///          absent from the submesh receive `fill_value`.
    /// @param data_submesh vector of per-vertex data values, one value per mesh vertex of the submesh.
    /// @param submesh_to_orig_mapping map<int, int>, mapping vertex indices of the submesh to vertex indices of the original, full mesh.
    /// @param orig_mesh_num_vertices number of vertices of the original, full mesh.
    /// @param fill_value value assigned to original-mesh vertices that are
    ///        not present in the submesh.  Defaults to
    ///        `std::numeric_limits<float>::quiet_NaN()`, which is
    ///        appropriate for curvature / thickness overlays (NaN renders
    ///        as transparent in most viewers).  Set to `0.0f` or another
    ///        sentinel if downstream code cannot handle NaNs.
    /// @see `fs::Mesh::submesh_vertex` for how to get the `submesh_to_orig_mapping` parameter.
    /// @return vector of per-vertex data values, one value per mesh vertex of the original mesh. Values for vertices that are not part of the submesh are set to `fill_value`.
    static std::vector<float> curv_data_for_orig_mesh(const std::vector<float> data_submesh, const std::unordered_map<int32_t, int32_t> submesh_to_orig_mapping, const int32_t orig_mesh_num_vertices, const float fill_value = std::numeric_limits<float>::quiet_NaN())
    {

      if (submesh_to_orig_mapping.size() != data_submesh.size())
      {
        throw std::domain_error("The number of vertices of the submesh and the number of values in the submesh_to_orig_mapping do not match: got " + std::to_string(data_submesh.size()) + " and " + std::to_string(submesh_to_orig_mapping.size()) + ".");
      }

      std::vector<float> data_orig_mesh(orig_mesh_num_vertices, fill_value);
      for (size_t i = 0; i < data_submesh.size(); i++)
      {
        auto got = submesh_to_orig_mapping.find(int(i));
        if (got != submesh_to_orig_mapping.end())
        {
          data_orig_mesh[got->second] = data_submesh[i];
        }
      }
      return (data_orig_mesh);
    }

    /// @brief Read a brainmesh from a Wavefront object format stream.
    /// @details This only reads the geometry, optional format extensions like materials are ignored (but files including them should parse fine).
    /// @param mesh pointer to fs:Mesh instance to be filled.
    /// @param is stream holding a text representation of a mesh in Wavefront object format.
    /// @see There exists an overloaded version that reads from a file.
    /// @throws std::domain_error if the file format is invalid.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface;
    /// const std::string in_path = fs::util::fullpath({"/tmp", "mesh.obj"});
    /// fs::Mesh::from_obj(&surface, in_path);
    /// @endcode
    static void from_obj(Mesh *mesh, std::istream *is)
    {
      // -- Security: null-pointer check (plan #8) --
      if (!mesh)
      {
        throw std::invalid_argument("mesh pointer must not be null");
      }

      std::string line;
      int line_idx = -1;
      size_t total_lines_processed = 0;

      std::vector<float> vertices;
      std::vector<int> faces;
      std::vector<uint8_t> vertex_colors;
      std::vector<float> raw_normals;    // raw `vn` data: 3 floats per normal, 1-based indexed
      std::vector<float> raw_texcoords;  // raw `vt` data: 2 floats per texcoord, 1-based indexed
      std::vector<int> face_vt_indices;  // per-face-vertex texcoord index (0 = absent), parallel to `faces`
      std::vector<int> face_vn_indices;  // per-face-vertex normal index (0 = absent), parallel to `faces`
      bool has_any_vt = false;
      bool has_any_vn = false;
      int detected_format = -1; // -1 = unknown, 0 = no vertex colors, 1 = has vertex colors (r g b after x y z)

#ifdef LIBFS_DBG_INFO
      size_t num_lines_ignored = 0; // Not comments, but custom extensions or material data lines which are ignored by libfs.
#endif

      while (total_lines_processed < LIBFS_MAX_OBJ_LINES)
      {
        // -- Use std::getline for performance (buffered I/O), then post-check length (plan #10) --
        if (!std::getline(*is, line))
        {
          break; // EOF or read error
        }
        total_lines_processed++;
        line_idx++;

        if (line.size() > LIBFS_MAX_OBJ_LINE_LENGTH)
        {
          throw std::runtime_error("OBJ line " + std::to_string(line_idx + 1) +
                                   " exceeds maximum allowed line length of " +
                                   std::to_string(LIBFS_MAX_OBJ_LINE_LENGTH) + " bytes.\n");
        }

        // -- Security: check allocation limits before parsing this line (plan #5) --
        if (!util::check_alloc(vertices.size() + 3, sizeof(float)) ||
            !util::check_alloc(faces.size() + 12, sizeof(int)) ||
            !util::check_alloc(vertex_colors.size() + 3, sizeof(uint8_t)))
        {
          throw std::runtime_error("OBJ data exceeds maximum allowed memory allocation (" + std::to_string(LIBFS_MAX_ALLOC_BYTES) + " bytes).\n");
        }

        std::istringstream iss(line);
        if (fs::util::starts_with(line, "#"))
        {
          continue; // skip comment.
        }
        else
        {
          if (fs::util::starts_with(line, "v "))
          {
            std::string elem_type_identifier;
            float x, y, z;
            if (!(iss >> elem_type_identifier >> x >> y >> z))
            {
              throw std::domain_error("Could not parse vertex line " + std::to_string(line_idx + 1) + " of OBJ data, invalid format.\n");
            }
            assert(elem_type_identifier == "v");

            // -- Security: validate finite coordinates --
            if (!util::is_finite_float(x) || !util::is_finite_float(y) || !util::is_finite_float(z))
            {
              throw std::domain_error("Non-finite vertex coordinate on line " + std::to_string(line_idx + 1) + " of OBJ data.\n");
            }

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Check for optional per-vertex colors: 6-value lines (x y z r g b) have colors,
            // 3-value lines (x y z) and 4-value lines (x y z w) do not.
            // Detect the format from the first vertex line.
            if (detected_format == -1)
            {
              float vr, vg, vb;
              if ((iss >> vr >> vg >> vb))
              {
                // We read 3 more floats successfully. Check if there is even more data
                // (e.g., x y z w nx ny nz) — if so, treat as no-colors format.
                float extra;
                if (iss >> extra)
                {
                  detected_format = 0;
                }
                else
                {
                  detected_format = 1;
                  // Store colors for the first vertex (already consumed from stream).
                  int ri = static_cast<int>(vr * 255.0f + 0.5f);
                  int gi = static_cast<int>(vg * 255.0f + 0.5f);
                  int bi = static_cast<int>(vb * 255.0f + 0.5f);
                  if (ri < 0) { ri = 0; }
                  if (ri > 255) { ri = 255; }
                  if (gi < 0) { gi = 0; }
                  if (gi > 255) { gi = 255; }
                  if (bi < 0) { bi = 0; }
                  if (bi > 255) { bi = 255; }
                  vertex_colors.push_back(static_cast<uint8_t>(ri));
                  vertex_colors.push_back(static_cast<uint8_t>(gi));
                  vertex_colors.push_back(static_cast<uint8_t>(bi));
                }
              }
              else
              {
                detected_format = 0;
              }
            }
            else if (detected_format == 1)
            {
              // Read colors for subsequent vertices.
              float vr, vg, vb;
              if (!(iss >> vr >> vg >> vb))
              {
                throw std::domain_error("Expected vertex colors (r g b) on line " + std::to_string(line_idx + 1) + " of OBJ data, but could not parse them.\n");
              }
              int ri = static_cast<int>(vr * 255.0f + 0.5f);
              int gi = static_cast<int>(vg * 255.0f + 0.5f);
              int bi = static_cast<int>(vb * 255.0f + 0.5f);
              if (ri < 0) { ri = 0; }
              if (ri > 255) { ri = 255; }
              if (gi < 0) { gi = 0; }
              if (gi > 255) { gi = 255; }
              if (bi < 0) { bi = 0; }
              if (bi > 255) { bi = 255; }
              vertex_colors.push_back(static_cast<uint8_t>(ri));
              vertex_colors.push_back(static_cast<uint8_t>(gi));
              vertex_colors.push_back(static_cast<uint8_t>(bi));
            }
          }
          else if (fs::util::starts_with(line, "vn "))
          {
            // -- Feature: parse vertex normals (plan #3) --
            std::string elem_type_identifier;
            float nx, ny, nz;
            if (!(iss >> elem_type_identifier >> nx >> ny >> nz))
            {
              throw std::domain_error("Could not parse vertex normal line " + std::to_string(line_idx + 1) + " of OBJ data, invalid format.\n");
            }
            assert(elem_type_identifier == "vn");
            if (!util::is_finite_float(nx) || !util::is_finite_float(ny) || !util::is_finite_float(nz))
            {
              throw std::domain_error("Non-finite vertex normal on line " + std::to_string(line_idx + 1) + " of OBJ data.\n");
            }
            raw_normals.push_back(nx);
            raw_normals.push_back(ny);
            raw_normals.push_back(nz);
          }
          else if (fs::util::starts_with(line, "vt "))
          {
            // -- Feature: parse texture coordinates (plan #4) --
            std::string elem_type_identifier;
            float u, v;
            if (!(iss >> elem_type_identifier >> u >> v))
            {
              throw std::domain_error("Could not parse texture coordinate line " + std::to_string(line_idx + 1) + " of OBJ data, invalid format.\n");
            }
            assert(elem_type_identifier == "vt");
            if (!util::is_finite_float(u) || !util::is_finite_float(v))
            {
              throw std::domain_error("Non-finite texture coordinate on line " + std::to_string(line_idx + 1) + " of OBJ data.\n");
            }
            raw_texcoords.push_back(u);
            raw_texcoords.push_back(v);
            // Ignore optional 3rd component (w), if present.
          }
          else if (fs::util::starts_with(line, "f "))
          {
            std::string elem_type_identifier;

            // -- Feature: read all vertex tokens (plan #1) --
            if (!(iss >> elem_type_identifier))
            {
              throw std::domain_error("Could not parse face line " + std::to_string(line_idx + 1) + " of OBJ data, invalid format.\n");
            }
            assert(elem_type_identifier == "f");

            std::vector<std::string> face_tokens;
            {
              std::string token;
              while (iss >> token)
              {
                face_tokens.push_back(token);
              }
            }

            if (face_tokens.size() < 3)
            {
              throw std::domain_error("Face line " + std::to_string(line_idx + 1) + " has fewer than 3 vertices, invalid format.\n");
            }

            // -- Warning for quads/n-gons (plan #13) --
#ifdef LIBFS_DBG_WARNING
            if (face_tokens.size() > 3)
            {
              std::cout << LIBFS_APPTAG << "[WARNING] Face line " << (line_idx + 1)
                        << " has " << face_tokens.size() << " vertices; fan-triangulating.\n";
            }
#endif

            // Parse all vertex indices from tokens, properly splitting v/vt/vn.
            std::vector<int> raw_indices;
            std::vector<int> tok_vt_indices;
            std::vector<int> tok_vn_indices;
            raw_indices.reserve(face_tokens.size());
            tok_vt_indices.reserve(face_tokens.size());
            tok_vn_indices.reserve(face_tokens.size());
            for (size_t ti = 0; ti < face_tokens.size(); ti++)
            {
              const std::string &token = face_tokens[ti];

              // Split "v/vt/vn", "v//vn", "v/vt", or just "v".
              // Count slashes to determine format.
              size_t slash1 = token.find('/');
              std::string v_part, vt_part, vn_part;
              if (slash1 == std::string::npos)
              {
                // "v" only
                v_part = token;
              }
              else
              {
                v_part = token.substr(0, slash1);
                size_t slash2 = token.find('/', slash1 + 1);
                if (slash2 == std::string::npos)
                {
                  // "v/vt" — texcoord only, no normal
                  vt_part = token.substr(slash1 + 1);
                }
                else
                {
                  // "v//vn" or "v/vt/vn"
                  if (slash2 == slash1 + 1)
                  {
                    // "v//vn" — empty texcoord field
                    vn_part = token.substr(slash2 + 1);
                  }
                  else
                  {
                    // "v/vt/vn"
                    vt_part = token.substr(slash1 + 1, slash2 - slash1 - 1);
                    vn_part = token.substr(slash2 + 1);
                  }
                }
              }

              // Parse vertex index.
              int vi;
              try { vi = std::stoi(v_part); }
              catch (const std::invalid_argument &) {
                throw std::domain_error("Invalid face vertex index '" + v_part + "' on line " + std::to_string(line_idx + 1) + " of OBJ data.\n");
              }
              catch (const std::out_of_range &) {
                throw std::domain_error("Face vertex index '" + v_part + "' out of integer range on line " + std::to_string(line_idx + 1) + " of OBJ data.\n");
              }
              raw_indices.push_back(vi);

              // Parse optional texcoord index.
              int vti = 0;
              if (!vt_part.empty())
              {
                try { vti = std::stoi(vt_part); }
                catch (const std::invalid_argument &) {
                  throw std::domain_error("Invalid texcoord index '" + vt_part + "' on line " + std::to_string(line_idx + 1) + " of OBJ data.\n");
                }
                catch (const std::out_of_range &) {
                  throw std::domain_error("Texcoord index '" + vt_part + "' out of integer range on line " + std::to_string(line_idx + 1) + " of OBJ data.\n");
                }
                has_any_vt = true;
              }
              tok_vt_indices.push_back(vti);

              // Parse optional normal index.
              int vni = 0;
              if (!vn_part.empty())
              {
                try { vni = std::stoi(vn_part); }
                catch (const std::invalid_argument &) {
                  throw std::domain_error("Invalid normal index '" + vn_part + "' on line " + std::to_string(line_idx + 1) + " of OBJ data.\n");
                }
                catch (const std::out_of_range &) {
                  throw std::domain_error("Normal index '" + vn_part + "' out of integer range on line " + std::to_string(line_idx + 1) + " of OBJ data.\n");
                }
                has_any_vn = true;
              }
              tok_vn_indices.push_back(vni);
            }

            // -- Feature: fan triangulation for quads and n-gons (plan #1) --
            // Emit triangles: (v[0], v[1], v[2]), (v[0], v[2], v[3]), ...
            // Indices are stored as-is (1-based, possibly negative) and resolved in the post-parse pass.
            for (size_t ti = 1; ti + 1 < raw_indices.size(); ti++)
            {
              // Vertex indices
              faces.push_back(raw_indices[0]);
              faces.push_back(raw_indices[ti]);
              faces.push_back(raw_indices[ti + 1]);
              // Parallel texcoord indices
              face_vt_indices.push_back(tok_vt_indices[0]);
              face_vt_indices.push_back(tok_vt_indices[ti]);
              face_vt_indices.push_back(tok_vt_indices[ti + 1]);
              // Parallel normal indices
              face_vn_indices.push_back(tok_vn_indices[0]);
              face_vn_indices.push_back(tok_vn_indices[ti]);
              face_vn_indices.push_back(tok_vn_indices[ti + 1]);
            }
          }
          else
          {
#ifdef LIBFS_DBG_INFO
            num_lines_ignored++;
#endif

            continue;
          }
        }
      }

      // -- Security: check if we hit the max-lines limit (plan #11) --
      if (total_lines_processed >= LIBFS_MAX_OBJ_LINES && !is->eof())
      {
        throw std::runtime_error("OBJ file exceeds maximum allowed line count of " + std::to_string(LIBFS_MAX_OBJ_LINES) + ".\n");
      }

#ifdef LIBFS_DBG_INFO
      if (num_lines_ignored > 0)
      {
        std::cout << LIBFS_APPTAG << "Ignored " << num_lines_ignored << " lines in Wavefront OBJ format mesh file.\n";
      }
#endif

      // -- Post-parse: resolve negative indices and convert to 0-based (plan #2, #6, #12) --
      int32_t nv = static_cast<int32_t>(vertices.size() / 3);
      if (nv == 0)
      {
        throw std::domain_error("OBJ file contains no vertices.\n");
      }

      for (size_t fi = 0; fi < faces.size(); fi++)
      {
        int idx = faces[fi];
        if (idx == 0)
        {
          throw std::domain_error("Face index 0 in OBJ data: OBJ indices are 1-based, index 0 is invalid.\n");
        }
        if (idx < 0)
        {
          // -- Security: guard against integer overflow in negative-index resolution (plan #12) --
          if (idx < -nv)
          {
            throw std::domain_error("Negative face index " + std::to_string(idx) + " exceeds vertex count " + std::to_string(nv) + ".\n");
          }
          // Convert negative 1-based-relative to 0-based: -1 → nv-1, -2 → nv-2, etc.
          idx = nv + idx;
        }
        else
        {
          // Convert positive 1-based to 0-based.
          idx = idx - 1;
        }

        // -- Security: validate face index range (plan #6) --
        if (idx < 0 || idx >= nv)
        {
          throw std::domain_error("Face index " + std::to_string(idx) + " out of range [0, " + std::to_string(nv - 1) + "] after resolution.\n");
        }

        faces[fi] = idx;
      }

      // -- Integrity: validate face count is a multiple of 3 (plan #14) --
      if (faces.size() % 3 != 0)
      {
        throw std::domain_error("Internal error: parsed face count " + std::to_string(faces.size()) + " is not a multiple of 3.\n");
      }

      // -- Post-parse: vertex deduplication by (position, texcoord, normal) tuple --
      // OBJ's data model is per-face-corner: the same vertex position can appear
      // with different texcoords/normals in different faces (at texture seams and
      // sharp edges). We build a new vertex list where each unique combination
      // becomes its own vertex, correctly handling these cases.
      int32_t num_normals = static_cast<int32_t>(raw_normals.size() / 3);
      int32_t num_texcoords = static_cast<int32_t>(raw_texcoords.size() / 2);

      if (has_any_vn && num_normals == 0)
      {
        throw std::domain_error("OBJ file references vertex normals in faces but contains no 'vn' lines.\n");
      }
      if (has_any_vt && num_texcoords == 0)
      {
        throw std::domain_error("OBJ file references texture coordinates in faces but contains no 'vt' lines.\n");
      }

      // Tuple key: (vertex_index, texcoord_index, normal_index)
      // Use -1 for absent texcoord or normal.
      using CornerKey = std::tuple<int32_t, int32_t, int32_t>;
      std::map<CornerKey, int32_t> corner_to_new_vertex;

      std::vector<float> dedup_vertices;
      std::vector<float> dedup_texcoords;
      std::vector<float> dedup_normals;
      std::vector<uint8_t> dedup_vertex_colors;
      std::vector<int> dedup_faces;

      size_t num_face_corners = faces.size();
      dedup_faces.reserve(num_face_corners);

      for (size_t fi = 0; fi < num_face_corners; fi++)
      {
        int32_t vidx = static_cast<int32_t>(faces[fi]); // already 0-based vertex index

        // Resolve texcoord index: -1 means absent.
        int32_t tidx = -1;
        if (has_any_vt)
        {
          int vt_raw = face_vt_indices[fi];
          if (vt_raw != 0)
          {
            if (vt_raw < 0)
            {
              if (vt_raw < -num_texcoords)
              {
                throw std::domain_error("Negative texcoord index " + std::to_string(vt_raw) + " exceeds texcoord count " + std::to_string(num_texcoords) + ".\n");
              }
              tidx = num_texcoords + vt_raw; // -1 → num_texcoords-1
            }
            else
            {
              tidx = vt_raw - 1;
            }
            if (tidx < 0 || tidx >= num_texcoords)
            {
              throw std::domain_error("Texcoord index out of range [1, " + std::to_string(num_texcoords) + "].\n");
            }
          }
        }

        // Resolve normal index: -1 means absent.
        int32_t nidx = -1;
        if (has_any_vn)
        {
          int vn_raw = face_vn_indices[fi];
          if (vn_raw != 0)
          {
            if (vn_raw < 0)
            {
              if (vn_raw < -num_normals)
              {
                throw std::domain_error("Negative normal index " + std::to_string(vn_raw) + " exceeds normal count " + std::to_string(num_normals) + ".\n");
              }
              nidx = num_normals + vn_raw; // -1 → num_normals-1
            }
            else
            {
              nidx = vn_raw - 1;
            }
            if (nidx < 0 || nidx >= num_normals)
            {
              throw std::domain_error("Normal index out of range [1, " + std::to_string(num_normals) + "].\n");
            }
          }
        }

        CornerKey key(vidx, tidx, nidx);
        auto it = corner_to_new_vertex.find(key);
        if (it != corner_to_new_vertex.end())
        {
          // Existing combination: reuse index.
          dedup_faces.push_back(it->second);
        }
        else
        {
          // New combination: create vertex entry.
          int32_t new_idx = static_cast<int32_t>(dedup_vertices.size() / 3);
          corner_to_new_vertex[key] = new_idx;
          dedup_faces.push_back(new_idx);

          // Copy vertex position.
          dedup_vertices.push_back(vertices[static_cast<size_t>(vidx) * 3]);
          dedup_vertices.push_back(vertices[static_cast<size_t>(vidx) * 3 + 1]);
          dedup_vertices.push_back(vertices[static_cast<size_t>(vidx) * 3 + 2]);

          // Copy texcoord (or 0,0 if absent).
          if (tidx >= 0)
          {
            dedup_texcoords.push_back(raw_texcoords[static_cast<size_t>(tidx) * 2]);
            dedup_texcoords.push_back(raw_texcoords[static_cast<size_t>(tidx) * 2 + 1]);
          }
          else if (has_any_vt)
          {
            dedup_texcoords.push_back(0.0f);
            dedup_texcoords.push_back(0.0f);
          }

          // Copy normal (or 0,0,0 if absent).
          if (nidx >= 0)
          {
            dedup_normals.push_back(raw_normals[static_cast<size_t>(nidx) * 3]);
            dedup_normals.push_back(raw_normals[static_cast<size_t>(nidx) * 3 + 1]);
            dedup_normals.push_back(raw_normals[static_cast<size_t>(nidx) * 3 + 2]);
          }
          else if (has_any_vn)
          {
            dedup_normals.push_back(0.0f);
            dedup_normals.push_back(0.0f);
            dedup_normals.push_back(0.0f);
          }

          // Copy vertex color from the source position vertex (if colors exist).
          if (detected_format == 1)
          {
            dedup_vertex_colors.push_back(vertex_colors[static_cast<size_t>(vidx) * 3]);
            dedup_vertex_colors.push_back(vertex_colors[static_cast<size_t>(vidx) * 3 + 1]);
            dedup_vertex_colors.push_back(vertex_colors[static_cast<size_t>(vidx) * 3 + 2]);
          }
        }
      }

      mesh->vertices = dedup_vertices;
      mesh->faces = dedup_faces;
      mesh->vertex_colors = dedup_vertex_colors;
      mesh->vertex_normals = dedup_normals;
      mesh->vertex_texcoords = dedup_texcoords;
    }

    /// @brief Read a brainmesh from a Wavefront object format mesh file.
    /// @details This only reads the geometry, optional format extensions like materials are ignored (but files including them should parse fine).
    /// @see There exists an overloaded version that reads from a stream.
    /// @param mesh pointer to fs:Mesh instance to be filled.
    /// @param filename path to input wavefront obj mesh to be read.
    /// @throws std::runtime_error if the file cannot be read.
    /// @throws std::domain_error if the file format is invalid.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface;
    /// fs::Mesh::from_obj(&surface, "mesh.obj");
    /// @endcode
    static void from_obj(Mesh *mesh, const std::string &filename)
    {
#ifdef LIBFS_DBG_INFO
      std::cout << LIBFS_APPTAG << "Reading brain mesh from Wavefront object format file " << filename << ".\n";
#endif
      // -- Security: file-size pre-check to reject huge files before parsing (plan #9) --
      size_t file_size = util::get_file_size(filename);
      if (file_size > LIBFS_MAX_OBJ_FILE_SIZE)
      {
        throw std::runtime_error("OBJ file '" + filename + "' size (" + std::to_string(file_size) +
                                 " bytes) exceeds maximum allowed (" + std::to_string(LIBFS_MAX_OBJ_FILE_SIZE) + " bytes).\n");
      }

      std::ifstream input(filename, std::fstream::in);
      if (input.is_open())
      {
        Mesh::from_obj(mesh, &input);
        input.close();
      }
      else
      {
        throw std::runtime_error("Could not open Wavefront object format mesh file '" + filename + "' for reading.\n");
      }
    }

    /// @brief Read a brainmesh from an Object File format (OFF) stream.
    /// @param mesh pointer to fs:Mesh instance to be filled.
    /// @param is An open std::istream or derived class stream from which to read the data, e.g., std::ifstream or std::istringstream.
    /// @param source_filename optional, used in error messages only. The source file name, if any.
    /// @see There exists an overloaded version that reads from a file.
    /// @throws std::domain_error if the file format is invalid.
    static void from_off(Mesh *mesh, std::istream *is, const std::string &source_filename = "")
    {
      // -- Security: null-pointer check (O3) --
      if (!mesh)
      {
        throw std::invalid_argument("mesh pointer must not be null");
      }

      std::string msg_source_file_part = source_filename.empty() ? "" : "'" + source_filename + "'";

      std::string line;
      int line_idx = -1;
      int noncomment_line_idx = -1;
      size_t total_lines_processed = 0;

      std::vector<float> vertices;
      std::vector<int> faces;
      size_t num_vertices = 0;
      size_t num_faces = 0;
      size_t num_edges = 0;
      size_t num_verts_parsed = 0;
      size_t num_faces_parsed = 0;
      bool has_vertex_colors = false;
      float x, y, z; // vertex xyz coords
      int r, g, b, a;   // vertex colors
      int num_verts_this_face, v0, v1, v2; // face, defined by number of vertices and vertex indices.
      std::vector<uint8_t> vertex_colors;

      while (total_lines_processed < LIBFS_MAX_OFF_LINES)
      {
        // -- Security: bounded line read (O4) --
        if (!std::getline(*is, line))
        {
          break; // EOF or read error
        }
        total_lines_processed++;
        line_idx++;

        if (line.size() > LIBFS_MAX_OFF_LINE_LENGTH)
        {
          throw std::runtime_error("OFF line " + std::to_string(line_idx + 1) +
                                   " exceeds maximum allowed line length of " +
                                   std::to_string(LIBFS_MAX_OFF_LINE_LENGTH) + " bytes.\n");
        }

        std::istringstream iss(line);
        if (fs::util::starts_with(line, "#"))
        {
          continue; // skip comment.
        }
        else
        {
          noncomment_line_idx++;
          if (noncomment_line_idx == 0)
          {
            std::string off_header_magic;
            if (!(iss >> off_header_magic))
            {
              throw std::domain_error("Could not parse first header line " + std::to_string(line_idx + 1) + " of OFF data, invalid format.\n");
            }
            if (!(off_header_magic == "OFF" || off_header_magic == "COFF"))
            {
              throw std::domain_error("OFF magic string invalid, file " + msg_source_file_part + " not in OFF format.\n");
            }
            has_vertex_colors = (off_header_magic == "COFF");
          }
          else if (noncomment_line_idx == 1)
          {
            if (!(iss >> num_vertices >> num_faces >> num_edges))
            {
              throw std::domain_error("Could not parse element count header line " + std::to_string(line_idx + 1) + " of OFF data " + msg_source_file_part + ", invalid format.\n");
            }

            // -- Security: validate header counts against allocation limit (O1) --
            if (!util::check_alloc(num_vertices, 3 * sizeof(float)) ||
                !util::check_alloc(num_faces, 3 * sizeof(int)))
            {
              throw std::runtime_error("OFF data " + msg_source_file_part + " header declares more vertices/faces than allowed by allocation limit (" +
                                       std::to_string(LIBFS_MAX_ALLOC_BYTES) + " bytes).\n");
            }
          }
          else
          {

            if (num_verts_parsed < num_vertices)
            {
              if (has_vertex_colors)
              {
                if (!(iss >> x >> y >> z >> r >> g >> b >> a))
                {
                  throw std::domain_error("Could not parse vertex coordinate and color line " + std::to_string(line_idx + 1) + " of COFF data " + msg_source_file_part + ", invalid format.\n");
                }
                // -- Security: clamp vertex colors to [0,255] (O8) --
                if (r < 0) { r = 0; } if (r > 255) { r = 255; }
                if (g < 0) { g = 0; } if (g > 255) { g = 255; }
                if (b < 0) { b = 0; } if (b > 255) { b = 255; }
                vertex_colors.push_back(static_cast<uint8_t>(r));
                vertex_colors.push_back(static_cast<uint8_t>(g));
                vertex_colors.push_back(static_cast<uint8_t>(b));
              }
              else
              {
                if (!(iss >> x >> y >> z))
                {
                  throw std::domain_error("Could not parse vertex coordinate line " + std::to_string(line_idx + 1) + " of OFF data " + msg_source_file_part + ", invalid format.\n");
                }
              }

              // -- Security: validate finite coordinates (O7) --
              if (!util::is_finite_float(x) || !util::is_finite_float(y) || !util::is_finite_float(z))
              {
                throw std::domain_error("Non-finite vertex coordinate on line " + std::to_string(line_idx + 1) + " of OFF data " + msg_source_file_part + ".\n");
              }

              vertices.push_back(x);
              vertices.push_back(y);
              vertices.push_back(z);
              num_verts_parsed++;
            }
            else
            {
              if (num_faces_parsed < num_faces)
              {
                if (!(iss >> num_verts_this_face >> v0 >> v1 >> v2))
                {
                  throw std::domain_error("Could not parse face line " + std::to_string(line_idx + 1) + " of OFF data " + msg_source_file_part + ", invalid format.\n");
                }
                if (num_verts_this_face != 3)
                {
                  throw std::domain_error("At OFF data " + msg_source_file_part + " line " + std::to_string(line_idx + 1) + ": only triangular meshes supported.\n");
                }
                faces.push_back(v0);
                faces.push_back(v1);
                faces.push_back(v2);
                num_faces_parsed++;
              }
            }
          }
        }
      }

      // -- Security: max-lines exceeded (O5) --
      if (total_lines_processed >= LIBFS_MAX_OFF_LINES && !is->eof())
      {
        throw std::runtime_error("OFF file exceeds maximum allowed line count of " + std::to_string(LIBFS_MAX_OFF_LINES) + ".\n");
      }

      if (num_verts_parsed < num_vertices)
      {
        throw std::domain_error("Vertex count mismatch between OFF data " + msg_source_file_part + " header (" + std::to_string(num_vertices) + ") and data (" + std::to_string(num_verts_parsed) + ").\n");
      }
      if (num_faces_parsed < num_faces)
      {
        throw std::domain_error("Face count mismatch between OFF data " + msg_source_file_part + " header  (" + std::to_string(num_faces) + ") and data (" + std::to_string(num_faces_parsed) + ").\n");
      }

      // -- Security: validate face indices against vertex count (O2) --
      int32_t nv = static_cast<int32_t>(num_vertices);
      for (size_t fi = 0; fi < faces.size(); fi++)
      {
        int idx = faces[fi];
        if (idx < 0 || idx >= nv)
        {
          throw std::domain_error("Face index " + std::to_string(idx) + " out of range [0, " + std::to_string(nv - 1) + "] in OFF data " + msg_source_file_part + ".\n");
        }
      }

      mesh->vertices = vertices;
      mesh->faces = faces;
      mesh->vertex_colors = vertex_colors;
    }

    /// @brief Read a brainmesh from an OFF format mesh file.
    /// @see There exists an overloaded version that reads from a stream.
    /// @details The OFF is the Object File Format (file extension .off) is a simple text-based mesh file format. Not to be confused with the Wavefront Object format (.obj).
    /// @param mesh pointer to fs:Mesh instance to be filled.
    /// @param filename path to input wavefront obj mesh to be read.
    /// @throws std::runtime_error if the file cannot be read.
    /// @throws std::domain_error if the file format is invalid.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface;
    /// fs::Mesh::from_off(&surface, "mesh.off");
    /// @endcode
    static void from_off(Mesh *mesh, const std::string &filename)
    {
#ifdef LIBFS_DBG_INFO
      std::cout << LIBFS_APPTAG << "Reading brain mesh from OFF format file " << filename << ".\n";
#endif
      // -- Security: file-size pre-check (O6) --
      size_t file_size = util::get_file_size(filename);
      if (file_size > LIBFS_MAX_OFF_FILE_SIZE)
      {
        throw std::runtime_error("OFF file '" + filename + "' size (" + std::to_string(file_size) +
                                 " bytes) exceeds maximum allowed (" + std::to_string(LIBFS_MAX_OFF_FILE_SIZE) + " bytes).\n");
      }

      std::ifstream input(filename, std::fstream::in);
      if (input.is_open())
      {
        Mesh::from_off(mesh, &input);
        input.close();
      }
      else
      {
        throw std::runtime_error("Could not open Object file format (OFF) mesh file '" + filename + "' for reading.\n");
      }
    }

    /// @brief Read a brainmesh from a Stanford PLY format stream.
    /// @param mesh pointer to fs:Mesh instance to be filled.
    /// @param is An open std::istream or derived class stream from which to read the data, e.g., std::ifstream or std::istringstream.
    /// @see There exists an overloaded version that reads from a file.
    /// @throws std::domain_error if the file format is invalid.
    static void from_ply(Mesh *mesh, std::istream *is)
    {
      // -- Security: null-pointer check (P4) --
      if (!mesh)
      {
        throw std::invalid_argument("mesh pointer must not be null");
      }

      std::string line;
      int line_idx = -1;
      int noncomment_line_idx = -1;
      size_t total_lines_processed = 0;

      std::vector<float> vertices;
      std::vector<int> faces;
      std::vector<uint8_t> vertex_colors;
      std::vector<float> ply_normals;    // collected per-vertex normals (3 floats per vertex)
      std::vector<float> ply_texcoords;  // collected per-vertex texcoords (2 floats per vertex)

      bool in_header = true;
      size_t num_verts = 0;
      size_t num_faces = 0;
      bool have_num_verts = false;
      bool have_num_faces = false;
      bool in_vertex_element = false;
      std::vector<std::string> vertex_properties;

      while (total_lines_processed < LIBFS_MAX_PLY_LINES)
      {
        // -- Security: bounded line read (P6) --
        if (!std::getline(*is, line))
        {
          break; // EOF or read error
        }
        total_lines_processed++;
        line_idx++;

        if (line.size() > LIBFS_MAX_PLY_LINE_LENGTH)
        {
          throw std::runtime_error("PLY line " + std::to_string(line_idx + 1) +
                                   " exceeds maximum allowed line length of " +
                                   std::to_string(LIBFS_MAX_PLY_LINE_LENGTH) + " bytes.\n");
        }

        std::istringstream iss(line);
        if (fs::util::starts_with(line, "comment"))
        {
          continue; // skip comment.
        }
        else
        {
          noncomment_line_idx++;
          if (in_header)
          {
            if (noncomment_line_idx == 0)
            {
              if (line != "ply")
                throw std::domain_error("Invalid PLY file");
            }
            else if (noncomment_line_idx == 1)
            {
              if (line != "format ascii 1.0")
                throw std::domain_error("Unsupported PLY file format, only format 'format ascii 1.0' is supported.");
            }

            if (line == "end_header")
            {
              in_header = false;

              // -- Security: validate header counts after header is complete --
              if (!have_num_verts || !have_num_faces)
              {
                throw std::domain_error("Invalid PLY file: missing element count lines in header.\n");
              }

              // -- Security: validate header counts against allocation limit (P2) --
              if (!util::check_alloc(num_verts, 3 * sizeof(float)) ||
                  !util::check_alloc(num_faces, 3 * sizeof(int)))
              {
                throw std::runtime_error("PLY header declares more vertices/faces than allowed by allocation limit (" +
                                         std::to_string(LIBFS_MAX_ALLOC_BYTES) + " bytes).\n");
              }
            }
            else if (fs::util::starts_with(line, "element vertex"))
            {
              std::string elem, elem_type_identifier;

              // -- Security: parse count as long long to avoid int overflow (P1) --
              long long parsed_num_verts;
              if (!(iss >> elem >> elem_type_identifier >> parsed_num_verts))
              {
                throw std::domain_error("Could not parse element vertex line of PLY header, invalid format.\n");
              }
              if (parsed_num_verts < 0)
              {
                throw std::domain_error("Negative vertex count in PLY header.\n");
              }
              num_verts = static_cast<size_t>(parsed_num_verts);
              have_num_verts = true;
              in_vertex_element = true;
            }
            else if (fs::util::starts_with(line, "element face"))
            {
              std::string elem, elem_type_identifier;

              // -- Security: parse count as long long to avoid int overflow (P1) --
              long long parsed_num_faces;
              if (!(iss >> elem >> elem_type_identifier >> parsed_num_faces))
              {
                throw std::domain_error("Could not parse element face line of PLY header, invalid format.\n");
              }
              if (parsed_num_faces < 0)
              {
                throw std::domain_error("Negative face count in PLY header.\n");
              }
              num_faces = static_cast<size_t>(parsed_num_faces);
              have_num_faces = true;
              in_vertex_element = false;
            }
            else if (fs::util::starts_with(line, "element "))
            {
              // Some other element (e.g., edges): stop tracking vertex properties.
              in_vertex_element = false;
            }
            else if (fs::util::starts_with(line, "property ") && in_vertex_element)
            {
              // Record property order for the vertex element so we can parse data lines correctly.
              std::string kw, type, name;
              if (iss >> kw >> type >> name)
              {
                vertex_properties.push_back(name);
              }
            }
          }
          else
          { // in data part.
            if (!have_num_verts || !have_num_faces)
            {
              throw std::domain_error("Invalid PLY file: missing element count lines of header.");
            }
            // Read vertices
            if (vertices.size() < num_verts * 3)
            {
              float x = 0.0f, y = 0.0f, z = 0.0f;
              float nx = 0.0f, ny = 0.0f, nz = 0.0f;
              float s = 0.0f, t = 0.0f;
              int r = 0, g = 0, b = 0;
              if (vertex_properties.empty())
              {
                // No property declarations tracked: fall back to default x y z order.
                if (!(iss >> x >> y >> z))
                {
                  throw std::domain_error("Could not parse vertex line " + std::to_string(line_idx) + " of PLY data, invalid format.\n");
                }
              }
              else
              {
                for (size_t pi = 0; pi < vertex_properties.size(); pi++)
                {
                  const std::string &pname = vertex_properties[pi];
                  if (pname == "x") { iss >> x; }
                  else if (pname == "y") { iss >> y; }
                  else if (pname == "z") { iss >> z; }
                  else if (pname == "nx") { iss >> nx; }
                  else if (pname == "ny") { iss >> ny; }
                  else if (pname == "nz") { iss >> nz; }
                  else if (pname == "s" || pname == "u") { iss >> s; }
                  else if (pname == "t" || pname == "v") { iss >> t; }
                  else if (pname == "red") { iss >> r; }
                  else if (pname == "green") { iss >> g; }
                  else if (pname == "blue") { iss >> b; }
                  else
                  {
                    // Skip unknown property.
                    std::string dummy; iss >> dummy;
                  }
                  if (iss.fail())
                  {
                    throw std::domain_error("Could not parse vertex property '" + pname + "' at line " + std::to_string(line_idx) + " of PLY data.\n");
                  }
                }
                if (iss.fail())
                {
                  throw std::domain_error("Could not parse vertex line " + std::to_string(line_idx) + " of PLY data, invalid format.\n");
                }
              }

              // -- Security: validate finite coordinates (P9) --
              if (!util::is_finite_float(x) || !util::is_finite_float(y) || !util::is_finite_float(z))
              {
                throw std::domain_error("Non-finite vertex coordinate on line " + std::to_string(line_idx) + " of PLY data.\n");
              }

              vertices.push_back(x);
              vertices.push_back(y);
              vertices.push_back(z);

              // Only store colors if red/green/blue were declared in the header.
              bool has_r = false, has_g = false, has_b = false;
              for (size_t pi = 0; pi < vertex_properties.size(); pi++)
              {
                if (vertex_properties[pi] == "red") has_r = true;
                if (vertex_properties[pi] == "green") has_g = true;
                if (vertex_properties[pi] == "blue") has_b = true;
              }
              if (has_r && has_g && has_b)
              {
                // -- Security: clamp vertex colors to [0,255] (P10) --
                if (r < 0) { r = 0; } if (r > 255) { r = 255; }
                if (g < 0) { g = 0; } if (g > 255) { g = 255; }
                if (b < 0) { b = 0; } if (b > 255) { b = 255; }
                vertex_colors.push_back(static_cast<uint8_t>(r));
                vertex_colors.push_back(static_cast<uint8_t>(g));
                vertex_colors.push_back(static_cast<uint8_t>(b));
              }

              // Collect normals if declared in header.
              bool has_nx = false, has_ny = false, has_nz = false;
              for (size_t pi = 0; pi < vertex_properties.size(); pi++)
              {
                if (vertex_properties[pi] == "nx") has_nx = true;
                if (vertex_properties[pi] == "ny") has_ny = true;
                if (vertex_properties[pi] == "nz") has_nz = true;
              }
              if (has_nx && has_ny && has_nz)
              {
                if (!util::is_finite_float(nx) || !util::is_finite_float(ny) || !util::is_finite_float(nz))
                {
                  throw std::domain_error("Non-finite normal on line " + std::to_string(line_idx) + " of PLY data.\n");
                }
                ply_normals.push_back(nx);
                ply_normals.push_back(ny);
                ply_normals.push_back(nz);
              }

              // Collect texcoords if declared in header.
              bool has_s = false, has_t = false;
              for (size_t pi = 0; pi < vertex_properties.size(); pi++)
              {
                if (vertex_properties[pi] == "s" || vertex_properties[pi] == "u") has_s = true;
                if (vertex_properties[pi] == "t" || vertex_properties[pi] == "v") has_t = true;
              }
              if (has_s && has_t)
              {
                if (!util::is_finite_float(s) || !util::is_finite_float(t))
                {
                  throw std::domain_error("Non-finite texcoord on line " + std::to_string(line_idx) + " of PLY data.\n");
                }
                ply_texcoords.push_back(s);
                ply_texcoords.push_back(t);
              }
            }
            else
            {
              if (faces.size() < num_faces * 3)
              {
                int verts_per_face, v0, v1, v2;
                if (!(iss >> verts_per_face >> v0 >> v1 >> v2))
                {
                  throw std::domain_error("Could not parse face line " + std::to_string(line_idx) + " of PLY data, invalid format.\n");
                }
                if (verts_per_face != 3)
                {
                  throw std::domain_error("Only triangular meshes are supported: PLY faces lines must contain exactly 3 vertex indices.\n");
                }
                faces.push_back(v0);
                faces.push_back(v1);
                faces.push_back(v2);
              }
            }
          }
        }
      }

      // -- Security: max-lines exceeded (P7) --
      if (total_lines_processed >= LIBFS_MAX_PLY_LINES && !is->eof())
      {
        throw std::runtime_error("PLY file exceeds maximum allowed line count of " + std::to_string(LIBFS_MAX_PLY_LINES) + ".\n");
      }

      // -- Throw if header was never terminated --
      if (in_header)
      {
        throw std::domain_error("Invalid PLY file: header not terminated with 'end_header'.\n");
      }

      // -- Security: throw on count mismatch instead of just warning (P5) --
      if (vertices.size() != num_verts * 3)
      {
        throw std::domain_error("PLY vertex count mismatch: header declares " + std::to_string(num_verts) +
                                " vertices, but found " + std::to_string(vertices.size() / 3) + ".\n");
      }
      if (faces.size() != num_faces * 3)
      {
        throw std::domain_error("PLY face count mismatch: header declares " + std::to_string(num_faces) +
                                " faces, but found " + std::to_string(faces.size() / 3) + ".\n");
      }

      // -- Security: validate face indices against vertex count (P3) --
      {
        int32_t nv = static_cast<int32_t>(num_verts);
        for (size_t fi = 0; fi < faces.size(); fi++)
        {
          int idx = faces[fi];
          if (idx < 0 || idx >= nv)
          {
            throw std::domain_error("Face index " + std::to_string(idx) + " out of range [0, " + std::to_string(nv - 1) + "] in PLY data.\n");
          }
        }
      }

      mesh->vertices = vertices;
      mesh->faces = faces;
      mesh->vertex_colors = vertex_colors;
      mesh->vertex_normals = ply_normals;
      mesh->vertex_texcoords = ply_texcoords;
    }

    /// @brief Read a brainmesh from a Stanford PLY format mesh file.
    /// @details The PLY format exists in text and binary forms, and the binary form can be little endian or big endian. This file reads the ASCII text format version.
    /// @param mesh pointer to fs:Mesh instance to be filled.
    /// @param filename path to input wavefront obj mesh to be read.
    /// @throws std::runtime_error if the file cannot be read.
    /// @throws std::domain_error if the file format is invalid.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface;
    /// fs::Mesh::from_ply(&surface, "mesh.ply");
    /// @endcode
    static void from_ply(Mesh *mesh, const std::string &filename)
    {
#ifdef LIBFS_DBG_INFO
      std::cout << LIBFS_APPTAG << "Reading brain mesh from PLY format file " << filename << ".\n";
#endif
      // -- Security: file-size pre-check (P8) --
      size_t file_size = util::get_file_size(filename);
      if (file_size > LIBFS_MAX_PLY_FILE_SIZE)
      {
        throw std::runtime_error("PLY file '" + filename + "' size (" + std::to_string(file_size) +
                                 " bytes) exceeds maximum allowed (" + std::to_string(LIBFS_MAX_PLY_FILE_SIZE) + " bytes).\n");
      }

      std::ifstream input(filename, std::fstream::in);
      if (input.is_open())
      {
        Mesh::from_ply(mesh, &input);
        input.close();
      }
      else
      {
        throw std::runtime_error("Could not open Stanford PLY format mesh file '" + filename + "' for reading.\n");
      }
    }

    /// @brief Return the number of vertices in this mesh.
    /// @return the vertex count
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// size_t nv = surface.num_vertices();
    /// @endcode
    size_t num_vertices() const
    {
      return (this->vertices.size() / 3);
    }

    /// @brief Return the number of faces in this mesh.
    /// @return the face count
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// size_t nv = surface.num_faces();
    /// @endcode
    size_t num_faces() const
    {
      return (this->faces.size() / 3);
    }

    /// @brief Retrieve a vertex index of a face, treating the faces vector as an nx3 matrix.
    /// @param i the row index, valid values are 0..num_faces-1.
    /// @param j the column index, valid values are 0..2 (for the 3 vertices of a face).
    /// @throws std::range_error on invalid index
    /// @return vertex index of vertex `j` of face `i`
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// int first_face_third_vert = surface.fm_at(0, 2);
    /// @endcode
    const int32_t &fm_at(const size_t i, const size_t j) const
    {
      size_t idx = _vidx_2d(i, j, 3);
      if (idx > this->faces.size() - 1)
      {
        throw std::range_error("Indices (" + std::to_string(i) + "," + std::to_string(j) + ") into Mesh.faces out of bounds. Hit " + std::to_string(idx) + " with max valid index " + std::to_string(this->faces.size() - 1) + ".\n");
      }
      return (this->faces[idx]);
    }

    /// @brief Get all vertex indices of the face, given by its index.
    /// @param face the face index
    /// @returns vector of length 3, the vertex indices of the face.
    /// @throws std::range_error on invalid index
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// auto first_face_verts = surface.face_vertices(0);
    /// @endcode
    std::vector<int32_t> face_vertices(const size_t face) const
    {
      if (face > this->num_faces() - 1)
      {
        throw std::range_error("Index " + std::to_string(face) + " into Mesh.faces out of bounds, max valid index is " + std::to_string(this->num_faces() - 1) + ".\n");
      }
      std::vector<int32_t> fv(3);
      fv[0] = this->fm_at(face, 0);
      fv[1] = this->fm_at(face, 1);
      fv[2] = this->fm_at(face, 2);
      return (fv);
    }

    /// @brief Get all coordinates of the vertex, given by its index.
    /// @param vertex the vertex index
    /// @returns vector of length 3, the x,y,z coordinates of the vertex.
    /// @throws std::range_error on invalid index
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// auto coords = surface.vertex_coords(0);
    /// @endcode
    std::vector<float> vertex_coords(const size_t vertex) const
    {
      if (vertex > this->num_vertices() - 1)
      {
        throw std::range_error("Index " + std::to_string(vertex) + " into Mesh.vertices out of bounds, max valid index is " + std::to_string(this->num_vertices() - 1) + ".\n");
      }
      std::vector<float> vc(3);
      vc[0] = this->vm_at(vertex, 0);
      vc[1] = this->vm_at(vertex, 1);
      vc[2] = this->vm_at(vertex, 2);
      return (vc);
    }

    /// @brief Retrieve a single (x, y, or z) coordinate of a vertex, treating the vertices vector as an nx3 matrix.
    /// @param i the row index, valid values are 0..num_vertices.
    /// @param j the column index, valid values are 0..2 (for the x,y,z coordinates).
    /// @throws std::range_error on invalid index
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// float v5_x = surface.vm_at(5, 0);
    /// float v5_y = surface.vm_at(5, 1);
    /// float v5_z = surface.vm_at(5, 2);
    /// @endcode
    const float &vm_at(const size_t i, const size_t j) const
    {
      size_t idx = _vidx_2d(i, j, 3);
      if (idx > this->vertices.size() - 1)
      {
        throw std::range_error("Indices (" + std::to_string(i) + "," + std::to_string(j) + ") into Mesh.vertices out of bounds. Hit " + std::to_string(idx) + " with max valid index " + std::to_string(this->vertices.size() - 1) + ".\n");
      }
      return (this->vertices[idx]);
    }

    /// @brief Return string representing the mesh in PLY format. Overload that works without passing a color vector.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::string ply_rep = surface.to_ply();
    /// @endcode
    std::string to_ply() const
    {
      std::vector<uint8_t> empty_col;
      return (this->to_ply(empty_col));
    }

    /// @brief Return string representing the mesh in PLY format.
    /// @param col u_char vector of RGB color values, 3 per vertex. They must appear by vertex, i.e. in order v0_red, v0_green, v0_blue, v1_red, v1_green, v1_blue. Leave empty if you do not want colors.
    /// @throws std::invalid_argument if the number of vertex colors does not match the number of vertices.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::string ply_rep = surface.to_ply();
    /// @endcode
    std::string to_ply(const std::vector<uint8_t> col) const
    {
      bool use_vertex_colors = col.size() != 0;
      bool use_normals = this->has_normals();
      bool use_texcoords = this->has_texcoords();
      std::stringstream plys;
      plys << "ply\nformat ascii 1.0\n";
      plys << "element vertex " << this->num_vertices() << "\n";
      plys << "property float x\nproperty float y\nproperty float z\n";
      if (use_normals)
      {
        plys << "property float nx\nproperty float ny\nproperty float nz\n";
      }
      if (use_texcoords)
      {
        plys << "property float s\nproperty float t\n";
      }
      if (use_vertex_colors)
      {
        if (col.size() != this->vertices.size())
        {
          throw std::invalid_argument("Number of vertex coordinates and vertex colors must match when writing PLY file, but got " + std::to_string(this->vertices.size()) + " and " + std::to_string(col.size()) + ".");
        }
        plys << "property uchar red\nproperty uchar green\nproperty uchar blue\n";
      }
      plys << "element face " << this->num_faces() << "\n";
      plys << "property list uchar int vertex_index\n";
      plys << "end_header\n";

#ifdef LIBFS_DBG_DEBUG
      fs::util::log("Writing " + std::to_string(this->vertices.size() / 3) + " PLY format vertices.", "INFO");
#endif

      for (size_t vidx = 0; vidx < this->vertices.size(); vidx += 3)
      { // vertex coords
        plys << vertices[vidx] << " " << vertices[vidx + 1] << " " << vertices[vidx + 2];
        if (use_normals)
        {
          plys << " " << vertex_normals[vidx] << " " << vertex_normals[vidx + 1] << " " << vertex_normals[vidx + 2];
        }
        if (use_texcoords)
        {
          size_t tcidx = (vidx / 3) * 2;
          plys << " " << vertex_texcoords[tcidx] << " " << vertex_texcoords[tcidx + 1];
        }
        if (use_vertex_colors)
        {
          plys << " " << (int)col[vidx] << " " << (int)col[vidx + 1] << " " << (int)col[vidx + 2];
        }
        plys << "\n";
      }

#ifdef LIBFS_DBG_DEBUG
      fs::util::log("Writing " + std::to_string(this->faces.size() / 3) + " PLY format faces.", "INFO");
#endif

      const int num_vertices_per_face = 3;
      for (size_t fidx = 0; fidx < this->faces.size(); fidx += 3)
      { // faces: vertex indices, 0-based
        plys << num_vertices_per_face << " " << faces[fidx] << " " << faces[fidx + 1] << " " << faces[fidx + 2] << "\n";
      }
      return (plys.str());
    }

    /// @brief Export this mesh to a file in Stanford PLY format.
    /// @throws std::runtime_error if the target file cannot be opened.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// surface.to_ply_file("mesh.ply");
    /// @endcode
    void to_ply_file(const std::string &filename) const
    {
#ifdef LIBFS_DBG_INFO
      fs::util::log("Writing mesh to PLY file '" + filename + "'.", "INFO");
#endif
      fs::util::str_to_file(filename, this->to_ply());
    }

    /// @brief Export this mesh to a file in Stanford PLY format with vertex colors.
    /// @throws std::runtime_error if the target file cannot be opened, std::invalid_argument if the number of vertex colors does not match the number of vertices.
    void to_ply_file(const std::string &filename, const std::vector<uint8_t> col) const
    {
      fs::util::str_to_file(filename, this->to_ply(col));
    }

    /// @brief Return string representing the mesh in OFF format. Overload that works without passing a color vector.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// std::string off_rep = surface.to_off();
    /// @endcode
    std::string to_off() const
    {
      std::vector<uint8_t> empty_col;
      return (this->to_off(empty_col));
    }

    /// @brief Return string representing the mesh in PLY format.
    /// @param col u_char vector of RGB color values, 3 per vertex. They must appear by vertex, i.e. in order v0_red, v0_green, v0_blue, v1_red, v1_green, v1_blue. Leave empty if you do not want colors.
    /// @throws std::invalid_argument if the number of vertex colors does not match the number of vertices.
    std::string to_off(const std::vector<uint8_t> col) const
    {
      bool use_vertex_colors = col.size() != 0;
      std::stringstream offs;
      if (use_vertex_colors)
      {
#ifdef LIBFS_DBG_INFO
        fs::util::log("Writing OFF representation of mesh with vertex colors.", "INFO");
#endif
        if (col.size() != this->vertices.size())
        {
          throw std::invalid_argument("Number of vertex coordinates and vertex colors must match when writing OFF file but got " + std::to_string(this->vertices.size()) + " and " + std::to_string(col.size()) + ".");
        }
        offs << "COFF\n";
      }
      else
      {
#ifdef LIBFS_DBG_INFO
        fs::util::log("Writing OFF representation of mesh without vertex colors.", "INFO");
#endif
        offs << "OFF\n";
      }
      offs << this->num_vertices() << " " << this->num_faces() << " 0\n";

      for (size_t vidx = 0; vidx < this->vertices.size(); vidx += 3)
      { // vertex coords
        offs << vertices[vidx] << " " << vertices[vidx + 1] << " " << vertices[vidx + 2];
        if (use_vertex_colors)
        {
          offs << " " << (int)col[vidx] << " " << (int)col[vidx + 1] << " " << (int)col[vidx + 2] << " 255";
        }
        offs << "\n";
      }

      const int num_vertices_per_face = 3;
      for (size_t fidx = 0; fidx < this->faces.size(); fidx += 3)
      { // faces: vertex indices, 0-based
        offs << num_vertices_per_face << " " << faces[fidx] << " " << faces[fidx + 1] << " " << faces[fidx + 2] << "\n";
      }
      return (offs.str());
    }

    /// @brief Export this mesh to a file in OFF format.
    /// @throws std::runtime_error if the target file cannot be opened.
    ///
    /// #### Examples
    ///
    /// @code
    /// fs::Mesh surface = fs::Mesh::construct_cube();
    /// surface.to_off_file("mesh.off");
    /// @endcode
    void to_off_file(const std::string &filename) const
    {
      fs::util::str_to_file(filename, this->to_off());
    }

    /// @brief Export this mesh to a file in OFF format with vertex colors (COFF).
    /// @throws std::runtime_error if the target file cannot be opened, std::invalid_argument if the number of vertex colors does not match the number of vertices.
    void to_off_file(const std::string &filename, const std::vector<uint8_t> col) const
    {
      fs::util::str_to_file(filename, this->to_off(col));
    }
  };

  /// Models a FreeSurfer curv file that contains per-vertex float data.
  struct Curv
  {

    /// Construct a Curv instance from the given per-vertex data.
    Curv(std::vector<float> curv_data) : num_faces(100000), num_vertices(0), num_values_per_vertex(1)
    {
      data = curv_data;
      num_vertices = int(data.size());
    }

    /// Construct an empty Curv instance.
    Curv() : num_faces(100000), num_vertices(0), num_values_per_vertex(1) {}

    /// The number of faces of the mesh to which this belongs, typically irrelevant and ignored.
    int32_t num_faces;

    /// The curvature data, one value per vertex. Something like the cortical thickness at each vertex.
    std::vector<float> data;

    /// The number of vertices of the mesh to which this belongs. Can be deduced from length of 'data'.
    int32_t num_vertices;

    /// The number of values per vertex, stored in this file. Almost all apps (including FreeSurfer itself) only support a value of 1 here. Ignored by most apps, and assumed to be 1.
    int32_t num_values_per_vertex;
  };

  /// The colortable from an Annot file, can be used for parcellations and integer labels. Typically each index (in all fields) describes a brain region.
  struct Colortable
  {
    std::vector<int32_t> id;       ///< internal region index
    std::vector<std::string> name; ///< region name
    std::vector<int32_t> r;        ///< red channel of RGBA color
    std::vector<int32_t> g;        ///< blue channel of RGBA color
    std::vector<int32_t> b;        ///< green channel of RGBA color
    std::vector<int32_t> a;        ///< alpha channel of RGBA color
    std::vector<int32_t> label;    ///< label integer computed from rgba values. Maps to the Annot.vertex_label field.

    /// @brief Get the number of enties (regions) in this Colortable.
    size_t num_entries() const
    {
      size_t num_ids = this->id.size();
      if (this->name.size() != num_ids || this->r.size() != num_ids || this->g.size() != num_ids || this->b.size() != num_ids || this->a.size() != num_ids || this->label.size() != num_ids)
      {
#ifdef LIBFS_DBG_ERROR
        std::cerr << "Inconsistent Colortable, vector sizes do not match.\n";
#endif
      }
      return num_ids;
    }

    /// @brief Get the index of a region in the Colortable by region name. Returns a negative value if the region is not found.
    int32_t get_region_idx(const std::string &query_name) const
    {
      for (size_t i = 0; i < this->num_entries(); i++)
      {
        if (this->name[i] == query_name)
        {
          return (int32_t)i;
        }
      }
      return (-1);
    }

    /// @brief Get the index of a region in the Colortable by label. Returns a negative value if the region is not found.
    int32_t get_region_idx(int32_t query_label) const
    {
      for (size_t i = 0; i < this->num_entries(); i++)
      {
        if (this->label[i] == query_label)
        {
          return (int32_t)i;
        }
      }
      return (-1);
    }
  };

  /// An annotation, also known as a brain surface parcellation. Assigns to each vertex a region, identified by the region_label. The region name and color for each region can be found in the Colortable.
  struct Annot
  {
    std::vector<int32_t> vertex_indices; ///< Indices of the vertices, these always go from 0 to N-1 (where N is the number of vertices in the respective surface/annotation). Not really needed.
    std::vector<int32_t> vertex_labels;  ///< The label code for each vertex, defining the region it belongs to. Check in the Colortable for a region that has this label.
    Colortable colortable;               ///< A Colortable defining the regions (most importantly, the region name and visualization color).

    /// @brief Get all vertices of a region given by name in the brain surface parcellation. Returns an integer vector, the vertex indices.
    std::vector<int32_t> region_vertices(const std::string &region_name) const
    {
      int32_t region_idx = this->colortable.get_region_idx(region_name);
      if (region_idx >= 0)
      {
        return (this->region_vertices(this->colortable.label[region_idx]));
      }
      else
      {
#ifdef LIBFS_DBG_ERROR
        std::cerr << "No such region in annot, returning empty vector.\n";
#endif
        std::vector<int32_t> empty;
        return (empty);
      }
    }

    /// @brief Get all vertices of a region given by label in the brain surface parcellation. Returns an integer vector, the vertex indices.
    std::vector<int32_t> region_vertices(int32_t region_label) const
    {
      std::vector<int32_t> reg_verts;
      for (size_t i = 0; i < this->vertex_labels.size(); i++)
      {
        if (this->vertex_labels[i] == region_label)
        {
          reg_verts.push_back(int(i));
        }
      }
      return (reg_verts);
    }

    /// @brief Get the vertex colors as an array of uchar values, 3 consecutive values are the red, green and blue channel values for a single vertex.
    /// @param alpha whether to include the alpha channel and return 4 values per vertex instead of 3.
    std::vector<uint8_t> vertex_colors(bool alpha = false) const
    {
      int num_channels = alpha ? 4 : 3;
      std::vector<uint8_t> col;
      col.reserve(this->num_vertices() * num_channels);
      std::vector<size_t> vertex_region_indices = this->vertex_regions();
      for (size_t i = 0; i < this->num_vertices(); i++)
      {
        col.push_back(this->colortable.r[vertex_region_indices[i]]);
        col.push_back(this->colortable.g[vertex_region_indices[i]]);
        col.push_back(this->colortable.b[vertex_region_indices[i]]);
        if (alpha)
        {
          col.push_back(this->colortable.a[vertex_region_indices[i]]);
        }
      }
      return (col);
    }

    /// @brief Get the number of vertices of this parcellation (or the associated surface).
    /// @throws std::runtime_error on invalid annot
    size_t num_vertices() const
    {
      size_t nv = this->vertex_indices.size();
      if (this->vertex_labels.size() != nv)
      {
        throw std::runtime_error("Inconsistent annot, number of vertex indices and labels does not match.\n");
      }
      return nv;
    }

    /// @brief Compute the region indices in the Colortable for all vertices in this brain surface parcellation. With the region indices, it becomes very easy to obtain all region names, labels, and color channel values from the Colortable.
    /// @see The function `vertex_region_names` uses this function to get the region names for all vertices.
    std::vector<size_t> vertex_regions() const
    {
      std::vector<size_t> vert_reg;
      for (size_t i = 0; i < this->num_vertices(); i++)
      {
        vert_reg.push_back(0); // init with zeros.
      }
      for (size_t region_idx = 0; region_idx < this->colortable.num_entries(); region_idx++)
      {
        std::vector<int32_t> reg_vertices = this->region_vertices(this->colortable.label[region_idx]);
        for (size_t region_vert_local_idx = 0; region_vert_local_idx < reg_vertices.size(); region_vert_local_idx++)
        {
          int32_t region_vert_idx = reg_vertices[region_vert_local_idx];
          vert_reg[region_vert_idx] = region_idx;
        }
      }
      return vert_reg;
    }

    /// @brief Compute the region names in the Colortable for all vertices in this brain surface parcellation.
    std::vector<std::string> vertex_region_names() const
    {
      std::vector<std::string> region_names;
      std::vector<size_t> vertex_region_indices = this->vertex_regions();
      for (size_t i = 0; i < this->num_vertices(); i++)
      {
        region_names.push_back(this->colortable.name[vertex_region_indices[i]]);
      }
      return (region_names);
    }
  };

  /// Models the header of an MGH file.
  struct MghHeader
  {
    MghHeader() {} ///< Empty default constuctor.
    MghHeader(Curv curv)
    { ///< Constuctor to fill header from a Curv instance.
      dim1length = curv.data.size();
      dim2length = 1;
      dim3length = 1;
      dim4length = 1;
      dtype = fs::MRI_FLOAT;
    }
    MghHeader(std::vector<float> curv_data)
    { ///< Constuctor to fill header from a 1D float array (curv data).
      dim1length = curv_data.size();
      dim2length = 1;
      dim3length = 1;
      dim4length = 1;
      dtype = fs::MRI_FLOAT;
    }
    int32_t dim1length = 0; ///< size of data along 1st dimension
    int32_t dim2length = 0; ///< size of data along 2nd dimension
    int32_t dim3length = 0; ///< size of data along 3rd dimension
    int32_t dim4length = 0; ///< size of data along 4th dimension

    int32_t dtype = 0;         ///< the MRI data type
    int32_t dof = 0;           ///< typically ignored
    int16_t ras_good_flag = 0; ///< flag indicating whether the data in the RAS fields (Mdc, Pxyz_c) are valid. 1 means valid, everything else means invalid.

    /// @brief Compute the number of values based on the dim*length header fields.
    size_t num_values() const
    {
      return ((size_t)dim1length * dim2length * dim3length * dim4length);
    }

    float xsize = 0.0;         ///< size of voxels along 1st axis (x or r)
    float ysize = 0.0;         ///< size of voxels along 2nd axis (y or a)
    float zsize = 0.0;         ///< size of voxels along 3rd axis (z or s)
    std::vector<float> Mdc;    ///< matrix
    std::vector<float> Pxyz_c; ///< x,y,z coordinates of central vertex
  };

  /// Models the data of an MGH file. Currently these are 1D vectors, but one can compute the 4D array using the dimXlength fields of the respective MghHeader.
  struct MghData
  {
    MghData() {}
    MghData(std::vector<int32_t> curv_data) { data_mri_int = curv_data; }            ///< constructor to create MghData from MRI_INT (int32_t) data.
    explicit MghData(std::vector<uint8_t> curv_data) { data_mri_uchar = curv_data; } ///< constructor to create MghData from MRI_UCHAR (uint8_t) data.
    explicit MghData(std::vector<short> curv_data) { data_mri_short = curv_data; }   ///< constructor to create MghData from MRI_SHORT (short) data.
    MghData(std::vector<float> curv_data) { data_mri_float = curv_data; }            ///< constructor to create MghData from MRI_FLOAT (float) data.
    MghData(Curv curv) { data_mri_float = curv.data; }                               ///< constructor to create MghData from a Curv instance
    std::vector<int32_t> data_mri_int;                                               ///< data of type MRI_INT, check the dtype to see whether this is relevant for this instance.
    std::vector<uint8_t> data_mri_uchar;                                             ///< data of type MRI_UCHAR, check the dtype to see whether this is relevant for this instance.
    std::vector<float> data_mri_float;                                               ///< data of type MRI_FLOAT, check the dtype to see whether this is relevant for this instance.
    std::vector<short> data_mri_short;                                               ///< data of type MRI_SHORT, check the dtype to see whether this is relevant for this instance.
  };

  /// Models a whole MGH file.
  struct Mgh
  {
    MghHeader header; ///< Header for this MGH instance.
    MghData data;     ///< 4D data for this MGH instance.
    Mgh() {}          ///< Empty default constuctor.
    Mgh(Curv curv)
    { ///< Constuctor to create MGH instance from Curv instance.
      header = MghHeader(curv);
      data = MghData(curv);
    }
    Mgh(std::vector<float> curv_data)
    { ///< Constuctor to create MGH instance from a 1D float array (curv data).
      header = MghHeader(curv_data);
      data = MghData(curv_data);
    }
  };

  /// @brief A simple 4D array datastructure, useful for representing volume data.
  /// @details By convention, for FreeSurfer data, the order of the 4 dimensions is: *time*, *x*, *y*, *z*.
  template <class T>
  struct Array4D
  {
    /// Constructor for creating an empty 4D array of the given dimensions.
    /// @throws std::domain_error if any dimension is zero.
    /// @throws std::overflow_error if the product of dimensions overflows size_t.
    /// @throws std::runtime_error if the allocation exceeds the configured limit.
    Array4D(unsigned int d1, unsigned int d2, unsigned int d3, unsigned int d4) : d1(d1), d2(d2), d3(d3), d4(d4), data(_compute_4d_size(d1, d2, d3, d4)) {}

    /// Constructor for creating an empty 4D array based on dimensions specified in an fs::MghHeader.
    /// @throws std::domain_error if any dimension is not positive (including the case where
    ///         the int32_t header field was negative and wrapped to a huge unsigned int).
    /// @throws std::overflow_error if the product of dimensions overflows size_t.
    /// @throws std::runtime_error if the allocation exceeds the configured limit.
    Array4D(MghHeader *mgh_header) : d1(_validate_mgh_dim(mgh_header->dim1length)), d2(_validate_mgh_dim(mgh_header->dim2length)), d3(_validate_mgh_dim(mgh_header->dim3length)), d4(_validate_mgh_dim(mgh_header->dim4length)), data(_compute_4d_size(d1, d2, d3, d4)) {}

    /// Constructor for creating an empty 4D array based on dimensions specified in the header of an fs::Mgh. Does not init the data.
    /// @throws std::domain_error if any dimension is not positive.
    /// @throws std::overflow_error if the product of dimensions overflows size_t.
    /// @throws std::runtime_error if the allocation exceeds the configured limit.
    Array4D(Mgh *mgh) : // This does NOT init the data atm.
                        d1(_validate_mgh_dim(mgh->header.dim1length)), d2(_validate_mgh_dim(mgh->header.dim2length)), d3(_validate_mgh_dim(mgh->header.dim3length)), d4(_validate_mgh_dim(mgh->header.dim4length)), data(_compute_4d_size(d1, d2, d3, d4))
    {
    }

    /// Get the value at the given 4D position.
    const T &at(const unsigned int i1, const unsigned int i2, const unsigned int i3, const unsigned int i4) const
    {
      return data[get_index(i1, i2, i3, i4)];
    }

    /// Get the index in the vector for the given 4D position.
    unsigned int get_index(const unsigned int i1, const unsigned int i2, const unsigned int i3, const unsigned int i4) const
    {
      assert(i1 >= 0 && i1 < d1);
      assert(i2 >= 0 && i2 < d2);
      assert(i3 >= 0 && i3 < d3);
      assert(i4 >= 0 && i4 < d4);
      return (((i1 * d2 + i2) * d3 + i3) * d4 + i4);
    }

    /// Get number of values/voxels.
    unsigned int num_values() const
    {
      return (d1 * d2 * d3 * d4);
    }

    unsigned int d1;     ///< size of data along 1st dimension
    unsigned int d2;     ///< size of data along 2nd dimension
    unsigned int d3;     ///< size of data along 3rd dimension
    unsigned int d4;     ///< size of data along 4th dimension
    std::vector<T> data; ///< the data, as a 1D vector. Use fs::Array4D::at for easy access in 4D.

  private:
    /// Validate that an int32_t MGH dimension field is positive and does not wrap around
    /// when cast to unsigned int.
    static unsigned int _validate_mgh_dim(int32_t dim)
    {
      if (dim <= 0)
      {
        throw std::domain_error("MGH dimension " + std::to_string(dim) + " is not positive.\n");
      }
      return static_cast<unsigned int>(dim);
    }

    /// Compute data vector size from 4 dimensions with overflow and allocation-limit checks.
    static size_t _compute_4d_size(unsigned int d1, unsigned int d2, unsigned int d3, unsigned int d4)
    {
      if (d1 == 0 || d2 == 0 || d3 == 0 || d4 == 0)
      {
        throw std::domain_error("Array4D dimensions must be positive.\n");
      }
      size_t s1, s2, s3;
      if (!fs::util::safe_multiply(d1, d2, s1) ||
          !fs::util::safe_multiply(s1, d3, s2) ||
          !fs::util::safe_multiply(s2, d4, s3))
      {
        throw std::overflow_error("Array4D dimensions cause size_t overflow.\n");
      }
      if (s3 > LIBFS_MAX_ALLOC_BYTES / sizeof(T))
      {
        throw std::runtime_error("Array4D size " + std::to_string(s3) +
                                 " elements exceeds maximum allowed allocation (" +
                                 std::to_string(LIBFS_MAX_ALLOC_BYTES) + " bytes).\n");
      }
      return s3;
    }
  };

  // More declarations, should also go to separate header.
  void read_mgh_header(MghHeader *, const std::string &);
  void read_mgh_header(MghHeader *, std::istream *);
  template <typename T>
  std::vector<T> _read_mgh_data(MghHeader *, const std::string &);
  template <typename T>
  std::vector<T> _read_mgh_data(MghHeader *, std::istream *);
  std::vector<int32_t> _read_mgh_data_int(MghHeader *, const std::string &);
  std::vector<int32_t> _read_mgh_data_int(MghHeader *, std::istream *);
  std::vector<uint8_t> _read_mgh_data_uchar(MghHeader *, const std::string &);
  std::vector<uint8_t> _read_mgh_data_uchar(MghHeader *, std::istream *);
  std::vector<short> _read_mgh_data_short(MghHeader *, const std::string &);
  std::vector<short> _read_mgh_data_short(MghHeader *, std::istream *);
  std::vector<float> _read_mgh_data_float(MghHeader *, const std::string &);
  std::vector<float> _read_mgh_data_float(MghHeader *, std::istream *);

  /// @brief Read a FreeSurfer volume file in MGH format into the given Mgh struct.
  /// @param mgh An Mgh instance that should be filled with the data from the filename.
  /// @param filename Path to the input MGH file.
  /// @see There exists an overloaded version that reads from a stream.
  /// @throws runtime_error if the file uses an unsupported MRI data type.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Mgh mgh;
  /// fs::read_mgh(&mgh, "somebrain.mgh");
  /// @endcode
  void read_mgh(Mgh *mgh, const std::string &filename)
  {
    MghHeader mgh_header;
    read_mgh_header(&mgh_header, filename);
    mgh->header = mgh_header;
    if (mgh->header.dtype == MRI_INT)
    {
      std::vector<int32_t> data = _read_mgh_data_int(&mgh_header, filename);
      mgh->data.data_mri_int = data;
    }
    else if (mgh->header.dtype == MRI_UCHAR)
    {
      std::vector<uint8_t> data = _read_mgh_data_uchar(&mgh_header, filename);
      mgh->data.data_mri_uchar = data;
    }
    else if (mgh->header.dtype == MRI_FLOAT)
    {
      std::vector<float> data = _read_mgh_data_float(&mgh_header, filename);
      mgh->data.data_mri_float = data;
    }
    else if (mgh->header.dtype == MRI_SHORT)
    {
      std::vector<short> data = _read_mgh_data_short(&mgh_header, filename);
      mgh->data.data_mri_short = data;
    }
    else
    {
#ifdef LIBFS_DBG_INFO
      if (fs::util::ends_with(filename, ".mgz"))
      {
#ifndef LIBFS_HAS_ZLIB
        std::cout << LIBFS_APPTAG << "Note: your MGH filename ends with '.mgz'. MGZ support requires zlib: link with -lz. If you already have zlib and see this, #define LIBFS_HAS_ZLIB before including libfs.h, or upgrade your compiler.\n";
#else
        std::cout << LIBFS_APPTAG << "Note: your MGH filename ends with '.mgz'. Did you mean to call read_mgz() instead of read_mgh()?\n";
#endif
      }
#endif
      throw std::runtime_error("Not reading MGH data from file '" + filename + "', data type " + std::to_string(mgh->header.dtype) + " not supported yet.\n");
    }
  }

  /// @brief Read a vector of subject identifiers from a FreeSurfer subjects file.
  /// @param filename a text file that contains one subject identifier per line.
  /// @throws runtime_error if the file cannot be read
  ///
  /// #### Examples
  ///
  /// @code
  /// std::vector<std::string> subjects = fs::read_subjectsfile("subjects.txt");
  /// @endcode
  std::vector<std::string> read_subjectsfile(const std::string &filename)
  {
    std::vector<std::string> subjects;
    std::ifstream input(filename, std::fstream::in);
    std::string line;

    if (!input.is_open())
    {
      throw std::runtime_error("Could not open subjects file '" + filename + "'.\n");
    }

    while (std::getline(input, line))
    {
      subjects.push_back(line);
    }
    return (subjects);
  }

  /// @brief Write a vector of subject identifiers to a FreeSurfer subjects file.
  /// @param filename Path to the output file (one subject ID per line).
  /// @param subjects The subject identifiers to write.
  /// @throws std::runtime_error if the file cannot be opened.
  ///
  /// #### Examples
  ///
  /// @code
  /// std::vector<std::string> subjects = {"subject1", "subject2"};
  /// fs::write_subjectsfile("subjects.txt", subjects);
  /// @endcode
  void write_subjectsfile(const std::string &filename, const std::vector<std::string> &subjects)
  {
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out);
    if (ofs.is_open())
    {
      for (size_t i = 0; i < subjects.size(); i++)
      {
        ofs << subjects[i] << "\n";
      }
      ofs.close();
    }
    else
    {
      throw std::runtime_error("Unable to open subjects file '" + filename + "' for writing.\n");
    }
  }

  /// @brief Read MGH data from a stream.
  /// @param mgh An Mgh instance that should be filled with the data from the stream.
  /// @param is Pointer to an open istream from which to read the MGH data.
  /// @see There exists an overloaded version that reads from a file.
  /// @throws runtime_error if the file uses an unsupported MRI data type.
  void read_mgh(Mgh *mgh, std::istream *is)
  {
    MghHeader mgh_header;
    read_mgh_header(&mgh_header, is);
    mgh->header = mgh_header;
    if (mgh->header.dtype == MRI_INT)
    {
      std::vector<int32_t> data = _read_mgh_data_int(&mgh_header, is);
      mgh->data.data_mri_int = data;
    }
    else if (mgh->header.dtype == MRI_UCHAR)
    {
      std::vector<uint8_t> data = _read_mgh_data_uchar(&mgh_header, is);
      mgh->data.data_mri_uchar = data;
    }
    else if (mgh->header.dtype == MRI_FLOAT)
    {
      std::vector<float> data = _read_mgh_data_float(&mgh_header, is);
      mgh->data.data_mri_float = data;
    }
    else if (mgh->header.dtype == MRI_SHORT)
    {
      std::vector<short> data = _read_mgh_data_short(&mgh_header, is);
      mgh->data.data_mri_short = data;
    }
    else
    {
      throw std::runtime_error("Not reading data from MGH stream, data type " + std::to_string(mgh->header.dtype) + " not supported yet.\n");
    }
  }

  /// @brief Read an MGH header from a stream.
  /// @param mgh_header An MghHeader instance that should be filled with the data from the stream.
  /// @param is Pointer to an open istream from which to read the MGH data.
  /// @see There exists an overloaded version that reads from a file.
  /// @throws runtime_error if the file uses an unsupported MRI file format version. Only version 1 is supported (the only existing version to my knowledge).
  void read_mgh_header(MghHeader *mgh_header, std::istream *is)
  {
    const int MGH_VERSION = 1;

    int format_version = _freadt<int32_t>(*is);
    if (format_version != MGH_VERSION)
    {
      throw std::runtime_error("Invalid MGH file or unsupported file format version: expected version " + std::to_string(MGH_VERSION) + ", found " + std::to_string(format_version) + ".\n");
    }
    mgh_header->dim1length = _freadt<int32_t>(*is);
    mgh_header->dim2length = _freadt<int32_t>(*is);
    mgh_header->dim3length = _freadt<int32_t>(*is);
    mgh_header->dim4length = _freadt<int32_t>(*is);

    // Validate dimensions: must be positive (negative would wrap to huge size_t).
    if (mgh_header->dim1length <= 0 || mgh_header->dim2length <= 0 ||
        mgh_header->dim3length <= 0 || mgh_header->dim4length <= 0)
    {
      throw std::domain_error("MGH header contains non-positive dimension(s): dims=(" +
                               std::to_string(mgh_header->dim1length) + "," +
                               std::to_string(mgh_header->dim2length) + "," +
                               std::to_string(mgh_header->dim3length) + "," +
                               std::to_string(mgh_header->dim4length) + ").\n");
    }

    // Validate total number of values against allocation limit.
    if (!fs::util::check_alloc(static_cast<size_t>(mgh_header->dim1length) *
                                   static_cast<size_t>(mgh_header->dim2length) *
                                   static_cast<size_t>(mgh_header->dim3length),
                               static_cast<size_t>(mgh_header->dim4length)))
    {
      throw std::runtime_error("MGH header volume size exceeds maximum allowed allocation (" +
                               std::to_string(LIBFS_MAX_ALLOC_BYTES) + " bytes).\n");
    }

    mgh_header->dtype = _freadt<int32_t>(*is);
    mgh_header->dof = _freadt<int32_t>(*is);

    int unused_header_space_size_left = 256; // in bytes
    mgh_header->ras_good_flag = _freadt<int16_t>(*is);
    unused_header_space_size_left -= 2; // for the ras_good_flag

    // Read the RAS part of the header.
    if (mgh_header->ras_good_flag == 1)
    {
      mgh_header->xsize = _freadt<float>(*is);
      mgh_header->ysize = _freadt<float>(*is);
      mgh_header->zsize = _freadt<float>(*is);

      // Validate voxel sizes: must be finite and non-zero to prevent division-by-zero
      // and NaN/Inf propagation in spatial transform calculations.
      if (!fs::util::is_finite_float(mgh_header->xsize) ||
          !fs::util::is_finite_float(mgh_header->ysize) ||
          !fs::util::is_finite_float(mgh_header->zsize))
      {
        throw std::domain_error("MGH header contains NaN or Inf voxel size(s): x=" +
                                 std::to_string(mgh_header->xsize) + " y=" +
                                 std::to_string(mgh_header->ysize) + " z=" +
                                 std::to_string(mgh_header->zsize) + ".\n");
      }
      if (mgh_header->xsize == 0.0f || mgh_header->ysize == 0.0f || mgh_header->zsize == 0.0f)
      {
        throw std::domain_error("MGH header contains zero voxel size(s): x=" +
                                 std::to_string(mgh_header->xsize) + " y=" +
                                 std::to_string(mgh_header->ysize) + " z=" +
                                 std::to_string(mgh_header->zsize) + ".\n");
      }

      for (int i = 0; i < 9; i++)
      {
        mgh_header->Mdc.push_back(_freadt<float>(*is));
      }
      for (int i = 0; i < 3; i++)
      {
        mgh_header->Pxyz_c.push_back(_freadt<float>(*is));
      }

      // Validate the direction cosine matrix (Mdc) and center coordinates (Pxyz_c).
      for (size_t i = 0; i < mgh_header->Mdc.size(); i++)
      {
        if (!fs::util::is_finite_float(mgh_header->Mdc[i]))
        {
          throw std::domain_error("MGH header Mdc matrix contains NaN or Inf at index " +
                                   std::to_string(i) + ".\n");
        }
      }
      for (size_t i = 0; i < mgh_header->Pxyz_c.size(); i++)
      {
        if (!fs::util::is_finite_float(mgh_header->Pxyz_c[i]))
        {
          throw std::domain_error("MGH header Pxyz_c contains NaN or Inf at index " +
                                   std::to_string(i) + ".\n");
        }
      }

      unused_header_space_size_left -= 60;
    }

    // Advance to data part. We do not seek here because that is not
    // possible if the stream is gzip-wrapped with zstr, as in the read_mgz example.
    uint8_t discarded;
    while (unused_header_space_size_left > 0)
    {
      discarded = _freadt<uint8_t>(*is);
      unused_header_space_size_left -= 1;
    }
    (void)discarded; // Suppress warnings about unused variable.
  }

  /// @brief Read MRI_INT data from MGH file
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  std::vector<int32_t> _read_mgh_data_int(MghHeader *mgh_header, const std::string &filename)
  {
    if (mgh_header->dtype != MRI_INT)
    {
#ifdef LIBFS_DBG_ERROR
      std::cerr << "Expected MRI data type " << MRI_INT << ", but found " << mgh_header->dtype << ".\n";
#endif
    }
    return (_read_mgh_data<int32_t>(mgh_header, filename));
  }

  /// @brief Read MRI_INT data from a stream.
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  std::vector<int32_t> _read_mgh_data_int(MghHeader *mgh_header, std::istream *is)
  {
    if (mgh_header->dtype != MRI_INT)
    {
#ifdef LIBFS_DBG_ERROR
      std::cerr << "Expected MRI data type " << MRI_INT << ", but found " << mgh_header->dtype << ".\n";
#endif
    }
    return (_read_mgh_data<int32_t>(mgh_header, is));
  }

  /// @brief Read MRI_SHORT data from MGH file
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  std::vector<short> _read_mgh_data_short(MghHeader *mgh_header, const std::string &filename)
  {
    if (mgh_header->dtype != MRI_SHORT)
    {
#ifdef LIBFS_DBG_ERROR
      std::cerr << "Expected MRI data type " << MRI_SHORT << ", but found " << mgh_header->dtype << ".\n";
#endif
    }
    return (_read_mgh_data<short>(mgh_header, filename));
  }

  /// @brief Read MRI_SHORT data from a stream.
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  std::vector<short> _read_mgh_data_short(MghHeader *mgh_header, std::istream *is)
  {
    if (mgh_header->dtype != MRI_SHORT)
    {
#ifdef LIBFS_DBG_ERROR
      std::cerr << "Expected MRI data type " << MRI_SHORT << ", but found " << mgh_header->dtype << ".\n";
#endif
    }
    return (_read_mgh_data<short>(mgh_header, is));
  }

  /// @brief Read the header of a FreeSurfer volume file in MGH format into the given MghHeader struct.
  ///
  /// @param mgh_header An MghHeader instance that should be filled with the data from the file.
  /// @param filename Path to the file from which to read the MGH data.
  /// @see There exists an overloaded version that reads from a stream.
  /// @throws runtime_error if the file cannot be opened
  void read_mgh_header(MghHeader *mgh_header, const std::string &filename)
  {
    std::ifstream ifs;
    ifs.open(filename, std::ios_base::in | std::ios::binary);
    if (ifs.is_open())
    {
      read_mgh_header(mgh_header, &ifs);
      ifs.close();
    }
    else
    {
      throw std::runtime_error("Unable to open MGH file '" + filename + "'.\n");
    }
  }

  /// @brief Read arbitrary MGH data from a file.
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @throws runtime_error if the file cannot be opened
  /// @private
  template <typename T>
  std::vector<T> _read_mgh_data(MghHeader *mgh_header, const std::string &filename)
  {
    std::ifstream ifs;
    ifs.open(filename, std::ios_base::in | std::ios::binary);
    if (ifs.is_open())
    {
      size_t num_values = mgh_header->num_values();

      // Cross-check: ensure the file has enough data after the 284-byte header.
      size_t file_size = fs::util::get_file_size(filename);
      if (file_size > 0)
      {
        const size_t HEADER_SIZE = 284;
        size_t expected_data_bytes = 0;
        if (!fs::util::safe_multiply(num_values, sizeof(T), expected_data_bytes))
        {
          throw std::overflow_error("MGH data size computation overflowed.\n");
        }
        if (file_size < HEADER_SIZE || (file_size - HEADER_SIZE) < expected_data_bytes)
        {
          throw std::runtime_error("MGH file '" + filename + "' is too small (" +
                                   std::to_string(file_size) + " bytes) for the data claimed in its header (" +
                                   std::to_string(HEADER_SIZE + expected_data_bytes) + " bytes).\n");
        }
      }

      if (!fs::util::check_alloc(num_values, sizeof(T)))
      {
        throw std::runtime_error("MGH file data size exceeds maximum allowed allocation.\n");
      }

      ifs.seekg(284, ifs.beg); // skip to end of header and beginning of data

      std::vector<T> data;
      data.reserve(num_values);
      for (size_t i = 0; i < num_values; i++)
      {
        data.push_back(_freadt<T>(ifs));
      }
      ifs.close();
      return (data);
    }
    else
    {
      throw std::runtime_error("Unable to open MGH file '" + filename + "'.\n");
    }
  }

  /// @brief Read arbitrary MGH data from a stream. The stream must be open and at the beginning of the MGH data.
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  template <typename T>
  std::vector<T> _read_mgh_data(MghHeader *mgh_header, std::istream *is)
  {
    size_t num_values = mgh_header->num_values();
    if (!fs::util::check_alloc(num_values, sizeof(T)))
    {
      throw std::runtime_error("MGH stream data size exceeds maximum allowed allocation.\n");
    }
    std::vector<T> data;
    data.reserve(num_values);
    for (size_t i = 0; i < num_values; i++)
    {
      data.push_back(_freadt<T>(*is));
    }
    return (data);
  }

  /// @brief Read MRI_FLOAT data from MGH file
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  std::vector<float> _read_mgh_data_float(MghHeader *mgh_header, const std::string &filename)
  {
    if (mgh_header->dtype != MRI_FLOAT)
    {
#ifdef LIBFS_DBG_ERROR
      std::cerr << "Expected MRI data type " << MRI_FLOAT << ", but found " << mgh_header->dtype << ".\n";
#endif
    }
    return (_read_mgh_data<float>(mgh_header, filename));
  }

  /// @brief Read MRI_FLOAT data from an MGH stream
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  std::vector<float> _read_mgh_data_float(MghHeader *mgh_header, std::istream *is)
  {
    if (mgh_header->dtype != MRI_FLOAT)
    {
#ifdef LIBFS_DBG_ERROR
      std::cerr << "Expected MRI data type " << MRI_FLOAT << ", but found " << mgh_header->dtype << ".\n";
#endif
    }
    return (_read_mgh_data<float>(mgh_header, is));
  }

  /// @brief Read MRI_UCHAR data from MGH file
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  std::vector<uint8_t> _read_mgh_data_uchar(MghHeader *mgh_header, const std::string &filename)
  {
    if (mgh_header->dtype != MRI_UCHAR)
    {
#ifdef LIBFS_DBG_ERROR
      std::cerr << "Expected MRI data type " << MRI_UCHAR << ", but found " << mgh_header->dtype << ".\n";
#endif
    }
    return (_read_mgh_data<uint8_t>(mgh_header, filename));
  }

  /// @brief Read MRI_UCHAR data from an MGH stream
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  std::vector<uint8_t> _read_mgh_data_uchar(MghHeader *mgh_header, std::istream *is)
  {
    if (mgh_header->dtype != MRI_UCHAR)
    {
#ifdef LIBFS_DBG_ERROR
      std::cerr << "Expected MRI data type " << MRI_UCHAR << ", but found " << mgh_header->dtype << ".\n";
#endif
    }
    return (_read_mgh_data<uint8_t>(mgh_header, is));
  }

  /// @brief Read a brain mesh from a file in binary FreeSurfer 'surf' format into the given Mesh instance.
  ///
  /// @param surface a Mesh instance representing a vertex-indexed tri-mesh. This will be filled.
  /// @param filename The path to the file from which to read the mesh. Must be in binary FreeSurfer surf format. An example file is `surf/lh.white`.
  /// @throws runtime_error if the file cannot be opened, domain_error if the surf file magic mismatches.
  /// @see fs::read_mesh, a generalized version that supports other mesh file formats as well.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Mesh surface;
  /// fs::read_surf(&surface, "lh.white");
  /// @endcode
  void read_surf(Mesh *surface, const std::string &filename)
  {
    const int SURF_TRIS_MAGIC = 16777214;
    std::ifstream is;
    is.open(filename, std::ios_base::in | std::ios::binary);
    if (is.is_open())
    {
      int magic = _fread3(is);
      if (magic != SURF_TRIS_MAGIC)
      {
        throw std::domain_error("Surf file '" + filename + "' magic code in header did not match: expected " + std::to_string(SURF_TRIS_MAGIC) + ", found " + std::to_string(magic) + ".\n");
      }
      std::string created_line = _freadstringnewline(is);
      std::string comment_line = _freadstringnewline(is);
      int num_verts = _freadt<int32_t>(is);
      int num_faces = _freadt<int32_t>(is);

      // Validate header fields.
      if (num_verts <= 0)
      {
        throw std::domain_error("Surf file '" + filename + "' has invalid num_verts: " + std::to_string(num_verts) + ".\n");
      }
      if (num_faces < 0)
      {
        throw std::domain_error("Surf file '" + filename + "' has invalid num_faces: " + std::to_string(num_faces) + ".\n");
      }

      // Safe multiplication: num_verts * 3 (x,y,z per vertex).
      size_t num_vert_coords = 0;
      if (!fs::util::safe_multiply(static_cast<size_t>(num_verts), 3, num_vert_coords))
      {
        throw std::overflow_error("Surf file '" + filename + "': num_verts * 3 overflowed.\n");
      }
      size_t num_face_indices = 0;
      if (!fs::util::safe_multiply(static_cast<size_t>(num_faces), 3, num_face_indices))
      {
        throw std::overflow_error("Surf file '" + filename + "': num_faces * 3 overflowed.\n");
      }

      // Cross-check against file size.
      size_t file_size = fs::util::get_file_size(filename);
      if (file_size > 0)
      {
        size_t vert_bytes = 0, face_bytes = 0;
        if (!fs::util::safe_multiply(num_vert_coords, sizeof(float), vert_bytes) ||
            !fs::util::safe_multiply(num_face_indices, sizeof(int32_t), face_bytes))
        {
          throw std::overflow_error("Surf file '" + filename + "': expected data size overflowed.\n");
        }
        // Guard against addition overflow (paranoid, since each is already <= LIBFS_MAX_ALLOC_BYTES).
        if (vert_bytes > std::numeric_limits<size_t>::max() - face_bytes)
        {
          throw std::overflow_error("Surf file '" + filename + "': total data size overflowed.\n");
        }
        size_t expected_total = vert_bytes + face_bytes;
        // Header takes some space, so file_size > raw data size for any valid file.
        if (file_size < expected_total)
        {
          throw std::runtime_error("Surf file '" + filename + "' is too small (" +
                                   std::to_string(file_size) + " bytes) for the data claimed in its header.\n");
        }
      }

      if (!fs::util::check_alloc(num_vert_coords, sizeof(float)) ||
          !fs::util::check_alloc(num_face_indices, sizeof(int32_t)))
      {
        throw std::runtime_error("Surf file '" + filename + "' data size exceeds maximum allowed allocation.\n");
      }

#ifdef LIBFS_DBG_INFO
      std::cout << LIBFS_APPTAG << "Read surface file with " << num_verts << " vertices, " << num_faces << " faces.\n";
#endif
      std::vector<float> vdata;
      vdata.reserve(num_vert_coords);
      for (size_t i = 0; i < num_vert_coords; i++)
      {
        vdata.push_back(_freadt<float>(is));
      }
      std::vector<int> fdata;
      fdata.reserve(num_face_indices);
      for (size_t i = 0; i < num_face_indices; i++)
      {
        fdata.push_back(_freadt<int32_t>(is));
      }
      is.close();
      surface->vertices = vdata;
      surface->faces = fdata;
    }
    else
    {
      throw std::runtime_error("Unable to open surface file '" + filename + "'.\n");
    }
  }

  /// @brief Read a triangular mesh from a surf, obj, or ply file into the given Mesh instance.
  ///
  /// @param surface a Mesh instance representing a vertex-indexed tri-mesh. This will be filled.
  /// @param filename The path to the file from which to read the mesh. The format will be determined from the file extension as follows. File names ending with '.obj' are loaded as Wavefront OBJ files. File names ending with '.ply' are loaded as Stanford PLY files in format version 'ascii 1.0'. All other files are loaded as FreeSurfer binary surf files.
  /// @throws runtime_error if the file cannot be opened, domain_error if the surf file magic mismatches.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::mesh surface;
  /// fs::read_mesh(&surface, "subject1/surf/lh.thickness");
  /// @endcode
  void read_mesh(Mesh *surface, const std::string &filename)
  {
    if (fs::util::ends_with(filename, ".obj"))
    {
      fs::Mesh::from_obj(surface, filename);
    }
    else if (fs::util::ends_with(filename, ".ply"))
    {
      fs::Mesh::from_ply(surface, filename);
    }
    else if (fs::util::ends_with(filename, ".off"))
    {
      fs::Mesh::from_off(surface, filename);
    }
    else
    {
      read_surf(surface, filename);
    }
  }

  /// @brief Determine the endianness of the system.
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @return boolean, whether the current system is big endian.
  /// @private
  bool _is_bigendian()
  {
    const short int number = 0x1;
    const char *numPtr = reinterpret_cast<const char *>(&number);
    return (numPtr[0] != 1);
  }

  /// @brief Read per-vertex brain morphometry data from a FreeSurfer curv stream.
  /// @details The curv format is a simple binary format that stores one floating point value per vertex of a related brain surface.
  /// @param curv A Curv instance to be filled.
  /// @param is An open istream from which to read the curv data.
  /// @param source_filename Optional human-readable name for the data source
  ///        (e.g., `"lh.thickness"`).  When supplied, it is included in
  ///        exception messages to help identify which file caused the error.
  ///        Leave empty (the default) to omit the filename from diagnostics.
  /// @throws domain_error if the curv file magic mismatches or the curv file header claims that the file contains more than 1 value per vertex.
  void read_curv(Curv *curv, std::istream *is, const std::string &source_filename = "")
  {
    const std::string msg_source_file_part = source_filename.empty() ? "" : "'" + source_filename + "' ";
    const int CURV_MAGIC = 16777215;
    int magic = _fread3(*is);
    if (magic != CURV_MAGIC)
    {
      throw std::domain_error("Curv file " + msg_source_file_part + "header magic did not match: expected " + std::to_string(CURV_MAGIC) + ", found " + std::to_string(magic) + ".\n");
    }
    curv->num_vertices = _freadt<int32_t>(*is);
    curv->num_faces = _freadt<int32_t>(*is);
    curv->num_values_per_vertex = _freadt<int32_t>(*is);

    // Validate header fields.
    if (curv->num_vertices <= 0)
    {
      throw std::domain_error("Curv file " + msg_source_file_part + "has invalid num_vertices: " + std::to_string(curv->num_vertices) + ".\n");
    }
    if (curv->num_faces < 0)
    {
      throw std::domain_error("Curv file " + msg_source_file_part + "has invalid num_faces: " + std::to_string(curv->num_faces) + ".\n");
    }

#ifdef LIBFS_DBG_INFO
    std::cout << LIBFS_APPTAG << "Read curv file with " << curv->num_vertices << " vertices, " << curv->num_faces << " faces and " << curv->num_values_per_vertex << " values per vertex.\n";
#endif
    if (curv->num_values_per_vertex != 1)
    { // Not supported, I know no case where this is used. Please submit a PR with a demo file if you have one, and let me know where it came from.
      throw std::domain_error("Curv file " + msg_source_file_part + "must contain exactly 1 value per vertex, found " + std::to_string(curv->num_values_per_vertex) + ".\n");
    }

    // File-size cross-check (only when reading from a file, not a generic stream).
    if (!source_filename.empty())
    {
      size_t file_size = fs::util::get_file_size(source_filename);
      if (file_size > 0)
      {
        // Curv header: 3 (magic) + 12 (three int32) = 15 bytes.
        const size_t CURV_HEADER_SIZE = 15;
        size_t expected_data_bytes = 0;
        if (!fs::util::safe_multiply(static_cast<size_t>(curv->num_vertices), sizeof(float), expected_data_bytes))
        {
          throw std::overflow_error("Curv file " + msg_source_file_part + "data size computation overflowed.\n");
        }
        if (file_size < CURV_HEADER_SIZE || (file_size - CURV_HEADER_SIZE) < expected_data_bytes)
        {
          throw std::runtime_error("Curv file " + msg_source_file_part + "is too small (" +
                                   std::to_string(file_size) + " bytes) for the data claimed in its header (" +
                                   std::to_string(CURV_HEADER_SIZE + expected_data_bytes) + " bytes expected).\n");
        }
      }
    }

    std::vector<float> data;
    if (!fs::util::check_alloc(static_cast<size_t>(curv->num_vertices), sizeof(float)))
    {
      throw std::runtime_error("Curv file " + msg_source_file_part + "data size exceeds maximum allowed allocation.\n");
    }
    data.reserve(static_cast<size_t>(curv->num_vertices));
    for (size_t i = 0; i < static_cast<size_t>(curv->num_vertices); i++)
    {
      data.push_back(_freadt<float>(*is));
    }
    curv->data = data;
  }

  /// @brief Read Curv instance from a FreeSurfer curv format file.
  /// @details The curv format is a simple binary format that stores one floating point value per vertex of a related brain surface.
  /// @param curv A Curv instance to be filled.
  /// @param filename Path to a file from which to read the curv data.
  /// @throws runtime_error if the file cannot be opened, domain_error if the curv file magic mismatches or the curv file header claims that the file contains more than 1 value per vertex.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Curv curv;
  /// fs::read_curv(&curv, "examples/read_curv/lh.thickness");
  /// @endcode
  void read_curv(Curv *curv, const std::string &filename)
  {
    std::ifstream is(filename, std::fstream::in | std::fstream::binary);
    if (is.is_open())
    {
      read_curv(curv, &is, filename);
      is.close();
    }
    else
    {
      throw std::runtime_error("Could not open curv file '" + filename + "' for reading.\n");
    }
  }

  /// Read an Annot Colortable from a stream.
  /// @private
  void _read_annot_colortable(Colortable *colortable, std::istream *is, int32_t num_entries)
  {
    // Validate num_entries against a reasonable cap.
    if (num_entries < 0 || static_cast<size_t>(num_entries) > LIBFS_MAX_COLORTABLE_ENTRIES)
    {
      throw std::domain_error("Annot colortable num_entries " + std::to_string(num_entries) +
                               " is invalid or exceeds maximum (" + std::to_string(LIBFS_MAX_COLORTABLE_ENTRIES) + ").\n");
    }

    int32_t num_chars_orig_filename = _freadt<int32_t>(*is); // The number of characters of the file this annot was built from.

    // Validate and cap the original filename length.
    if (num_chars_orig_filename < 0 || static_cast<size_t>(num_chars_orig_filename) > LIBFS_MAX_STRING_LENGTH)
    {
      throw std::domain_error("Annot colortable original filename length " + std::to_string(num_chars_orig_filename) +
                               " exceeds maximum (" + std::to_string(LIBFS_MAX_STRING_LENGTH) + ").\n");
    }

    // It follows the name of the file this annot was built from. This is development metadata and irrelevant afaik. We skip it.
    uint8_t discarded;
    for (int32_t i = 0; i < num_chars_orig_filename; i++)
    {
      discarded = _freadt<uint8_t>(*is);
    }
    (void)discarded; // Suppress warnings about unused variable.

    int32_t num_entries_duplicated = _freadt<int32_t>(*is); // Yes, once more.
    if (num_entries != num_entries_duplicated)
    {
#ifdef LIBFS_DBG_ERROR
      std::cerr << "Warning: the two num_entries header fields of this annotation do not match. Use with care.\n";
#endif
    }

    colortable->id.reserve(static_cast<size_t>(num_entries));
    colortable->name.reserve(static_cast<size_t>(num_entries));
    colortable->r.reserve(static_cast<size_t>(num_entries));
    colortable->g.reserve(static_cast<size_t>(num_entries));
    colortable->b.reserve(static_cast<size_t>(num_entries));
    colortable->a.reserve(static_cast<size_t>(num_entries));
    colortable->label.reserve(static_cast<size_t>(num_entries));

    int32_t entry_num_chars;
    for (int32_t i = 0; i < num_entries; i++)
    {
      colortable->id.push_back(_freadt<int32_t>(*is));
      entry_num_chars = _freadt<int32_t>(*is);
      // Pass a tighter max_length for region names (256 chars should be plenty).
      colortable->name.push_back(_freadfixedlengthstring(*is, entry_num_chars, true, 256));
      colortable->r.push_back(_freadt<int32_t>(*is));
      colortable->g.push_back(_freadt<int32_t>(*is));
      colortable->b.push_back(_freadt<int32_t>(*is));
      colortable->a.push_back(_freadt<int32_t>(*is));
      colortable->label.push_back(static_cast<uint32_t>(colortable->r[i]) + static_cast<uint32_t>(colortable->g[i]) * 256u + static_cast<uint32_t>(colortable->b[i]) * 65536u + static_cast<uint32_t>(colortable->a[i]) * 16777216u);
    }
  }

  /// Compute the vector index for treating a vector of length n*m as a matrix with n rows and m columns.
  /// @private
  size_t _vidx_2d(size_t row, size_t column, size_t row_length = 3)
  {
    return (row + 1) * row_length - row_length + column;
  }

  /// @brief Read a FreeSurfer annotation or brain surface parcellation from an annot stream.
  /// @details A brain parcellations contains a region table and assigns to each vertex of a surface a region.
  /// @param annot An Annot instance to be filled.
  /// @param is An open istream from which to read the annot data.
  /// @throws domain_error if the file format version is not supported or the file is missing the color table.
  void read_annot(Annot *annot, std::istream *is)
  {

    int32_t num_vertices = _freadt<int32_t>(*is);

    // Validate num_vertices.
    if (num_vertices <= 0)
    {
      throw std::domain_error("Annot file has invalid num_vertices: " + std::to_string(num_vertices) + ".\n");
    }

    // Safe multiplication: num_vertices * 2 (vertex index + label per vertex).
    size_t num_entries = 0;
    if (!fs::util::safe_multiply(static_cast<size_t>(num_vertices), 2, num_entries))
    {
      throw std::overflow_error("Annot: num_vertices * 2 overflowed.\n");
    }
    if (!fs::util::check_alloc(num_entries, sizeof(int32_t)))
    {
      throw std::runtime_error("Annot vertex/label data size exceeds maximum allowed allocation.\n");
    }

    std::vector<int32_t> vertices;
    std::vector<int32_t> labels;
    vertices.reserve(num_vertices);
    labels.reserve(num_vertices);
    for (size_t i = 0; i < num_entries; i++)
    { // The vertices and their labels are stored directly after one another: v1,v1_label,v2,v2_label,...
      if (i % 2 == 0)
      {
        vertices.push_back(_freadt<int32_t>(*is));
      }
      else
      {
        labels.push_back(_freadt<int32_t>(*is));
      }
    }
    annot->vertex_indices = vertices;
    annot->vertex_labels = labels;
    int32_t has_colortable = _freadt<int32_t>(*is);
    if (has_colortable == 1)
    {
      int32_t num_colortable_entries_old_format = _freadt<int32_t>(*is);
      if (num_colortable_entries_old_format > 0)
      {
        throw std::domain_error("Reading annotation in old format not supported. Please open an issue and supply an example file if you need this.\n");
      }
      else
      {
        int32_t colortable_format_version = -num_colortable_entries_old_format; // If the value is negative, we are in new format and its absolute value is the format version.
        if (colortable_format_version == 2)
        {
          int32_t num_colortable_entries = _freadt<int32_t>(*is); // This time for real.
          _read_annot_colortable(&annot->colortable, is, num_colortable_entries);
        }
        else
        {
          throw std::domain_error("Reading annotation in new format version !=2 not supported. Please open an issue and supply an example file if you need this.\n");
        }
      }
    }
    else
    {
      throw std::domain_error("Reading annotation without colortable not supported. Maybe invalid annotation file?\n");
    }
  }

  /// @brief Read a FreeSurfer annotation from a file.
  /// @param annot An Annot instance that should be filled.
  /// @param filename Path to the label file that should be read.
  /// @see There exists an overload to read from a stream instead.
  /// @throws runtime_error if the file cannot be opened, domain_error if the file format version is not supported or the file is missing the color table.
  ///
  /// #### Examples
  ///
  /// @code
  /// std::string annot_fname = "lh.aparc.annot";
  /// fs::Annot annot;
  /// fs::read_annot(&annot, annot_fname);
  /// @endcode
  void read_annot(Annot *annot, const std::string &filename)
  {
    std::ifstream is(filename, std::fstream::in | std::fstream::binary);
    if (is.is_open())
    {
      read_annot(annot, &is);
      is.close();
    }
    else
    {
      throw std::runtime_error("Could not open annot file '" + filename + "' for reading.\n");
    }
  }

  /// @brief Read per-vertex brain morphometry data from a FreeSurfer curv format file.
  /// @details The curv format is a simple binary format that stores one floating point value per vertex of a related brain surface.
  /// @param filename Path to a file from which to read the curv data.
  /// @return a vector of float values, one per vertex.
  /// @throws runtime_error if the file cannot be opened, domain_error if the curv file magic mismatches or the curv file header claims that the file contains more than 1 value per vertex.
  ///
  /// #### Examples
  ///
  /// @code
  /// std::string curv_fname = "lh.thickness";
  /// std::vector<float> data = fs::read_curv_data(curv_fname);
  /// @endcode
  std::vector<float> read_curv_data(const std::string &filename)
  {
    Curv curv;
    read_curv(&curv, filename);
    return (curv.data);
  }

  /// @brief Read per-vertex brain morphometry data from a FreeSurfer curv, MGH, or NIfTI format file.
  /// @param filename Path to a file from which to read the data. If the name ends with '.mgh' / '.MGH' it is read as MGH; if it ends with '.nii' / '.nii.gz' it is read as NIfTI; otherwise it is assumed to be a curv file.  The data must be of type MRI_FLOAT, and only one spatial dimension may have size > 1.
  /// @return a vector of float values, one per vertex.
  /// @throws runtime_error if the file cannot be opened, domain_error if the curv file magic mismatches or the curv file header claims that the file contains more than 1 value per vertex.
  ///
  /// #### Examples
  ///
  /// @code
  /// std::string curv_fname = "lh.thickness";
  /// std::vector<float> data1 = fs::read_desc_data(curv_fname);
  /// std::string mgh_fname = "lh.thickness.mgh";
  /// std::vector<float> data2 = fs::read_desc_data(mgh_fname);
  /// std::string nii_fname = "lh.thickness.nii.gz";
  /// std::vector<float> data3 = fs::read_desc_data(nii_fname);
  /// @endcode
  inline std::vector<float> read_desc_data(const std::string &filename)
  {
    if (fs::util::ends_with(filename, {".MGH", ".mgh"}))
    {
      fs::Mgh mgh;
      fs::read_mgh(&mgh, filename);
      assert(mgh.header.dtype == fs::MRI_FLOAT);
      int num_gt_1 = 0;
      std::vector<int> dims = {mgh.header.dim1length, mgh.header.dim2length, mgh.header.dim3length, mgh.header.dim4length};
      for (size_t i = 0; i < dims.size(); i++)
      {
        if (dims[i] > 1)
        {
          num_gt_1++;
        }
      }
      if (num_gt_1 > 1)
      {
#ifdef LIBFS_DBG_ERROR
        std::cerr << "MGH file '" << filename << "' contains more than one non-empty dimension. Returning concatinated data.\n";
#endif
      }
      return mgh.data.data_mri_float;
    }
    else if (fs::util::ends_with(filename, {".NII", ".nii", ".NII.GZ", ".nii.gz"}))
    {
      fs::Mgh mgh;
      fs::read_nifti(&mgh, filename);
      if (mgh.header.dtype != fs::MRI_FLOAT)
      {
        throw std::runtime_error("read_desc_data currently only supports NIfTI files with FLOAT32 data.\n");
      }
      int num_gt_1 = 0;
      std::vector<int> dims = {mgh.header.dim1length, mgh.header.dim2length, mgh.header.dim3length, mgh.header.dim4length};
      for (size_t i = 0; i < dims.size(); i++)
      {
        if (dims[i] > 1)
        {
          num_gt_1++;
        }
      }
      if (num_gt_1 > 1)
      {
#ifdef LIBFS_DBG_ERROR
        std::cerr << "NIfTI file '" << filename << "' contains more than one non-empty dimension. Returning concatenated data.\n";
#endif
      }
      return mgh.data.data_mri_float;
    }
    else
    {
      Curv curv;
      read_curv(&curv, filename);
      return (curv.data);
    }
  }

  /// Swap endianness of a value.
  ///
  /// Uses memcpy through unsigned char (well-defined in C++11) rather than
  /// union type-punning (which is UB in C++, only valid in C).
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  template <typename T>
  T _swap_endian(T u)
  {
    static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

    unsigned char src[sizeof(T)];
    unsigned char dst[sizeof(T)];
    std::memcpy(src, &u, sizeof(T));

    for (size_t k = 0; k < sizeof(T); k++)
    {
      dst[k] = src[sizeof(T) - k - 1];
    }

    T result;
    std::memcpy(&result, dst, sizeof(T));
    return result;
  }

  /// Read a big endian value from a stream.
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  template <typename T>
  T _freadt(std::istream &is)
  {
    T t;
    is.read(reinterpret_cast<char *>(&t), sizeof(t));
    if (static_cast<size_t>(is.gcount()) != sizeof(T))
    {
      if (is.gcount() == 0)
      {
        throw std::runtime_error("Unexpected end of binary stream: expected " + std::to_string(sizeof(T)) + " bytes, got EOF.\n");
      }
      throw std::runtime_error("Short read in binary stream: expected " + std::to_string(sizeof(T)) + " bytes, got " + std::to_string(is.gcount()) + ".\n");
    }
    if (!_is_bigendian())
    {
      t = _swap_endian<T>(t);
    }
    return (t);
  }

  /// Read 3 big endian bytes as a single integer from a stream.
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  int _fread3(std::istream &is)
  {
    uint32_t i = 0;
    is.read(reinterpret_cast<char *>(&i), 3);
    if (static_cast<size_t>(is.gcount()) != 3)
    {
      if (is.gcount() == 0)
      {
        throw std::runtime_error("Unexpected end of binary stream: expected 3 bytes, got EOF.\n");
      }
      throw std::runtime_error("Short read in binary stream: expected 3 bytes, got " + std::to_string(is.gcount()) + ".\n");
    }
    if (!_is_bigendian())
    {
      i = _swap_endian<std::uint32_t>(i);
    }
    i = ((i >> 8) & 0xffffff);
    return (i);
  }

  /// Write a value to a stream as big endian.
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  template <typename T>
  void _fwritet(std::ostream &os, T t)
  {
    if (!_is_bigendian())
    {
      t = _swap_endian<T>(t);
    }
    os.write(reinterpret_cast<const char *>(&t), sizeof(t));
  }

  // Write big endian 24 bit integer to a stream, extracted from the first 3 bytes of an unsigned 32 bit integer.
  //
  // THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  void _fwritei3(std::ostream &os, uint32_t i)
  {
    unsigned char b1 = (i >> 16) & 255;
    unsigned char b2 = (i >> 8) & 255;
    unsigned char b3 = i & 255;

    os.write(reinterpret_cast<const char *>(&b1), sizeof(b1));
    os.write(reinterpret_cast<const char *>(&b2), sizeof(b2));
    os.write(reinterpret_cast<const char *>(&b3), sizeof(b3));
  }

  /// Write a fixed-length C-style string to a binary stream. Writes exactly @p len bytes
  /// (padded with null bytes if the string is shorter, truncated if longer).
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  void _fwritefixedlengthstring(std::ostream &os, const std::string &str, size_t len)
  {
    std::string buf(len, '\0');
    size_t copy_len = str.size() < len ? str.size() : len;
    std::memcpy(&buf[0], str.data(), copy_len);
    os.write(buf.data(), static_cast<std::streamsize>(len));
  }

  /// Read a '\n'-terminated ASCII string from a stream.
  ///
  /// THIS FUNCTION IS INTERNAL AND SHOULD NOT BE CALLED BY API CLIENTS.
  /// @private
  std::string _freadstringnewline(std::istream &is)
  {
    std::string s;
    std::getline(is, s, '\n');
    return s;
  }

  /// Read a fixed length C-style string from an open binary stream. This does not care about trailing NULL bytes or anything, it just reads the given length of bytes.
  /// @throws std::domain_error if length is zero or exceeds max_length.
  /// @throws std::runtime_error if a short read occurs.
  /// @private
  std::string _freadfixedlengthstring(std::istream &is, size_t length, bool strip_last_char = true, size_t max_length = LIBFS_MAX_STRING_LENGTH)
  {
    if (length == 0)
    {
      throw std::domain_error("Fixed-length string read with zero length.\n");
    }
    if (length > max_length)
    {
      throw std::domain_error("Fixed-length string length " + std::to_string(length) + " exceeds maximum " + std::to_string(max_length) + ".\n");
    }
    std::string str;
    str.resize(length);
    is.read(&str[0], length);
    if (static_cast<size_t>(is.gcount()) != length)
    {
      if (is.gcount() == 0)
      {
        throw std::runtime_error("Unexpected end of binary stream while reading fixed-length string: expected " + std::to_string(length) + " bytes, got EOF.\n");
      }
      throw std::runtime_error("Short read in binary stream while reading fixed-length string: expected " + std::to_string(length) + " bytes, got " + std::to_string(is.gcount()) + ".\n");
    }
    if (strip_last_char)
    {
      str = str.substr(0, length - 1);
    }
    return str;
  }

  /// @brief Write a FreeSurfer annotation (brain surface parcellation) to a stream.
  /// @param annot The Annot instance to write.
  /// @param os An open output stream (binary mode).
  /// @see There exists an overload to write to a file.
  /// @throws std::domain_error if the annot or colortable data is inconsistent.
  void write_annot(const Annot &annot, std::ostream &os)
  {
    int32_t num_vertices = static_cast<int32_t>(annot.num_vertices());
    _fwritet<int32_t>(os, num_vertices);

    // Interleaved vertex indices and labels.
    for (size_t i = 0; i < static_cast<size_t>(num_vertices); i++)
    {
      _fwritet<int32_t>(os, annot.vertex_indices[i]);
      _fwritet<int32_t>(os, annot.vertex_labels[i]);
    }

    // Colortable presence flag + version tag (version 2, no old-format entries).
    _fwritet<int32_t>(os, 1);  // has_colortable
    _fwritet<int32_t>(os, -2); // version tag: negative means new format, abs value is version

    int32_t num_entries = static_cast<int32_t>(annot.colortable.num_entries());
    _fwritet<int32_t>(os, num_entries);

    // Original filename (not meaningful when writing, write "unknown" as placeholder).
    std::string orig_filename = "unknown";
    int32_t orig_filename_len = static_cast<int32_t>(orig_filename.size());
    _fwritet<int32_t>(os, orig_filename_len);
    _fwritefixedlengthstring(os, orig_filename, static_cast<size_t>(orig_filename_len));

    // Duplicate num_entries (yes, the format stores it twice).
    _fwritet<int32_t>(os, num_entries);

    for (int32_t i = 0; i < num_entries; i++)
    {
      _fwritet<int32_t>(os, annot.colortable.id[i]);
      // Name length: strlen + 1 for the trailing null byte, matching _freadfixedlengthstring(strip_last_char=true).
      int32_t name_len = static_cast<int32_t>(annot.colortable.name[i].size()) + 1;
      _fwritet<int32_t>(os, name_len);
      _fwritefixedlengthstring(os, annot.colortable.name[i] + '\0', static_cast<size_t>(name_len));
      _fwritet<int32_t>(os, annot.colortable.r[i]);
      _fwritet<int32_t>(os, annot.colortable.g[i]);
      _fwritet<int32_t>(os, annot.colortable.b[i]);
      _fwritet<int32_t>(os, annot.colortable.a[i]);
    }
  }

  /// @brief Write a FreeSurfer annotation (brain surface parcellation) to a file.
  /// @param annot The Annot instance to write.
  /// @param filename Path to the output file.
  /// @see There exists an overload to write to a stream.
  /// @throws std::runtime_error if the file cannot be opened.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Annot annot;
  /// fs::read_annot(&annot, "lh.aparc.annot");
  /// // modify annot here …
  /// fs::write_annot(annot, "lh.aparc.modified.annot");
  /// @endcode
  void write_annot(const Annot &annot, const std::string &filename)
  {
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out | std::ofstream::binary);
    if (ofs.is_open())
    {
      write_annot(annot, ofs);
      ofs.close();
    }
    else
    {
      throw std::runtime_error("Unable to open annot file '" + filename + "' for writing.\n");
    }
  }

  /// @brief Write curv data to a stream.
  /// @details A curv file contains one floating point value per vertex (or a related mesh).
  /// @param os An output stream to which to write the data. The stream must be open, and this function will not close it after writing to it.
  /// @param curv_data the data to write.
  /// @param num_faces the value for the header field `num_faces`. This is not needed afaik and typically ignored.
  void write_curv(std::ostream &os, std::vector<float> curv_data, int32_t num_faces = 100000)
  {
    const uint32_t CURV_MAGIC = 16777215;
    _fwritei3(os, CURV_MAGIC);
    _fwritet<int32_t>(os, int(curv_data.size()));
    _fwritet<int32_t>(os, num_faces);
    _fwritet<int32_t>(os, 1); // Number of values per vertex.
    for (size_t i = 0; i < curv_data.size(); i++)
    {
      _fwritet<float>(os, curv_data[i]);
    }
  }

  /// @brief Write curv data to a file.
  /// @details A curv file contains one floating point value per vertex (or a related mesh).
  /// @param filename The path to the output file.
  /// @param curv_data the data to write.
  /// @param num_faces the value for the header field `num_faces`. This is not needed afaik and typically ignored.
  /// @throws std::runtime_error if the file cannot be opened.
  ///
  /// #### Examples
  ///
  /// @code
  /// std::vector<float> data = fs::read_curv_data("lh.thickness");
  /// // Do something with 'data' here, maybe?
  /// fs::write_curv("output.curv", data);
  /// @endcode
  void write_curv(const std::string &filename, std::vector<float> curv_data, const int32_t num_faces = 100000)
  {
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out | std::ofstream::binary);
    if (ofs.is_open())
    {
      write_curv(ofs, curv_data, num_faces);
      ofs.close();
    }
    else
    {
      throw std::runtime_error("Unable to open curvature file '" + filename + "' for writing.\n");
    }
  }

  /// @brief Write MGH data to a stream.
  /// @details The MGH format is a binary, big-endian FreeSurfer file format for storing 4D data. Several data types are supported, and one has to check the header to see which one is contained in a file.
  /// @param mgh An Mgh instance that should be written.
  /// @param os An output stream to which to write the data. The stream must be open, and this function will not close it after writing to it.
  /// @throws std::logic_error if the mgh header and data are inconsistent, std::domain_error if the given MRI data type is unknown or unsupported.
  void write_mgh(const Mgh &mgh, std::ostream &os)
  {
    _fwritet<int32_t>(os, 1); // MGH file format version
    _fwritet<int32_t>(os, mgh.header.dim1length);
    _fwritet<int32_t>(os, mgh.header.dim2length);
    _fwritet<int32_t>(os, mgh.header.dim3length);
    _fwritet<int32_t>(os, mgh.header.dim4length);

    _fwritet<int32_t>(os, mgh.header.dtype);
    _fwritet<int32_t>(os, mgh.header.dof);

    size_t unused_header_space_size_left = 256; // in bytes
    _fwritet<int16_t>(os, mgh.header.ras_good_flag);
    unused_header_space_size_left -= 2; // for RAS flag

    // Write RAS part of of header if flag is 1.
    if (mgh.header.ras_good_flag == 1)
    {
      if (mgh.header.Mdc.size() < 9 || mgh.header.Pxyz_c.size() < 3)
      {
        throw std::logic_error("MGH header ras_good_flag set but Mdc and/or Pxyz_c vectors are undersized.\n");
      }
      _fwritet<float>(os, mgh.header.xsize);
      _fwritet<float>(os, mgh.header.ysize);
      _fwritet<float>(os, mgh.header.zsize);

      for (int i = 0; i < 9; i++)
      {
        _fwritet<float>(os, mgh.header.Mdc[i]);
      }
      for (int i = 0; i < 3; i++)
      {
        _fwritet<float>(os, mgh.header.Pxyz_c[i]);
      }

      unused_header_space_size_left -= 60;
    }

    for (size_t i = 0; i < unused_header_space_size_left; i++)
    { // Fill rest of header space.
      _fwritet<uint8_t>(os, 0);
    }

    // Write data
    size_t num_values = mgh.header.num_values();
    if (mgh.header.dtype == MRI_INT)
    {
      if (mgh.data.data_mri_int.size() != num_values)
      {
        throw std::logic_error("Detected mismatch of MRI_INT data size and MGH header dim length values.\n");
      }
      for (size_t i = 0; i < num_values; i++)
      {
        _fwritet<int32_t>(os, mgh.data.data_mri_int[i]);
      }
    }
    else if (mgh.header.dtype == MRI_FLOAT)
    {
      if (mgh.data.data_mri_float.size() != num_values)
      {
        throw std::logic_error("Detected mismatch of MRI_FLOAT data size and MGH header dim length values.\n");
      }
      for (size_t i = 0; i < num_values; i++)
      {
        _fwritet<float>(os, mgh.data.data_mri_float[i]);
      }
    }
    else if (mgh.header.dtype == MRI_UCHAR)
    {
      if (mgh.data.data_mri_uchar.size() != num_values)
      {
        throw std::logic_error("Detected mismatch of MRI_UCHAR data size and MGH header dim length values.\n");
      }
      for (size_t i = 0; i < num_values; i++)
      {
        _fwritet<uint8_t>(os, mgh.data.data_mri_uchar[i]);
      }
    }
    else if (mgh.header.dtype == MRI_SHORT)
    {
      if (mgh.data.data_mri_short.size() != num_values)
      {
        throw std::logic_error("Detected mismatch of MRI_SHORT data size and MGH header dim length values.\n");
      }
      for (size_t i = 0; i < num_values; i++)
      {
        _fwritet<short>(os, mgh.data.data_mri_short[i]);
      }
    }
    else
    {
      throw std::domain_error("Unsupported MRI data type " + std::to_string(mgh.header.dtype) + ", cannot write MGH data.\n");
    }
  }

  /// @brief Write MGH data to a file.
  /// @details The MGH format is a binary, big-endian FreeSurfer file format for storing 4D data. Several data types are supported, and one has to check the header to see which one is contained in a file.
  /// @param mgh An Mgh instance that should be written.
  /// @param filename Path to an output file to which to write.
  /// @see There exists an overload to write to a stream.
  /// @throws std::runtime_error if the file cannot be opened, std::logic_error if the mgh header and data are inconsistent, std::domain_error if the given MRI data type is unknown or unsupported.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Mgh mgh;
  /// fs::read_mgh(&mgh, "somebrain.mgh");
  /// // Do something with 'mgh' here, maybe?
  /// fs::write_mgh(mgh, "output.mgh");
  /// @endcode
  void write_mgh(const Mgh &mgh, const std::string &filename)
  {
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out | std::ofstream::binary);
    if (ofs.is_open())
    {
      write_mgh(mgh, ofs);
      ofs.close();
    }
    else
    {
      throw std::runtime_error("Unable to open MGH file '" + filename + "' for writing.\n");
    }
  }

#ifdef LIBFS_HAS_ZLIB

  /// @brief Read a FreeSurfer volume file in MGZ format (gzipped MGH) into the given Mgh struct.
  /// @details The MGZ format is just a gzipped MGH file. This function uses zlib to decompress the
  ///          file and then delegates to the stream-based read_mgh(). Requires linking with -lz.
  /// @param mgh An Mgh instance that should be filled with the data from the file.
  /// @param filename Path to the input MGZ file.
  /// @throws std::runtime_error if the file cannot be opened or decompressed.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Mgh mgh;
  /// fs::read_mgz(&mgh, "brain.mgz");
  /// @endcode
  inline void read_mgz(Mgh *mgh, const std::string &filename)
  {
    gzFile gz = gzopen(filename.c_str(), "rb");
    if (!gz)
    {
      int errnum = 0;
      const char *errstr = gzerror(gz, &errnum);
      throw std::runtime_error("Could not open MGZ file '" + filename + "' for reading: " +
                               (errstr ? std::string(errstr) : "unknown error") + "\n");
    }
    std::vector<char> buf;
    char chunk[131072];
    int n;
    while ((n = gzread(gz, chunk, sizeof(chunk))) > 0)
    {
      buf.insert(buf.end(), chunk, chunk + n);
    }
    if (n < 0)
    {
      int errnum = 0;
      const char *errstr = gzerror(gz, &errnum);
      gzclose(gz);
      throw std::runtime_error("Error decompressing MGZ file '" + filename + "': " +
                               (errstr ? std::string(errstr) : "unknown error") + "\n");
    }
    gzclose(gz);
    std::istringstream iss(std::string(buf.data(), buf.size()));
    read_mgh(mgh, &iss);
  }

  /// @brief Write MGH data to an MGZ format file (gzipped MGH).
  /// @details This function uses zlib to compress the data after writing it with the
  ///          stream-based write_mgh(). Requires linking with -lz.
  /// @param mgh An Mgh instance that should be written.
  /// @param filename Path to the output MGZ file.
  /// @throws std::runtime_error if the file cannot be opened or written.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Mgh mgh;
  /// fs::read_mgz(&mgh, "brain.mgz");
  /// // Do something with 'mgh' here, maybe?
  /// fs::write_mgz(mgh, "output.mgz");
  /// @endcode
  inline void write_mgz(const Mgh &mgh, const std::string &filename)
  {
    std::ostringstream oss;
    write_mgh(mgh, oss);
    std::string data = oss.str();

    gzFile gz = gzopen(filename.c_str(), "wb");
    if (!gz)
    {
      int errnum = 0;
      const char *errstr = gzerror(gz, &errnum);
      throw std::runtime_error("Could not open MGZ file '" + filename + "' for writing: " +
                               (errstr ? std::string(errstr) : "unknown error") + "\n");
    }
    z_size_t total_written = 0;
    while (total_written < data.size())
    {
      int written = gzwrite(gz, data.data() + total_written, static_cast<unsigned int>(data.size() - total_written));
      if (written <= 0)
      {
        int errnum = 0;
        const char *errstr = gzerror(gz, &errnum);
        gzclose(gz);
        throw std::runtime_error("Error writing MGZ file '" + filename + "': " +
                                 (errstr ? std::string(errstr) : "unknown error") + "\n");
      }
      total_written += static_cast<z_size_t>(written);
    }
    gzclose(gz);
  }

#endif // LIBFS_HAS_ZLIB

  // ========================================================================
  // NIfTI-1 Support
  // ========================================================================

  /// @name NIfTI-1 Data Type Constants
  /// @brief Pixel data type codes defined by the NIfTI-1 specification.
  /// @details These constants correspond to the `datatype` field of the
  ///          NIfTI-1 header.  They encode both the underlying C type and
  ///          the bit width.  The numeric values are taken directly from
  ///          the NIfTI-1 standard (see the `nifti1.h` reference
  ///          implementation).  When reading a file, the `datatype` field
  ///          is compared against these constants to determine how to
  ///          interpret the raw voxel bytes.
  /// @{

  /// No data / unknown type (value 0).
  const int16_t NIFTI_DT_NONE       = 0;

  /// Binary mask: each voxel is a single bit packed into a byte (value 1).
  /// Useful for segmentation labels that are strictly 0 or 1.
  const int16_t NIFTI_DT_BINARY     = 1;

  /// Unsigned 8-bit integer, range [0, 255] (value 2).
  /// Common for RGB component planes, label masks, and CT data.
  const int16_t NIFTI_DT_UINT8      = 2;

  /// Signed 16-bit integer, range [-32768, 32767] (value 4).
  /// Widely used for structural MRI after scaling with `scl_slope`/
  /// `scl_inter`.
  const int16_t NIFTI_DT_INT16      = 4;

  /// Signed 32-bit integer, range [-2^31, 2^31-1] (value 8).
  /// Used for high-dynamic-range label maps and processed statistical
  /// images.
  const int16_t NIFTI_DT_INT32      = 8;

  /// 32-bit IEEE-754 single-precision float (value 16).
  /// The most common floating-point format for processed neuroimaging
  /// data (e.g., z-statistics, correlation maps).
  const int16_t NIFTI_DT_FLOAT32    = 16;

  /// Complex number stored as two consecutive 32-bit floats
  /// (real, imaginary), 64 bits total (value 32).  Rarely used in
  /// practice; mainly for frequency-domain / phase data.
  const int16_t NIFTI_DT_COMPLEX64  = 32;

  /// 64-bit IEEE-754 double-precision float (value 64).
  /// Preferred when numerical precision is critical, e.g., for
  /// deformation fields or when accumulating statistics.
  const int16_t NIFTI_DT_FLOAT64    = 64;

  /// RGB triple: three consecutive `uint8` values per voxel (R, G, B),
  /// 24 bits total (value 128).  Used for 2-D colour images stored in
  /// NIfTI format (e.g., tissue-class overlays).
  const int16_t NIFTI_DT_RGB24      = 128;

  /// Signed 8-bit integer, range [-128, 127] (value 256).
  /// Provides a compact representation when the dynamic range fits in a
  /// single signed byte.
  const int16_t NIFTI_DT_INT8       = 256;

  /// Unsigned 16-bit integer, range [0, 65535] (value 512).
  /// Common in DICOM-derived data and some microscopy formats.
  const int16_t NIFTI_DT_UINT16     = 512;

  /// Unsigned 32-bit integer, range [0, 2^32-1] (value 768).
  /// Useful for large label sets and extended-count voxel data.
  const int16_t NIFTI_DT_UINT32     = 768;

  /// Signed 64-bit integer, range [-2^63, 2^63-1] (value 1024).
  /// Suitable for high-precision integer processing and very large
  /// label spaces.
  const int16_t NIFTI_DT_INT64      = 1024;

  /// Unsigned 64-bit integer, range [0, 2^64-1] (value 1280).
  /// The widest integral type available in the NIfTI-1 spec.
  const int16_t NIFTI_DT_UINT64     = 1280;

  /// 128-bit IEEE-754 quadruple-precision float (value 1536).
  /// Rarely supported in practice — most toolchains read it but fall
  /// back to double precision.
  const int16_t NIFTI_DT_FLOAT128   = 1536;

  /// Complex number stored as two consecutive 64-bit floats
  /// (real, imaginary), 128 bits total (value 1792).
  const int16_t NIFTI_DT_COMPLEX128 = 1792;

  /// Complex number stored as two consecutive 128-bit floats
  /// (real, imaginary), 256 bits total (value 2048).
  /// Highest precision complex storage; virtually never encountered in
  /// real-world datasets.
  const int16_t NIFTI_DT_COMPLEX256 = 2048;

  /// @}

  /// NIfTI-1 header structure (348 bytes, packed).
#pragma pack(push, 1)
  struct Nifti1Header
  {
    int32_t sizeof_hdr;       ///< must be 348
    char    data_type[10];    ///< unused
    char    db_name[18];      ///< unused
    int32_t extents;          ///< unused
    int16_t session_error;    ///< unused
    char    regular;          ///< unused
    char    dim_info;         ///< MRI slice ordering
    int16_t dim[8];           ///< dim[0]=ndim, dim[1..7]=dimensions
    float   intent_p1;        ///< intent parameter 1
    float   intent_p2;        ///< intent parameter 2
    float   intent_p3;        ///< intent parameter 3
    int16_t intent_code;      ///< NIfTI intent code
    int16_t datatype;         ///< NIfTI data type code
    int16_t bitpix;           ///< bits per voxel
    int16_t slice_start;      ///< first slice index
    float   pixdim[8];        ///< voxel dimensions (mm)
    float   vox_offset;       ///< byte offset to data from header start
    float   scl_slope;        ///< scaling slope
    float   scl_inter;        ///< scaling intercept
    int16_t slice_end;        ///< last slice index
    char    slice_code;       ///< slice timing code
    char    xyzt_units;       ///< units for pixdim[] dimensions
    float   cal_max;          ///< calibrated max
    float   cal_min;          ///< calibrated min
    float   slice_duration;   ///< slice timing duration
    float   toffset;          ///< time offset
    int32_t glmax;            ///< global max (unused)
    int32_t glmin;            ///< global min (unused)
    char    descrip[80];      ///< description
    char    aux_file[24];     ///< auxiliary filename
    int16_t qform_code;       ///< quaternion transform code (>0 = valid)
    int16_t sform_code;       ///< affine transform code (>0 = valid)
    float   quatern_b;        ///< quaternion b param
    float   quatern_c;        ///< quaternion c param
    float   quatern_d;        ///< quaternion d param
    float   qoffset_x;        ///< quaternion x shift
    float   qoffset_y;        ///< quaternion y shift
    float   qoffset_z;        ///< quaternion z shift
    float   srow_x[4];        ///< affine transform row x
    float   srow_y[4];        ///< affine transform row y
    float   srow_z[4];        ///< affine transform row z
    char    intent_name[16];  ///< intent name
    char    magic[4];         ///< "n+1\0" (single file) or "ni1\0" (header/img pair)
  };
#pragma pack(pop)

  // --- Internal NIfTI helpers ---

  /// @brief Map a NIfTI-1 data type code to an MGH MRI_* constant.
  /// @throws std::runtime_error if the NIfTI type is unsupported.
  /// @private
  inline int _nifti_dtype_to_mri(int16_t nifti_dtype)
  {
    switch (nifti_dtype)
    {
    case NIFTI_DT_UINT8:   return MRI_UCHAR;
    case NIFTI_DT_INT16:   return MRI_SHORT;
    case NIFTI_DT_INT32:   return MRI_INT;
    case NIFTI_DT_FLOAT32: return MRI_FLOAT;
    default:
      throw std::runtime_error("Unsupported NIfTI data type " + std::to_string(nifti_dtype) +
                               ".  Supported types: UINT8 (2), INT16 (4), INT32 (8), FLOAT32 (16).\n");
    }
  }

  /// @brief Map an MGH MRI_* data type to a NIfTI-1 data type code.
  /// @throws std::runtime_error if the MGH type is unsupported for NIfTI output.
  /// @private
  inline int16_t _mri_dtype_to_nifti(int32_t mri_dtype)
  {
    switch (mri_dtype)
    {
    case MRI_UCHAR: return NIFTI_DT_UINT8;
    case MRI_SHORT: return NIFTI_DT_INT16;
    case MRI_INT:   return NIFTI_DT_INT32;
    case MRI_FLOAT: return NIFTI_DT_FLOAT32;
    default:
      throw std::runtime_error("Unsupported MGH data type " + std::to_string(mri_dtype) +
                               " for NIfTI output.\n");
    }
  }

  /// @brief Read and validate a NIfTI-1 header from a stream, detecting endianness.
  /// @param is  Input stream positioned at byte 0 of the NIfTI file.
  /// @param file_is_bigendian  [out] set to true if the file is big-endian.
  /// @return The parsed and byte-order-corrected header.
  /// @throws std::runtime_error on short read or invalid sizeof_hdr.
  /// @private
  inline Nifti1Header _read_nifti1_header(std::istream &is, bool &file_is_bigendian)
  {
    Nifti1Header hdr;
    is.read(reinterpret_cast<char *>(&hdr), sizeof(Nifti1Header));
    if (static_cast<size_t>(is.gcount()) != sizeof(Nifti1Header))
    {
      throw std::runtime_error("NIfTI file too small for header: expected " +
                               std::to_string(sizeof(Nifti1Header)) + " bytes.\n");
    }

    // Detect endianness: sizeof_hdr must be 348.
    if (hdr.sizeof_hdr != 348)
    {
      int32_t swapped = _swap_endian(hdr.sizeof_hdr);
      if (swapped == 348)
      {
        file_is_bigendian = true;
      }
      else
      {
        throw std::runtime_error("Invalid NIfTI file: sizeof_hdr = " +
                                 std::to_string(hdr.sizeof_hdr) + " (expected 348).\n");
      }
    }
    else
    {
      file_is_bigendian = false;
    }

    // If file endianness differs from host, byte-swap the numeric fields.
    bool need_swap = (file_is_bigendian != _is_bigendian());
    if (need_swap)
    {
      hdr.sizeof_hdr    = 348; // already correct, keep it
      hdr.extents       = _swap_endian(hdr.extents);
      hdr.session_error = _swap_endian(hdr.session_error);
      // dim_info, regular are char — no swap
      for (int i = 0; i < 8; i++) hdr.dim[i]        = _swap_endian(hdr.dim[i]);
      hdr.intent_p1     = _swap_endian(hdr.intent_p1);
      hdr.intent_p2     = _swap_endian(hdr.intent_p2);
      hdr.intent_p3     = _swap_endian(hdr.intent_p3);
      hdr.intent_code   = _swap_endian(hdr.intent_code);
      hdr.datatype      = _swap_endian(hdr.datatype);
      hdr.bitpix        = _swap_endian(hdr.bitpix);
      hdr.slice_start   = _swap_endian(hdr.slice_start);
      for (int i = 0; i < 8; i++) hdr.pixdim[i]     = _swap_endian(hdr.pixdim[i]);
      hdr.vox_offset    = _swap_endian(hdr.vox_offset);
      hdr.scl_slope     = _swap_endian(hdr.scl_slope);
      hdr.scl_inter     = _swap_endian(hdr.scl_inter);
      hdr.slice_end     = _swap_endian(hdr.slice_end);
      // slice_code, xyzt_units are char — no swap
      hdr.cal_max       = _swap_endian(hdr.cal_max);
      hdr.cal_min       = _swap_endian(hdr.cal_min);
      hdr.slice_duration = _swap_endian(hdr.slice_duration);
      hdr.toffset       = _swap_endian(hdr.toffset);
      hdr.glmax         = _swap_endian(hdr.glmax);
      hdr.glmin         = _swap_endian(hdr.glmin);
      hdr.qform_code    = _swap_endian(hdr.qform_code);
      hdr.sform_code    = _swap_endian(hdr.sform_code);
      hdr.quatern_b     = _swap_endian(hdr.quatern_b);
      hdr.quatern_c     = _swap_endian(hdr.quatern_c);
      hdr.quatern_d     = _swap_endian(hdr.quatern_d);
      hdr.qoffset_x     = _swap_endian(hdr.qoffset_x);
      hdr.qoffset_y     = _swap_endian(hdr.qoffset_y);
      hdr.qoffset_z     = _swap_endian(hdr.qoffset_z);
      for (int i = 0; i < 4; i++) hdr.srow_x[i]   = _swap_endian(hdr.srow_x[i]);
      for (int i = 0; i < 4; i++) hdr.srow_y[i]   = _swap_endian(hdr.srow_y[i]);
      for (int i = 0; i < 4; i++) hdr.srow_z[i]   = _swap_endian(hdr.srow_z[i]);
    }

    // Validate magic.
    if (std::memcmp(hdr.magic, "n+1\0", 4) != 0 &&
        std::memcmp(hdr.magic, "ni1\0", 4) != 0)
    {
      // The magic may also need swapping.
      throw std::runtime_error("NIfTI file has invalid magic string.  "
                               "Only single-file .nii (n+1) is supported.\n");
    }

    return hdr;
  }

  /// @brief Read a single value from a NIfTI data stream with known file endianness.
  /// @private
  template <typename T>
  inline T _nifti_read_data_element(std::istream &is, bool file_is_bigendian)
  {
    T val;
    is.read(reinterpret_cast<char *>(&val), sizeof(T));
    if (static_cast<size_t>(is.gcount()) != sizeof(T))
    {
      throw std::runtime_error("Unexpected end of NIfTI data stream.\n");
    }
    if (file_is_bigendian != _is_bigendian())
    {
      val = _swap_endian(val);
    }
    return val;
  }

  /// @brief Write a single value to a NIfTI data stream in the file's endianness.
  /// @private
  template <typename T>
  inline void _nifti_write_data_element(std::ostream &os, T val, bool file_is_bigendian)
  {
    if (file_is_bigendian != _is_bigendian())
    {
      val = _swap_endian(val);
    }
    os.write(reinterpret_cast<const char *>(&val), sizeof(T));
  }

  /// @brief Extract RAS spatial metadata from a NIfTI-1 header into an MghHeader.
  /// @details Prefers sform over qform.  Sets ras_good_flag = 1 on success.
  /// @private
  inline void _nifti_extract_ras(const Nifti1Header &hdr, MghHeader *mgh_header)
  {
    if (hdr.sform_code > 0)
    {
      // Use affine (sform) transform.
      mgh_header->ras_good_flag = 1;
      mgh_header->xsize = hdr.pixdim[1];
      mgh_header->ysize = hdr.pixdim[2];
      mgh_header->zsize = hdr.pixdim[3];
      mgh_header->Mdc.clear();
      mgh_header->Pxyz_c.clear();
      // Mdc: 3×3 rotation/scale part of srow (column-major to row-major, but
      // MGH stores 9 floats in row-major order: [r11,r12,r13, r21,r22,r23, r31,r32,r33]).
      // srow_x = [r11, r12, r13, tx], srow_y = [r21, r22, r23, ty], srow_z = [r31, r32, r33, tz].
      mgh_header->Mdc.push_back(hdr.srow_x[0]); mgh_header->Mdc.push_back(hdr.srow_x[1]); mgh_header->Mdc.push_back(hdr.srow_x[2]);
      mgh_header->Mdc.push_back(hdr.srow_y[0]); mgh_header->Mdc.push_back(hdr.srow_y[1]); mgh_header->Mdc.push_back(hdr.srow_y[2]);
      mgh_header->Mdc.push_back(hdr.srow_z[0]); mgh_header->Mdc.push_back(hdr.srow_z[1]); mgh_header->Mdc.push_back(hdr.srow_z[2]);
      mgh_header->Pxyz_c.push_back(hdr.srow_x[3]);
      mgh_header->Pxyz_c.push_back(hdr.srow_y[3]);
      mgh_header->Pxyz_c.push_back(hdr.srow_z[3]);
    }
    else if (hdr.qform_code > 0)
    {
      // Compute rotation from quaternion and store as affine.
      float b = hdr.quatern_b;
      float c = hdr.quatern_c;
      float d = hdr.quatern_d;
      float a = std::sqrt(std::max(0.0f, 1.0f - (b * b + c * c + d * d)));
      float qfac = (hdr.pixdim[0] < 0.0f) ? -1.0f : 1.0f;

      mgh_header->ras_good_flag = 1;
      mgh_header->xsize = hdr.pixdim[1];
      mgh_header->ysize = hdr.pixdim[2];
      mgh_header->zsize = hdr.pixdim[3];
      mgh_header->Mdc.clear();
      mgh_header->Pxyz_c.clear();

      // Rotation matrix from unit quaternion.
      float R11 = a * a + b * b - c * c - d * d;
      float R12 = 2.0f * (b * c - a * d);
      float R13 = 2.0f * (b * d + a * c);
      float R21 = 2.0f * (b * c + a * d);
      float R22 = a * a + c * c - b * b - d * d;
      float R23 = 2.0f * (c * d - a * b);
      float R31 = 2.0f * (b * d - a * c);
      float R32 = 2.0f * (c * d + a * b);
      float R33 = a * a + d * d - b * b - c * c;

      // Apply pixdim scaling and qfac.
      float sx = hdr.pixdim[1];
      float sy = hdr.pixdim[2];
      float sz = hdr.pixdim[3] * qfac;

      mgh_header->Mdc.push_back(R11 * sx); mgh_header->Mdc.push_back(R12 * sy); mgh_header->Mdc.push_back(R13 * sz);
      mgh_header->Mdc.push_back(R21 * sx); mgh_header->Mdc.push_back(R22 * sy); mgh_header->Mdc.push_back(R23 * sz);
      mgh_header->Mdc.push_back(R31 * sx); mgh_header->Mdc.push_back(R32 * sy); mgh_header->Mdc.push_back(R33 * sz);

      mgh_header->Pxyz_c.push_back(hdr.qoffset_x);
      mgh_header->Pxyz_c.push_back(hdr.qoffset_y);
      mgh_header->Pxyz_c.push_back(hdr.qoffset_z);
    }
    else
    {
      // No valid spatial transform — just store voxel sizes.
      mgh_header->ras_good_flag = 0;
      mgh_header->xsize = hdr.pixdim[1];
      mgh_header->ysize = hdr.pixdim[2];
      mgh_header->zsize = hdr.pixdim[3];
    }
  }

  // --- Public NIfTI read API ---

  /// @brief Read a NIfTI-1 file into an Mgh struct (stream overload).
  /// @param mgh   The Mgh instance to fill with the NIfTI data.
  /// @param is    An open input stream positioned at the start of the NIfTI file.
  /// @param force_standard  If true, reject non-conformant headers including the FreeSurfer hack.
  /// @throws std::runtime_error on unsupported data types, I/O errors, or dimension overflows.
  inline void read_nifti(Mgh *mgh, std::istream *is, bool force_standard)
  {
    // 1. Determine stream size (for FS hack recovery and validation).
    std::streampos start_pos = is->tellg();
    is->seekg(0, std::ios::end);
    std::streamsize total_file_size = is->tellg();
    is->seekg(start_pos, std::ios::beg);

    // 2. Read and validate header.
    bool file_is_bigendian = false;
    Nifti1Header hdr = _read_nifti1_header(*is, file_is_bigendian);

    // 3. Detect FreeSurfer hack.
    int64_t true_dim1 = hdr.dim[1];
    bool hack_detected = false;

    // dim[1] is int16_t; values > 32767 wrap to negative via signed overflow.
    if (hdr.dim[1] < 0 && hdr.dim[2] == 1 && hdr.dim[3] == 1)
    {
      int bytes_per_element = hdr.bitpix / 8;
      // dim[4]: NIfTI convention says dim[i] for i>dim[0] should be 1,
      // but FreeSurfer files may set it to 0.  Treat 0 and 1 both as 1 frame.
      int64_t frames = (hdr.dim[4] > 1) ? static_cast<int64_t>(hdr.dim[4]) : 1;

      int64_t payload_bytes = total_file_size - static_cast<int64_t>(hdr.vox_offset);
      int64_t computed_x = payload_bytes / (bytes_per_element * frames);

      // False-positive mitigation: only accept if the recovered vertex count
      // is in a plausible range for a FreeSurfer surface mesh (1K – 5M vertices).
      if (computed_x >= 1000 && computed_x <= 5000000)
      {
        hack_detected = true;
        true_dim1 = computed_x;
      }
    }

    // If the caller requested strict conformance, reject the hack.
    if (force_standard && hack_detected)
    {
      throw std::runtime_error(
          "NIfTI file does not conform to the NIfTI-1 standard: "
          "dim[1] overflow detected (likely FreeSurfer hack).  "
          "Re-run with force_standard=false to recover surface data.\n");
    }

    // 4. Map dimensions (treat dim[i] <= 0 as 1).
    int32_t dim2 = (hdr.dim[2] > 0) ? hdr.dim[2] : 1;
    int32_t dim3 = (hdr.dim[3] > 0) ? hdr.dim[3] : 1;
    int32_t dim4 = (hdr.dim[4] > 0) ? hdr.dim[4] : 1;
    int     bytes_per_element = hdr.bitpix / 8;

    // 5. Overflow-safe size validation.
    uint64_t total_elements = static_cast<uint64_t>(true_dim1) *
                              static_cast<uint64_t>(dim2) *
                              static_cast<uint64_t>(dim3) *
                              static_cast<uint64_t>(dim4);
    uint64_t expected_payload = total_elements * static_cast<uint64_t>(bytes_per_element);

    if (!fs::util::check_alloc(static_cast<size_t>(total_elements), static_cast<size_t>(bytes_per_element)))
    {
      throw std::runtime_error("NIfTI dimensions exceed maximum allowed allocation (" +
                               std::to_string(LIBFS_MAX_ALLOC_BYTES) + " bytes).\n");
    }

    uint64_t available_bytes = static_cast<uint64_t>(total_file_size) - static_cast<uint64_t>(hdr.vox_offset);
    if (expected_payload > available_bytes)
    {
      throw std::runtime_error("Corrupted NIfTI file: dimensions require " +
                               std::to_string(expected_payload) + " bytes but only " +
                               std::to_string(available_bytes) + " available.\n");
    }

    if (hdr.vox_offset < 348 || static_cast<uint64_t>(hdr.vox_offset) >= static_cast<uint64_t>(total_file_size))
    {
      throw std::runtime_error("Corrupted NIfTI file: invalid vox_offset " +
                               std::to_string(hdr.vox_offset) + ".\n");
    }

    // 6. Map data type and prepare MGH header.
    int mri_dtype = _nifti_dtype_to_mri(hdr.datatype);
    mgh->header.dim1length = static_cast<int32_t>(true_dim1);
    mgh->header.dim2length = dim2;
    mgh->header.dim3length = dim3;
    mgh->header.dim4length = dim4;
    mgh->header.dtype = mri_dtype;
    mgh->header.dof = 0;

    // Extract spatial metadata.
    _nifti_extract_ras(hdr, &mgh->header);

    // 7. Skip any extensions and seek to voxel data.
    is->seekg(start_pos + std::streamoff(static_cast<int64_t>(hdr.vox_offset)), std::ios::beg);

    // 8. Read data and apply scaling.
    float slope = (hdr.scl_slope != 0.0f) ? hdr.scl_slope : 1.0f;
    float inter = hdr.scl_inter;
    size_t num_voxels = static_cast<size_t>(total_elements);
    // Suppress unused variable warning in builds without LIBFS_DBG_INFO
    (void)num_voxels;

#ifdef LIBFS_DBG_INFO
    std::cout << LIBFS_APPTAG << "Reading NIfTI file: " << true_dim1 << "x" << dim2
              << "x" << dim3 << "x" << dim4 << " (" << num_voxels << " voxels), dtype="
              << hdr.datatype << (hack_detected ? " [FS hack]" : "") << "\n";
#endif

    if (mri_dtype == MRI_INT)
    {
      mgh->data.data_mri_int.reserve(num_voxels);
      for (size_t i = 0; i < num_voxels; i++)
      {
        int32_t raw = _nifti_read_data_element<int32_t>(*is, file_is_bigendian);
        mgh->data.data_mri_int.push_back(static_cast<int32_t>(std::round(raw * slope + inter)));
      }
    }
    else if (mri_dtype == MRI_FLOAT)
    {
      mgh->data.data_mri_float.reserve(num_voxels);
      for (size_t i = 0; i < num_voxels; i++)
      {
        float raw = _nifti_read_data_element<float>(*is, file_is_bigendian);
        mgh->data.data_mri_float.push_back(raw * slope + inter);
      }
    }
    else if (mri_dtype == MRI_UCHAR)
    {
      mgh->data.data_mri_uchar.reserve(num_voxels);
      for (size_t i = 0; i < num_voxels; i++)
      {
        uint8_t raw = _nifti_read_data_element<uint8_t>(*is, file_is_bigendian);
        mgh->data.data_mri_uchar.push_back(static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, std::round(raw * slope + inter)))));
      }
    }
    else if (mri_dtype == MRI_SHORT)
    {
      mgh->data.data_mri_short.reserve(num_voxels);
      for (size_t i = 0; i < num_voxels; i++)
      {
        int16_t raw = _nifti_read_data_element<int16_t>(*is, file_is_bigendian);
        mgh->data.data_mri_short.push_back(static_cast<short>(std::round(raw * slope + inter)));
      }
    }
  }

  /// @brief Read a NIfTI-1 file into an Mgh struct (filename overload).
  /// @details Auto-detects .nii and .nii.gz by file extension.
  /// @param mgh   The Mgh instance to fill with the NIfTI data.
  /// @param filename  Path to the input file (.nii or .nii.gz).
  /// @param force_standard  If true, reject non-conformant headers including the FreeSurfer hack.
  /// @throws std::runtime_error on unsupported data types, I/O errors, or dimension overflows.
  inline void read_nifti(Mgh *mgh, const std::string &filename, bool force_standard)
  {
    if (fs::util::ends_with(filename, ".nii.gz") || fs::util::ends_with(filename, ".NII.GZ"))
    {
#ifdef LIBFS_HAS_ZLIB
      read_nifti_gz(mgh, filename, force_standard);
      return;
#else
      throw std::runtime_error("Cannot read .nii.gz file '" + filename +
                               "': zlib support not enabled.  "
                               "Link with -lz or decompress the file first.\n");
#endif
    }

    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open())
    {
      throw std::runtime_error("Could not open NIfTI file '" + filename + "' for reading.\n");
    }
    read_nifti(mgh, &ifs, force_standard);
    ifs.close();
  }

  // --- NIfTI write API ---

  /// @brief Write MGH data to a NIfTI-1 file (stream overload).
  /// @details Writes a standard volumetric NIfTI-1 file in big-endian byte order
  ///          (NIfTI-1 standard).  The FreeSurfer hack is never produced.
  /// @param mgh  The Mgh data to write.
  /// @param os   An open output stream.
  /// @throws std::runtime_error if any dimension exceeds 32767 or the data type is unsupported.
  inline void write_nifti(const Mgh &mgh, std::ostream &os)
  {
    // Validate dimensions: NIfTI-1 uses int16_t for dim[].
    if (mgh.header.dim1length > 32767 || mgh.header.dim2length > 32767 ||
        mgh.header.dim3length > 32767 || mgh.header.dim4length > 32767)
    {
      throw std::runtime_error("MGH dimensions exceed NIfTI-1 int16 limit (32767).  "
                               "Cannot write as NIfTI.\n");
    }

    bool file_is_bigendian = true; // NIfTI standard is big-endian on disk.

    // Build header (all zeroed first).
    Nifti1Header hdr;
    std::memset(&hdr, 0, sizeof(hdr));
    hdr.sizeof_hdr = 348;
    hdr.dim[0] = 4; // always 4D for our purposes
    hdr.dim[1] = static_cast<int16_t>(mgh.header.dim1length);
    hdr.dim[2] = static_cast<int16_t>(mgh.header.dim2length);
    hdr.dim[3] = static_cast<int16_t>(mgh.header.dim3length);
    hdr.dim[4] = static_cast<int16_t>(mgh.header.dim4length);
    hdr.dim[5] = 1;
    hdr.dim[6] = 1;
    hdr.dim[7] = 1;

    hdr.datatype = _mri_dtype_to_nifti(mgh.header.dtype);
    hdr.bitpix = 0;
    switch (mgh.header.dtype)
    {
    case MRI_UCHAR: hdr.bitpix = 8;  break;
    case MRI_SHORT: hdr.bitpix = 16; break;
    case MRI_INT:   hdr.bitpix = 32; break;
    case MRI_FLOAT: hdr.bitpix = 32; break;
    }

    // Voxel sizes and spatial transform.
    hdr.pixdim[0] = 1.0f;
    hdr.pixdim[1] = mgh.header.xsize > 0.0f ? mgh.header.xsize : 1.0f;
    hdr.pixdim[2] = mgh.header.ysize > 0.0f ? mgh.header.ysize : 1.0f;
    hdr.pixdim[3] = mgh.header.zsize > 0.0f ? mgh.header.zsize : 1.0f;
    hdr.pixdim[4] = 1.0f;
    hdr.pixdim[5] = 1.0f;
    hdr.pixdim[6] = 1.0f;
    hdr.pixdim[7] = 1.0f;

    hdr.vox_offset = 352.0f; // 348-byte header + 4-byte extension indicator
    hdr.scl_slope  = 1.0f;
    hdr.scl_inter  = 0.0f;

    if (mgh.header.ras_good_flag == 1 && mgh.header.Mdc.size() >= 9 && mgh.header.Pxyz_c.size() >= 3)
    {
      hdr.sform_code = 1; // Scanner Anatomical
      hdr.qform_code = 1;
      hdr.srow_x[0] = mgh.header.Mdc[0]; hdr.srow_x[1] = mgh.header.Mdc[1]; hdr.srow_x[2] = mgh.header.Mdc[2]; hdr.srow_x[3] = mgh.header.Pxyz_c[0];
      hdr.srow_y[0] = mgh.header.Mdc[3]; hdr.srow_y[1] = mgh.header.Mdc[4]; hdr.srow_y[2] = mgh.header.Mdc[5]; hdr.srow_y[3] = mgh.header.Pxyz_c[1];
      hdr.srow_z[0] = mgh.header.Mdc[6]; hdr.srow_z[1] = mgh.header.Mdc[7]; hdr.srow_z[2] = mgh.header.Mdc[8]; hdr.srow_z[3] = mgh.header.Pxyz_c[2];

      // Set quaternion fields from sform for consistency (optional, but good practice).
      hdr.quatern_b = 0.0f;
      hdr.quatern_c = 0.0f;
      hdr.quatern_d = 0.0f;
      hdr.qoffset_x = hdr.srow_x[3];
      hdr.qoffset_y = hdr.srow_y[3];
      hdr.qoffset_z = hdr.srow_z[3];
    }
    else
    {
      hdr.sform_code = 0;
      hdr.qform_code = 0;
    }

    // Set magic for single-file NIfTI.
    std::memcpy(hdr.magic, "n+1\0", 4);

    // Write header.
    bool need_swap = (file_is_bigendian != _is_bigendian());
    if (need_swap)
    {
      Nifti1Header hdr_swapped = hdr;
      hdr_swapped.sizeof_hdr    = _swap_endian(hdr.sizeof_hdr);
      hdr_swapped.extents       = _swap_endian(hdr.extents);
      hdr_swapped.session_error = _swap_endian(hdr.session_error);
      for (int i = 0; i < 8; i++) hdr_swapped.dim[i]        = _swap_endian(hdr.dim[i]);
      hdr_swapped.intent_p1     = _swap_endian(hdr.intent_p1);
      hdr_swapped.intent_p2     = _swap_endian(hdr.intent_p2);
      hdr_swapped.intent_p3     = _swap_endian(hdr.intent_p3);
      hdr_swapped.intent_code   = _swap_endian(hdr.intent_code);
      hdr_swapped.datatype      = _swap_endian(hdr.datatype);
      hdr_swapped.bitpix        = _swap_endian(hdr.bitpix);
      hdr_swapped.slice_start   = _swap_endian(hdr.slice_start);
      for (int i = 0; i < 8; i++) hdr_swapped.pixdim[i]     = _swap_endian(hdr.pixdim[i]);
      hdr_swapped.vox_offset    = _swap_endian(hdr.vox_offset);
      hdr_swapped.scl_slope     = _swap_endian(hdr.scl_slope);
      hdr_swapped.scl_inter     = _swap_endian(hdr.scl_inter);
      hdr_swapped.slice_end     = _swap_endian(hdr.slice_end);
      hdr_swapped.cal_max       = _swap_endian(hdr.cal_max);
      hdr_swapped.cal_min       = _swap_endian(hdr.cal_min);
      hdr_swapped.slice_duration = _swap_endian(hdr.slice_duration);
      hdr_swapped.toffset       = _swap_endian(hdr.toffset);
      hdr_swapped.glmax         = _swap_endian(hdr.glmax);
      hdr_swapped.glmin         = _swap_endian(hdr.glmin);
      hdr_swapped.qform_code    = _swap_endian(hdr.qform_code);
      hdr_swapped.sform_code    = _swap_endian(hdr.sform_code);
      hdr_swapped.quatern_b     = _swap_endian(hdr.quatern_b);
      hdr_swapped.quatern_c     = _swap_endian(hdr.quatern_c);
      hdr_swapped.quatern_d     = _swap_endian(hdr.quatern_d);
      hdr_swapped.qoffset_x     = _swap_endian(hdr.qoffset_x);
      hdr_swapped.qoffset_y     = _swap_endian(hdr.qoffset_y);
      hdr_swapped.qoffset_z     = _swap_endian(hdr.qoffset_z);
      for (int i = 0; i < 4; i++) hdr_swapped.srow_x[i]   = _swap_endian(hdr.srow_x[i]);
      for (int i = 0; i < 4; i++) hdr_swapped.srow_y[i]   = _swap_endian(hdr.srow_y[i]);
      for (int i = 0; i < 4; i++) hdr_swapped.srow_z[i]   = _swap_endian(hdr.srow_z[i]);
      os.write(reinterpret_cast<const char *>(&hdr_swapped), sizeof(Nifti1Header));
    }
    else
    {
      os.write(reinterpret_cast<const char *>(&hdr), sizeof(Nifti1Header));
    }

    // Write 4-byte extension indicator (0 = no extensions).
    int32_t ext_indicator = 0;
    if (file_is_bigendian != _is_bigendian())
    {
      ext_indicator = _swap_endian(ext_indicator);
    }
    os.write(reinterpret_cast<const char *>(&ext_indicator), 4);

    // Write voxel data.
    size_t num_values = mgh.header.num_values();
    if (mgh.header.dtype == MRI_INT)
    {
      for (size_t i = 0; i < num_values; i++)
      {
        _nifti_write_data_element<int32_t>(os, mgh.data.data_mri_int[i], file_is_bigendian);
      }
    }
    else if (mgh.header.dtype == MRI_FLOAT)
    {
      for (size_t i = 0; i < num_values; i++)
      {
        _nifti_write_data_element<float>(os, mgh.data.data_mri_float[i], file_is_bigendian);
      }
    }
    else if (mgh.header.dtype == MRI_UCHAR)
    {
      for (size_t i = 0; i < num_values; i++)
      {
        _nifti_write_data_element<uint8_t>(os, mgh.data.data_mri_uchar[i], file_is_bigendian);
      }
    }
    else if (mgh.header.dtype == MRI_SHORT)
    {
      for (size_t i = 0; i < num_values; i++)
      {
        _nifti_write_data_element<short>(os, mgh.data.data_mri_short[i], file_is_bigendian);
      }
    }
    else
    {
      throw std::domain_error("Unsupported MRI data type " + std::to_string(mgh.header.dtype) +
                              " for NIfTI output.\n");
    }
  }

  /// @brief Write MGH data to a NIfTI-1 file (filename overload).
  /// @param mgh  The Mgh data to write.
  /// @param filename  Path to the output file (.nii or .nii.gz).
  /// @throws std::runtime_error if the file cannot be opened.
  inline void write_nifti(const Mgh &mgh, const std::string &filename)
  {
    if (fs::util::ends_with(filename, ".nii.gz") || fs::util::ends_with(filename, ".NII.GZ"))
    {
#ifdef LIBFS_HAS_ZLIB
      write_nifti_gz(mgh, filename);
      return;
#else
      throw std::runtime_error("Cannot write .nii.gz file '" + filename +
                               "': zlib support not enabled.  Link with -lz.\n");
#endif
    }

    std::ofstream ofs(filename, std::ofstream::out | std::ofstream::binary);
    if (!ofs.is_open())
    {
      throw std::runtime_error("Unable to open NIfTI file '" + filename + "' for writing.\n");
    }
    write_nifti(mgh, ofs);
    ofs.close();
  }

  // --- Gzip-compressed NIfTI (.nii.gz) ---

#ifdef LIBFS_HAS_ZLIB

  /// @brief Read a gzip-compressed NIfTI-1 file (.nii.gz) into an Mgh struct.
  /// @details Decompresses with zlib, then delegates to stream-based read_nifti().
  /// @param mgh   The Mgh instance to fill.
  /// @param filename  Path to the .nii.gz file.
  /// @param force_standard  If true, reject non-conformant headers.
  inline void read_nifti_gz(Mgh *mgh, const std::string &filename, bool force_standard)
  {
    gzFile gz = gzopen(filename.c_str(), "rb");
    if (!gz)
    {
      int errnum = 0;
      const char *errstr = gzerror(gz, &errnum);
      throw std::runtime_error("Could not open NIfTI.GZ file '" + filename + "' for reading: " +
                               (errstr ? std::string(errstr) : "unknown error") + "\n");
    }
    std::vector<char> buf;
    char chunk[131072];
    int n;
    while ((n = gzread(gz, chunk, sizeof(chunk))) > 0)
    {
      buf.insert(buf.end(), chunk, chunk + n);
    }
    if (n < 0)
    {
      int errnum = 0;
      const char *errstr = gzerror(gz, &errnum);
      gzclose(gz);
      throw std::runtime_error("Error decompressing NIfTI.GZ file '" + filename + "': " +
                               (errstr ? std::string(errstr) : "unknown error") + "\n");
    }
    gzclose(gz);
    std::istringstream iss(std::string(buf.data(), buf.size()));
    read_nifti(mgh, &iss, force_standard);
  }

  /// @brief Write MGH data to a gzip-compressed NIfTI-1 file (.nii.gz).
  /// @details Compresses with zlib after writing NIfTI via stream-based write_nifti().
  /// @param mgh   The Mgh data to write.
  /// @param filename  Path to the .nii.gz output file.
  inline void write_nifti_gz(const Mgh &mgh, const std::string &filename)
  {
    std::ostringstream oss;
    write_nifti(mgh, oss);
    std::string data = oss.str();

    gzFile gz = gzopen(filename.c_str(), "wb");
    if (!gz)
    {
      int errnum = 0;
      const char *errstr = gzerror(gz, &errnum);
      throw std::runtime_error("Could not open NIfTI.GZ file '" + filename + "' for writing: " +
                               (errstr ? std::string(errstr) : "unknown error") + "\n");
    }
    z_size_t total_written = 0;
    while (total_written < data.size())
    {
      z_size_t remaining = data.size() - total_written;
      z_size_t chunk = (remaining > 131072) ? 131072 : remaining;
      int written = gzwrite(gz, data.data() + total_written, static_cast<unsigned int>(chunk));
      if (written <= 0)
      {
        int errnum = 0;
        const char *errstr = gzerror(gz, &errnum);
        gzclose(gz);
        throw std::runtime_error("Error writing NIfTI.GZ file '" + filename + "': " +
                                 (errstr ? std::string(errstr) : "unknown error") + "\n");
      }
      total_written += static_cast<z_size_t>(written);
    }
    gzclose(gz);
  }

#endif // LIBFS_HAS_ZLIB  (NIfTI GZ support)

  // --- NIfTI ↔ MGH conversion helpers ---

  /// @brief Convert a NIfTI-1 file directly to MGH by reading it.
  /// @details This is just a convenience wrapper: read_nifti already populates
  ///          an Mgh struct, so this provides a more explicit name for the
  ///          "convert" use case.
  /// @param filename  Path to the input .nii or .nii.gz file.
  /// @return An Mgh instance containing the NIfTI data.
  /// @throws std::runtime_error on read errors.
  inline Mgh nifti_to_mgh(const std::string &filename)
  {
    Mgh mgh;
    read_nifti(&mgh, filename);
    return mgh;
  }

  // ========================================================================
  // End NIfTI-1 Support
  // ========================================================================

  /// Models a FreeSurfer label.
  /// Can be a surface or volume label.
  /// A label contains entries for a subset of the vertices of a mesh (or the voxels of a volume).
  /// For a surface label, the 'vertex' field contains a vertex index, the coord_* fields may contain the vertex coordinates, and the value field may contain some per-vertex descriptor or analysis result, like a p-value.
  /// For a volume label, the 'vertex' field contains a running number, the coord_* fields contain (as floats) the voxel R,A,S indices, and the value field may contain some per-voxel descriptor or analysis result, like a p-value.
  /// Note: The variable names herre assume this is a surface label, do not let that confuse you when working with volume labels.
  struct Label
  {

    /// @brief Default constructor for a label.
    Label() {}

    /// Construct a Label from the given vertices / voxel numbers and values.
    Label(std::vector<int> vertices, std::vector<float> values)
    {
      assert(vertices.size() == values.size());
      vertex = vertices;
      value = values;
      coord_x = std::vector<float>(vertices.size(), 0.0f);
      coord_y = std::vector<float>(vertices.size(), 0.0f);
      coord_z = std::vector<float>(vertices.size(), 0.0f);
    }

    /// Construct a Label from the given vertices / voxel numbers.
    Label(std::vector<int> vertices)
    {
      vertex = vertices;
      value = std::vector<float>(vertices.size(), 0.0f);
      coord_x = std::vector<float>(vertices.size(), 0.0f);
      coord_y = std::vector<float>(vertices.size(), 0.0f);
      coord_z = std::vector<float>(vertices.size(), 0.0f);
    }

    std::vector<int> vertex;    ///< vertex indices for the data in this label if it is a surface label. These are indices into the vertices of a surface mesh to which this label belongs.
    std::vector<float> coord_x; ///< x coordinates of the vertices in case of a surface label, or voxels coordinates for a volume label.
    std::vector<float> coord_y; ///< y coordinates of the vertices in case of a surface label, or voxels coordinates for a volume label.
    std::vector<float> coord_z; ///< z coordinates of the vertices in case of a surface label, or voxels coordinates for a volume label.
    std::vector<float> value;   ///< the value of the label, can represent continuous data like a p-value, or sometimes simply 1.0 or 0.0 (interpreted as int/bool) to mark vertices inside/outside of a certain area.

    /// Compute for each vertex of the surface whether it is inside the label.
    std::vector<bool> vert_in_label(size_t surface_num_verts) const
    {
      if (surface_num_verts < this->vertex.size())
      { // nonsense, so we warn (but don't throw, maybe the user really wants this).
#ifdef LIBFS_DBG_ERROR
        std::cerr << "Invalid number of vertices for surface, must be at least " << this->vertex.size() << "\n";
#endif
      }
      std::vector<bool> is_in = std::vector<bool>(surface_num_verts, false);

      for (size_t i = 0; i < this->vertex.size(); i++)
      {
        is_in[this->vertex[i]] = true;
      }
      return (is_in);
    }

    /// Return the number of entries (vertices/voxels) in this label.
    size_t num_entries() const
    {
      size_t num_ent = this->vertex.size();
      if (this->coord_x.size() != num_ent || this->coord_y.size() != num_ent || this->coord_z.size() != num_ent || this->value.size() != num_ent)
      {
#ifdef LIBFS_DBG_ERROR
        std::cerr << "Inconsistent label: sizes of property vectors do not match.\n";
#endif
      }
      return (num_ent);
    }
  };

  /// @brief Write a mesh to a stream in FreeSurfer surf format.
  /// @details A surf file contains a vertex index representation of a mesh, i.e., the vertices and faces vectors.
  /// @param vertices vector of float, length 3n for n vertices. The 3D coordinates of the vertices, typically from `<Mesh_instance>.vertices`.
  /// @param faces vector of int, length 3n for n faces. The 3 vertex indices for each face, typically from `<Mesh_instance>.faces`.
  /// @param os An output stream to which to write the data. The stream must be open, and this function will not close it after writing to it.
  /// @throws std::runtime_error if the file cannot be opened.
  void write_surf(std::vector<float> vertices, std::vector<int32_t> faces, std::ostream &os)
  {
    const uint32_t SURF_TRIS_MAGIC = 16777214;
    _fwritei3(os, SURF_TRIS_MAGIC);
    std::string created_and_comment_lines = "Created by fslib\n\n";
    os << created_and_comment_lines;
    _fwritet<int32_t>(os, int(vertices.size() / 3)); // number of vertices
    _fwritet<int32_t>(os, int(faces.size() / 3));    // number of faces
    for (size_t i = 0; i < vertices.size(); i++)
    {
      _fwritet<float>(os, vertices[i]);
    }
    for (size_t i = 0; i < faces.size(); i++)
    {
      _fwritet<int32_t>(os, faces[i]);
    }
  }

  /// @brief Write a mesh to a binary file in FreeSurfer surf format.
  /// @details A surf file contains a vertex index representation of a mesh, i.e., the vertices and faces vectors.
  /// @param vertices vector of float, length 3n for n vertices. The 3D coordinates of the vertices, typically from `<Mesh_instance>.vertices`.
  /// @param faces vector of int, length 3n for n faces. The 3 vertex indices for each face, typically from `<Mesh_instance>.faces`.
  /// @param filename The path to the output file.
  /// @throws std::runtime_error if the file cannot be opened.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Mesh surface = fs::Mesh::construct_cube();
  /// fs::write_surf(surface.vertices, surface.faces, "lh.cube");
  /// @endcode
  void write_surf(std::vector<float> vertices, std::vector<int32_t> faces, const std::string &filename)
  {
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out | std::ofstream::binary);
    if (ofs.is_open())
    {
      write_surf(vertices, faces, ofs);
      ofs.close();
    }
    else
    {
      throw std::runtime_error("Unable to open surf file '" + filename + "' for writing.\n");
    }
  }

  /// @brief Write a mesh to a binary file in FreeSurfer surf format.
  /// @details A surf file contains a vertex index representation of a mesh, i.e., the vertices and faces vectors.
  /// @param mesh The `Mesh` instance to write.
  /// @param filename The path to the output file.
  /// @throws std::runtime_error if the file cannot be opened.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Mesh surface = fs::Mesh::construct_cube();
  /// fs::write_surf(surface, "lh.cube");
  /// @endcode
  void write_surf(const Mesh &mesh, const std::string &filename)
  {
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out | std::ofstream::binary);
    if (ofs.is_open())
    {
      write_surf(mesh.vertices, mesh.faces, ofs);
      ofs.close();
    }
    else
    {
      throw std::runtime_error("Unable to open surf file '" + filename + "' for writing.\n");
    }
  }

  /// @brief Read a FreeSurfer ASCII label from a stream.
  /// @details A label is a list of vertices (for a surface label, given by index) or voxels (for a volume label, given by the xyz coordinates) and one floating point value per vertex/voxel. Sometimes a label is only used to define a set of vertices/voxels (like a certain brain region), and the values are irrelevant (and typically left at 0.0).
  /// @param label A Label instance that should be filled.
  /// @param is An open std::istream or derived class stream from which to read the data, e.g., std::ifstream or std::istringstream.
  /// @see There exists an overload to read from a file instead.
  /// @throws std::domain_error if the label data format is incorrect
  void read_label(Label *label, std::istream *is)
  {
    std::string line;
    int line_idx = -1;
    size_t num_entries_header = 0; // number of vertices/voxels according to header
    size_t num_entries = 0;        // number of vertices/voxels for which the file contains label entries.
    while (std::getline(*is, line))
    {
      line_idx += 1;
      std::istringstream iss(line);
      if (line_idx == 0)
      {
        continue; // skip comment.
      }
      else
      {
        if (line_idx == 1)
        {
          if (!(iss >> num_entries_header))
          {
            throw std::domain_error("Could not parse entry count from label file, invalid format.\n");
          }
        }
        else
        {
          int vertex;
          float x, y, z, value;
          if (!(iss >> vertex >> x >> y >> z >> value))
          {
            throw std::domain_error("Could not parse line " + std::to_string(line_idx + 1) + " of label file, invalid format.\n");
          }
          label->vertex.push_back(vertex);
          label->coord_x.push_back(x);
          label->coord_y.push_back(y);
          label->coord_z.push_back(z);
          label->value.push_back(value);
          num_entries++;
        }
      }
    }
    if (num_entries != num_entries_header)
    {
      throw std::domain_error("Expected " + std::to_string(num_entries_header) + " entries from label file header, but found " + std::to_string(num_entries) + " in file, invalid label file.\n");
    }
    if (label->vertex.size() != num_entries || label->coord_x.size() != num_entries || label->coord_y.size() != num_entries || label->coord_z.size() != num_entries || label->value.size() != num_entries)
    {
      throw std::domain_error("Expected " + std::to_string(num_entries) + " entries in all Label vectors, but some did not match.\n");
    }
  }

  /// @brief Read a FreeSurfer ASCII label from a file.
  /// @details A label is a list of vertices (for a surface label, given by index) or voxels (for a volume label, given by the xyz coordinates) and one floating point value per vertex/voxel. Sometimes a label is only used to define a set of vertices/voxels (like a certain brain region), and the values are irrelevant (and typically left at 0.0).
  /// @param label A Label instance that should be filled.
  /// @param filename Path to the label file that should be read.
  /// @see There exists an overload to read from a stream instead.
  /// @throws std::domain_error if the label data format is incorrect, std::runtime_error if the file cannot be opened.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Label label;
  /// fs::read_label(&label, "subject1/label/lh.cortex.label");
  /// @endcode
  void read_label(Label *label, const std::string &filename)
  {
    std::ifstream infile(filename, std::fstream::in);
    if (infile.is_open())
    {
      read_label(label, &infile);
      infile.close();
    }
    else
    {
      throw std::runtime_error("Could not open label file '" + filename + "' for reading.\n");
    }
  }

  /// @brief Write label data to a stream.
  /// @param label The label to write.
  /// @param os An open output stream.
  /// @see There exists an onverload of this function to write a label to a file.
  void write_label(const Label &label, std::ostream &os)
  {
    const size_t num_entries = label.num_entries();
    os << "#!ascii label from subject anonymous\n"
       << num_entries << "\n";
    for (size_t i = 0; i < num_entries; i++)
    {
      os << label.vertex[i] << " " << label.coord_x[i] << " " << label.coord_y[i] << " " << label.coord_z[i] << " " << label.value[i] << "\n";
    }
  }

  /// @brief Write label data to a file.
  /// @param label an fs::Label instance
  /// @param filename Path to the label file that should be written.
  /// @see There exists an overload to write to a stream.
  /// @throws std::runtime_error if the file cannot be opened.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Label label;
  /// fs::read_label(&label, "subject1/label/lh.cortex.label");
  /// fs::write_label(label, "out.label");
  /// @endcode
  void write_label(const Label &label, const std::string &filename)
  {
    std::ofstream ofs;
    ofs.open(filename, std::ofstream::out);
    if (ofs.is_open())
    {
      write_label(label, ofs);
      ofs.close();
    }
    else
    {
      throw std::runtime_error("Unable to open label file '" + filename + "' for writing.\n");
    }
  }

  /// @brief Write a mesh to a file in different formats.
  /// @details The output format will be auto-determined from the file extension.
  /// @param mesh The fs::Mesh instance to write.
  /// @param filename The path to the output file.
  /// @throws std::runtime_error if the file cannot be opened.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Mesh surface = fs::Mesh::construct_cube();
  /// fs::write_mesh(surface, "cube.ply");
  /// fs::write_mesh(surface, "cube.off");
  /// fs::write_mesh(surface, "cube.obj");
  /// fs::write_mesh(surface, "cube");  // writes FS surf format.
  /// @endcode
  void write_mesh(const Mesh &mesh, const std::string &filename)
  {
    if (fs::util::ends_with(filename, {".ply", ".PLY"}))
    {
      mesh.to_ply_file(filename);
    }
    else if (fs::util::ends_with(filename, {".obj", ".OBJ"}))
    {
      mesh.to_obj_file(filename);
    }
    else if (fs::util::ends_with(filename, {".off", ".OFF"}))
    {
      mesh.to_off_file(filename);
    }
    else
    {
      fs::write_surf(mesh, filename);
    }
  }

  /// @brief Write a mesh to a file in different formats, with vertex colors.
  /// @details The output format will be auto-determined from the file extension. The colors are written for PLY, OFF, and OBJ formats; the surf format ignores them.
  /// @param mesh The fs::Mesh instance to write.
  /// @param filename The path to the output file.
  /// @param col u_char vector of RGB color values, 3 per vertex.
  /// @throws std::runtime_error if the file cannot be opened.
  ///
  /// #### Examples
  ///
  /// @code
  /// fs::Mesh surface = fs::Mesh::construct_cube();
  /// fs::write_mesh(surface, "cube.ply", surface.vertex_colors);
  /// @endcode
  void write_mesh(const Mesh &mesh, const std::string &filename, const std::vector<uint8_t> col)
  {
    if (fs::util::ends_with(filename, {".ply", ".PLY"}))
    {
      mesh.to_ply_file(filename, col);
    }
    else if (fs::util::ends_with(filename, {".obj", ".OBJ"}))
    {
      mesh.to_obj_file(filename, col);
    }
    else if (fs::util::ends_with(filename, {".off", ".OFF"}))
    {
      mesh.to_off_file(filename, col);
    }
    else
    {
      fs::write_surf(mesh, filename);
    }
  }

} // End namespace fs
