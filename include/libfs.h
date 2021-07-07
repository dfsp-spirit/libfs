
#include <bit> // C++20, for std::endian
#include <iostream>
#include <climits>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <cassert>




namespace fs {
  const int MRI_UCHAR = 0; // MRI data types
    const int MRI_INT = 1;
    const int MRI_FLOAT = 3; 
    const int MRI_SHORT = 4;

  int fread3(std::istream& infile);
  float freadf4(std::istream& infile);
  int freadi32(std::istream& infile);
  int freadi16(std::istream& infile);
  uint8_t freadu8(std::istream& infile);
  std::string freadstringzero(std::istream& stream);
  std::string freadstringnewline(std::istream& stream);
  bool ends_with(std::string const &fullString, std::string const &ending);
  

  class Mesh {
    public:
      std::vector<float> vertices;
      std::vector<int> faces;
  };

  // https://stackoverflow.com/questions/33113403/store-a-4d-array-in-a-vector
  struct Array4D {
    Array4D(int d1, int d2, int d3, int d4) :
      d1(d1), d2(d2), d3(d3), d4(d4), data(d1*d2*d3*d4) {}
  
    float& at(int i1, int i2, int i3, int i4)
    {
      return data[getIndex(i1, i2, i3, i4)];
    }
  
    int getIndex(int i1, int i2, int i3, int i4)
    {
      assert(i1 >= 0 && i1 < d1);
      assert(i2 >= 0 && i2 < d2);
      assert(i3 >= 0 && i3 < d3);
      assert(i4 >= 0 && i4 < d4);
      return (((i1*d2 + i2)*d3 + i3)*d4 + i4);
    }
  
    int d1;
    int d2;
    int d3;
    int d4;
    std::vector<float> data;
  };

  struct MghHeader {
    int32_t dim1length;
    int32_t dim2length;
    int32_t dim3length;
    int32_t dim4length;

    int32_t dtype;
    int32_t dof;
    int16_t ras_good_flag;

    float xsize;
    float ysize;
    float zsize;
    std::vector<float> Mdc;
    std::vector<float> Pxyz_c;
  };

  struct MghData {
    std::vector<int32_t> data_mri_int;
    std::vector<uint8_t> data_mri_uchar;
    std::vector<float> data_mri_float;
  };

  struct Mgh {
    MghHeader header;
    MghData data;    
  };

  void read_mgh_header(MghHeader* mgh_header, std::string filename);
  std::vector<int32_t> read_mgh_data_int(MghHeader*, std::string);
  std::vector<uint8_t> read_mgh_data_uchar(MghHeader*, std::string);
  std::vector<float> read_mgh_data_float(MghHeader*, std::string);

  void read_mgh(Mgh* mgh, std::string filename) {
    MghHeader mgh_header;
    read_mgh_header(&mgh_header, filename);
    mgh->header = mgh_header;
    if(mgh->header.dtype == MRI_INT) {
      std::vector<int32_t> data = read_mgh_data_int(&mgh_header, filename);
      mgh->data.data_mri_int = data;
    } else if(mgh->header.dtype == MRI_UCHAR) {
      std::vector<uint8_t> data = read_mgh_data_uchar(&mgh_header, filename);
      mgh->data.data_mri_uchar = data;
    } else if(mgh->header.dtype == MRI_FLOAT) {
      std::vector<float> data = read_mgh_data_float(&mgh_header, filename);
      mgh->data.data_mri_float = data;
    } else {      
      std::cout << "Not reading MGH data from file '" << filename << "', data type " << mgh->header.dtype << " not supported yet.\n";
      if(ends_with(filename, ".mgz")) {
        std::cout << "Note: your MGH filename ends with '.mgz'. Keep in mind that MGZ format is not supported yet.\n";  
      }
      exit(1);
    }
  }

  void read_mgh_header(MghHeader* mgh_header, std::string filename) {
    const int MGH_VERSION = 1;    

    std::ifstream infile;
    infile.open(filename, std::ios_base::in | std::ios::binary);
    if(infile.is_open()) {
      int format_version = freadi32(infile);
      if(format_version != MGH_VERSION) {        
        std::cerr << "Invalid MGH file or unsupported file format version: expected version " << MGH_VERSION << ", found " << format_version << ".\n";
        if(ends_with(filename, ".mgz")) {
          std::cout << "Note: your MGH filename ends with '.mgz'. Keep in mind that MGZ format is not supported yet.\n";  
        }
        exit(1);
      }
      mgh_header->dim1length = freadi32(infile);
      mgh_header->dim2length = freadi32(infile);
      mgh_header->dim3length = freadi32(infile);
      mgh_header->dim4length = freadi32(infile);

      mgh_header->dtype = freadi32(infile);
      mgh_header->dof = freadi32(infile);

      int unused_header_space_size_left = 256;  // in bytes
      mgh_header->ras_good_flag = freadi16(infile);
      unused_header_space_size_left -= 2; // for the ras_good_flag

      // Read the RAS part of the header.
      if(mgh_header->ras_good_flag == 1) {
        mgh_header->xsize = freadf4(infile);
        mgh_header->ysize = freadf4(infile);
        mgh_header->zsize = freadf4(infile);

        for(int i=0; i<9; i++) {
          mgh_header->Mdc.push_back(freadf4(infile));
        }
        for(int i=0; i<3; i++) {
          mgh_header->Pxyz_c.push_back(freadf4(infile));
        }
        unused_header_space_size_left -= 60;
      }
      // TODO: MGH files may contain an optional footer after the data with some metadata that we could read.
      //infile.seekg(unused_header_space_size_left, infile.cur);  // skip rest of header.
      // skip past data
      // check if something left in file and read footer if existant

      infile.close();
    } else {
      std::cerr << "Unable to open MGH file '" << filename << "'.\n";
      exit(0);
    }
  }

