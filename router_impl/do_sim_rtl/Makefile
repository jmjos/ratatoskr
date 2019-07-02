
.PHONY: all run clean

all: work run


work : ../rtl/*/*.vhd ../beh/*/*.vhd
	vsim -c -do vsim_compile_noc.do
	vsim -c -do vsim_compile_testbench.do
	touch work


run :
	vsim -do vsim_run_testbench_router_pl.do	

clean :
	rm -rf ./work
	rm -f ./transcript
	rm -f ./vsim.wlf
	rm -f ./*~
