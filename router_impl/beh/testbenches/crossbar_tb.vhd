-------------------------------------------------------------------------------
-- Titleb      : Testbench for design "crossbar"
-- Project    : 
-------------------------------------------------------------------------------
-- File       : crossbar_tb.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-11-02
-- Last update: 2018-11-15
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-11-02  1.0      bamberg Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.NOC_3D_PACKAGE.all;
use work.TURNS_3D_NOC.all;

-------------------------------------------------------------------------------

entity crossbar_tb is

end entity crossbar_tb;

-------------------------------------------------------------------------------

architecture testbench of crossbar_tb is

  -- component generics
  constant port_num   : positive      := 7;
  constant port_exist : integer_vec := (0, 1, 2, 3, 4, 5, 6);
  constant routing    : string        := "DXYU";
  --constant routing    : string        := "XXX";  --non existing routing to check
    --ref

  -- component ports
  signal crossbar_in : flit_vector(port_num-1 downto 0);
  signal crossbar_ctrl : std_logic_vector(
    port_num*bit_width(port_num-1)-1 downto 0) := (others => '0');
  signal crossbar_out, crossbar_out_ref : flit_vector(port_num-1 downto 0);
  signal crossbar_out_0, crossbar_out_1, crossbar_out_2, crossbar_out_3, crossbar_out_4,
    crossbar_out_5, crossbar_out_6 : std_logic_vector(flit_size-1 downto 0);
  signal crossbar_out_ref_0, crossbar_out_ref_1, crossbar_out_ref_2, crossbar_out_ref_3,
    crossbar_out_ref_4, crossbar_out_ref_5, crossbar_out_ref_6 :
    std_logic_vector(flit_size-1 downto 0);


  -- clock
  signal Clk : std_logic := '1';

begin  -- architecture testbench

  -- component instantiation
  DUT1 : entity work.crossbar
    generic map (
      port_num   => port_num,
      port_exist => port_exist,
      rout_algo  => routing)
    port map (
      crossbar_in   => crossbar_in,
      crossbar_ctrl => crossbar_ctrl,
      crossbar_out  => crossbar_out);

  REF_DUT : entity work.crossbar_full
    generic map (
      port_num => port_num)
    port map (
      crossbar_in   => crossbar_in,
      crossbar_ctrl => crossbar_ctrl,
      crossbar_out  => crossbar_out_ref);

  -- clock generation
  Clk <= not Clk after 10 ns;

  -- constant input data for the data_input in accordance with the port_num
  CONST_DATA_IN : process
  begin
    for i in 0 to port_num-1 loop
      crossbar_in(i) <= std_logic_vector(to_unsigned(i, crossbar_in(i)'length));
    end loop;
    wait;
  end process CONST_DATA_IN;

  -- toggle the select ports every 1 ns
  SEL_SIGNALS : process
    variable selected_input : unsigned(bit_width(port_num-1)-1 downto 0) := (others => '0');
    variable port_sel_width : positive                                   := bit_width(port_num-1);
  begin
    wait for 1 ns;
    selected_input := selected_input + 1;
    for i in 0 to port_num-1 loop
      crossbar_ctrl((i+1)*port_sel_width-1 downto i*port_sel_width)
        <= std_logic_vector(selected_input);
    end loop;
  end process;

  -- check end condition
  CHECK_END : process
  begin
    wait for 5.5 ns;
    assert false report "NORMAL COMPLETION OF SIMULATION"
      severity failure;
  end process;

  -- map to std_logic_vector to be dumpable in vcd
  crossbar_out_6 <= crossbar_out(6);
  crossbar_out_5 <= crossbar_out(5);
  crossbar_out_4 <= crossbar_out(4);
  crossbar_out_3 <= crossbar_out(3);
  crossbar_out_2 <= crossbar_out(2);
  crossbar_out_1 <= crossbar_out(1);
  crossbar_out_0 <= crossbar_out(0);

  crossbar_out_ref_6 <= crossbar_out_ref(6);
  crossbar_out_ref_5 <= crossbar_out_ref(5);
  crossbar_out_ref_4 <= crossbar_out_ref(4);
  crossbar_out_ref_3 <= crossbar_out_ref(3);
  crossbar_out_ref_2 <= crossbar_out_ref(2);
  crossbar_out_ref_1 <= crossbar_out_ref(1);
  crossbar_out_ref_0 <= crossbar_out_ref(0);
end architecture testbench;


-------------------------------------------------------------------------------

configuration crossbar_tb_testbench_cfg of crossbar_tb is
  for testbench
  end for;
end crossbar_tb_testbench_cfg;

-------------------------------------------------------------------------------
