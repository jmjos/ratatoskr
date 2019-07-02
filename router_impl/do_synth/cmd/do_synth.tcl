# Load local library
# source cmd/lib_synth.tcl

set hdliset hdlin_keep_signal_name "none"rep
set_clock_gating_style -sequential_cell none -minimum_bitwidth 3
do_synth_def router_pl 1000.0 asap7sram_tc clk rst
remove_unconnected_ports -blast_buses [find -hierarchy cell {"*"}]
# ungroup [find -hierarchy cell {"*"}]
#compile

compile_ultra -gate_clock
remove_unconnected_ports -blast_buses [find -hierarchy cell {"*"}]
compile_ultra -gate_clock
