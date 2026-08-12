

#define LIBFS_DBG_WARNING

#include "libfs.h"
#include "catch.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdint>


TEST_CASE( "Reading the demo curv file with read_curv_data works" ) {

    std::string curv_file_path = fs::util::fullpath({"examples", "read_curv" , "lh.thickness"});
    if (! fs::util::file_exists(curv_file_path)) {
        std::cerr << "Cannot access test data curv file at '" << curv_file_path << "'." << std::endl;
    }
    std::vector<float> data = fs::read_curv_data(curv_file_path);

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


TEST_CASE( "Reading curv and mgh files with read_desc works" ) {

    std::string curv_file = "examples/read_curv/lh.thickness";
    std::string mgh_file = "examples/read_mgh/lh.thickness.mgh";

    std::vector<float> data_curv = fs::read_desc_data(curv_file);
    std::vector<float> data_mgh = fs::read_desc_data(mgh_file);

    SECTION("The number of values read from the curv file is correct" ) {
        REQUIRE( data_curv.size() == 149244);
    }

    SECTION("The number of values read from the MGH file is correct" ) {
        REQUIRE( data_mgh.size() == 149244);
    }

    SECTION("The range of the values read from the curv file is correct" ) {
        float min_entry = *std::min_element(data_curv.begin(), data_curv.end());
        float max_entry = *std::max_element(data_curv.begin(), data_curv.end());
        REQUIRE(min_entry == Approx(0.0));
        REQUIRE(max_entry == Approx(5.0));
    }

    SECTION("The range of the values read from the MGH file is correct" ) {
        float min_entry = *std::min_element(data_mgh.begin(), data_mgh.end());
        float max_entry = *std::max_element(data_mgh.begin(), data_mgh.end());
        REQUIRE(min_entry == Approx(0.0));
        REQUIRE(max_entry == Approx(5.0));
    }

    SECTION("Some values read from the curv file are as expected") {
        REQUIRE(data_curv[0] == Approx(2.561705));
        REQUIRE(data_curv[100] == Approx(2.579938));
        REQUIRE(data_curv[100000] == Approx(0.0));
    }

    SECTION("Some values read from the MGH file are as expected") {
        REQUIRE(data_mgh[0] == Approx(2.561705));
        REQUIRE(data_mgh[100] == Approx(2.579938));
        REQUIRE(data_mgh[100000] == Approx(0.0));
    }
}


TEST_CASE( "Creating a new MGH file from Curv works" ) {

    // This example writes 1-dimensional curv data (per-vertex data from a single subject/hemisphere) into a new MGH file.

    fs::Curv curv;
    fs::read_curv(&curv, "examples/read_curv/lh.thickness");

    fs::Mgh mgh = fs::Mgh(curv);

    SECTION("The MRI_DTYPE is correct" ) {
        REQUIRE( mgh.header.dtype == fs::MRI_FLOAT);
    }

    SECTION("The number of values read is correct" ) {
        REQUIRE( curv.data.size() == 149244);
        REQUIRE(mgh.header.dim1length == 149244);
        REQUIRE(mgh.header.dim2length == 1);
        REQUIRE(mgh.header.dim3length == 1);
        REQUIRE(mgh.header.dim4length == 1);
    }

    // Also write and re-read to check for issues with rest of default header fields.
    const std::string mgh_out_file = "examples/read_mgh/thickness_tmp.mgh";
    fs::write_mgh(mgh, mgh_out_file);

    fs::Mgh mgh2;
    fs::read_mgh(&mgh2, mgh_out_file);

    SECTION("The MRI_DTYPE is correct" ) {
        REQUIRE( mgh2.header.dtype == fs::MRI_FLOAT);
    }

    SECTION("The number of values read is correct" ) {
        REQUIRE( curv.data.size() == 149244);
        REQUIRE(mgh2.header.dim1length == 149244);
        REQUIRE(mgh2.header.dim2length == 1);
        REQUIRE(mgh2.header.dim3length == 1);
        REQUIRE(mgh2.header.dim4length == 1);
    }
}


TEST_CASE( "Creating a new MGH file from scratch works" ) {

    // This example writes 1-dimensional curv data (per-vertex data from a single subject/hemisphere) into a new MGH file.

    fs::Curv curv;
    fs::read_curv(&curv, "examples/read_curv/lh.thickness");

    fs::Mgh mgh = fs::Mgh(curv.data);

    SECTION("The MRI_DTYPE is correct" ) {
        REQUIRE( mgh.header.dtype == fs::MRI_FLOAT);
    }

    SECTION("The number of values read is correct" ) {
        REQUIRE( curv.data.size() == 149244);
        REQUIRE(mgh.header.dim1length == 149244);
        REQUIRE(mgh.header.dim2length == 1);
        REQUIRE(mgh.header.dim3length == 1);
        REQUIRE(mgh.header.dim4length == 1);
    }

    // Also write and re-read to check for issues with rest of default header fields.
    const std::string mgh_out_file = "examples/read_mgh/brain_tmp.mgh";
    fs::write_mgh(mgh, mgh_out_file);

    fs::Mgh mgh2;
    fs::read_mgh(&mgh2, mgh_out_file);

    SECTION("The MRI_DTYPE is correct" ) {
        REQUIRE( mgh2.header.dtype == fs::MRI_FLOAT);
    }

    SECTION("The number of values read is correct" ) {
        REQUIRE( curv.data.size() == 149244);
        REQUIRE(mgh2.header.dim1length == 149244);
        REQUIRE(mgh2.header.dim2length == 1);
        REQUIRE(mgh2.header.dim3length == 1);
        REQUIRE(mgh2.header.dim4length == 1);
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

TEST_CASE( "Writing and re-reading an MGH file with UCHAR data works" ) {

    // Read our demo MGH file. It contains MRI_UCHAR data already.
    fs::Mgh mgh;
    fs::read_mgh(&mgh, "examples/read_mgh/brain.mgh");

    // Write a copy to disk.
    const std::string mgh_out_file = "examples/read_mgh/brain_exp.mgh";
    fs::write_mgh(mgh, mgh_out_file);

    // Re-read it.
    fs::Mgh mgh2;
    fs::read_mgh(&mgh2, mgh_out_file);

    SECTION("The MRI_DTYPE is correct" ) {
        REQUIRE( mgh2.header.dtype == fs::MRI_UCHAR);
    }

    SECTION("The number of values read is correct" ) {
        REQUIRE( mgh2.data.data_mri_uchar.size() == 256*256*256);
    }

    SECTION("The range of the values read is correct" ) {
        uint8_t min_entry = *std::min_element(mgh2.data.data_mri_uchar.begin(), mgh2.data.data_mri_uchar.end());
        uint8_t max_entry = *std::max_element(mgh2.data.data_mri_uchar.begin(), mgh2.data.data_mri_uchar.end());
        REQUIRE(min_entry == 0);
        REQUIRE(max_entry == 156);
    }

    SECTION("The sum of the values is as expected") {
        int dsum = std::accumulate(mgh2.data.data_mri_uchar.begin(), mgh2.data.data_mri_uchar.end(), 0);
        REQUIRE(dsum == 121035479);
    }
}


TEST_CASE( "Writing and re-reading an MGH file with MRI_SHORT data works" ) {

    // Read our demo MGH file. It contains MRI_UCHAR data, so we convert that.
    fs::Mgh mgh;
    fs::read_mgh(&mgh, "examples/read_mgh/brain.mgh");

    // Convert data.
    mgh.header.dtype = fs::MRI_SHORT;
    size_t num_values = mgh.data.data_mri_uchar.size();
    mgh.data.data_mri_short = std::vector<short>(num_values);
    for(size_t i=0; i<num_values; i++) {
        mgh.data.data_mri_short[i] = (short)mgh.data.data_mri_uchar[i];
    }

    // Write a copy to disk.
    const std::string mgh_out_file = "examples/read_mgh/brain_exp_short.mgh";
    fs::write_mgh(mgh, mgh_out_file);

    // Re-read it.
    fs::Mgh mgh2;
    fs::read_mgh(&mgh2, mgh_out_file);

    SECTION("The MRI_DTYPE is correct" ) {
        REQUIRE( mgh2.header.dtype == fs::MRI_SHORT);
    }

    SECTION("The number of values read is correct" ) {
        REQUIRE( mgh2.data.data_mri_short.size() == 256*256*256);
    }

    SECTION("The range of the values read is correct" ) {
        short min_entry = *std::min_element(mgh2.data.data_mri_short.begin(), mgh2.data.data_mri_short.end());
        short max_entry = *std::max_element(mgh2.data.data_mri_short.begin(), mgh2.data.data_mri_short.end());
        REQUIRE(min_entry == 0);
        REQUIRE(max_entry == 156);
    }

    SECTION("The sum of the values is as expected") {
        int dsum = std::accumulate(mgh2.data.data_mri_short.begin(), mgh2.data.data_mri_short.end(), 0);
        REQUIRE(dsum == 121035479);
    }
}

TEST_CASE( "Meshes can be constructed" ) {

    SECTION("Meshed can be constructed from 1D vectors." ) {
        std::vector<float> vertices = {1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 3.0, 3.0, 3.0 };
        std::vector<int32_t> faces = {0, 0, 0, 1, 1, 1, 2, 2, 2, 0, 1, 2 };
        fs::Mesh surface = fs::Mesh(vertices, faces);
        REQUIRE(surface.num_vertices() == 3);
        REQUIRE(surface.num_faces() == 4);
    }

    SECTION("Meshed can be constructed from 2D vectors." ) {
        std::vector<std::vector<float>> vertices = std::vector<std::vector<float>>();
        vertices.push_back({1.0, 1.0, 1.0});
        vertices.push_back({2.0, 2.0, 2.0});
        vertices.push_back({3.0, 3.0, 3.0});

        std::vector<std::vector<int32_t>> faces = std::vector<std::vector<int32_t>>();
        faces.push_back({0, 0, 0});
        faces.push_back({1, 1, 1});
        faces.push_back({2, 2, 2});
        faces.push_back({0, 1, 2});

        fs::Mesh surface = fs::Mesh(vertices, faces);
        REQUIRE(surface.num_vertices() == 3);
        REQUIRE(surface.num_faces() == 4);
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

    SECTION("Using submesh_vertex to compute a patch mesh works") {
        fs::Label label;
        fs::read_label(&label, "examples/read_label/lh.cortex.label");

        std::pair <std::unordered_map<int32_t, int32_t>, fs::Mesh> result = surface.submesh_vertex(label.vertex);
        fs::Mesh patch = result.second;
        REQUIRE(patch.num_vertices() == label.vertex.size());
        REQUIRE(patch.num_faces() < surface.num_faces());
        REQUIRE(patch.num_faces() == 281410);
        //fs::util::str_to_file("lh.cortex.obj", patch.to_obj());  // check this mesh visually with meshlab
    }

    SECTION("Using curv_data_for_origmesh to stretch submesh per-vertex data to the original mesh works") {

        std::vector<float> pvd_full = fs::read_curv_data("examples/subjects_dir/subject1/surf/lh.sulc");
        fs::Label label;
        fs::read_label(&label, "examples/read_label/lh.cortex.label");

        auto result = surface.submesh_vertex(label.vertex);
        fs::Mesh patch = result.second;
        std::unordered_map<int32_t, int32_t> mapping = result.first;

        // Construct data for submesh
        std::vector<float> pvd_submesh(patch.num_vertices());
        for(size_t i=0; i<patch.num_vertices(); i++) {
            pvd_submesh[i] = pvd_full[label.vertex[i]];
        }

        REQUIRE(patch.num_vertices() == pvd_submesh.size());

        // Restore data for full mesh from submesh data. The values for vertices not in the submesh are NAN.
        std::vector<float>pvd_full_restored = fs::Mesh::curv_data_for_orig_mesh(pvd_submesh, mapping, int(surface.num_vertices()));

        REQUIRE(pvd_full_restored.size() == surface.num_vertices());

        // write pvd for submesh and mesh out for visual inspection in meshlab.
        bool do_export = false;
        if (do_export) {
            fs::write_mesh(patch, "submesh.surf");
            fs::write_curv("submesh_pvd.curv", pvd_submesh);
            fs::write_mesh(surface, "fullmesh.surf");
            fs::write_curv("fullmesh_pvd.curv", pvd_full);
            fs::write_curv("fullmesh_pvd_restored.curv", pvd_full_restored);
            std::cout << "NOTE: submesh and fullmesh files exported, please check them visually in meshlab. You can export to colored mesh with 'export_brainmesh' app from my 'cpp_geodesics' repository.\n";
            std::cout << "  ../cpp_geodesics/export_brainmesh fullmesh.surf fullmesh_pvd.curv fullmesh_origdata.ply\n";
            std::cout << "  ../cpp_geodesics/export_brainmesh fullmesh.surf fullmesh_pvd_restored.curv fullmesh_datarestored.ply\n";
            std::cout << "  ../cpp_geodesics/export_brainmesh submesh.surf submesh_pvd.curv submesh.ply\n";
            std::cout << "  meshlab fullmesh_origdata.ply\n";
            std::cout << "  meshlab fullmesh_datarestored.ply\n";
            std::cout << "  meshlab submesh.ply\n";
        }

        std::vector<bool> vertex_is_cortical = label.vert_in_label(surface.num_vertices());
        for(size_t i = 0; i < surface.num_vertices(); i++) {
            if(vertex_is_cortical[i]) {
                REQUIRE(pvd_full_restored[i] == Approx(pvd_full[i]));
            } else {
                REQUIRE(std::isnan(pvd_full_restored[i]));
            }
        }
    }
}

TEST_CASE( "Computing alternative representations for meshes works." ) {

    fs::Mesh surface = fs::Mesh::construct_cube();

    SECTION("The adjacency matrix for the mesh can be computed." ) {
        std::vector<std::vector <bool>> adjm = surface.as_adjmatrix();
        REQUIRE(adjm.size() == surface.num_vertices());
        for(size_t i = 0; i < surface.num_vertices(); i++) {
            REQUIRE(adjm[i].size() == surface.num_vertices());
        }

        // Compute min and max number of neighbors from adj matrix.
        size_t min_neigh = (size_t)-1;  // Init with size_t max value.
        size_t max_neigh = 0;
        size_t num_this_row;
        for(size_t i = 0; i < adjm.size(); i++) {
            num_this_row = 0;
            for(size_t j = 0; j < adjm[i].size(); j++) {
                if(adjm[i][j]) {
                    num_this_row++;
                }
            }
            min_neigh = num_this_row < min_neigh ? num_this_row : min_neigh;
            max_neigh = num_this_row > max_neigh ? num_this_row : max_neigh;
        }
        REQUIRE(min_neigh == 4);
        REQUIRE(max_neigh == 6);
    }

    SECTION("The adjacency list for the mesh can be computed via a matrix." ) {
        std::vector<std::vector <size_t>> adjl = surface.as_adjlist(true);
        REQUIRE(adjl.size() == surface.num_vertices());

        // Compute min and max number of neighbors from adj list.
        size_t min_neigh = (size_t)-1;  // Init with size_t max value.
        size_t max_neigh = 0;
        size_t num_this_row;
        for(size_t i = 0; i < adjl.size(); i++) {
            num_this_row = adjl[i].size();
            min_neigh = num_this_row < min_neigh ? num_this_row : min_neigh;
            max_neigh = num_this_row > max_neigh ? num_this_row : max_neigh;
        }
        REQUIRE(min_neigh == 4);
        REQUIRE(max_neigh == 6);

        for(size_t vi = 0; vi < adjl.size(); vi++) {
            REQUIRE(std::find(adjl[vi].begin(), adjl[vi].end(), vi) == adjl[vi].end()); // It must not contain the source vertex.
        }
    }

    SECTION("The adjacency list for the mesh can be computed via an edge set." ) {
        std::vector<std::vector <size_t>> adjl = surface.as_adjlist(false);
        REQUIRE(adjl.size() == surface.num_vertices());

        // Compute min and max number of neighbors from adj list.
        size_t min_neigh = (size_t)-1;  // Init with size_t max value.
        size_t max_neigh = 0;
        size_t num_this_row;
        for(size_t i = 0; i < adjl.size(); i++) {
            num_this_row = adjl[i].size();
            min_neigh = num_this_row < min_neigh ? num_this_row : min_neigh;
            max_neigh = num_this_row > max_neigh ? num_this_row : max_neigh;
        }
        REQUIRE(min_neigh == 4);
        REQUIRE(max_neigh == 6);

        for(size_t vi = 0; vi < adjl.size(); vi++) {
            REQUIRE(std::find(adjl[vi].begin(), adjl[vi].end(), vi) == adjl[vi].end()); // It must not contain the source vertex.
        }
    }

    SECTION("The edge list for the mesh can be computed." ) {
        fs::Mesh::edge_set edges = surface.as_edgelist();
        REQUIRE(edges.size() == 36);  // Each edge occurs twice, as i->j and j->i.
        std::tuple<size_t, size_t> e = std::make_tuple(0, 1);
        REQUIRE(edges.count(e));  // Make sure edge is contained.
    }
}

TEST_CASE( "A mesh neighborhood can be expanded." ) {

    fs::Mesh surface = fs::Mesh::construct_cube();

    SECTION("A mesh neighborhood can be expanded." ) {
        std::vector<std::vector <size_t>> adjl = surface.as_adjlist(true);
        std::vector<std::vector <size_t>> adjl_ext = fs::Mesh::extend_adj(adjl, 1);
        REQUIRE(adjl_ext.size() == adjl.size());
        for(size_t vi = 0; vi < adjl.size(); vi++) {
            REQUIRE(adjl_ext[vi].size() > adjl[vi].size());  // Neighborhood must have grown.
            REQUIRE(std::find(adjl_ext[vi].begin(), adjl_ext[vi].end(), vi) == adjl_ext[vi].end()); // It must not contain the source vertex.
        }
    }
}

TEST_CASE( "Smoothing per-vertex data for meshes works." ) {

    fs::Mesh surface = fs::Mesh::construct_cube();
    std::vector<float> pvd = {1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f};

    SECTION("The per-vertex data can be smoothed using a class method." ) {
        std::vector<float> pvd_smooth = surface.smooth_pvd_nn(pvd, 2);
        REQUIRE(pvd_smooth.size() == pvd.size());
    }


    SECTION("The per-vertex data can be smoothed using the static method and a pre-computed adj list." ) {
        std::vector<std::vector<size_t>> mesh_adj = surface.as_adjlist();
        std::vector<float> pvd = {1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f};
        std::vector<float> pvd_smooth = fs::Mesh::smooth_pvd_nn(mesh_adj, pvd, 2);
        REQUIRE(pvd_smooth.size() == pvd.size());
    }

    SECTION("Per-vertex data including NANs can be smoothed using the static method and a pre-computed adj list." ) {
        std::vector<std::vector<size_t>> mesh_adj = surface.as_adjlist();
        std::vector<float> pvd = {1.0f, 1.1f, 1.2f, NAN, 1.4f, 1.5f, 1.6f, 1.7f};
        std::vector<float> pvd_smooth = fs::Mesh::smooth_pvd_nn(mesh_adj, pvd, 2);
        REQUIRE(pvd_smooth.size() == pvd.size());
        REQUIRE(std::isnan(pvd_smooth[3]));
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

    SECTION("Reading OBJ files exported from Blender works." ) {
        const std::string obj_file = "examples/read_surf/blender_export_lh_white.obj";

        fs::Mesh surface_obj;
        fs::read_mesh(&surface_obj, obj_file);

        // Check vertex and face counts.
        // After deduplication by (position, texcoord, normal), vertices at texture
        // seams and sharp edges are split into separate entries.
        REQUIRE( surface_obj.vertices.size() == size_t(895296 * 3));
        REQUIRE( surface_obj.faces.size() == size_t(298484 * 3));

        // Check face vertex indices
        int vmin_entry = *std::min_element(surface_obj.faces.begin(), surface_obj.faces.end()); // could use minmax for single call
        int vmax_entry = *std::max_element(surface_obj.faces.begin(), surface_obj.faces.end());
        REQUIRE(vmin_entry == 0);
        REQUIRE(vmax_entry == 895295);

        // We do not test the coordinate range for this file, as the Blender import/export seems to have messed with
        // the coordinates.
    }

    SECTION("Reading OFF files exported from MeshLab works.") {
        const std::string off_file = "examples/read_surf/lh_white.off";

        fs::Mesh surface2;
        fs::read_mesh(&surface2, off_file);

        // Check vertex and face counts
        REQUIRE( surface.vertices.size() == size_t(149244 * 3));
        REQUIRE( surface.faces.size() == size_t(298484 * 3));

        // Check face vertex indices
        int vmin_entry = *std::min_element(surface.faces.begin(), surface.faces.end()); // could use minmax for single call
        int vmax_entry = *std::max_element(surface.faces.begin(), surface.faces.end());
        REQUIRE(vmin_entry == 0);
        REQUIRE(vmax_entry == 149243);

        // The range of vertex coordinates is correct"
        float cmin_entry = *std::min_element(surface.vertices.begin(), surface.vertices.end()); // could use minmax for single call
        float cmax_entry = *std::max_element(surface.vertices.begin(), surface.vertices.end());
        REQUIRE(cmin_entry == Approx(-108.6204));
        REQUIRE(cmax_entry == Approx(106.1743));
    }


    SECTION("Writing and re-reading OFF files works.") {

        const std::string off_file = "examples/read_surf/lh.white_exp.off";
        surface.to_off_file(off_file);

        fs::Mesh surface2;
        fs::read_mesh(&surface2, off_file);

        // Check vertex and face countss
        REQUIRE( surface2.vertices.size() == size_t(149244 * 3));
        REQUIRE( surface2.faces.size() == size_t(298484 * 3));

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

    SECTION("Writing and re-reading FreeSurfer surf files works based on vertices and faces vectors.") {

        const std::string out_surf_file = "examples/read_surf/lh.white_exported";;
        fs::write_surf(surface.vertices, surface.faces, out_surf_file);
        fs::Mesh surface2;
        fs::read_mesh(&surface2, out_surf_file);

        // Check vertex and face countss
        REQUIRE( surface2.vertices.size() == surface.vertices.size());
        REQUIRE( surface2.faces.size() == surface.faces.size());

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

    SECTION("Writing and re-reading FreeSurfer surf files works based Mesh instance.") {

        const std::string out_surf_file = "examples/read_surf/lh.white_exported";;
        fs::write_surf(surface, out_surf_file);
        fs::Mesh surface2;
        fs::read_mesh(&surface2, out_surf_file);

        // Check vertex and face countss
        REQUIRE( surface2.vertices.size() == surface.vertices.size());
        REQUIRE( surface2.faces.size() == surface.faces.size());

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

    SECTION("Constructing a cube Mesh instance works.") {

        fs::Mesh surface2 = fs::Mesh::construct_cube();

        // Check vertex and face counts
        REQUIRE( surface2.num_vertices() == 8);
        REQUIRE( surface2.num_faces() == 12);

        int min_vertex_idx = *std::min_element(surface2.faces.begin(), surface2.faces.end());
        int max_vertex_idx = *std::max_element(surface2.faces.begin(), surface2.faces.end());
        REQUIRE(min_vertex_idx == 0);
        REQUIRE(max_vertex_idx == 7);
    }

    SECTION("Constructing a 2D grid Mesh instance with custom settings works.") {

        fs::Mesh surface2 = fs::Mesh::construct_grid(3, 5, 1.0, 1.0);

        // Check vertex and face counts
        REQUIRE( surface2.num_vertices() == 15);
        REQUIRE( surface2.num_faces() == 16);

        int min_vertex_idx = *std::min_element(surface2.faces.begin(), surface2.faces.end());
        int max_vertex_idx = *std::max_element(surface2.faces.begin(), surface2.faces.end());
        REQUIRE(min_vertex_idx == 0);
        REQUIRE(max_vertex_idx == 14);
    }

    SECTION("Constructing a 2D grid Mesh instance with custom settings and nx > ny works.") {

        fs::Mesh surface2 = fs::Mesh::construct_grid(5, 3, 1.0, 1.0);

        // Check vertex and face counts
        REQUIRE( surface2.num_vertices() == 15);
        REQUIRE( surface2.num_faces() == 16);

        int min_vertex_idx = *std::min_element(surface2.faces.begin(), surface2.faces.end());
        int max_vertex_idx = *std::max_element(surface2.faces.begin(), surface2.faces.end());
        REQUIRE(min_vertex_idx == 0);
        REQUIRE(max_vertex_idx == 14);
    }

    SECTION("Constructing a 2D grid Mesh instance with default settings works.") {

        fs::Mesh surface2 = fs::Mesh::construct_grid();

        // Check vertex and face counts
        REQUIRE( surface2.num_vertices() == 20);
        REQUIRE( surface2.num_faces() == 24);

        int min_vertex_idx = *std::min_element(surface2.faces.begin(), surface2.faces.end());
        int max_vertex_idx = *std::max_element(surface2.faces.begin(), surface2.faces.end());
        REQUIRE(min_vertex_idx == 0);
        REQUIRE(max_vertex_idx == 19);
    }

    SECTION("Constructing a pyramid works.") {

        fs::Mesh surface2 = fs::Mesh::construct_pyramid();

        // Check vertex and face counts
        REQUIRE( surface2.num_vertices() == size_t(5));
        REQUIRE( surface2.num_faces() == size_t(6));

        // Note: To have a look at the constructed mesh, uncomment the next line,
        //       and then run 'meshlab pyramid.ply' from your sytem shell.
        //surface2.to_ply_file("pyramid.ply");

        int min_vertex_idx = *std::min_element(surface2.faces.begin(), surface2.faces.end());
        int max_vertex_idx = *std::max_element(surface2.faces.begin(), surface2.faces.end());
        REQUIRE(min_vertex_idx == 0);
        REQUIRE(max_vertex_idx == 4);
    }

    SECTION("PLY round-trip preserves vertex colors.") {
        fs::Mesh surface2 = fs::Mesh::construct_cube();
        // Assign a simple per-vertex color pattern.
        std::vector<uint8_t> col_in;
        for (size_t i = 0; i < surface2.num_vertices(); i++) {
            col_in.push_back(static_cast<uint8_t>(i * 31));       // red
            col_in.push_back(static_cast<uint8_t>(i * 17 + 50));  // green
            col_in.push_back(static_cast<uint8_t>(255 - i * 31)); // blue
        }

        const std::string ply_file = "examples/read_surf/cube_colors.ply";
        surface2.to_ply_file(ply_file, col_in);

        fs::Mesh surface3;
        fs::Mesh::from_ply(&surface3, ply_file);

        REQUIRE(surface3.num_vertices() == surface2.num_vertices());
        REQUIRE(surface3.vertex_colors.size() == col_in.size());
        for (size_t i = 0; i < col_in.size(); i++) {
            REQUIRE((int)surface3.vertex_colors[i] == (int)col_in[i]);
        }
    }

    SECTION("Reading a PLY file without vertex colors yields an empty vertex_colors vector.") {
        fs::Mesh surface2 = fs::Mesh::construct_cube();
        const std::string ply_file = "examples/read_surf/cube_nocolor.ply";
        surface2.to_ply_file(ply_file);

        fs::Mesh surface3;
        fs::Mesh::from_ply(&surface3, ply_file);

        REQUIRE(surface3.vertex_colors.empty());
    }

    SECTION("COFF round-trip preserves vertex colors.") {
        fs::Mesh surface2 = fs::Mesh::construct_cube();
        std::vector<uint8_t> col_in;
        for (size_t i = 0; i < surface2.num_vertices(); i++) {
            col_in.push_back(static_cast<uint8_t>(i * 30));
            col_in.push_back(static_cast<uint8_t>(128));
            col_in.push_back(static_cast<uint8_t>(255 - i * 30));
        }

        const std::string off_file = "examples/read_surf/cube_colors.off";
        surface2.to_off_file(off_file, col_in);

        fs::Mesh surface3;
        fs::Mesh::from_off(&surface3, off_file);

        REQUIRE(surface3.num_vertices() == surface2.num_vertices());
        REQUIRE(surface3.vertex_colors.size() == col_in.size());
        for (size_t i = 0; i < col_in.size(); i++) {
            REQUIRE((int)surface3.vertex_colors[i] == (int)col_in[i]);
        }
    }

    SECTION("Reading an OFF file without vertex colors yields an empty vertex_colors vector.") {
        fs::Mesh surface2 = fs::Mesh::construct_cube();
        const std::string off_file = "examples/read_surf/cube_nocolor.off";
        surface2.to_off_file(off_file);

        fs::Mesh surface3;
        fs::Mesh::from_off(&surface3, off_file);

        REQUIRE(surface3.vertex_colors.empty());
    }

    SECTION("OBJ round-trip preserves vertex colors (x y z r g b convention).") {
        fs::Mesh surface2 = fs::Mesh::construct_cube();
        std::vector<uint8_t> col_in;
        for (size_t i = 0; i < surface2.num_vertices(); i++) {
            col_in.push_back(static_cast<uint8_t>(i * 31));
            col_in.push_back(static_cast<uint8_t>(i * 17 + 50));
            col_in.push_back(static_cast<uint8_t>(255 - i * 31));
        }

        // Write an OBJ file with inline vertex colours (x y z r g b convention).
        const std::string obj_file = "examples/read_surf/cube_colors.obj";
        {
            std::ofstream ofs(obj_file);
            ofs << "# OBJ with per-vertex colours\n";
            for (size_t vi = 0; vi < surface2.num_vertices(); vi++) {
                ofs << "v " << surface2.vertices[vi * 3] << " " << surface2.vertices[vi * 3 + 1] << " " << surface2.vertices[vi * 3 + 2]
                    << " " << (col_in[vi * 3] / 255.0f)
                    << " " << (col_in[vi * 3 + 1] / 255.0f)
                    << " " << (col_in[vi * 3 + 2] / 255.0f) << "\n";
            }
            for (size_t fi = 0; fi < surface2.num_faces(); fi++) {
                ofs << "f " << (surface2.faces[fi * 3] + 1) << " " << (surface2.faces[fi * 3 + 1] + 1) << " " << (surface2.faces[fi * 3 + 2] + 1) << "\n";
            }
        }

        fs::Mesh surface3;
        fs::Mesh::from_obj(&surface3, obj_file);

        REQUIRE(surface3.num_vertices() == surface2.num_vertices());
        REQUIRE(surface3.vertex_colors.size() == col_in.size());
        // Deduplication may reorder vertices; match colors by position.
        for (size_t vi = 0; vi < surface2.num_vertices(); vi++) {
            float px = surface2.vertices[vi * 3];
            float py = surface2.vertices[vi * 3 + 1];
            float pz = surface2.vertices[vi * 3 + 2];
            bool found = false;
            for (size_t vj = 0; vj < surface3.num_vertices(); vj++) {
                if (surface3.vertices[vj * 3] == Approx(px) &&
                    surface3.vertices[vj * 3 + 1] == Approx(py) &&
                    surface3.vertices[vj * 3 + 2] == Approx(pz)) {
                    REQUIRE((int)surface3.vertex_colors[vj * 3] == (int)col_in[vi * 3]);
                    REQUIRE((int)surface3.vertex_colors[vj * 3 + 1] == (int)col_in[vi * 3 + 1]);
                    REQUIRE((int)surface3.vertex_colors[vj * 3 + 2] == (int)col_in[vi * 3 + 2]);
                    found = true;
                    break;
                }
            }
            REQUIRE(found);
        }
    }

    SECTION("Reading an OBJ file without vertex colors yields an empty vertex_colors vector.") {
        fs::Mesh surface2 = fs::Mesh::construct_cube();
        const std::string obj_file = "examples/read_surf/cube_nocolor.obj";
        surface2.to_obj_file(obj_file);

        fs::Mesh surface3;
        fs::Mesh::from_obj(&surface3, obj_file);

        REQUIRE(surface3.vertex_colors.empty());
    }

    SECTION("OBJ round-trip via to_obj_file(col) preserves vertex colors.") {
        fs::Mesh surface2 = fs::Mesh::construct_cube();
        std::vector<uint8_t> col_in;
        for (size_t i = 0; i < surface2.num_vertices(); i++) {
            col_in.push_back(static_cast<uint8_t>(i * 31));
            col_in.push_back(static_cast<uint8_t>(i * 17 + 50));
            col_in.push_back(static_cast<uint8_t>(255 - i * 31));
        }

        const std::string obj_file = "examples/read_surf/cube_colors2.obj";
        surface2.to_obj_file(obj_file, col_in);

        fs::Mesh surface3;
        fs::Mesh::from_obj(&surface3, obj_file);

        REQUIRE(surface3.num_vertices() == surface2.num_vertices());
        REQUIRE(surface3.vertex_colors.size() == col_in.size());
        // Deduplication may reorder vertices; match colors by position.
        for (size_t vi = 0; vi < surface2.num_vertices(); vi++) {
            float px = surface2.vertices[vi * 3];
            float py = surface2.vertices[vi * 3 + 1];
            float pz = surface2.vertices[vi * 3 + 2];
            bool found = false;
            for (size_t vj = 0; vj < surface3.num_vertices(); vj++) {
                if (surface3.vertices[vj * 3] == Approx(px) &&
                    surface3.vertices[vj * 3 + 1] == Approx(py) &&
                    surface3.vertices[vj * 3 + 2] == Approx(pz)) {
                    REQUIRE((int)surface3.vertex_colors[vj * 3] == (int)col_in[vi * 3]);
                    REQUIRE((int)surface3.vertex_colors[vj * 3 + 1] == (int)col_in[vi * 3 + 1]);
                    REQUIRE((int)surface3.vertex_colors[vj * 3 + 2] == (int)col_in[vi * 3 + 2]);
                    found = true;
                    break;
                }
            }
            REQUIRE(found);
        }
    }

    SECTION("write_mesh with colors writes and reads back correctly for all formats.") {
        fs::Mesh surface2 = fs::Mesh::construct_cube();
        std::vector<uint8_t> col_in;
        for (size_t i = 0; i < surface2.num_vertices(); i++) {
            col_in.push_back(static_cast<uint8_t>(i * 30));
            col_in.push_back(static_cast<uint8_t>(128));
            col_in.push_back(static_cast<uint8_t>(255 - i * 30));
        }

        // PLY
        {
            const std::string f = "examples/read_surf/wm_cc.ply";
            fs::write_mesh(surface2, f, col_in);
            fs::Mesh s;
            fs::read_mesh(&s, f);
            REQUIRE(s.vertex_colors.size() == col_in.size());
            for (size_t i = 0; i < col_in.size(); i++) {
                REQUIRE((int)s.vertex_colors[i] == (int)col_in[i]);
            }
        }
        // OBJ
        {
            const std::string f = "examples/read_surf/wm_cc.obj";
            fs::write_mesh(surface2, f, col_in);
            fs::Mesh s;
            fs::read_mesh(&s, f);
            REQUIRE(s.vertex_colors.size() == col_in.size());
            // Deduplication may reorder vertices; match colors by position.
            for (size_t vi = 0; vi < surface2.num_vertices(); vi++) {
                float px = surface2.vertices[vi * 3];
                float py = surface2.vertices[vi * 3 + 1];
                float pz = surface2.vertices[vi * 3 + 2];
                bool found = false;
                for (size_t vj = 0; vj < s.num_vertices(); vj++) {
                    if (s.vertices[vj * 3] == Approx(px) &&
                        s.vertices[vj * 3 + 1] == Approx(py) &&
                        s.vertices[vj * 3 + 2] == Approx(pz)) {
                        REQUIRE((int)s.vertex_colors[vj * 3] == (int)col_in[vi * 3]);
                        REQUIRE((int)s.vertex_colors[vj * 3 + 1] == (int)col_in[vi * 3 + 1]);
                        REQUIRE((int)s.vertex_colors[vj * 3 + 2] == (int)col_in[vi * 3 + 2]);
                        found = true;
                        break;
                    }
                }
                REQUIRE(found);
            }
        }
        // OFF
        {
            const std::string f = "examples/read_surf/wm_cc.off";
            fs::write_mesh(surface2, f, col_in);
            fs::Mesh s;
            fs::read_mesh(&s, f);
            REQUIRE(s.vertex_colors.size() == col_in.size());
            for (size_t i = 0; i < col_in.size(); i++) {
                REQUIRE((int)s.vertex_colors[i] == (int)col_in[i]);
            }
        }
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

    SECTION("A label can be constructed from vertex indices." ) {
        std::vector<int> vertices = { 0, 1, 5, 6, 7 };
        fs::Label label = fs::Label(vertices);
        REQUIRE( label.vertex.size() == 5);
        REQUIRE( label.value.size() == 5);
    }

    SECTION("A label can be constructed from vertex indices and values." ) {
        std::vector<int> vertices = { 0, 1, 5, 6, 7 };
        std::vector<float> values = { 0.0f, 0.0f, 0.1f, 0.1f, 0.5f };
        fs::Label label = fs::Label(vertices, values);
        REQUIRE( label.vertex.size() == size_t(5));
        REQUIRE( label.value.size() == size_t(5));
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

TEST_CASE( "Reading label data from a stream works" ) {
    SECTION("One can compute whether the vertices of the surface are part of the label." ) {
        std::string label_contents = "#!ascii label  , from subject  vox2ras=TkReg\n";
        label_contents += "2\n";
        label_contents += "0  -1.852  -107.983  22.770 0.0000000000\n";
        label_contents += "1  -2.139  -108.102  22.826 0.0000000000\n";
        std::istringstream label_str(label_contents);

        fs::Label label2;
        fs::read_label(&label2, &label_str);
        REQUIRE( label2.vertex.size() == 2);
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

    SECTION("One can compute the vertex colors for all vertices." ) {
        std::vector<uint8_t> vertex_colors_rgb = annot.vertex_colors();
        REQUIRE( vertex_colors_rgb.size() == surface_num_vertices * 3);

        std::vector<uint8_t> vertex_colors_rgba = annot.vertex_colors(true);
        REQUIRE( vertex_colors_rgba.size() == surface_num_vertices * 4);
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

    SECTION("The v2d function works with floats." ) {
        std::vector<float> input = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };
        std::vector<std::vector<float>> res = fs::util::v2d(input, 2);
        REQUIRE(res.size() == 3); // 3 rows, 2 columns.
        REQUIRE(res[0].size() == 2);  // 1rst row
        REQUIRE(res[1].size() == 2);  // 2nd row
        REQUIRE(res[2].size() == 2);  // 3rd row
    }

    SECTION("The v2d function works with ints and 7 elements." ) {
        std::vector<int> input = { 1, 2, 3, 4, 5, 6, 7 };
        std::vector<std::vector<int>> res = fs::util::v2d(input, 2);
        REQUIRE(res.size() == 4); // 4 rows, 2 columns.
        REQUIRE(res[0].size() == 2);  // 1rst row
        REQUIRE(res[1].size() == 2);  // 2nd row
        REQUIRE(res[2].size() == 2);  // 3rd row
        REQUIRE(res[3].size() == 1);  // 4th row

        // Check order of values.
        REQUIRE(res[0][0] == 1);
        REQUIRE(res[0][1] == 2);
        REQUIRE(res[1][0] == 3);
        REQUIRE(res[1][1] == 4);
        REQUIRE(res[2][0] == 5);
        REQUIRE(res[2][1] == 6);
        REQUIRE(res[3][0] == 7);
    }

    SECTION("The vflatten function works.") {
        std::vector<int> input1d = { 1, 2, 3, 4, 5, 6, 7 };
        std::vector<std::vector<int>> input2d = fs::util::v2d(input1d, 2);
        std::vector<int> res = fs::util::vflatten(input2d);
        REQUIRE(res.size() == input1d.size());
        for(size_t i = 0; i < res.size(); i++) {
            REQUIRE(res[i] == input1d[i]);
        }
    }
}

TEST_CASE( "The viridis colormap function works" ) {

    SECTION("The returned color vector has the right size and interleave order." ) {
        std::vector<float> data = { 0.0f, 1.0f, 2.0f };
        std::vector<uint8_t> col = fs::util::viridis(data);
        REQUIRE(col.size() == data.size() * 3);
    }

    SECTION("The colormap endpoints are correct (matplotlib viridis)." ) {
        // With an explicit [0, 1] range, the endpoints must match the official
        // viridis colors: bottom (68, 1, 84) and top (253, 231, 37).
        std::vector<float> data = { 0.0f, 1.0f };
        std::vector<uint8_t> col = fs::util::viridis(data, 0.0f, 1.0f);
        REQUIRE((int)col[0] == 68);
        REQUIRE((int)col[1] == 1);
        REQUIRE((int)col[2] == 84);
        REQUIRE((int)col[3] == 253);
        REQUIRE((int)col[4] == 231);
        REQUIRE((int)col[5] == 37);
    }

    SECTION("Auto range normalization maps the data min to the bottom and max to the top." ) {
        std::vector<float> data = { -1.0f, 2.0f };
        std::vector<uint8_t> col = fs::util::viridis(data);
        REQUIRE((int)col[0] == 68);   // data min -> bottom of colormap
        REQUIRE((int)col[1] == 1);
        REQUIRE((int)col[2] == 84);
        REQUIRE((int)col[3] == 253);  // data max -> top of colormap
        REQUIRE((int)col[4] == 231);
        REQUIRE((int)col[5] == 37);
    }

    SECTION("Out-of-range values are clamped to the colormap endpoints." ) {
        std::vector<float> data = { -1.0f, 2.0f };
        std::vector<uint8_t> col = fs::util::viridis(data, 0.0f, 1.0f);
        REQUIRE((int)col[0] == 68);   // below vmin -> bottom
        REQUIRE((int)col[3] == 253);  // above vmax -> top
    }

    SECTION("NaN input values are mapped to white (the neuroimaging default)." ) {
        std::vector<float> data = { 0.0f, NAN, 1.0f };
        std::vector<uint8_t> col = fs::util::viridis(data, 0.0f, 1.0f);
        REQUIRE((int)col[3] == 255);
        REQUIRE((int)col[4] == 255);
        REQUIRE((int)col[5] == 255);
    }

    SECTION("The NaN color is configurable." ) {
        std::vector<float> data = { 0.0f, NAN, 1.0f };
        // Black NaN.
        std::vector<uint8_t> colb = fs::util::viridis(data, 0.0f, 1.0f, 0, 0, 0);
        REQUIRE((int)colb[3] == 0);
        REQUIRE((int)colb[4] == 0);
        REQUIRE((int)colb[5] == 0);
        // Grey NaN.
        std::vector<uint8_t> colg = fs::util::viridis(data, 0.0f, 1.0f, 128, 128, 128);
        REQUIRE((int)colg[3] == 128);
        REQUIRE((int)colg[4] == 128);
        REQUIRE((int)colg[5] == 128);
        // Only override vmin to keep auto vmax but use black NaN via explicit NAN for vmax.
        std::vector<uint8_t> colr = fs::util::viridis(data, NAN, NAN, 0, 0, 0);
        REQUIRE((int)colr[3] == 0);
        REQUIRE((int)colr[4] == 0);
        REQUIRE((int)colr[5] == 0);
    }

    SECTION("An all-NaN input vector yields an all-white color vector of correct size." ) {
        std::vector<float> data = { NAN, NAN, NAN };
        std::vector<uint8_t> col = fs::util::viridis(data);
        REQUIRE(col.size() == data.size() * 3);
        for (size_t i = 0; i < col.size(); i++) {
            REQUIRE((int)col[i] == 255);
        }
    }

    SECTION("An all-NaN input with a custom NaN color yields that color for all vertices." ) {
        std::vector<float> data = { NAN, NAN, NAN };
        std::vector<uint8_t> col = fs::util::viridis(data, NAN, NAN, 0, 0, 0);
        REQUIRE(col.size() == data.size() * 3);
        for (size_t i = 0; i < col.size(); i++) {
            REQUIRE((int)col[i] == 0);
        }
    }

    SECTION("An empty input vector yields an empty color vector." ) {
        std::vector<float> data;
        std::vector<uint8_t> col = fs::util::viridis(data);
        REQUIRE(col.empty());
    }

    SECTION("Constant data is mapped to a single (middle) color." ) {
        std::vector<float> data = { 5.0f, 5.0f, 5.0f };
        std::vector<uint8_t> col = fs::util::viridis(data);
        REQUIRE(col.size() == data.size() * 3);
        REQUIRE((int)col[0] == (int)col[3]);
        REQUIRE((int)col[3] == (int)col[6]);
        // Should not be an endpoint color.
        REQUIRE(((int)col[0] != 68 || (int)col[1] != 1 || (int)col[2] != 84));
    }

    SECTION("Passing vmin greater than vmax throws std::invalid_argument." ) {
        std::vector<float> data = { 0.0f, 1.0f };
        REQUIRE_THROWS_AS( fs::util::viridis(data, 1.0f, 0.0f), std::invalid_argument );
    }

    SECTION("The colors are usable directly with fs::Mesh::to_ply()." ) {
        fs::Mesh surface = fs::Mesh::construct_cube();
        std::vector<float> data = { 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f, 0.1f, 0.9f };
        std::vector<uint8_t> col = fs::util::viridis(data, 0.0f, 1.0f);
        REQUIRE(col.size() == surface.num_vertices() * 3);
        std::string ply = surface.to_ply(col);
        REQUIRE(!ply.empty());
    }
}


// ─── NIfTI-1 Tests ─────────────────────────────────────────────────────────

#ifdef LIBFS_HAS_ZLIB
TEST_CASE("NIfTI-1: read FS hack surface file (.nii.gz)", "[nifti]")
{
    std::string nii_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "surf", "lh.thickness.nii.gz"});
    if (!fs::util::file_exists(nii_file))
    {
        std::cerr << "Cannot access test NIfTI file at '" << nii_file << "'." << std::endl;
    }

    fs::Mgh mgh;
    fs::read_nifti(&mgh, nii_file);

    SECTION("Dimensions are correct for FS hack surface")
    {
        REQUIRE(mgh.header.dim1length == 149244);
        REQUIRE(mgh.header.dim2length == 1);
        REQUIRE(mgh.header.dim3length == 1);
        REQUIRE(mgh.header.dim4length == 1);
    }

    SECTION("Data type is MRI_FLOAT")
    {
        REQUIRE(mgh.header.dtype == fs::MRI_FLOAT);
    }

    SECTION("Data vector has correct size")
    {
        REQUIRE(mgh.data.data_mri_float.size() == 149244);
    }

    SECTION("First values match expected")
    {
        REQUIRE(mgh.data.data_mri_float[0] == Approx(2.561705));
        REQUIRE(mgh.data.data_mri_float[100] == Approx(2.579938));
    }
}
#endif // LIBFS_HAS_ZLIB

#ifdef LIBFS_HAS_ZLIB
TEST_CASE("NIfTI-1: read small INT32 volume (.nii.gz)", "[nifti]")
{
    std::string nii_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "mri", "vol27int.nii.gz"});
    if (!fs::util::file_exists(nii_file))
    {
        std::cerr << "Cannot access test NIfTI file at '" << nii_file << "'." << std::endl;
    }

    fs::Mgh mgh;
    fs::read_nifti(&mgh, nii_file);

    SECTION("Dimensions are correct")
    {
        REQUIRE(mgh.header.dim1length == 3);
        REQUIRE(mgh.header.dim2length == 3);
        REQUIRE(mgh.header.dim3length == 3);
        REQUIRE(mgh.header.dim4length == 1);
    }

    SECTION("Data type is MRI_INT")
    {
        REQUIRE(mgh.header.dtype == fs::MRI_INT);
    }

    SECTION("Data vector has 27 elements")
    {
        REQUIRE(mgh.data.data_mri_int.size() == 27);
    }
}
#endif // LIBFS_HAS_ZLIB

TEST_CASE("NIfTI-1: read UINT8 brain volume (.nii, uncompressed)", "[nifti]")
{
    std::string nii_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "mri", "brain.nii"});
    if (!fs::util::file_exists(nii_file))
    {
        std::cerr << "Cannot access test NIfTI file at '" << nii_file << "'." << std::endl;
    }

    fs::Mgh mgh;
    fs::read_nifti(&mgh, nii_file);

    SECTION("Dimensions are 256x256x256")
    {
        REQUIRE(mgh.header.dim1length == 256);
        REQUIRE(mgh.header.dim2length == 256);
        REQUIRE(mgh.header.dim3length == 256);
    }

    SECTION("Data type is MRI_UCHAR")
    {
        REQUIRE(mgh.header.dtype == fs::MRI_UCHAR);
    }

    SECTION("Data vector has correct size")
    {
        REQUIRE(mgh.data.data_mri_uchar.size() == 256 * 256 * 256);
    }
}

#ifdef LIBFS_HAS_ZLIB
TEST_CASE("NIfTI-1: read UINT8 brain volume (.nii.gz)", "[nifti]")
{
    std::string nii_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "mri", "brain.nii.gz"});
    if (!fs::util::file_exists(nii_file))
    {
        std::cerr << "Cannot access test NIfTI file at '" << nii_file << "'." << std::endl;
    }

    fs::Mgh mgh;
    fs::read_nifti(&mgh, nii_file);

    SECTION("Dimensions are 256x256x256")
    {
        REQUIRE(mgh.header.dim1length == 256);
        REQUIRE(mgh.header.dim2length == 256);
        REQUIRE(mgh.header.dim3length == 256);
    }

    SECTION("Data vector has correct size")
    {
        REQUIRE(mgh.data.data_mri_uchar.size() == 256 * 256 * 256);
    }
}
#endif // LIBFS_HAS_ZLIB

#ifdef LIBFS_HAS_ZLIB
TEST_CASE("NIfTI-1: force_standard rejects FS hack", "[nifti]")
{
    std::string nii_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "surf", "lh.thickness.nii.gz"});
    if (!fs::util::file_exists(nii_file))
    {
        std::cerr << "Cannot access test NIfTI file at '" << nii_file << "'." << std::endl;
    }

    fs::Mgh mgh;
    REQUIRE_THROWS_AS(fs::read_nifti(&mgh, nii_file, true), std::runtime_error);
}
#endif // LIBFS_HAS_ZLIB

#ifdef LIBFS_HAS_ZLIB
TEST_CASE("NIfTI-1: round-trip write/read", "[nifti]")
{
    std::string nii_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "mri", "vol27int.nii.gz"});
    if (!fs::util::file_exists(nii_file))
    {
        std::cerr << "Cannot access test NIfTI file at '" << nii_file << "'." << std::endl;
    }

    fs::Mgh mgh;
    fs::read_nifti(&mgh, nii_file);

    std::string tmp_file = "libfs_test_roundtrip.nii";
    fs::write_nifti(mgh, tmp_file);

    fs::Mgh mgh2;
    fs::read_nifti(&mgh2, tmp_file);

    SECTION("Dimensions match after round-trip")
    {
        REQUIRE(mgh2.header.dim1length == mgh.header.dim1length);
        REQUIRE(mgh2.header.dim2length == mgh.header.dim2length);
        REQUIRE(mgh2.header.dim3length == mgh.header.dim3length);
        REQUIRE(mgh2.header.dim4length == mgh.header.dim4length);
    }

    SECTION("Data type matches after round-trip")
    {
        REQUIRE(mgh2.header.dtype == mgh.header.dtype);
    }

    SECTION("Data values match after round-trip")
    {
        REQUIRE(mgh2.data.data_mri_int.size() == mgh.data.data_mri_int.size());
        for (size_t i = 0; i < mgh.data.data_mri_int.size(); i++)
        {
            REQUIRE(mgh2.data.data_mri_int[i] == mgh.data.data_mri_int[i]);
        }
    }

    std::remove(tmp_file.c_str());
}
#endif // LIBFS_HAS_ZLIB

