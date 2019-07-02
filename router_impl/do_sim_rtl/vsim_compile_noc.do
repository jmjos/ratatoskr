vlib work


vcom -2002 -check_synthesis -lint -work ./work/ ../rtl/packages/*.vhd  
vcom -2002 -check_synthesis -lint  -work ./work/ ../rtl/crossbar/*.vhd
vcom -2002 -check_synthesis -lint  -work ./work/ ../rtl/input_buffer/*.vhd
vcom -2002 -check_synthesis -lint  -work ./work/ ../rtl/output_register/*.vhd
vcom -2002 -check_synthesis -lint  -work ./work/ ../rtl/routing_algos/*routing.vhd
vcom -2002 -check_synthesis -lint  -work ./work/ ../rtl/routing_algos/routing_calc.vhd
vcom -2002 -check_synthesis -lint  -work ./work/ ../rtl/arbiter/rr_arbiter*.vhd
vcom -2002 -check_synthesis -lint  -work ./work/ ../rtl/arbiter/seq_packet_counter.vhd
vcom -2002 -check_synthesis -lint  -work ./work/ ../rtl/arbiter/vc_output_allocator.vhd ../rtl/arbiter/header_arbiter_and_decoder.vhd

vcom -2002 -check_synthesis  -work ./work/ ../rtl/arbiter/vc_allocator.vhd
vcom -2002 -check_synthesis  -work ./work/ ../rtl/arbiter/vc_output_allocator_high_perf.vhd
vcom -2002 -check_synthesis  -work ./work/ ../rtl/arbiter/vc_allocator_high_perf.vhd
vcom -2002 -check_synthesis  -work ./work/ ../rtl/arbiter/credit_count_single.vhd
vcom -2002 -check_synthesis  -work ./work/ ../rtl/arbiter/switch_allocator.vhd
vcom -2002 -check_synthesis  -work ./work/ ../rtl/arbiter/arbiter.vhd
vcom -2002 -check_synthesis  -work ./work/ ../rtl/router/router.vhd ../rtl/router/router_pl.vhd

quit