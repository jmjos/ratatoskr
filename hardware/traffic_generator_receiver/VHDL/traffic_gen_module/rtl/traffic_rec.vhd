-------------------------------------------------------------------------------
-- Title      : Test pattern receiver
-- Project    : NoC testbench generator
-------------------------------------------------------------------------------
-- File       : traffic_rec.vhd
-- Author     : Seyed Nima Omidsajedi  <nima@omidsajedi.com>
-- Company    : University of Bremen
-------------------------------------------------------------------------------
-- Copyright (c) 2019 
-------------------------------------------------------------------------------
-- Vesion     : 1.9.0
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_textio.all;
use std.textio.all;
use work.NOC_3D_PACKAGE.all;

entity traffic_rec is
generic(
	flit_width : positive := flit_size;
	rec_time_text : string := "receive_time_noc.txt";
	rec_data_text : string := "receive_data_noc.txt"
	);
port(
	clk, rst : in std_logic := '0';
	valid    : in std_logic := '0';
	incr     : out std_logic := '0';
	data_in  : in flit := (others => '0')
	);
end entity;

architecture behave of traffic_rec is

-- Used text files
file rec_time : text open write_mode is rec_time_text;
file rec_data : text open write_mode is rec_data_text;

begin

-- Set increment
incr <= valid;

-------------------------------------------------------------------
--------------------------- write Process -------------------------

write_data: process(clk, rst)
	variable rowOut: line;
	variable data_time: time := 0 ns;
begin
	if clk = '1' and clk'event and rst = not(RST_LVL) then		
		if (valid = '1') then
			write(rowOut, data_in);
			writeline(rec_data, rowOut);
			data_time := now - clk_period;
			write(rowOut, data_time);
			writeline(rec_time, rowOut);
		end if;	
	end if;
end process;

--------------------------------------------------------------------
-------------------------------------------------------------------

end architecture;
