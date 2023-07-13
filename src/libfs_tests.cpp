

#define LIBFS_DBG_WARNING

#include "libfs.h"
#include "catch.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <sstream>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdint>


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
        uint8_t min_entry = *std::min_element(mgh2.data.data_mri_short.begin(), mgh2.data.data_mri_short.end());
        uint8_t max_entry = *std::max_element(mgh2.data.data_mri_short.begin(), mgh2.data.data_mri_short.end());
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
        std::vector<float>pvd_full_restored = fs::Mesh::curv_data_for_orig_mesh(pvd_submesh, mapping, surface.num_vertices());

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
    std::vector<float> pvd = {1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7};

    SECTION("The per-vertex data can be smoothed using a class method." ) {
        std::vector<float> pvd_smooth = surface.smooth_pvd_nn(pvd, 2);
        REQUIRE(pvd_smooth.size() == pvd.size());
    }


    SECTION("The per-vertex data can be smoothed using the static method and a pre-computed adj list." ) {
        std::vector<std::vector<size_t>> mesh_adj = surface.as_adjlist();
        std::vector<float> pvd = {1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7};
        std::vector<float> pvd_smooth = fs::Mesh::smooth_pvd_nn(mesh_adj, pvd, 2);
        REQUIRE(pvd_smooth.size() == pvd.size());
    }

    SECTION("Per-vertex data including NANs can be smoothed using the static method and a pre-computed adj list." ) {
        std::vector<std::vector<size_t>> mesh_adj = surface.as_adjlist();
        std::vector<float> pvd = {1.0, 1.1, 1.2, NAN, 1.4, 1.5, 1.6, 1.7};
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

        // Check vertex and face counts
        REQUIRE( surface_obj.vertices.size() == size_t(149244 * 3));
        REQUIRE( surface_obj.faces.size() == size_t(298484 * 3));

        // Check face vertex indices
        int vmin_entry = *std::min_element(surface_obj.faces.begin(), surface_obj.faces.end()); // could use minmax for single call
        int vmax_entry = *std::max_element(surface_obj.faces.begin(), surface_obj.faces.end());
        REQUIRE(vmin_entry == 0);
        REQUIRE(vmax_entry == 149243);

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
        std::vector<float> values = { 0.0, 0.0, 0.1, 0.1, 0.5 };
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

