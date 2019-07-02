-------------------------------------------------------------------------------
-- Title      : Switch allocator
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : switch_allocator.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-11-15
-- Last update: 2018-11-28
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: Switch allocator witch sets the input to output connections
--              between assigned input VCs (with valid data) and the according
--              outputs VCs (when credit is available).
-------------------------------------------------------------------------------
--Desginer comments: With an encoded vc_transfer and a valid signal the
--                   input VC select signal could be determined in advance.
--                   Since this is part of the crit. path, it has a potential
--                   to enhance timing if we slightly mod. the design (-power).
--                   Second possiblity to improve speed would be to pre-calcu-
--                   late the next switch allocation. This is realized by
--                   simply swapping the regular RR-arbiters by delayed RR-
--                   arbiters (see DESIGNER-HINT l. 181). The drawback would be
--                   a one clock-cycle bigger initial latency for the first flit
--                   of a new package.
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-11-15  1.0      bamberg Created
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_misc.all;
use work.NOC_3D_PACKAGE.all;
use work.TURNS_3D_NOC.all;

entity switch_allocator is

  generic (
    port_num           : integer           := 7;
    -- integer vector of range "0 to port_num-1"
    port_exist         : integer_vec       := (0, 1, 2, 3, 4, 5, 6);
    vc_num_vec         : integer_vec       := (2, 2, 2, 2, 2, 2, 2);
    vc_num_out_vec     : integer_vec       := (2, 2, 2, 2, 2, 2, 2);
    -- integer vector of range "0 to port_num-1, 0 to max_vc_num-1"                                   
    vc_depth_array     : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4));
    vc_depth_out_array : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4));
    rout_algo          : string            := "XYZ_ref"
    );
  port(
    clk, rst          : in  std_logic;
    -- Inputs from VC allocator
    input_vc_in_use   : in  std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
    output_vc_in_use  : in  std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
    crossbar_ctrl_vec : in  std_logic_vector(int_vec_sum(vc_num_out_vec)*
                                            bit_width(port_num-1)-1 downto 0);
    vc_sel_enc_vec    : in  vc_status_array_enc(int_vec_sum(vc_num_out_vec)-1 downto 0);
    -- Inputs from Input buffer
    valid_data_vc_vec : in  std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);  --
    -- Inputs from Link
    incr_rx_vec       : in  std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
    -- Output to crossbar
    crossbar_ctrl     : out std_logic_vector(port_num*bit_width(port_num-1)-1 downto 0);
    -- output to Inbut Buffer (vc_transfer) and output-buffer (incr_tx)
    vc_transfer_vec   : out std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
    vc_write_tx_vec   : out std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0)
    );
end entity switch_allocator;

architecture rtl of switch_allocator is
  constant poss_routes             : turn_table_3D := routes_3D(rout_algo);
  constant sel_width               : positive      := bit_width(port_num-1);
  signal switch_rq                 : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal switch_rq_grant           : vc_status_array(port_num-1 downto 0);
  signal vc_transfer_vec_int       : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  type switch_ack_array is array(port_num-1 downto 0) of std_logic_vector(port_num-1 downto 0);
  signal switch_acks               : switch_ack_array;
  signal switch_ack                : std_logic_vector(port_num-1 downto 0);
  type crossbar_ctrl_array_type is array(int_vec_sum(vc_num_out_vec)-1 downto 0) of std_logic_vector(sel_width-1 downto 0);
  signal crossbar_ctrl_vc_out      : crossbar_ctrl_array_type;
  type rq_array is array (port_num-1 downto 0) of vc_status_array(port_num-2 downto 0);
  signal poss_channel_rq           : rq_array;
  signal channel_rq, channel_grant : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
  signal credit_avail              : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
begin  -- architecture rtl

-------------------------------------------------------------------------------
-- Transform the crossbar ctrls into an array for better readability ----------
-------------------------------------------------------------------------------
  XBAR_CTRL : for i in 0 to int_vec_sum(vc_num_out_vec)-1 generate
    crossbar_ctrl_vc_out(i) <= crossbar_ctrl_vec((i+1)*sel_width-1 downto i*sel_width);
  end generate;