#ifdef LIBFS_HAS_ZLIB
TEST_CASE("NIfTI-1: read_desc_data with NIfTI", "[nifti]")
{
    std::string nii_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "surf", "lh.thickness.nii.gz"});
    if (!fs::util::file_exists(nii_file))
    {
        std::cerr << "Cannot access test NIfTI file at '" << nii_file << "'." << std::endl;
    }

    std::vector<float> data = fs::read_desc_data(nii_file);

    SECTION("Data size matches")
    {
        REQUIRE(data.size() == 149244);
    }

    SECTION("First values match expected")
    {
        REQUIRE(data[0] == Approx(2.561705));
        REQUIRE(data[100] == Approx(2.579938));
    }
}
#endif // LIBFS_HAS_ZLIB

TEST_CASE("NIfTI-1: reject unsupported data type", "[nifti]")
{
    // Corrupt the first file's datatype to an unsupported value and write temp.
    std::string src_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "mri", "brain.nii"});
    if (!fs::util::file_exists(src_file))
    {
        std::cerr << "Cannot access test NIfTI file at '" << src_file << "'." << std::endl;
    }

    // Read raw bytes, corrupt datatype, write temp.
    std::ifstream ifs(src_file, std::ios::binary);
    REQUIRE(ifs.is_open());
    std::vector<char> raw((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();
    REQUIRE(raw.size() >= 348);

    // datatype is at offset 70 (int16_t).  Set to DT_FLOAT64 = 64 (unsupported).
    int16_t bad_dtype = 64;
    std::memcpy(&raw[70], &bad_dtype, 2);

    std::string tmp_file = "libfs_test_baddtype.nii";
    std::ofstream ofs(tmp_file, std::ios::binary);
    REQUIRE(ofs.is_open());
    ofs.write(raw.data(), static_cast<std::streamsize>(raw.size()));
    ofs.close();

    fs::Mgh mgh;
    REQUIRE_THROWS_AS(fs::read_nifti(&mgh, tmp_file), std::runtime_error);

    std::remove(tmp_file.c_str());
}

TEST_CASE("NIfTI-1: write rejects oversized dimensions", "[nifti]")
{
    fs::Mgh mgh;
    mgh.header.dim1length = 40000; // > 32767, cannot fit in NIfTI int16
    mgh.header.dim2length = 1;
    mgh.header.dim3length = 1;
    mgh.header.dim4length = 1;
    mgh.header.dtype = fs::MRI_FLOAT;
    mgh.data.data_mri_float.resize(40000, 0.0f);

    std::string tmp_file = "libfs_test_oversized.nii";
    REQUIRE_THROWS_AS(fs::write_nifti(mgh, tmp_file), std::runtime_error);
}

#ifdef LIBFS_HAS_ZLIB
TEST_CASE("NIfTI-1: nifti_to_mgh convenience function", "[nifti]")
{
    std::string nii_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "mri", "vol27int.nii.gz"});
    if (!fs::util::file_exists(nii_file))
    {
        std::cerr << "Cannot access test NIfTI file at '" << nii_file << "'." << std::endl;
    }

    fs::Mgh mgh = fs::nifti_to_mgh(nii_file);
    REQUIRE(mgh.header.dim1length == 3);
    REQUIRE(mgh.data.data_mri_int.size() == 27);
}
#endif // LIBFS_HAS_ZLIB


