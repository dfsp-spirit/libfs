
#include "libfs.h"
#include "catch.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>


TEST_CASE( "Reading the demo curv file with read_curv_data works" ) {

    std::vector<float> data = fs::read_curv_data("examples/read_curv/lh.thickness");

    SECTION("The number of values read is correct" ) {        
        REQUIRE( data.size() == 149244);
    }

    SECTION("The range of the values read is correct" ) {    
        float min_entry = *std::min_element(data.begin(), data.end());
        float max_entry = *std::max_element(data.begin(), data.end());    
        REQUIRE(min_entry == Approx(0.0));
        REQUIRE(max_entry == Approx(5.0));
    }

    SECTION("Some values are as expected") {
        REQUIRE(data[0] == Approx(2.561705));
        REQUIRE(data[100] == Approx(2.579938));
        REQUIRE(data[100000] == Approx(0.0));
    }
}

TEST_CASE( "Reading the demo curv file with read_curv works" ) {

    fs::Curv curv;
    fs::read_curv(&curv, "examples/read_curv/lh.thickness");

    SECTION("The number of values read is correct" ) {        
        REQUIRE( curv.data.size() == 149244);
    }

    SECTION("The range of the values read is correct" ) {    
        float min_entry = *std::min_element(curv.data.begin(), curv.data.end());
        float max_entry = *std::max_element(curv.data.begin(), curv.data.end());    
        REQUIRE(min_entry == Approx(0.0));
        REQUIRE(max_entry == Approx(5.0));
    }

    SECTION("Some values are as expected") {
        REQUIRE(curv.data[0] == Approx(2.561705));
        REQUIRE(curv.data[100] == Approx(2.579938));
        REQUIRE(curv.data[100000] == Approx(0.0));
    }
}


TEST_CASE( "Reading the demo MGH file works" ) {

    fs::Mgh mgh;
    fs::read_mgh(&mgh, "examples/read_mgh/brain.mgh");

    SECTION("The MRI_DTYPE is correct" ) {        
        REQUIRE( mgh.header.dtype == fs::MRI_UCHAR);
    }

    SECTION("The number of values read is correct" ) {        
        REQUIRE( mgh.data.data_mri_uchar.size() == 256*256*256);
    }

    SECTION("The range of the values read is correct" ) {    
        uint8_t min_entry = *std::min_element(mgh.data.data_mri_uchar.begin(), mgh.data.data_mri_uchar.end());
        uint8_t max_entry = *std::max_element(mgh.data.data_mri_uchar.begin(), mgh.data.data_mri_uchar.end());    
        REQUIRE(min_entry == 0);
        REQUIRE(max_entry == 156);
    }

    SECTION("The sum of the values is as expected") {
        int dsum = std::accumulate(mgh.data.data_mri_uchar.begin(), mgh.data.data_mri_uchar.end(), 0);
        REQUIRE(dsum == 121035479);
    }
}


