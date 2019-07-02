-------------------------------------------------------------------------------
-- Title      : Testbench for design "output_register"
-- Project    : 
-------------------------------------------------------------------------------
-- File       : output_register_tb.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-11-14
-- Last update: 2018-11-23
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-11-14  1.0      bamberg Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use work.NOC_3D_PACKAGE.all;
-------------------------------------------------------------------------------

entity output_register_tb is

end entity output_register_tb;

-------------------------------------------------------------------------------




architecture testbench of output_register_tb is

  -- component generics
  constant vc_num     : positive := 2;
  constant vc_num_out : positive := 2;

  -- component ports
  signal rst     : std_logic;
  signal data_tx : flit;
  signal vc_write_tx : std_logic_vector(
    vc_num_out-1 downto 0);
  signal incr_tx            : std_logic_vector(vc_num-1 downto 0);
  signal data_tx_pl         : std_logic_vector(flit_size-1 downto 0);
  signal vc_write_tx_pl : std_logic_vector(vc_num_out-1 downto 0);
  signal incr_tx_pl         : std_logic_vector(vc_num-1 downto 0);

  -- clock
  signal Clk : std_logic := '1';

begin  -- architecture testbench

  -- component instantiation
  DUT : entity work.output_register
    generic map (
      vc_num     => vc_num,
      vc_num_out => vc_num_out)
    port map (
      clk                => clk,
      rst                => rst,
      data_tx            => data_tx,
      vc_write_tx    => vc_write_tx,
      incr_tx            => incr_tx,
      data_tx_pl         => data_tx_pl,
      vc_write_tx_pl => vc_write_tx_pl,
      incr_tx_pl         => incr_tx_pl);

  -- clock generation
  Clk <= not Clk after 10 ns;

  -- waveform generation
  WaveGen_Proc : process
  begin
    -- insert signal assignments here

    wait until Clk = '1';
  end process WaveGen_Proc;



end architecture testbench;

-------------------------------------------------------------------------------

configuration output_register_tb_testbench_cfg of output_register_tb is
  for testbench
  end for;
end output_register_tb_testbench_cfg;

-------------------------------------------------------------------------------