// ── NIfTI-1 RAS / sform roundtrip tests ────────────────────────────────────

#ifdef LIBFS_HAS_ZLIB
TEST_CASE("NIfTI-1: RAS roundtrip MGH→NIfTI→MGH preserves spatial metadata", "[nifti][ras]")
{
    // Read a real MGH/MGZ file that has ras_good_flag=1 (brain.mgz).
    std::string mgz_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "mri", "brain.mgz"});
    if (!fs::util::file_exists(mgz_file))
    {
        std::cerr << "Cannot access brain.mgz at '" << mgz_file << "'." << std::endl;
    }

    fs::Mgh mgh_orig;
    fs::read_mgz(&mgh_orig, mgz_file);

    REQUIRE(mgh_orig.header.ras_good_flag == 1);
    REQUIRE(mgh_orig.header.Mdc.size() == 9);
    REQUIRE(mgh_orig.header.Pxyz_c.size() == 3);

    // Record original RAS values.
    float orig_xsize = mgh_orig.header.xsize;
    float orig_ysize = mgh_orig.header.ysize;
    float orig_zsize = mgh_orig.header.zsize;
    std::vector<float> orig_Mdc = mgh_orig.header.Mdc;
    std::vector<float> orig_Pxyz_c = mgh_orig.header.Pxyz_c;

    // Roundtrip through NIfTI.
    std::string tmp_file = "libfs_test_ras_roundtrip.nii";
    fs::write_nifti(mgh_orig, tmp_file);

    fs::Mgh mgh_rt;
    fs::read_nifti(&mgh_rt, tmp_file);

    SECTION("ras_good_flag is preserved")
    {
        REQUIRE(mgh_rt.header.ras_good_flag == 1);
    }

    SECTION("Voxel sizes are preserved")
    {
        REQUIRE(mgh_rt.header.xsize == Approx(orig_xsize));
        REQUIRE(mgh_rt.header.ysize == Approx(orig_ysize));
        REQUIRE(mgh_rt.header.zsize == Approx(orig_zsize));
    }

    SECTION("Mdc matrix is preserved")
    {
        REQUIRE(mgh_rt.header.Mdc.size() == 9);
        for (int i = 0; i < 9; i++)
        {
            REQUIRE(mgh_rt.header.Mdc[i] == Approx(orig_Mdc[i]));
        }
    }

    SECTION("Pxyz_c (center coordinates) is preserved")
    {
        REQUIRE(mgh_rt.header.Pxyz_c.size() == 3);
        for (int i = 0; i < 3; i++)
        {
            REQUIRE(mgh_rt.header.Pxyz_c[i] == Approx(orig_Pxyz_c[i]));
        }
    }

    std::remove(tmp_file.c_str());
}
#endif // LIBFS_HAS_ZLIB

