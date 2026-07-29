// libFuzzer harness for libfs binary file parsers.
//
// Build (requires Clang):
//   clang++ -std=c++11 -fsanitize=fuzzer,address,undefined \
//           -Iinclude tests/fuzz_parsers.cpp -o build/fuzz_parsers
//
// Run:
//   ./build/fuzz_parsers -max_len=65536 -runs=100000

#include "libfs.h"
#include <sstream>
#include <cstdint>
#include <cstddef>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size == 0)
    {
        return 0; // libFuzzer may pass empty input; skip it.
    }

    std::string input(reinterpret_cast<const char *>(data), size);

    // --- Fuzz fs::read_curv (stream overload) ---
    {
        try
        {
            std::istringstream is(input, std::ios::binary);
            fs::Curv curv;
            fs::read_curv(&curv, &is);
        }
        catch (...) {}
    }

    // --- Fuzz fs::read_annot (stream overload) ---
    {
        try
        {
            std::istringstream is(input, std::ios::binary);
            fs::Annot annot;
            fs::read_annot(&annot, &is);
        }
        catch (...) {}
    }

    // --- Fuzz fs::read_mgh (stream overload) ---
    {
        try
        {
            std::istringstream is(input, std::ios::binary);
            fs::Mgh mgh;
            fs::read_mgh(&mgh, &is);
        }
        catch (...) {}
    }

    return 0; // Non-zero return reserved for future use by libFuzzer.
}
