#!/bin/sh

mkdir build
cd build
cmake ../../.. -DDEFINE_ENABLE_NETRACE=ON
make -j
cd ..
cp build/sim .
wget https://www.cs.utexas.edu/~netrace/download/netrace-1.0.tar.bz2
tar -xf netrace-1.0.tar.bz2
mv netrace-1.0/testraces/example.tra.bz2 config/
cp config/ntConfig.xml config/config.xml
./sim
