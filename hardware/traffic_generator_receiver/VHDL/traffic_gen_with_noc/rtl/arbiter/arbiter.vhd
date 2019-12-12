-------------------------------------------------------------------------------
-- Title      : Centralized arbiter
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : arbiter.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-11-28
-- Last update: 2018-11-28
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: Centralized arbiter, made up of an virtual channel and an switch
--              allocator. Poss routes are exploited to heavily reduce compl.
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-11-28  1.0      bamberg Created
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use work.NOC_3D_PACKAGE.all;


entity arbiter is

  generic (
    port_num                     : positive          := 7;
    -- Integer range has to be / is (0 to port_num-1)
    port_exist                   : integer_vec       := (0, 1, 2, 3, 4, 5, 6);
    Xis                          : natural           := 1;
    Yis                          : natural           := 1;
    Zis                          : natural           := 1;
    header_incl_in_packet_length : boolean           := true;
    rout_algo                    : string            := "XYZ_ref";
    vc_num_vec                   : integer_vec       := (4 ,4 ,4 ,4 ,4 ,4 ,4 );
    vc_num_out_vec               : integer_vec       := (4 ,4 ,4 ,4 ,4 ,4 ,4 );
    -- integer vector of range "0 to port_num-1, 0 to max_vc_num-1"                                   
    vc_depth_array               : vc_prop_int_array := ((4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) );
    vc_depth_out_array           : vc_prop_int_array := ((4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) ,(4 ,4 ,4 ,4 ) )
    );
  port (
    clk, rst          : in  std_logic;
    header            : in  header_inf_vector(int_vec_sum(vc_num_vec)-1 downto 0);
    valid_data_vc_vec : in  std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
    incr_rx_vec       : in  std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
    crossbar_ctrl     : out std_logic_vector(port_num*bit_width(port_num-1)-1 downto 0);
    vc_transfer_vec   : out std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
    vc_write_tx_vec   : out std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0));
end entity arbiter;



architecture structural of arbiter is
  signal vc_transfer_vec_int : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal input_vc_in_use     : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal crossbar_ctrl_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)*
                                              bit_width(port_num-1)-1 downto 0);
  signal output_vc_in_use : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
  signal vc_sel_enc_vec   : vc_status_array_enc(int_vec_sum(vc_num_out_vec)-1 downto 0);

begin  -- architecture structural

  --vc_allocator_1 : entity work.vc_allocator -- use the less cmplx/performant one 
  vc_allocator_1 : entity work.vc_allocator_high_perf  -- use the more cmplx/performant one
    generic map (
      port_num                     => port_num,
      port_exist                   => port_exist,
      Xis                          => Xis,
      Yis                          => Yis,
      Zis                          => Zis,
      header_incl_in_packet_length => header_incl_in_packet_length,
      rout_algo                    => rout_algo,
      vc_num_vec                   => vc_num_vec,
      vc_num_out_vec               => vc_num_out_vec)
    port map (
      clk               => clk,
      rst               => rst,
      header            => header,
      enr_vc            => vc_transfer_vec_int,
      valid_data_vc_vec => valid_data_vc_vec,
      input_vc_in_use   => input_vc_in_use,
      crossbar_ctrl_vec => crossbar_ctrl_vec,
      vc_sel_enc_vec    => vc_sel_enc_vec,
      output_vc_in_use  => output_vc_in_use);


  switch_allocator_1 : entity work.switch_allocator
    generic map (
      port_num           => port_num,
      port_exist         => port_exist,
      vc_num_vec         => vc_num_vec,
      vc_num_out_vec     => vc_num_out_vec,
      vc_depth_array     => vc_depth_array,
      vc_depth_out_array => vc_depth_out_array,
      rout_algo          => rout_algo)
    port map (
      clk               => clk,
      rst               => rst,
      input_vc_in_use   => input_vc_in_use,
      output_vc_in_use  => output_vc_in_use,
      crossbar_ctrl_vec => crossbar_ctrl_vec,
      vc_sel_enc_vec    => vc_sel_enc_vec,
      valid_data_vc_vec => valid_data_vc_vec,
      incr_rx_vec       => incr_rx_vec,
      crossbar_ctrl     => crossbar_ctrl,
      vc_transfer_vec   => vc_transfer_vec_int,
      vc_write_tx_vec   => vc_write_tx_vec);

  vc_transfer_vec <= vc_transfer_vec_int;

end architecture structural;
