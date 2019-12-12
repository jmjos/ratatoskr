-------------------------------------------------------------------------------
-- Title      : Test pattern receiver
-- Project    : NoC testbench generator
-------------------------------------------------------------------------------
-- File       : traffic_corr_tb.vhd
-- Author     : Seyed Nima Omidsajedi  <nima@omidsajedi.com>
-- Company    : University of Bremen
-------------------------------------------------------------------------------
-- Copyright (c) 2019 
-------------------------------------------------------------------------------
-- Vesion     : 1.7.0
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_textio.all;
use std.textio.all;
use work.NOC_3D_PACKAGE.all;

entity traffic_corr_tb is

end entity;

architecture behave of traffic_corr_tb is

constant cnt_flit_width          : positive := flit_size;
constant cnt_router_credit       : integer := 4;
constant cnt_srl_fifo_depth      : integer := 200;
constant cnt_rec_time_text       : string := "data/pic/receive_time_noc.txt";
constant cnt_rec_data_text       : string := "data/pic/receive_data_noc.txt";
constant cnt_inj_time_text       : string := "data/pic/injection_time.txt";
constant cnt_packet_length_text  : string := "data/pic/packet_header_length.txt";
constant cnt_image_2_flits_text  : string := "data/pic/data_header.txt";
constant cnt_inj_time_2_noc_text : string := "data/pic/inj_time_2_noc.txt";

-------------------------------------------------------------------

signal clk : std_logic := '0';
signal rst : std_logic := RST_LVL;
signal local_rx : flit_vector(48-1 downto 0) := (others => (others => '0'));
signal local_vc_write_rx: std_logic_vector(192-1 downto 0) := (others => '0');
signal local_incr_rx_vec: std_logic_vector(192-1 downto 0) := (others => '0');
signal local_tx : flit_vector(48-1 downto 0);
signal local_vc_write_tx : std_logic_vector(192-1 downto 0);
signal local_incr_tx_vec : std_logic_vector(192-1 downto 0);

-------------------------------------------------------------------
--------------------- Component declaration -----------------------

-- Traffic Receiver
component traffic_rec is
generic(
	flit_width : positive := cnt_flit_width;
	rec_time_text : string := cnt_rec_time_text;
	rec_data_text : string := cnt_rec_data_text
	);
port(
	clk, rst: in std_logic;
	valid: in std_logic;
	incr: out std_logic;
	data_in: in flit := (others => '0')
	);
end component traffic_rec;

-- NoC
component full_noc is
port(
  clk, rst          : in  std_logic;
  local_rx          : in  flit_vector(48-1 downto 0);
  local_vc_write_rx : in  std_logic_vector(192-1 downto 0);
  local_incr_rx_vec : in  std_logic_vector(192-1 downto 0);
  local_tx          : out flit_vector(48-1 downto 0);
  local_vc_write_tx : out std_logic_vector(192-1 downto 0);
  local_incr_tx_vec : out std_logic_vector(192-1 downto 0)
  );
end component full_noc;

-- Traffic Generator
component traffic_gen is 
generic(
	flit_width          : positive := cnt_flit_width;
	router_credit       : integer  := cnt_router_credit;
	srl_fifo_depth      : integer  := cnt_srl_fifo_depth;
	inj_time_text       : string   := cnt_inj_time_text;
	packet_length_text  : string   := cnt_packet_length_text;
	image_2_flits_text  : string   := cnt_image_2_flits_text;
	inj_time_2_noc_text : string   := cnt_inj_time_2_noc_text
	);
port(
	clk, rst : in std_logic;
	valid    : out std_logic;
	incr     : in std_logic;
	data_out : out flit
	);
end component traffic_gen;

begin

-------------------------------------------------------------------
------------------- Component instantiations ----------------------

traffic_gen_comp_1: entity work.traffic_gen
	generic map(
		flit_width          => cnt_flit_width,
		router_credit       => cnt_router_credit,
		srl_fifo_depth      => cnt_srl_fifo_depth,
		inj_time_text       => cnt_inj_time_text,
		packet_length_text  => cnt_packet_length_text,
		image_2_flits_text  => cnt_image_2_flits_text,
		inj_time_2_noc_text => cnt_inj_time_2_noc_text
		)
	port map(
		clk      => clk,
		rst      => rst,
		valid    => local_vc_write_rx(36),
		incr     => local_incr_tx_vec(36),
		data_out => local_rx(9)
		);

full_noc_comp: entity work.full_noc
	port map(
		clk      => clk,
		rst      => rst,
		local_rx => local_rx,
		local_vc_write_rx => local_vc_write_rx,
		local_incr_rx_vec => local_incr_rx_vec,
		local_tx => local_tx,
		local_vc_write_tx => local_vc_write_tx,
		local_incr_tx_vec => local_incr_tx_vec
		);

traffic_rec_comp_1: entity work.traffic_rec
	generic map(
		flit_width     => cnt_flit_width,
		rec_time_text  => cnt_rec_time_text,
		rec_data_text  => cnt_rec_data_text
	)
	port map(
		clk      => clk,
		rst      => rst,
		valid    => local_vc_write_tx(4),
		incr     => local_incr_rx_vec(4),
		data_in  => local_tx(1)
		);

-------------------------------------------------------------------
----------------------RST & CLK generation-------------------------

rst_gen: process
begin
rst <= RST_LVL;
	wait for (clk_period * 2);
rst <= not(RST_LVL);
	wait;
end process;

clk_gen:process
begin
	clk <= '1';
		wait for (clk_period / 2);
	clk <= '0';
		wait for (clk_period / 2);
end process;

--------------------------------------------------------------------
-------------------------------------------------------------------

end architecture;