TEST_CASE( "Reading the demo surface file works" ) {

    fs::Mesh surface;
    fs::read_surf(&surface, "examples/read_surf/lh.white");

    SECTION("The number of vertices and faces is correct" ) {        
        REQUIRE( surface.vertices.size() == 149244 * 3);
        REQUIRE( surface.faces.size() == 298484 * 3);
    }

    SECTION("Reading a mesh also works with read_mesh" ) { 
        fs::Mesh surface2;
        fs::read_mesh(&surface2, "examples/read_surf/lh.white");       
        REQUIRE( surface2.vertices.size() == 149244 * 3);
        REQUIRE( surface2.faces.size() == 298484 * 3);
    }

    SECTION("The range of vertex indices in the faces is correct" ) {
        int min_entry = *std::min_element(surface.faces.begin(), surface.faces.end()); // could use minmax for single call
        int max_entry = *std::max_element(surface.faces.begin(), surface.faces.end());    
        REQUIRE(min_entry == 0);
        REQUIRE(max_entry == 149243);
    }

    SECTION("The range of vertex coordinates is correct" ) {
        float min_entry = *std::min_element(surface.vertices.begin(), surface.vertices.end()); // could use minmax for single call
        float max_entry = *std::max_element(surface.vertices.begin(), surface.vertices.end());    
        REQUIRE(min_entry == Approx(-108.6204));
        REQUIRE(max_entry == Approx(106.1743));
    }

    SECTION("Matrix indices into the vertices vector can be computed." ) {        
        REQUIRE(surface.vm_at(0,0) == Approx(surface.vertices[0]));
        REQUIRE(surface.vm_at(0,1) == Approx(surface.vertices[1]));
        REQUIRE(surface.vm_at(0,2) == Approx(surface.vertices[2]));
        REQUIRE(surface.vm_at(1,0) == Approx(surface.vertices[3]));
        REQUIRE(surface.vm_at(1,1) == Approx(surface.vertices[4]));
        REQUIRE(surface.vm_at(1,2) == Approx(surface.vertices[5]));
        REQUIRE(surface.vm_at(2,0) == Approx(surface.vertices[6]));
        REQUIRE(surface.vm_at(2,1) == Approx(surface.vertices[7]));
        REQUIRE(surface.vm_at(2,2) == Approx(surface.vertices[8]));  

        // Check last vertex as well
        REQUIRE(surface.vm_at(surface.num_vertices()-1,0) == Approx(surface.vertices[surface.vertices.size() -1 -2]));
        REQUIRE(surface.vm_at(surface.num_vertices()-1,1) == Approx(surface.vertices[surface.vertices.size() -1 -1]));
        REQUIRE(surface.vm_at(surface.num_vertices()-1,2) == Approx(surface.vertices[surface.vertices.size() -1 -0]));
    }

    SECTION("Trying to access invalid vertex coord indices results in an exception." ) {
        REQUIRE_THROWS(surface.vm_at(surface.num_vertices(),0));
    }

    SECTION("Matrix indices into the faces vectors can be computed." ) {        
        REQUIRE(surface.fm_at(0,0) == surface.faces[0]);
        REQUIRE(surface.fm_at(0,1) == surface.faces[1]);
        REQUIRE(surface.fm_at(0,2) == surface.faces[2]);
        REQUIRE(surface.fm_at(1,0) == surface.faces[3]);
        REQUIRE(surface.fm_at(1,1) == surface.faces[4]);
        REQUIRE(surface.fm_at(1,2) == surface.faces[5]);
        REQUIRE(surface.fm_at(2,0) == surface.faces[6]);
        REQUIRE(surface.fm_at(2,1) == surface.faces[7]);
        REQUIRE(surface.fm_at(2,2) == surface.faces[8]);

        // Test known values as well: first face consists of vertices 0,1,5
        REQUIRE(surface.fm_at(0,0) == 0);
        REQUIRE(surface.fm_at(0,1) == 1);
        REQUIRE(surface.fm_at(0,2) == 5);

        // Check last face as well
        REQUIRE(surface.fm_at(surface.num_faces()-1,0) == Approx(surface.faces[surface.faces.size() -1 -2]));
        REQUIRE(surface.fm_at(surface.num_faces()-1,1) == Approx(surface.faces[surface.faces.size() -1 -1]));
        REQUIRE(surface.fm_at(surface.num_faces()-1,2) == Approx(surface.faces[surface.faces.size() -1 -0]));
    }

    SECTION("Trying to access invalid face indices results in an exception." ) {
        REQUIRE_THROWS(surface.fm_at(surface.num_faces(),0));
    }
}


