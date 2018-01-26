#!/bin/bash

mkdir build
cd build
cmake ..
make 
cp sim ..
cd ..
rm -rf build
