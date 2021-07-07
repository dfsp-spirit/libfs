
#include <bit> // C++20, for std::endian
#include <iostream>
#include <climits>
#include <stdio.h>
#include <vector>
#include <fstream>




namespace fs {

  int fread3(std::istream& infile);
  float freadf4(std::istream& infile);
  int freadi32(std::istream& infile);
  uint8_t freadu8(std::istream& infile);
  std::string freadstringzero(std::istream& stream);
  std::string freadstringnewline(std::istream& stream);

  class Mesh {
    public:
      std::vector<float> vertices;
      std::vector<int> faces;
  };

  void read_fssurface(Mesh* surface, std::string filename) {
    const int SURF_TRIS_MAGIC = 16777214;
    std::ifstream infile;
    infile.open(filename, std::ios_base::in | std::ios::binary);
    if(infile.is_open()) {
      int magic = fread3(infile);
      if(magic != SURF_TRIS_MAGIC) {
        std::cerr << "Magic did not match: expected " << SURF_TRIS_MAGIC << ", found " << magic << ".\n";
      }
      //std::string created_line = freadstringzero(infile);
      std::string created_line = freadstringnewline(infile);
      std::string comment_line = freadstringnewline(infile);
      int num_verts = freadi32(infile);
      int num_faces = freadi32(infile);      
      std::cout << "Read surface file with " << num_verts << " vertices, " << num_faces << " faces.\n";
      std::vector<float> vdata;
      for(int i=0; i<num_verts; i++) {
        vdata.push_back(freadf4(infile));
      }
      std::vector<int> fdata;
      for(int i=0; i<num_faces; i++) {
        fdata.push_back(freadi32(infile));
      }
      infile.close();
      surface->vertices = vdata;
      surface->faces = fdata;
    } else {
      std::cerr << "Unable to open surface file '" << filename << "'.\n";
      exit(0);
    }

  }

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
      if(num_values_per_vertex != 1) { // Not supported, I know no case where this is used. Please submit a PR with a demo file if you have one, and let me know where it came from.
        std::cerr << "Curv file must contain exactly 1 value per vertex, found " << num_values_per_vertex << ".\n";  
      }
      std::vector<float> data;
      for(int i=0; i<num_verts; i++) {
        data.push_back(freadf4(infile));
      }
      infile.close();
      return(data);
    } else {
      std::cerr << "Unable to open curvature file '" << filename << "'.\n";
      exit(0);
    }
    
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

  // Read big endian 4 byte float from a stream.
  float freadf4(std::istream& infile) {
    _Float32 f;
    infile.read(reinterpret_cast<char*>(&f), sizeof(f));
    if(! is_bigendian()) {
      f = swap_endian<_Float32>(f);
    }
    return(f);
  }

  // Read a C-style zero-terminated ASCII string from a stream.
  std::string freadstringzero(std::istream &stream) {
    std::string s;
    std::getline(stream, s, '\0');
    return s;
  }

  // Read a '\n'-terminated ASCII string from a stream.
  std::string freadstringnewline(std::istream &stream) {
    std::string s;
    std::getline(stream, s, '\n');
    return s;
  }


}