-------------------------------------------------------------------------------
-- When an input VC is assigned and contains data: a switch request is made ---
-------------------------------------------------------------------------------
  switch_rq <= input_vc_in_use and valid_data_vc_vec;

-------------------------------------------------------------------------------
-- For each input VC arbitrate grant one switch request in a RR manner --------
-------------------------------------------------------------------------------  
  INPUT_ARB_GEN : for i in 0 to port_num-1 generate
    constant ur_vc : natural := upper_range(vc_num_vec, i);
    constant lr_vc : natural := lower_range(vc_num_vec, i);
  begin
    PASS_NO_VC : if vc_num_vec(i) = 1 generate  -- single vc --> no arb. required
      switch_rq_grant(i)(0) <= switch_rq(lr_vc);
    end generate;
    GEN_RR : if vc_num_vec(i) > 1 generate
      rr_arbiter : entity work.rr_arbiter_no_delay
        generic map (
          CNT => vc_num_vec(i))
        port map (
          clk   => clk,
          rst   => rst,
          req   => switch_rq(ur_vc downto lr_vc),
          ack   => switch_ack(i),
          grant => switch_rq_grant(i)(vc_num_vec(i)-1 downto 0));
    end generate;
  end generate;

-------------------------------------------------------------------------------
-- Wiring of granted switch request to the according outputs in the order the
-- crossbar select is defined (clock-wise / modulo). Thereby we exploits that 
-- some request are not possible, since a routing forbids it for dead and live-
-- lock  avoidance (don't care to reduce the circuit complexity after synthesis)
-------------------------------------------------------------------------------  
  WIRING : process(switch_rq_grant)
    variable var_in : natural;
  begin
    poss_channel_rq <= (others => (others => (others => '-')));
    for y in 0 to port_num-1 loop       -- For the phy channel at output y,
      for x in 0 to port_num-2 loop     -- the x^th possible input is 
        if y+x < port_num-1 then
          var_in := y+x+1;              -- clock wise
        else
          var_in := y+x-port_num+1;     -- modulo      
        end if;
        if poss_routes(port_exist(var_in))(port_exist(y)) then
          poss_channel_rq(y)(x) <= switch_rq_grant(var_in);
        end if;
      end loop;
    end loop;
  end process;

-------------------------------------------------------------------------------
-- Choose (multiplex) the "channel_rq" out of the "poss_channel_rq" in depend.
-- of VC allocation ("crossbar_ctrl_vec" & "vc_sel_enc_vec") ------------------
-------------------------------------------------------------------------------  
  MUX_CHANNEL_RQ_GEN : process(credit_avail, crossbar_ctrl_vc_out, output_vc_in_use,
                               poss_channel_rq, vc_sel_enc_vec)
    variable assigned_input       : std_logic_vector(sel_width-1 downto 0);
    variable assigned_vc          : vc_status_vec_enc;
    variable lr_vc_out, ur_vc_out : natural range 0 to int_vec_sum(vc_num_out_vec)-1;
  begin
    for port_i in 0 to port_num-1 loop
      lr_vc_out := lower_range(vc_num_out_vec, port_i);
      ur_vc_out := upper_range(vc_num_out_vec, port_i);
      for vc_i in lr_vc_out to ur_vc_out loop
        assigned_input := crossbar_ctrl_vc_out(vc_i);
        assigned_vc    := vc_sel_enc_vec(vc_i);
        channel_rq(vc_i) <=
          poss_channel_rq(port_i)(slv2int(assigned_input))(slv2int(assigned_vc))
          and output_vc_in_use(vc_i) and credit_avail(vc_i);
      end loop;
    end loop;
  end process;

-------------------------------------------------------------------------------  
-- Generate the physical channel (output) arbiters ---------------------------- 
-------------------------------------------------------------------------------
  OUTPUT_ARB_GEN : for i in 0 to port_num-1 generate
    constant ur_vc_out : natural := upper_range(vc_num_out_vec, i);
    constant lr_vc_out : natural := lower_range(vc_num_out_vec, i);
  begin
    PASS_NO_VC : if vc_num_out_vec(i) = 1 generate  -- single vc --> no arb. required
      channel_grant(lr_vc_out) <= channel_rq(lr_vc_out);
    end generate;
-- DESIGNER_HINT maybe use a delayed rr_arbitter (allows speed improvement)
    GEN_RR : if vc_num_out_vec(i) > 1 generate
      rr_arbiter : entity work.rr_arbiter_no_delay
        generic map (
          CNT => vc_num_out_vec(i))
        port map (
          clk   => clk,
          rst   => rst,
          req   => channel_rq(ur_vc_out downto lr_vc_out),
          ack   => '1',
          grant => channel_grant(ur_vc_out downto lr_vc_out));
    end generate;
    CREDIT_COUNT_GENERATE : for vc_i in lr_vc_out to ur_vc_out generate
      credit_count_i : entity work.credit_count_single
        generic map (
          vc_depth_out => vc_depth_out_array(i)(vc_i-lr_vc_out))
        port map (
          clk          => clk,
          rst          => rst,
          incr_rx      => incr_rx_vec(vc_i),
          vc_write_tx  => channel_grant(vc_i),      -- EQUAL: vc_write_tx_vec
          credit_avail => credit_avail(vc_i));
    end generate;
  end generate;
  vc_write_tx_vec <= channel_grant;


  -----------------------------------------------------------------------------
  -- Crossbar ctrl, input acknowledge out of winner----------------------------
  -----------------------------------------------------------------------------
  DECODE_INPUT_ACK : for i in 0 to port_num-1 generate
    constant ur : natural := upper_range(vc_num_out_vec, i);
    constant lr : natural := lower_range(vc_num_out_vec, i);
  begin
    process(channel_grant(ur downto lr), crossbar_ctrl_vc_out)
      variable winner       : natural range 0 to int_vec_sum(vc_num_out_vec)-1;
      variable input_winner : natural range 0 to port_num-1;
    begin
      crossbar_ctrl((i+1)*sel_width-1 downto i*sel_width) <= (others => '-');
      switch_acks(i)                                      <= (others => '0');
      if vc_num_out_vec(i) = 1 then
        winner := lr;
      else
        winner := lr + one_hot2int(channel_grant(ur downto lr));
      end if;
      crossbar_ctrl((i+1)*sel_width-1 downto i*sel_width) <= crossbar_ctrl_vc_out(winner);
      if or_reduce(channel_grant(ur downto lr)) = '1' then
        input_winner                 := (slv2int(crossbar_ctrl_vc_out(winner))+ i + 1) mod port_num;
        switch_acks(i)(input_winner) <= '1';
      end if;
    end process;
  end generate;

  INP_ACK : process(switch_acks)
    variable switch_ack_var : std_logic_vector(port_num-1 downto 0);
  begin
    switch_ack_var := (others => '0');
    for i in 0 to port_num-1 loop
      switch_ack_var := switch_ack_var or switch_acks(i);
    end loop;
    switch_ack <= switch_ack_var;
  end process;

  process(switch_ack, switch_rq_grant)
    variable ur, lr : natural range 0 to int_vec_sum(vc_num_vec)-1;
  begin
    vc_transfer_vec_int <= (others => '0');
    for i in 0 to port_num-1 loop
      if switch_ack(i) = '1' then
        ur                                := upper_range(vc_num_vec, i);
        lr                                := lower_range(vc_num_vec, i);
        vc_transfer_vec_int(ur downto lr) <= switch_rq_grant(i)(vc_num_vec(i)-1 downto 0);
      end if;
    end loop;
  end process;
  vc_transfer_vec <= vc_transfer_vec_int;
end architecture;




