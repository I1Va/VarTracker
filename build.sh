#!/bin/bash

cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
./build/VarTracker > graph.dot
dot -Tpng graph.dot -o graph.png