  // Read MRI_INT data from MGH file
  std::vector<int32_t> read_mgh_data_int(MghHeader* mgh_header, std::string filename) {
    std::ifstream infile;
    infile.open(filename, std::ios_base::in | std::ios::binary);
    if(mgh_header->dtype != 1) {
      std::cerr << "Expected MRI data type 1, but found " << mgh_header->dtype << ".\n";
    }
    if(infile.is_open()) {
      infile.seekg(284, infile.beg); // skip to end of header and beginning of data

      int num_values = mgh_header->dim1length * mgh_header->dim2length * mgh_header->dim3length * mgh_header->dim4length;
      std::vector<int32_t> data;
      for(int i=0; i<num_values; i++) {
        data.push_back(freadi32(infile));
      }
      infile.close();
      return(data);
    } else {
      std::cerr << "Unable to open MGH file '" << filename << "'.\n";
      exit(0);
    }
  }

  // Read MRI_FLOAT data from MGH file
  std::vector<float> read_mgh_data_float(MghHeader* mgh_header, std::string filename) {
    std::ifstream infile;
    infile.open(filename, std::ios_base::in | std::ios::binary);
    if(mgh_header->dtype != 3) {
      std::cerr << "Expected MRI data type 3, but found " << mgh_header->dtype << ".\n";
    }
    if(infile.is_open()) {
      infile.seekg(284, infile.beg); // skip to end of header and beginning of data

      int num_values = mgh_header->dim1length * mgh_header->dim2length * mgh_header->dim3length * mgh_header->dim4length;
      std::vector<float> data;
      for(int i=0; i<num_values; i++) {
        data.push_back(freadf4(infile));
      }
      infile.close();
      return(data);
    } else {
      std::cerr << "Unable to open MGH file '" << filename << "'.\n";
      exit(0);
    }
  }

  // Read MRI_UCHAR data from MGH file
  std::vector<uint8_t> read_mgh_data_uchar(MghHeader* mgh_header, std::string filename) {
    std::ifstream infile;
    infile.open(filename, std::ios_base::in | std::ios::binary);
    if(mgh_header->dtype != 0) {
      std::cerr << "Expected MRI data type 0, but found " << mgh_header->dtype << ".\n";
    }
    if(infile.is_open()) {
      infile.seekg(284, infile.beg); // skip to end of header and beginning of data

      int num_values = mgh_header->dim1length * mgh_header->dim2length * mgh_header->dim3length * mgh_header->dim4length;
      std::vector<uint8_t> data;
      for(int i=0; i<num_values; i++) {
        data.push_back(freadu8(infile));
      }
      infile.close();
      return(data);
    } else {
      std::cerr << "Unable to open MGH file '" << filename << "'.\n";
      exit(0);
    }
  }

  void read_fssurface(Mesh* surface, std::string filename) {
    const int SURF_TRIS_MAGIC = 16777214;
    std::ifstream infile;
    infile.open(filename, std::ios_base::in | std::ios::binary);
    if(infile.is_open()) {
      int magic = fread3(infile);
      if(magic != SURF_TRIS_MAGIC) {
        std::cerr << "Magic did not match: expected " << SURF_TRIS_MAGIC << ", found " << magic << ".\n";
      }
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


  // Read a single big endian 16 bit integer from a stream.
  int freadi16(std::istream& infile) {
    int16_t i;
    infile.read(reinterpret_cast<char*>(&i), sizeof(i));
    if(! is_bigendian()) {
      i = swap_endian<std::int16_t>(i);
    }
    return(i);
  }

  // Read a single big endian uint8 from a stream. UNTESTED.
  uint8_t freadu8(std::istream& infile) {
    uint8_t i;
    infile.read(reinterpret_cast<char*>(&i), sizeof(i));
    //std::cout << "Read raw uint8_t " << (unsigned int)i << ".\n";
    
    if(! is_bigendian()) {
      i = swap_endian<std::uint8_t>(i);
    }
    //std::cout << " -Produced int " << (int)i << "\n";
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

  // Check whether a string ends with the given suffix.
  // https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
  bool ends_with (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}



}