TEST_CASE("NIfTI-1: MGH without RAS data writes sform_code=0", "[nifti][ras]")
{
    // Create an MGH without spatial metadata (ras_good_flag=0 by default).
    fs::Mgh mgh;
    mgh.header.dim1length = 4;
    mgh.header.dim2length = 4;
    mgh.header.dim3length = 4;
    mgh.header.dim4length = 1;
    mgh.header.dtype = fs::MRI_FLOAT;
    mgh.header.xsize = 2.0f;
    mgh.header.ysize = 2.0f;
    mgh.header.zsize = 2.0f;
    mgh.data.data_mri_float.resize(64, 0.0f);

    std::string tmp_file = "libfs_test_noras.nii";
    fs::write_nifti(mgh, tmp_file);

    // Read back and verify RAS is not set.
    fs::Mgh mgh_rt;
    fs::read_nifti(&mgh_rt, tmp_file);

    SECTION("ras_good_flag stays 0")
    {
        REQUIRE(mgh_rt.header.ras_good_flag == 0);
    }

    SECTION("Voxel sizes are still preserved from pixdim")
    {
        REQUIRE(mgh_rt.header.xsize == Approx(2.0f));
        REQUIRE(mgh_rt.header.ysize == Approx(2.0f));
        REQUIRE(mgh_rt.header.zsize == Approx(2.0f));
    }

    std::remove(tmp_file.c_str());
}

