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

echo "$apptag ========== Running annot example... =========="
cd $bd/read_annot && g++ -I../../include/ read_annot.cpp -o read_annot && ./read_annot && cd ..

echo "$apptag ========== Running curv example... =========="
cd $bd/read_curv && g++ -I../../include/ read_curv.cpp -o read_curv && ./read_curv && cd ..

echo "$apptag ========== Running label example... =========="
cd $bd/read_label && g++ -I../../include/ read_label.cpp -o read_label && ./read_label && cd ..

echo "$apptag ========== Running MGH example... =========="
cd $bd/read_mgh && g++ -I../../include/ read_mgh.cpp -o read_mgh && ./read_mgh && cd ..

echo "$apptag ========== Running MGZ example (requires zlib)... =========="
cd $bd/read_mgz && g++ -I../../include/ -I./include_zstr/ read_mgz.cpp -o read_mgz -lz && ./read_mgz && cd ..

echo "$apptag ========== Running surf example... =========="
cd $bd/read_surf && g++ -I../../include/ read_surf.cpp -o read_surf && ./read_surf && cd ..

echo "$apptag ========== Running descriptor info example... =========="
cd $bd/descriptor_info && g++ -I../../include/ descriptor_info.cpp -o descriptor_info && ./descriptor_info && cd ..



