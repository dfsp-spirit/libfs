#!/bin/bash

apptag="[RUNEXPL]"

if [ ! -d "read_mgh" ]; then
    if [ -d "examples" ]; then
        cd examples/
    else
        echo "Invalid working directory, call this from the examples directory or from the repo root."
        exit 0
    fi
fi

bd=$(pwd)

# If a directory name is given as first argument, run only that example.
# Otherwise run all examples.
if [ -n "$1" ]; then
    target="$1"
    if [ ! -d "$bd/$target" ]; then
        echo "$apptag ERROR: example directory '$target' not found in $bd"
        echo "$apptag Available examples:"
        ls -d "$bd"/*/ | while read d; do basename "$d"; done
        exit 1
    fi
    echo "$apptag Running single example: $target"
fi

run_example() {
    local name="$1"
    if [ -n "$target" ] && [ "$name" != "$target" ]; then
        return 0
    fi
    shift
    local cmd="$*"
    echo "$apptag ========== Running $name example... =========="
    (cd "$bd/$name" && eval "$cmd") || exit $?
}

run_example read_annot      "g++ -std=c++11 -I../../include/ read_annot.cpp -o read_annot && ./read_annot"
run_example read_curv       "g++ -std=c++11 -I../../include/ read_curv.cpp -o read_curv && ./read_curv"
run_example read_label      "g++ -std=c++11 -I../../include/ read_label.cpp -o read_label && ./read_label"
run_example read_mgh        "g++ -std=c++11 -I../../include/ read_mgh.cpp -o read_mgh && ./read_mgh"
run_example read_mgz        "g++ -std=c++11 -I../../include/ -I./include_zstr/ read_mgz.cpp -o read_mgz -lz && ./read_mgz"
run_example read_mgz_native "g++ -std=c++11 -I../../include/ read_mgz_native.cpp -o read_mgz_native -lz && ./read_mgz_native"
run_example read_surf       "g++ -std=c++11 -I../../include/ read_surf.cpp -o read_surf && ./read_surf"
run_example write_annot     "g++ -std=c++11 -I../../include/ write_annot.cpp -o write_annot && ./write_annot"
run_example write_subjectsfile "g++ -std=c++11 -I../../include/ write_subjectsfile.cpp -o write_subjectsfile && ./write_subjectsfile"
run_example descriptor_info "g++ -std=c++11 -I../../include/ descriptor_info.cpp -o descriptor_info && ./descriptor_info"
run_example vertex_color_export "g++ -std=c++11 -I../../include/ vertex_color_export.cpp -o vertex_color_export && ./vertex_color_export"



