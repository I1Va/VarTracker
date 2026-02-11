#!/bin/bash
g++ -std=c++17 -O0 -fno-elide-constructors -I ./../../inc main.cpp -o ./build/main.out 
./build/main.out
