// The main for the libfs demo app.

#include "libfs.h"

#include <string>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    std::string subject_id;
    std::string subjects_dir;

    if(argc < 2 || argc > 3) {
        std::cout << "===" << argv[0] << " -- libfs demo app -- read various FreeSurfer files from recon-all output dir ===\n";
        std::cout << "Usage: " << argv[0] << " <subject_id> [subjects_dir]>\n";
        std::cout << "   <subject_id>    : str, the subject identifier of the subject that was pre-processed with FreeSurfer's recon all.\n";
        std::cout << "   <subjects_dir>  : str, the path to the SUBJECTS_DIR, the recon-all output dir for all subjects.\n";
        std::cout << "                     Defaults to env variable SUBJECTS_DIR if omitted (in which case that env var must exist).\n";
        std::cout << "Example: If you have set subject_id to 'subject1' and subjects_dir to './mystudy', this program expects\n";
        std::cout << "          the following file to exist: './mystudy/subject1/surf/lh.white'.\n";
        exit(1);
    }
    subject_id = argv[1];
    if(argc == 3) {
        subjects_dir = argv[2];
    } else {
        if(const char* env_p = std::getenv("SUBJECTS_DIR")) {
            subjects_dir = std::string(env_p);
        } else {
            std::cerr << "Environment variable 'SUBJECTS_DIR' not set, must specify 'subjects_dir' command line argument in that case.\n";
        }
    }
    std::string sdd = fs::util::fullpath({subjects_dir, subject_id});  // 'sdd' stands for 'subject data dir'
    std::cout << "Using subject data from '" << sdd << "'.\n";

    std::vector<std::string> hemis = { "lh", "rh" };
    std::vector<std::string> surfaces = { "white", "pial" };

    std::string surf_file;
    fs::Mesh mesh;

    for (std::string surf : surfaces) {
        for (std::string hemi : hemis) {
            surf_file = fs::util::fullpath({sdd, "surf", (hemi + "." + surf)});
            if(fs::util::file_exists(surf_file)) {
                fs::read_surf(&mesh, surf_file);
                std::cout << "Found " << hemi << " mesh of " << surf << " surface containing " << mesh.num_vertices() << " vertices and "<< mesh.num_faces() << " faces.\n";
            } else {
                std::cout << "Missing " << hemi << " mesh of " << surf << " surface at '" << surf_file << "'.\n";
            }
        }
    }

}