
#include <bit> // C++20, for std::endian
#include <iostream>

namespace fs {

  float twotimes(const float in) {
    return(in*2.0);    
  }

  int is_bigendian() {
    if constexpr (!(std::endian::native == std::endian::big || std::endian::native == std::endian::little)) {
      std::cerr << "Mixed endian systems not supported\n.";
      exit(1);
    }
    const int is_be = std::endian::native == std::endian::big;
    return(is_be);
  }


}