TEST_CASE("NIfTI-1: read NIfTI with sform_code=1 extracts full RAS metadata", "[nifti][ras]")
{
    // brain.nii has sform_code=1, qform_code=1 with valid affine data.
    std::string nii_file = fs::util::fullpath({"examples", "subjects_dir", "subject1", "mri", "brain.nii"});
    if (!fs::util::file_exists(nii_file))
    {
        std::cerr << "Cannot access brain.nii at '" << nii_file << "'." << std::endl;
    }

    fs::Mgh mgh;
    fs::read_nifti(&mgh, nii_file);

    SECTION("ras_good_flag is 1 when sform_code > 0")
    {
        REQUIRE(mgh.header.ras_good_flag == 1);
    }

    SECTION("Mdc matrix is extracted from sform")
    {
        REQUIRE(mgh.header.Mdc.size() == 9);
        // srow_x[0..2] = [-1, 0, 0]
        REQUIRE(mgh.header.Mdc[0] == Approx(-1.0f));
        REQUIRE(mgh.header.Mdc[1] == Approx(0.0f));
        REQUIRE(mgh.header.Mdc[2] == Approx(0.0f));
    }

    SECTION("Pxyz_c (center) is extracted from sform translation")
    {
        REQUIRE(mgh.header.Pxyz_c.size() == 3);
        REQUIRE(mgh.header.Pxyz_c[0] == Approx(127.5f));
        REQUIRE(mgh.header.Pxyz_c[1] == Approx(-98.6273f));
        REQUIRE(mgh.header.Pxyz_c[2] == Approx(79.0953f));
    }

    SECTION("Voxel sizes are read from pixdim")
    {
        REQUIRE(mgh.header.xsize == Approx(1.0f));
        REQUIRE(mgh.header.ysize == Approx(1.0f));
        REQUIRE(mgh.header.zsize == Approx(1.0f));
    }
}


