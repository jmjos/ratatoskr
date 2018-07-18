#!/bin/bash

mkdir build
cd build
cmake ..
make -j
cp sim ..
cd ..
rm -rf build
