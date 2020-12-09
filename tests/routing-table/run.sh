#!/bin/bash

cp ../../simulator/sim .

make
source source_me.sh

python3 plot_network.py

./sim