// ============================================================================
// OBJ Loader: security and feature tests (plan #1–#14)
// ============================================================================

TEST_CASE("OBJ loader rejects null mesh pointer", "[obj][security]")
{
    std::istringstream iss("v 0 0 0\nf 1 1 1\n");
    REQUIRE_THROWS_AS(fs::Mesh::from_obj(nullptr, &iss), std::invalid_argument);
}

TEST_CASE("OBJ loader handles invalid face index via stoi", "[obj][security]")
{
    // Empty face index field
    {
        std::istringstream iss("v 0 0 0\nv 1 0 0\nv 0 1 0\nf  2 3\n");
        fs::Mesh m;
        REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
    }
    // Out-of-range integer
    {
        std::istringstream iss("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 99999999999999999999 2 3\n");
        fs::Mesh m;
        REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
    }
    // Non-numeric face index
    {
        std::istringstream iss("v 0 0 0\nv 1 0 0\nv 0 1 0\nf abc 2 3\n");
        fs::Mesh m;
        REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
    }
}

TEST_CASE("OBJ loader rejects face index 0", "[obj][security]")
{
    std::istringstream iss("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 0 2 3\n");
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
}

TEST_CASE("OBJ loader rejects out-of-range face index", "[obj][security]")
{
    // Index larger than vertex count
    {
        std::istringstream iss("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 999\n");
        fs::Mesh m;
        REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
    }
}

TEST_CASE("OBJ loader rejects negative index exceeding vertex count", "[obj][security]")
{
    // -10 with only 3 vertices → out of range
    std::istringstream iss("v 0 0 0\nv 1 0 0\nv 0 1 0\nf -10 2 3\n");
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
}

TEST_CASE("OBJ loader rejects non-finite vertex coordinates", "[obj][security]")
{
    // NaN coordinate
    {
        std::istringstream iss("v nan 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
        fs::Mesh m;
        REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
    }
    // Inf coordinate
    {
        std::istringstream iss("v inf 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
        fs::Mesh m;
        REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
    }
}

TEST_CASE("OBJ loader rejects empty file (no vertices)", "[obj]")
{
    std::istringstream iss("# just a comment\n");
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
}

TEST_CASE("OBJ loader rejects face with fewer than 3 vertices", "[obj]")
{
    std::istringstream iss("v 0 0 0\nv 1 0 0\nf 1 2\n");
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
}

TEST_CASE("OBJ loader triangulates quads via fan triangulation", "[obj][feature]")
{
    // 4 vertices, 1 quad face
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 1 1 0\n"
        "v 0 1 0\n"
        "f 1 2 3 4\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    // Quad should produce 2 triangles = 6 face indices
    REQUIRE(m.num_vertices() == 4);
    REQUIRE(m.num_faces() == 2);
    REQUIRE(m.faces.size() == 6);

    // Fan triangulation: (0,1,2), (0,2,3)
    REQUIRE(m.faces[0] == 0);
    REQUIRE(m.faces[1] == 1);
    REQUIRE(m.faces[2] == 2);
    REQUIRE(m.faces[3] == 0);
    REQUIRE(m.faces[4] == 2);
    REQUIRE(m.faces[5] == 3);
}

TEST_CASE("OBJ loader triangulates n-gons (pentagon) via fan triangulation", "[obj][feature]")
{
    // 5 vertices, 1 pentagon face
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 1 1 0\n"
        "v 0.5 1.5 0\n"
        "v 0 1 0\n"
        "f 1 2 3 4 5\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    // Pentagon should produce 3 triangles = 9 face indices
    REQUIRE(m.num_vertices() == 5);
    REQUIRE(m.num_faces() == 3);
    REQUIRE(m.faces.size() == 9);

    // Fan triangulation: (0,1,2), (0,2,3), (0,3,4)
    REQUIRE(m.faces[0] == 0); REQUIRE(m.faces[1] == 1); REQUIRE(m.faces[2] == 2);
    REQUIRE(m.faces[3] == 0); REQUIRE(m.faces[4] == 2); REQUIRE(m.faces[5] == 3);
    REQUIRE(m.faces[6] == 0); REQUIRE(m.faces[7] == 3); REQUIRE(m.faces[8] == 4);
}

TEST_CASE("OBJ loader handles negative (relative) vertex indices", "[obj][feature]")
{
    // -1 = last vertex, -2 = second-to-last
    std::string obj_data =
        "v 0 0 0\n"  // index 1
        "v 1 0 0\n"  // index 2
        "v 0 1 0\n"  // index 3
        "f -3 -2 -1\n"; // should resolve to 1 2 3 → 0-based: 0 1 2
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.num_faces() == 1);
    REQUIRE(m.faces[0] == 0);
    REQUIRE(m.faces[1] == 1);
    REQUIRE(m.faces[2] == 2);
}

TEST_CASE("OBJ loader handles mixed positive and negative indices", "[obj][feature]")
{
    // Mix of positive and negative indices in the same face
    std::string obj_data =
        "v 0 0 0\n"  // index 1
        "v 1 0 0\n"  // index 2
        "v 1 1 0\n"  // index 3
        "v 0 1 0\n"  // index 4
        "f 1 -1 2 -2\n"; // → 1, 4, 2, 3 → fan: (0,3,1), (0,1,2)
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    REQUIRE(m.num_vertices() == 4);
    REQUIRE(m.num_faces() == 2);
    // Deduplication re-indexes vertices by first encounter order:
    // face corners [0,3,1,0,1,2] → unique keys (0),(3),(1),(2) → [0,1,2,0,2,3]
    REQUIRE(m.faces[0] == 0);
    REQUIRE(m.faces[1] == 1);
    REQUIRE(m.faces[2] == 2);
    // Second triangle: 0, 2, 3
    REQUIRE(m.faces[3] == 0);
    REQUIRE(m.faces[4] == 2);
    REQUIRE(m.faces[5] == 3);
}

TEST_CASE("OBJ loader handles faces with slashes (vn/vt syntax)", "[obj][feature]")
{
    // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vt 0 0\n"
        "vt 1 0\n"
        "vt 0 1\n"
        "vn 0 0 1\n"
        "vn 0 0 1\n"
        "vn 0 0 1\n"
        "f 1/1/1 2/2/2 3/3/3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.num_faces() == 1);
    REQUIRE(m.faces[0] == 0);
    REQUIRE(m.faces[1] == 1);
    REQUIRE(m.faces[2] == 2);
}

TEST_CASE("OBJ loader handles faces with double slashes (//vn syntax)", "[obj][feature]")
{
    // f v1//vn1 v2//vn2 v3//vn3
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vn 0 0 1\n"
        "vn 0 0 1\n"
        "vn 0 0 1\n"
        "f 3//1 2//2 1//3\n"; // reversed order
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.num_faces() == 1);
    // Deduplication by (vi, -1, vni): each corner has a different normal,
    // so all three become unique vertices with sequential indices.
    REQUIRE(m.faces[0] == 0);
    REQUIRE(m.faces[1] == 1);
    REQUIRE(m.faces[2] == 2);
}

