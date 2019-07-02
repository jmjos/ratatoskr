-------------------------------------------------------------------------------
-- Title      : Sequential packet counter
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : seq_packet_counter.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : University of Bremen
-- Created    : 2018-11-28
-- Last update: 2018-11-28
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: When a new packet is assigend, the coutner is set to the packet
--              length. Whenever a flit is transfered, the counter is decr.
-------------------------------------------------------------------------------
-- Copyright (c) 2018 University of Bremen
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-11-28  1.0      bamberg	Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_misc.all;
use work.NOC_3D_PACKAGE.all;

entity seq_packet_counter is

  generic (
    header_incl_in_packet_length : boolean := true);
  port(
    clk, rst   : in  std_logic;
    allocated  : in  std_logic;
    packet_len : in  std_logic_vector(packet_len_width-1 downto 0);
    enr_vc     : in  std_logic;
    flit_count : out unsigned(packet_len_width-1 downto 0));
end entity seq_packet_counter;

architecture rtl of seq_packet_counter is
  signal flit_count_nxt, flit_count_load, flit_count_int
    : unsigned(packet_len_width-1 downto 0);

begin



  -----------------------------------------------------------------------------
  -- Flip Flop Counter --------------------------------------------------------
  -----------------------------------------------------------------------------
  process(clk, rst)
  begin
    if rst = RST_LVL then
      flit_count_int <= (others => '0');
    elsif clk'event and clk = '1' then
      if (allocated or enr_vc) = '1' then  -- flip flop enable
        flit_count_int <= flit_count_nxt;
      end if;
    end if;
  end process;

  -----------------------------------------------------------------------------
  -- Combinatorial ------------------------------------------------------------
  -----------------------------------------------------------------------------
  flit_count_load <= unsigned(packet_len) when header_incl_in_packet_length
                     else (unsigned(packet_len)+1);

  process(allocated, enr_vc, flit_count_int, flit_count_load)
  begin
    if allocated = '1' and enr_vc = '0' then
      flit_count_nxt <= flit_count_load;
    elsif allocated = '0' and enr_vc = '1' then
      flit_count_nxt <= flit_count_int-1;
    else
      flit_count_nxt <= (others => '-');
    end if;
  end process;
  flit_count <= flit_count_int;
end architecture;
