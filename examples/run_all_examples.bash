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

echo "$apptag ========== Running curv example... =========="
cd read_curv && g++ -I../../include/ read_curv.cpp -o read_curv && ./read_curv && cd ..

echo "$apptag ========== Running label example... =========="
cd read_label && g++ -I../../include/ read_label.cpp -o read_label && ./read_label && cd ..

echo "$apptag ========== Running MGH example... =========="
cd read_mgh && g++ -I../../include/ read_mgh.cpp -o read_mgh && ./read_mgh && cd ..

echo "$apptag ========== Running surf example... =========="
cd read_surf && g++ -I../../include/ read_surf.cpp -o read_surf && ./read_surf && cd ..



