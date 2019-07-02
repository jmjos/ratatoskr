-------------------------------------------------------------------------------
-- Title      : Testbench for design "rr_arbiter_no_delay"
-- Project    : 
-------------------------------------------------------------------------------
-- File       : rr_arbiter_no_delay_tb.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-11-05
-- Last update: 2018-11-05
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-11-05  1.0      bamberg	Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use work.NOC_3D_PACKAGE.all;

-------------------------------------------------------------------------------

entity rr_arbiter_no_delay_tb is

end entity rr_arbiter_no_delay_tb;

-------------------------------------------------------------------------------

architecture testbench of rr_arbiter_no_delay_tb is
  --
  constant clock_period : time := 10 ns;
  
  -- component generics
  constant CNT : integer := 4;

  -- component ports
  signal rst   : std_logic := RST_LVL;
  signal req   : std_logic_vector(CNT-1 downto 0) := (others => '0');
  signal ack   : std_logic := '0';
  signal grant : std_logic_vector(CNT-1 downto 0);

  -- clock
  signal Clk : std_logic := '1';

begin  -- architecture testbench

  -- component instantiation
  DUT: entity work.rr_arbiter_no_delay
    generic map (
      CNT => CNT)
    port map (
      clk   => Clk,
      rst   => rst,
      req   => req,
      ack   => ack,
      grant => grant);

  -- clock generation
  Clk <= not Clk after clock_period/2;

  -- rst generation
  process
  begin
    wait until falling_edge(Clk);
    rst <= not rst;
    wait;
  end process;

--   waveform generation
  WaveGen_Proc: process
  begin
    wait for clock_period/2;
    req(0) <= '1';
    wait for clock_period;
    req(2) <= '1';
    wait for clock_period;
    ack <= '1';
    wait for clock_period;
    ack <= '0';
    wait for clock_period;
    ack <= '1';
    wait for clock_period;
    ack <= '0';
    wait for clock_period;
    req <= (others => '0');
    wait for clock_period;
    req(0) <= '1';
    req(1) <= '1';
    req(3) <= '1';
    wait for clock_period;
    ack <= '1';
    wait for clock_period/2;
    --req(3) <= '0';
    wait for clock_period;
      ack <= '1';
    wait for clock_period/2;
    req <= (others => '1');
    wait for 8*clock_period;
    req(1) <= '0';
    wait for 8*clock_period;
    assert false report "NORMAL COMPLETION of SIMULATION" severity failure;
end process WaveGen_Proc;

  

end architecture testbench;

-------------------------------------------------------------------------------

configuration rr_arbiter_no_delay_tb_testbench_cfg of rr_arbiter_no_delay_tb is
  for testbench
  end for;
end rr_arbiter_no_delay_tb_testbench_cfg;

-------------------------------------------------------------------------------
