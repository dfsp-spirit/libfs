
#include <bit> // C++20, for std::endian
#include <iostream>
#include <climits>
#include <stdio.h>
#include <vector>
#include <fstream>




namespace fs {

  int fread3(std::istream& infile);
  int freadi32(std::istream& infile);
  uint8_t freadu8(std::istream& infile);

  float twotimes(const float in) {
    return(in*2.0);    
  }

  // Determine the endianness of the system.
  int is_bigendian() {
    if constexpr (!(std::endian::native == std::endian::big || std::endian::native == std::endian::little)) {
      std::cerr << "Mixed endian systems not supported\n.";
      exit(1);
    }
    const int is_be = std::endian::native == std::endian::big;
    return(is_be);
  }

  // Read a FreeSurfer curv format file. WIP.
  std::vector<float> read_curv(std::string filename) {
    const int CURV_MAGIC = 16777215;
    std::ifstream infile;
    infile.open(filename, std::ios_base::in | std::ios::binary);
    if(infile.is_open()) {
      int magic = fread3(infile);
      if(magic != CURV_MAGIC) {
        std::cerr << "Magic did not match: expected " << CURV_MAGIC << ", found " << magic << ".\n";
      }
      int num_verts = freadi32(infile);
      int num_faces = freadi32(infile);
      int num_values_per_vertex = freadi32(infile);
      std::cout << "Read file with " << num_verts << " vertices, " << num_faces << " faces and " << num_values_per_vertex << " values per vertex.\n";
      infile.close();
    } else {
      std::cerr << "Unable to open file.";
      exit(0);
    }
    std::vector<float> v;
    v.push_back(1.34f);
    return(v);
  }
  
  // Swap endianness of a value.
  template <typename T>
  T swap_endian(T u) {
      static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

      union
      {
          T u;
          unsigned char u8[sizeof(T)];
      } source, dest;

      source.u = u;

      for (size_t k = 0; k < sizeof(T); k++)
          dest.u8[k] = source.u8[sizeof(T) - k - 1];

      return(dest.u);
  }

  // Read a single big endian 32 bit integer from a stream.
  int freadi32(std::istream& infile) {
    int32_t i;
    infile.read(reinterpret_cast<char*>(&i), sizeof(i));
    if(! is_bigendian()) {
      i = swap_endian<std::int32_t>(i);
    }
    return(i);
  }

  // Read a single big endian uint8 from a stream. UNTESTED.
  uint8_t freadu8(std::istream& infile) {
    uint8_t i;
    infile.read(reinterpret_cast<char*>(&i), sizeof(i));
    std::cout << "Read raw uint8_t " << (unsigned int)i << ".\n";
    
    if(! is_bigendian()) {
      i = swap_endian<std::uint8_t>(i);
    }
    std::cout << " -Produced int " << (int)i << "\n";
    return(i);
  }

  // Read 3 big endian bytes as a single integer from a stream.
  int fread3(std::istream& infile) {
    uint32_t i;
    infile.read(reinterpret_cast<char*>(&i), 3);
    if(! is_bigendian()) {
      i = swap_endian<std::uint32_t>(i);
    }
    i = ((i >> 8) & 0xffffff);
    return(i);
  }


}





