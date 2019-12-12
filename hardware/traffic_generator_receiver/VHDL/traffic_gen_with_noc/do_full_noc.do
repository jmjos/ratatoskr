vlib noc_lib
vmap work noc_lib
vmap

vcom -check_synthesis rtl/packages/*.vhd
vcom -check_synthesis rtl/input_buffer/*.vhd
vcom -check_synthesis rtl/routing_algos/xy_routing.vhd
vcom -check_synthesis rtl/routing_algos/xyz_routing.vhd
vcom -check_synthesis rtl/routing_algos/zxy_routing.vhd
vcom -check_synthesis rtl/routing_algos/dxyu_routing.vhd
vcom -check_synthesis rtl/routing_algos/uxyd_routing.vhd
vcom -check_synthesis rtl/routing_algos/routing_calc.vhd
vcom -check_synthesis rtl/crossbar/*.vhd
vcom -check_synthesis rtl/arbiter/seq_packet_counter.vhd
vcom -check_synthesis rtl/arbiter/credit_count_single.vhd
vcom -check_synthesis rtl/arbiter/rr_arbiter_no_delay.vhd
vcom -check_synthesis rtl/arbiter/rr_arbiter.vhd
vcom -check_synthesis rtl/arbiter/header_arbiter_and_decoder.vhd
vcom -check_synthesis rtl/arbiter/vc_output_allocator_high_perf.vhd
vcom -check_synthesis rtl/arbiter/vc_output_allocator.vhd
vcom -check_synthesis rtl/arbiter/vc_allocator_high_perf.vhd
vcom -check_synthesis rtl/arbiter/vc_allocator.vhd
vcom -check_synthesis rtl/arbiter/switch_allocator.vhd
vcom -check_synthesis rtl/arbiter/arbiter.vhd
vcom -check_synthesis rtl/output_register/*.vhd
vcom -check_synthesis rtl/router/router_fast.vhd
vcom -check_synthesis rtl/router/router_pl.vhd
vcom -check_synthesis rtl/traffic_gen/*.vhd
vlog gate/tcbn45gsbwp.v
vlog gate/router_fast_110_T=10.0_TECH=tcbn40lptc.v
vcom -check_synthesis rtl/full_noc.vhd
vcom behav/traffic_rand_tb.vhd
vcom behav/traffic_corr_tb.vhd

vsim work.traffic_rand_tb -t ps -sdftyp /full_noc_comp/router_110=./gate/router_fast_110_T=10.0_TECH=tcbn40lptc.sdf

add wave -position insertpoint  \
sim:/traffic_rand_tb/*

run 650 us