TEST_CASE( "Importing and exporting meshes works" ) {

    fs::Mesh surface;
    fs::read_surf(&surface, "examples/read_surf/lh.white");

    SECTION("The mesh can be exported to OBJ format and re-read." ) {        
        const std::string obj_file = "examples/read_surf/lh.white.obj";
        surface.to_obj_file(obj_file);

        fs::Mesh surface2;
        fs::Mesh::from_obj(&surface2, obj_file);

        // Check vertex and face counts
        REQUIRE( surface2.vertices.size() == 149244 * 3);
        REQUIRE( surface2.faces.size() == 298484 * 3);

        // Check face vertex indices
        int vmin_entry = *std::min_element(surface2.faces.begin(), surface2.faces.end()); // could use minmax for single call
        int vmax_entry = *std::max_element(surface2.faces.begin(), surface2.faces.end());    
        REQUIRE(vmin_entry == 0);
        REQUIRE(vmax_entry == 149243);    

        // The range of vertex coordinates is correct"
        float cmin_entry = *std::min_element(surface2.vertices.begin(), surface2.vertices.end()); // could use minmax for single call
        float cmax_entry = *std::max_element(surface2.vertices.begin(), surface2.vertices.end());    
        REQUIRE(cmin_entry == Approx(-108.6204));
        REQUIRE(cmax_entry == Approx(106.1743));
    }

    SECTION("The mesh can be exported to PLY format and re-read." ) {        
        const std::string ply_file = "examples/read_surf/lh.white.ply";
        surface.to_ply_file(ply_file);

        fs::Mesh surface2;
        fs::Mesh::from_ply(&surface2, ply_file);

        // Check vertex and face counts
        REQUIRE( surface2.vertices.size() == 149244 * 3);
        REQUIRE( surface2.faces.size() == 298484 * 3);

        // Check face vertex indices
        int vmin_entry = *std::min_element(surface2.faces.begin(), surface2.faces.end()); // could use minmax for single call
        int vmax_entry = *std::max_element(surface2.faces.begin(), surface2.faces.end());    
        REQUIRE(vmin_entry == 0);
        REQUIRE(vmax_entry == 149243);    

        // The range of vertex coordinates is correct"
        float cmin_entry = *std::min_element(surface2.vertices.begin(), surface2.vertices.end()); // could use minmax for single call
        float cmax_entry = *std::max_element(surface2.vertices.begin(), surface2.vertices.end());    
        REQUIRE(cmin_entry == Approx(-108.6204));
        REQUIRE(cmax_entry == Approx(106.1743));
    }

    SECTION("Re-reading OBJ also works with read_mesh." ) {        
        const std::string obj_file = "examples/read_surf/lh.white.obj";
        surface.to_obj_file(obj_file);

        fs::Mesh surface2;
        fs::read_mesh(&surface2, obj_file);

        // Check vertex and face counts
        REQUIRE( surface2.vertices.size() == 149244 * 3);
        REQUIRE( surface2.faces.size() == 298484 * 3);

        // Check face vertex indices
        int vmin_entry = *std::min_element(surface2.faces.begin(), surface2.faces.end()); // could use minmax for single call
        int vmax_entry = *std::max_element(surface2.faces.begin(), surface2.faces.end());    
        REQUIRE(vmin_entry == 0);
        REQUIRE(vmax_entry == 149243);    

        // The range of vertex coordinates is correct"
        float cmin_entry = *std::min_element(surface2.vertices.begin(), surface2.vertices.end()); // could use minmax for single call
        float cmax_entry = *std::max_element(surface2.vertices.begin(), surface2.vertices.end());    
        REQUIRE(cmin_entry == Approx(-108.6204));
        REQUIRE(cmax_entry == Approx(106.1743));
    }

    SECTION("Re-reading PLY also works with read_mesh." ) {        
        const std::string ply_file = "examples/read_surf/lh.white.ply";
        surface.to_ply_file(ply_file);

        fs::Mesh surface2;
        fs::read_mesh(&surface2, ply_file);

        // Check vertex and face counts
        REQUIRE( surface2.vertices.size() == 149244 * 3);
        REQUIRE( surface2.faces.size() == 298484 * 3);

        // Check face vertex indices
        int vmin_entry = *std::min_element(surface2.faces.begin(), surface2.faces.end()); // could use minmax for single call
        int vmax_entry = *std::max_element(surface2.faces.begin(), surface2.faces.end());    
        REQUIRE(vmin_entry == 0);
        REQUIRE(vmax_entry == 149243);    

        // The range of vertex coordinates is correct"
        float cmin_entry = *std::min_element(surface2.vertices.begin(), surface2.vertices.end()); // could use minmax for single call
        float cmax_entry = *std::max_element(surface2.vertices.begin(), surface2.vertices.end());    
        REQUIRE(cmin_entry == Approx(-108.6204));
        REQUIRE(cmax_entry == Approx(106.1743));
    }            
}