TEST_CASE("OBJ loader handles quad with slashes", "[obj][feature]")
{
    // Quad with full v/vt/vn syntax
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 1 1 0\n"
        "v 0 1 0\n"
        "vt 0 0\nvt 1 0\nvt 1 1\nvt 0 1\n"
        "vn 0 0 1\nvn 0 0 1\nvn 0 0 1\nvn 0 0 1\n"
        "f 1/1/1 2/2/2 3/3/3 4/4/4\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    REQUIRE(m.num_vertices() == 4);
    REQUIRE(m.num_faces() == 2);
    REQUIRE(m.faces[0] == 0); REQUIRE(m.faces[1] == 1); REQUIRE(m.faces[2] == 2);
    REQUIRE(m.faces[3] == 0); REQUIRE(m.faces[4] == 2); REQUIRE(m.faces[5] == 3);
}


// ============================================================================
// OFF Loader: security tests
// ============================================================================

TEST_CASE("OFF loader rejects null mesh pointer", "[off][security]")
{
    std::istringstream iss("OFF\n0 0 0\n");
    REQUIRE_THROWS_AS(fs::Mesh::from_off(nullptr, &iss), std::invalid_argument);
}

TEST_CASE("OFF loader rejects out-of-range face index", "[off][security]")
{
    // 3 vertices, face index 5 is out of range
    std::string off_data =
        "OFF\n"
        "3 1 0\n"
        "0 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "3 0 1 5\n";
    std::istringstream iss(off_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_off(&m, &iss), std::domain_error);
}

TEST_CASE("OFF loader rejects negative face index", "[off][security]")
{
    std::string off_data =
        "OFF\n"
        "3 1 0\n"
        "0 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "3 0 1 -1\n";
    std::istringstream iss(off_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_off(&m, &iss), std::domain_error);
}

TEST_CASE("OFF loader rejects non-finite vertex coordinate", "[off][security]")
{
    std::string off_data =
        "OFF\n"
        "3 1 0\n"
        "nan 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "3 0 1 2\n";
    std::istringstream iss(off_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_off(&m, &iss), std::domain_error);
}

TEST_CASE("OFF loader clamps out-of-range vertex colors", "[off][security]")
{
    // COFF with color values outside [0,255] — format is: x y z r g b a
    std::string off_data =
        "COFF\n"
        "1 0 0\n"
        "0 0 0 -10 500 300 0\n"; // r=-10,g=500,b=300 -> clamped to 0,255,255
    std::istringstream iss(off_data);
    fs::Mesh m;
    fs::Mesh::from_off(&m, &iss);

    REQUIRE(m.num_vertices() == 1);
    REQUIRE(m.vertex_colors.size() == 3);
    REQUIRE((int)m.vertex_colors[0] == 0);   // -10 clamped to 0
    REQUIRE((int)m.vertex_colors[1] == 255); // 500 clamped to 255
    REQUIRE((int)m.vertex_colors[2] == 255); // 300 clamped to 255
}

TEST_CASE("OFF loader works correctly with valid data", "[off]")
{
    std::string off_data =
        "OFF\n"
        "4 2 0\n"
        "0 0 0\n"
        "1 0 0\n"
        "1 1 0\n"
        "0 1 0\n"
        "3 0 1 2\n"
        "3 0 2 3\n";
    std::istringstream iss(off_data);
    fs::Mesh m;
    fs::Mesh::from_off(&m, &iss);

    REQUIRE(m.num_vertices() == 4);
    REQUIRE(m.num_faces() == 2);
    REQUIRE(m.faces[0] == 0); REQUIRE(m.faces[1] == 1); REQUIRE(m.faces[2] == 2);
    REQUIRE(m.faces[3] == 0); REQUIRE(m.faces[4] == 2); REQUIRE(m.faces[5] == 3);
}

TEST_CASE("OFF loader rejects non-triangular faces", "[off]")
{
    std::string off_data =
        "OFF\n"
        "4 1 0\n"
        "0 0 0\n"
        "1 0 0\n"
        "1 1 0\n"
        "0 1 0\n"
        "4 0 1 2 3\n"; // quad — not supported
    std::istringstream iss(off_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_off(&m, &iss), std::domain_error);
}

TEST_CASE("OFF loader rejects vertex/face count mismatch", "[off]")
{
    // Header says 3 vertices but only 2 provided
    std::string off_data =
        "OFF\n"
        "3 1 0\n"
        "0 0 0\n"
        "1 0 0\n"
        "3 0 1 2\n";
    std::istringstream iss(off_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_off(&m, &iss), std::domain_error);
}

TEST_CASE("OFF loader with COFF reads vertex colors", "[off]")
{
    std::string off_data =
        "COFF\n"
        "3 1 0\n"
        "0 0 0 255 0 0 255\n"
        "1 0 0 0 255 0 255\n"
        "0 1 0 0 0 255 255\n"
        "3 0 1 2\n";
    std::istringstream iss(off_data);
    fs::Mesh m;
    fs::Mesh::from_off(&m, &iss);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.vertex_colors.size() == 9);
    REQUIRE((int)m.vertex_colors[0] == 255); REQUIRE((int)m.vertex_colors[1] == 0); REQUIRE((int)m.vertex_colors[2] == 0);
    REQUIRE((int)m.vertex_colors[3] == 0); REQUIRE((int)m.vertex_colors[4] == 255); REQUIRE((int)m.vertex_colors[5] == 0);
    REQUIRE((int)m.vertex_colors[6] == 0); REQUIRE((int)m.vertex_colors[7] == 0); REQUIRE((int)m.vertex_colors[8] == 255);
}


// ============================================================================
// PLY Loader: security tests
// ============================================================================

TEST_CASE("PLY loader rejects null mesh pointer", "[ply][security]")
{
    std::istringstream iss("ply\nformat ascii 1.0\nend_header\n");
    REQUIRE_THROWS_AS(fs::Mesh::from_ply(nullptr, &iss), std::invalid_argument);
}

TEST_CASE("PLY loader rejects out-of-range face index", "[ply][security]")
{
    // 3 vertices, face index 5 is out of range
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "3 0 1 5\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_ply(&m, &iss), std::domain_error);
}

TEST_CASE("PLY loader rejects negative face index", "[ply][security]")
{
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "3 0 1 -1\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_ply(&m, &iss), std::domain_error);
}

TEST_CASE("PLY loader rejects non-finite vertex coordinate", "[ply][security]")
{
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "nan 0 0\n"
        "1 0 0\n"
        "0 1 0\n"
        "3 0 1 2\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_ply(&m, &iss), std::domain_error);
}

TEST_CASE("PLY loader clamps out-of-range vertex colors", "[ply][security]")
{
    // Colors above 255 — PLY parses into int, values are clamped.
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 1\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "property uchar red\n"
        "property uchar green\n"
        "property uchar blue\n"
        "element face 0\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0 300 500 700\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    fs::Mesh::from_ply(&m, &iss);

    REQUIRE(m.num_vertices() == 1);
    REQUIRE(m.vertex_colors.size() == 3);
    REQUIRE((int)m.vertex_colors[0] == 255); // 300 clamped to 255
    REQUIRE((int)m.vertex_colors[1] == 255); // 500 clamped to 255
    REQUIRE((int)m.vertex_colors[2] == 255); // 700 clamped to 255
}

TEST_CASE("PLY loader rejects header without end_header", "[ply][security]")
{
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 1\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 0\n"
        "property list uchar int vertex_indices\n";
    // No 'end_header' line
    std::istringstream iss(ply_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_ply(&m, &iss), std::domain_error);
}

TEST_CASE("PLY loader throws on vertex count mismatch", "[ply][security]")
{
    // Header says 3 vertices but only 2 provided
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "3 0 1 2\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_ply(&m, &iss), std::domain_error);
}

TEST_CASE("PLY loader works correctly with valid data", "[ply]")
{
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 4\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 2\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "1 1 0\n"
        "0 1 0\n"
        "3 0 1 2\n"
        "3 0 2 3\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    fs::Mesh::from_ply(&m, &iss);

    REQUIRE(m.num_vertices() == 4);
    REQUIRE(m.num_faces() == 2);
    REQUIRE(m.faces[0] == 0); REQUIRE(m.faces[1] == 1); REQUIRE(m.faces[2] == 2);
    REQUIRE(m.faces[3] == 0); REQUIRE(m.faces[4] == 2); REQUIRE(m.faces[5] == 3);
}

TEST_CASE("PLY loader reads vertex colors with explicit properties", "[ply]")
{
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 3\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "property uchar red\n"
        "property uchar green\n"
        "property uchar blue\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0 255 0 0\n"
        "1 0 0 0 255 0\n"
        "0 1 0 0 0 255\n"
        "3 0 1 2\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    fs::Mesh::from_ply(&m, &iss);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.vertex_colors.size() == 9);
    REQUIRE((int)m.vertex_colors[0] == 255); REQUIRE((int)m.vertex_colors[1] == 0); REQUIRE((int)m.vertex_colors[2] == 0);
    REQUIRE((int)m.vertex_colors[3] == 0); REQUIRE((int)m.vertex_colors[4] == 255); REQUIRE((int)m.vertex_colors[5] == 0);
    REQUIRE((int)m.vertex_colors[6] == 0); REQUIRE((int)m.vertex_colors[7] == 0); REQUIRE((int)m.vertex_colors[8] == 255);
}

TEST_CASE("PLY loader rejects non-triangular faces", "[ply]")
{
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 4\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 1\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0\n"
        "1 0 0\n"
        "1 1 0\n"
        "0 1 0\n"
        "4 0 1 2 3\n"; // quad
    std::istringstream iss(ply_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_ply(&m, &iss), std::domain_error);
}


// ============================================================================
// OBJ Loader: vertex normals and texture coordinates (plan #3, #4)
// ============================================================================

TEST_CASE("OBJ loader parses vertex normals from vn lines", "[obj][feature][normals]")
{
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vn 0 0 1\n"
        "vn 1 0 0\n"
        "vn 0 1 0\n"
        "f 1//1 2//2 3//3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.has_normals());
    REQUIRE(m.vertex_normals.size() == 9);
    // Vertex 0 gets normal from vn 1: (0,0,1)
    REQUIRE(m.vertex_normals[0] == Approx(0.0f));
    REQUIRE(m.vertex_normals[1] == Approx(0.0f));
    REQUIRE(m.vertex_normals[2] == Approx(1.0f));
    // Vertex 1 gets normal from vn 2: (1,0,0)
    REQUIRE(m.vertex_normals[3] == Approx(1.0f));
    REQUIRE(m.vertex_normals[4] == Approx(0.0f));
    REQUIRE(m.vertex_normals[5] == Approx(0.0f));
    // Vertex 2 gets normal from vn 3: (0,1,0)
    REQUIRE(m.vertex_normals[6] == Approx(0.0f));
    REQUIRE(m.vertex_normals[7] == Approx(1.0f));
    REQUIRE(m.vertex_normals[8] == Approx(0.0f));
}

