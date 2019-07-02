-------------------------------------------------------------------------------
-- Title      : Output register for the the link
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : output_register.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-10-25
-- Last update: 2018-11-28
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: Just a pipeline stage for the link.
--              "pl" indicates a pipelined signal.
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-10-25  1.0      bamberg Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_misc.all;
use ieee.std_logic_misc.all;
use work.NOC_3D_PACKAGE.all;

entity output_register is
  generic(
    vc_num     : positive := 2;         -- Number of VCs in the according
    -- input buffer
    vc_num_out : positive := 2);        -- Number of VC in the input buffer at
  -- the other side of the link
  port(
    clk, rst       : in  std_logic;
    data_tx        : in  flit;
    vc_write_tx    : in  std_logic_vector(
      vc_num_out-1 downto 0);
    incr_tx        : in  std_logic_vector(vc_num-1 downto 0);
    data_tx_pl     : out std_logic_vector(flit_size-1 downto 0);
    vc_write_tx_pl : out std_logic_vector(vc_num_out-1 downto 0);
    incr_tx_pl     : out std_logic_vector(vc_num-1 downto 0)
    );
end output_register;

architecture rtl of output_register is
begin
-- Flip-Flops enabled by req-transfer to reduce the switching activity
  process(clk, rst)
  begin
    if rst = RST_LVL then
      data_tx_pl     <= (others => '0');
      vc_write_tx_pl <= (others => '0');
      incr_tx_pl     <= (others => '0');
    elsif rising_edge(clk) then
      if or_reduce(vc_write_tx) = '1' then
        data_tx_pl <= data_tx;
      end if;
      incr_tx_pl     <= incr_tx;
      vc_write_tx_pl <= vc_write_tx;
    end if;
  end process;

end rtl;