TEST_CASE( "Reading the demo label file works" ) {

    fs::Label label;
    fs::read_label(&label, "examples/read_label/lh.cortex.label");
    const size_t surface_num_vertices = 149244;
    const size_t label_num_entries = 140891;

    SECTION("The number of vertices in the label is correct" ) {        
        REQUIRE( label.vertex.size() == label_num_entries);
    }

    SECTION("The number of vertices, coordinates and values in the label are identical and correct" ) {        
        REQUIRE( label.vertex.size() == label_num_entries);
        REQUIRE( label.coord_x.size() == label_num_entries);
        REQUIRE( label.coord_y.size() == label_num_entries);
        REQUIRE( label.coord_z.size() == label_num_entries);
        REQUIRE( label.value.size() == label_num_entries);
    }

    SECTION("One can compute whether the vertices of the surface are part of the label." ) {
        std::vector<bool> vert_in = label.vert_in_label(surface_num_vertices);
        int num_in = 0;
        for(size_t i=0; i<vert_in.size(); i++) {
            if(vert_in[i]) {
                num_in++;
            }
        }
        REQUIRE( num_in == label_num_entries);
    }
}


TEST_CASE( "Reading the demo annot file works" ) {

    fs::Annot annot;
    fs::read_annot(&annot, "examples/read_annot/lh.aparc.annot");
    const size_t surface_num_vertices = 149244;

    SECTION("The number of vertices in the annot is correct" ) {        
        REQUIRE( annot.num_vertices() == surface_num_vertices);
    }

    SECTION("The number of regions in the annot Colortable is correct" ) {        
        REQUIRE( annot.colortable.num_entries() == 36);
    }

    SECTION("One can compute all vertices of a region." ) {
        std::string region_name = "bankssts";
        std::vector<int32_t> bankssts_vertices = annot.region_vertices(region_name);
        REQUIRE( bankssts_vertices.size() == 1722);
    }

    SECTION("One can compute the region for all vertices." ) {
        std::string region_name = "bankssts";
        std::vector<std::string> vertex_reg_names = annot.vertex_region_names();
        
        // Ensure that bankssts occurs 1722 times in total.
        int32_t num_bankssts = 0;
        for(size_t i=0; i<vertex_reg_names.size(); i++) {
            if(vertex_reg_names[i] == region_name) {
                num_bankssts++;
            }
        }
        REQUIRE( num_bankssts == 1722);
    }
}


TEST_CASE( "Reading metadata works" ) {

    
    SECTION("A simple subjects file can be read." ) {
        const std::string subjects_file = "examples/read_metadata/subjects.txt";
        std::vector<std::string> subjects = fs::read_subjectsfile(subjects_file);
        REQUIRE( subjects.size() == 3);
        REQUIRE( subjects[0] == "subject001");
        REQUIRE( subjects[1] == "subject002");
        REQUIRE( subjects[2] == "subject003");
    }

    
}


TEST_CASE( "The util functions work" ) {

    
    SECTION("The starts_with function works." ) {
        const std::string some_file = "examples/read_metadata/subjects.txt";        
        REQUIRE( fs::util::starts_with(some_file, "examples"));
        REQUIRE( fs::util::starts_with(some_file, "e"));
        REQUIRE( fs::util::starts_with(some_file, some_file));

        REQUIRE( fs::util::starts_with(some_file, "nope") == false);
    }

    SECTION("The ends_with function works." ) {
        const std::string some_file = "examples/read_metadata/subjects.txt";        
        REQUIRE( fs::util::ends_with(some_file, ".txt"));
        REQUIRE( fs::util::ends_with(some_file, "t"));
        REQUIRE( fs::util::ends_with(some_file, some_file));

        REQUIRE( fs::util::ends_with(some_file, "nope") == false);
    }

    SECTION("The fullpath function works." ) {        
        REQUIRE( fs::util::fullpath({"a", "b"}) == "a/b");
        REQUIRE( fs::util::fullpath({"/a", "b"}) == "/a/b");
        REQUIRE( fs::util::fullpath({"/a", "/b"}) == "/a/b");
        REQUIRE( fs::util::fullpath({"/a", "b/"}) == "/a/b");
        REQUIRE( fs::util::fullpath({"/a", "/b/"}) == "/a/b");
        REQUIRE( fs::util::fullpath({"/a/", "/b/"}) == "/a/b");
        REQUIRE( fs::util::fullpath({"/a/", "/b"}) == "/a/b");
        REQUIRE( fs::util::fullpath({"/a/", "b/"}) == "/a/b");
    }

    
}