TEST_CASE("OBJ loader parses texture coordinates from vt lines", "[obj][feature][texcoords]")
{
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vt 0 0\n"
        "vt 1 0\n"
        "vt 0 1\n"
        "f 1/1 2/2 3/3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.has_texcoords());
    REQUIRE(m.vertex_texcoords.size() == 6);
    // Vertex 0: texcoord from vt 1: (0,0)
    REQUIRE(m.vertex_texcoords[0] == Approx(0.0f));
    REQUIRE(m.vertex_texcoords[1] == Approx(0.0f));
    // Vertex 1: texcoord from vt 2: (1,0)
    REQUIRE(m.vertex_texcoords[2] == Approx(1.0f));
    REQUIRE(m.vertex_texcoords[3] == Approx(0.0f));
    // Vertex 2: texcoord from vt 3: (0,1)
    REQUIRE(m.vertex_texcoords[4] == Approx(0.0f));
    REQUIRE(m.vertex_texcoords[5] == Approx(1.0f));
}

TEST_CASE("OBJ loader handles both normals and texcoords in faces", "[obj][feature]")
{
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vn 0 0 1\n"
        "vn 1 0 0\n"
        "vn 0 1 0\n"
        "vt 0 0\n"
        "vt 1 0\n"
        "vt 0 1\n"
        "f 1/1/1 2/2/2 3/3/3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.has_normals());
    REQUIRE(m.has_texcoords());
}

TEST_CASE("OBJ loader throws when faces reference vn but no vn lines", "[obj][security]")
{
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "f 1//1 2//2 3//3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
}

TEST_CASE("OBJ loader throws when faces reference vt but no vt lines", "[obj][security]")
{
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "f 1/1 2/2 3/3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    REQUIRE_THROWS_AS(fs::Mesh::from_obj(&m, &iss), std::domain_error);
}

TEST_CASE("OBJ round-trip preserves vertex normals", "[obj][feature][normals]")
{
    fs::Mesh m1 = fs::Mesh::construct_cube();
    // Assign per-vertex normals
    for (size_t vi = 0; vi < m1.num_vertices(); vi++)
    {
        m1.vertex_normals.push_back(0.0f);
        m1.vertex_normals.push_back(0.0f);
        m1.vertex_normals.push_back(1.0f);
    }

    // Write and re-read
    std::string obj_str = m1.to_obj();
    std::istringstream iss(obj_str);
    fs::Mesh m2;
    fs::Mesh::from_obj(&m2, &iss);

    REQUIRE(m2.num_vertices() == m1.num_vertices());
    REQUIRE(m2.has_normals());
    REQUIRE(m2.vertex_normals.size() == m1.vertex_normals.size());
    for (size_t i = 0; i < m1.vertex_normals.size(); i++)
    {
        REQUIRE(m2.vertex_normals[i] == Approx(m1.vertex_normals[i]));
    }
}

TEST_CASE("OBJ round-trip preserves texture coordinates", "[obj][feature][texcoords]")
{
    fs::Mesh m1 = fs::Mesh::construct_cube();
    // Assign per-vertex texcoords
    for (size_t vi = 0; vi < m1.num_vertices(); vi++)
    {
        m1.vertex_texcoords.push_back(static_cast<float>(vi) * 0.1f);
        m1.vertex_texcoords.push_back(static_cast<float>(vi) * 0.1f + 0.5f);
    }

    // Write and re-read
    std::string obj_str = m1.to_obj();
    std::istringstream iss(obj_str);
    fs::Mesh m2;
    fs::Mesh::from_obj(&m2, &iss);

    REQUIRE(m2.num_vertices() == m1.num_vertices());
    REQUIRE(m2.has_texcoords());
    REQUIRE(m2.vertex_texcoords.size() == m1.vertex_texcoords.size());
    // After deduplication, vertex/texcoord ordering may differ from the original.
    // Verify that each original (position, texcoord) pair is preserved.
    for (size_t vi = 0; vi < m1.num_vertices(); vi++)
    {
        float px = m1.vertices[vi * 3];
        float py = m1.vertices[vi * 3 + 1];
        float pz = m1.vertices[vi * 3 + 2];
        float tu = m1.vertex_texcoords[vi * 2];
        float tv = m1.vertex_texcoords[vi * 2 + 1];
        bool found = false;
        for (size_t vj = 0; vj < m2.num_vertices(); vj++)
        {
            if (m2.vertices[vj * 3] == Approx(px) &&
                m2.vertices[vj * 3 + 1] == Approx(py) &&
                m2.vertices[vj * 3 + 2] == Approx(pz))
            {
                REQUIRE(m2.vertex_texcoords[vj * 2] == Approx(tu));
                REQUIRE(m2.vertex_texcoords[vj * 2 + 1] == Approx(tv));
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }
}

TEST_CASE("OBJ loader defaults to no normals/texcoords when not present", "[obj][feature]")
{
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "f 1 2 3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE_FALSE(m.has_normals());
    REQUIRE_FALSE(m.has_texcoords());
    REQUIRE(m.vertex_normals.empty());
    REQUIRE(m.vertex_texcoords.empty());
}

TEST_CASE("OBJ loader preserve_vertex_indices avoids vertex expansion", "[obj][feature][preserve_vertex_indices]")
{
    // Same position (vertex 1) is referenced with two different texture
    // coordinates across faces, which would normally cause vertex expansion.
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vt 0 0\n"
        "vt 1 0\n"
        "vt 0 1\n"
        "vt 1 1\n"
        "f 1/1 2/2 3/3\n"
        "f 1/4 2/2 3/3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss, true);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE_FALSE(m.has_texcoords());
    REQUIRE_FALSE(m.has_normals());
    REQUIRE(m.vertex_texcoords.empty());
    REQUIRE(m.vertex_normals.empty());

    // Face indices map 1:1 to file vertex positions (1-based -> 0-based).
    REQUIRE(m.faces.size() == 6);
    REQUIRE(m.faces[0] == 0);
    REQUIRE(m.faces[1] == 1);
    REQUIRE(m.faces[2] == 2);
    REQUIRE(m.faces[3] == 0);
    REQUIRE(m.faces[4] == 1);
    REQUIRE(m.faces[5] == 2);
}

TEST_CASE("OBJ loader preserve_vertex_indices keeps unreferenced vertices", "[obj][feature][preserve_vertex_indices]")
{
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "v 9 9 9\n" // never referenced by any face
        "f 1 2 3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss, true);

    REQUIRE(m.num_vertices() == 4);
    // The unreferenced 4th vertex stays at index 3 with its file coordinates.
    REQUIRE(m.vertices[9] == Approx(9.0f));
    REQUIRE(m.vertices[10] == Approx(9.0f));
    REQUIRE(m.vertices[11] == Approx(9.0f));
}

TEST_CASE("OBJ loader preserve_vertex_indices resolves negative indices", "[obj][feature][preserve_vertex_indices]")
{
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "f -3 -2 -1\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss, true);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.faces[0] == 0);
    REQUIRE(m.faces[1] == 1);
    REQUIRE(m.faces[2] == 2);
}

TEST_CASE("OBJ loader preserve_vertex_indices keeps per-vertex colors", "[obj][feature][preserve_vertex_indices]")
{
    std::string obj_data =
        "v 0 0 0 1.0 0.0 0.0\n"
        "v 1 0 0 0.0 1.0 0.0\n"
        "v 0 1 0 0.0 0.0 1.0\n"
        "f 1 2 3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss, true);

    REQUIRE(m.num_vertices() == 3);
    REQUIRE(m.vertex_colors.size() == 9);
    REQUIRE(m.vertex_colors[0] == 255);
    REQUIRE(m.vertex_colors[1] == 0);
    REQUIRE(m.vertex_colors[2] == 0);
}

TEST_CASE("OBJ loader default expands vertices at texture seams", "[obj][feature]")
{
    std::string obj_data =
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "vt 0 0\n"
        "vt 1 0\n"
        "vt 0 1\n"
        "vt 1 1\n"
        "f 1/1 2/2 3/3\n"
        "f 1/4 2/2 3/3\n";
    std::istringstream iss(obj_data);
    fs::Mesh m;
    fs::Mesh::from_obj(&m, &iss);

    // Default behavior still expands: vertex 1 occurs with two different
    // texture coordinates, producing 4 vertices instead of 3.
    REQUIRE(m.num_vertices() == 4);
}

TEST_CASE("PLY loader parses vertex normals", "[ply][feature][normals]")
{
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 2\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "property float nx\n"
        "property float ny\n"
        "property float nz\n"
        "element face 0\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0 0 0 1\n"
        "1 0 0 1 0 0\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    fs::Mesh::from_ply(&m, &iss);

    REQUIRE(m.num_vertices() == 2);
    REQUIRE(m.has_normals());
    REQUIRE(m.vertex_normals.size() == 6);
    REQUIRE(m.vertex_normals[0] == Approx(0.0f));
    REQUIRE(m.vertex_normals[1] == Approx(0.0f));
    REQUIRE(m.vertex_normals[2] == Approx(1.0f));
    REQUIRE(m.vertex_normals[3] == Approx(1.0f));
    REQUIRE(m.vertex_normals[4] == Approx(0.0f));
    REQUIRE(m.vertex_normals[5] == Approx(0.0f));
}

TEST_CASE("PLY loader parses texture coordinates", "[ply][feature][texcoords]")
{
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 2\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "property float s\n"
        "property float t\n"
        "element face 0\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0 0.0 0.0\n"
        "1 0 0 0.5 1.0\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    fs::Mesh::from_ply(&m, &iss);

    REQUIRE(m.num_vertices() == 2);
    REQUIRE(m.has_texcoords());
    REQUIRE(m.vertex_texcoords.size() == 4);
    REQUIRE(m.vertex_texcoords[0] == Approx(0.0f));
    REQUIRE(m.vertex_texcoords[1] == Approx(0.0f));
    REQUIRE(m.vertex_texcoords[2] == Approx(0.5f));
    REQUIRE(m.vertex_texcoords[3] == Approx(1.0f));
}

TEST_CASE("PLY loader handles all properties together", "[ply][feature]")
{
    // x y z nx ny nz s t red green blue
    std::string ply_data =
        "ply\n"
        "format ascii 1.0\n"
        "element vertex 1\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "property float nx\n"
        "property float ny\n"
        "property float nz\n"
        "property float s\n"
        "property float t\n"
        "property uchar red\n"
        "property uchar green\n"
        "property uchar blue\n"
        "element face 0\n"
        "property list uchar int vertex_indices\n"
        "end_header\n"
        "0 0 0 0 0 1 0.5 0.5 255 128 0\n";
    std::istringstream iss(ply_data);
    fs::Mesh m;
    fs::Mesh::from_ply(&m, &iss);

    REQUIRE(m.num_vertices() == 1);
    REQUIRE(m.has_normals());
    REQUIRE(m.has_texcoords());
    REQUIRE(m.vertex_colors.size() == 3);
    REQUIRE(m.vertex_normals[2] == Approx(1.0f));
    REQUIRE(m.vertex_texcoords[0] == Approx(0.5f));
    REQUIRE((int)m.vertex_colors[0] == 255);
    REQUIRE((int)m.vertex_colors[1] == 128);
    REQUIRE((int)m.vertex_colors[2] == 0);
}

