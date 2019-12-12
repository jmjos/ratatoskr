Examining the introduced "Traffic Generator" with a 3D Network-On-Chip developed at the "University of Bremen".

The network characteristics:
- size is 4x4x3
- Each router has 4 VCs in each port
- Channel depth for all VCs is 4
- Header is included as a flit
- Router at the position x=y=1 and z=0 is synthesized with 10 ns clock and the verilog code and standard delay file (SDF) is stored.


RTL Folder: includes all needed VHDL files
Behav Folder: includes packet injector and testbenchs for full_noc
gate Folder: includes synthesized router_110 in verilog and its related library and delay file (SDF)
reports Folder: generated input and output data for noc_with_pe will be stored in this folder

-------------------------------

For the random file test:

vcom behav/traffic_rand_tb.vhd
vsim work.traffic_rand_tb -t ps -sdftyp /full_noc_comp/router_110=./gate/router_fast_110_T=10.0_TECH=tcbn40lptc.sdf
add wave -position insertpoint  \
sim:/traffic_rand_tb/*

For the correlated file test:

vcom behav/traffic_corr_tb.vhd
vsim work.traffic_corr_tb -t ps -sdftyp /full_noc_comp/router_110=./gate/router_fast_110_T=10.0_TECH=tcbn40lptc.sdf
add wave -position insertpoint  \
sim:/traffic_corr_tb/*
