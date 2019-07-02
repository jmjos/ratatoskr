-------------------------------------------------------------------------------
-- Title      : Allocation of output VCs of a port in a prioroty manner (VC0 first)
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : vc_output_allocator.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-11-09
-- Last update: 2018-11-28
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-11-09  1.0      bamberg Created
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_misc.all;
use work.NOC_3D_PACKAGE.all;

entity vc_output_allocator is

  generic (
    port_num   : positive := 7;
    vc_num_out : positive := 2);
  port (
    clk, rst          : in  std_logic;
    rq_vc_out         : in  std_logic_vector(port_num-2 downto 0);
    granted_vc        : in  vc_status_array(port_num-2 downto 0);
    packet_end        : in  vc_status_array(port_num-2 downto 0);
    crossbar_ctrl_vec : out std_logic_vector
    (vc_num_out*bit_width(port_num-1)-1 downto 0);
    vc_sel_enc        : out vc_status_array_enc(vc_num_out-1 downto 0);
    output_vc_in_use  : out std_logic_vector(vc_num_out-1 downto 0);
    ack_rq_vc_out     : out std_logic_vector(port_num-2 downto 0));
end entity vc_output_allocator;


architecture rtl of vc_output_allocator is
  constant sel_wdth : integer := bit_width(port_num-1);
  type crossbar_sel_vec is array(vc_num_out-1 downto 0)
    of std_logic_vector(sel_wdth-1 downto 0);
  signal crossbar_sels : crossbar_sel_vec;
  signal vc_available  : std_logic;
  signal valid_rq      : std_logic;
  signal output_vc_in_use_int, free_vc_out, next_vc_out :
    std_logic_vector(vc_num_out-1 downto 0);
  signal vc_sel_enc_int : vc_status_array_enc(vc_num_out-1 downto 0);
  signal grant          : std_logic_vector(port_num-2 downto 0);
  signal next_sel       : std_logic_vector(sel_wdth-1 downto 0);
  signal next_vc_in_enc : std_logic_vector(
    bit_width(max_vc_num)-1 downto 0);

begin
  -----------------------------------------------------------------------------
  -- Round robin arbitration between all request ------------------------------
  -----------------------------------------------------------------------------
  rr_arbiter : entity work.rr_arbiter_no_delay
    generic map (
      CNT => port_num-1)
    port map (
      clk   => clk,
      rst   => rst,
      req   => rq_vc_out,
      ack   => vc_available,
      grant => grant);
  -- Extract. information of the winning/next-granted rquest
  valid_rq <= or_reduce(grant);
  next_sel <= one_hot2slv(grant);
  next_vc_in_enc <= one_hot2slv(
    granted_vc(one_hot2int(grant)));

  -----------------------------------------------------------------------------
  -- Register that contain information about all output VCs such as source
  -- (Input & VC), and the logic for the register transitions -----------------
  -----------------------------------------------------------------------------
  STOR_GEN : for i in 0 to vc_num_out-1 generate
  begin
    crossbar_ctrl_vec((i+1)*sel_wdth-1 downto i*sel_wdth) <= crossbar_sels(i);
    -- Reg gen to store the information of the output VC usage
    process(clk, rst)
    begin
      if rst = RST_LVL then
        crossbar_sels(i)  <= (others => '0');
        vc_sel_enc_int(i) <= (others => '0');
      elsif rising_edge(clk) then
        if (next_vc_out(i) and valid_rq) = '1' then
          crossbar_sels(i)  <= next_sel;
          vc_sel_enc_int(i) <= next_vc_in_enc;
        end if;
      end if;
    end process;
    -- 1-bit register to keep the status of the VC
    process(clk, rst)
    begin
      if rst = RST_LVL then
        output_vc_in_use_int(i) <= '0';
      elsif rising_edge(clk) then
        if (next_vc_out(i) and valid_rq) = '1' then
          output_vc_in_use_int(i) <= '1';
        elsif free_vc_out(i) = '1' then
          output_vc_in_use_int(i) <= '0';
        end if;
      end if;
    end process;
    -- Signal that checks if an assigned output vc is free again
    free_vc_out(i) <= packet_end(slv2int(crossbar_sels(i)))
                      (slv2int(vc_sel_enc_int(i)));
  --free_vc_out(i)<= packet_end(1)(1);
  end generate;
  -- Isolate LSB of all free/unused output VCs (next assigned VC)
  next_vc_out <= not(output_vc_in_use_int) and
                 std_logic_vector(unsigned(output_vc_in_use_int) + 1);

  -----------------------------------------------------------------------------
  -- Acknowledge input request the assignment of an output VC-----------------
  -----------------------------------------------------------------------------
  vc_available     <= or_reduce(not(output_vc_in_use_int));
  ack_rq_vc_out    <= grant when vc_available = '1' else (others => '0');
  output_vc_in_use <= output_vc_in_use_int;
  vc_sel_enc       <= vc_sel_enc_int;
end architecture rtl;
