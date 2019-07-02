-------------------------------------------------------------------------------
-- Title      : Testbench for design "router_pl"
-- Project    : 
-------------------------------------------------------------------------------
-- File       : router_pl_tb.vhd
-- Author     : Lennart Bamberg  <lennart@t440s>
-- Company    : 
-- Created    : 2018-11-23
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
-- 2018-11-23  1.0      lennart Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_misc.all;
use work.NOC_3D_PACKAGE.all;
-------------------------------------------------------------------------------

entity router_pl_tb is

end entity router_pl_tb;

-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- Testbench1: Arrival of a single package at the local port, which has to be
-- forwarded to the UP-port. No incr. is received from the UP-Port, so the trans-
-- mission has to stop after sending "vc_depth_array(5,0)" flits. -------------
-------------------------------------------------------------------------------

architecture testbench1 of router_pl_tb is

  -- component generics
  constant port_num                     : integer       := 7;
  constant Xis                          : natural       := 1;
  constant Yis                          : natural       := 1;
  constant Zis                          : natural       := 1;
  constant header_incl_in_packet_length : boolean       := true;
  constant port_exist                   : integer_vec   := (0, 1, 2, 3, 4, 5, 6);
  constant vc_num_vec                   : integer_vec   := (2, 2, 2, 2, 2, 2, 2);
  constant vc_num_out_vec               : integer_vec   := (2, 2, 2, 2, 2, 2, 2);
  constant vc_depth_array               : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4));
  constant vc_depth_out_array           : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4));
  constant rout_algo                    : string        := "ZXY";
  -- component ports
  --- inputs
  signal data_rx : flit_vector(port_num-1 downto 0) :=
    (others => (others => '0'));
  signal vc_write_rx_vec : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0) :=
    (others => '0');
  signal incr_rx_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0)
    := (others => '0');
  --- outputs
  signal data_tx_pl         : flit_vector(port_num-1 downto 0);
  signal vc_write_tx_pl_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
  signal incr_tx_pl_vec     : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  -- clock
  signal clk : std_logic := '1';
  -- rest
  signal rst : std_logic := RST_LVL;

begin  -- architecture testbench

  -- component instantiation
  DUT : entity work.router_pl
    generic map (
      port_num                     => port_num,
      Xis                          => Xis,
      Yis                          => Yis,
      Zis                          => Zis,
      header_incl_in_packet_length => header_incl_in_packet_length,
      port_exist                   => port_exist,
      vc_num_vec                   => vc_num_vec,
      vc_num_out_vec               => vc_num_out_vec,
      vc_depth_array               => vc_depth_array,
      vc_depth_out_array           => vc_depth_out_array,
      rout_algo                    => rout_algo)
    port map (
      clk                => clk,
      rst                => rst,
      data_rx            => data_rx,
      vc_write_rx_vec    => vc_write_rx_vec,
      incr_rx_vec        => incr_rx_vec,
      data_tx_pl         => data_tx_pl,
      vc_write_tx_pl_vec => vc_write_tx_pl_vec,
      incr_tx_pl_vec     => incr_tx_pl_vec);

  -- clock generation
  clk <= not clk after 10 ns;

  -- reset generatrion
  process
  begin
    wait until falling_edge(clk);               -- wait for first falling edge
    rst <= not(RST_LVL);
    wait;
  end process;

  -- waveform generation
  SingleDataTrans : process
  begin
    -- packet of maximim lengths that wants to go to 3,3,3
    data_rx(0)         <= (others => '1');
    -- over first virtual channel
    vc_write_rx_vec(0) <= '1';
    wait;
  end process SingleDataTrans;



end architecture testbench1;



-------------------------------------------------------------------------------
-- Testbench2: Arrival of a single package at the local port of a router at the
-- corner of the NOC (position 0,0,0), which must be forwarded to the UP-port.
-- No incr. is received from the EAST-Port, so the transmission has to stop
-- after sending "vc_depth_array(2,0)" flits.----------------------------------
-------------------------------------------------------------------------------

architecture testbench2 of router_pl_tb is

  -- component generics
  constant port_num                     : integer       := 4;
  constant Xis                          : natural       := 0;
  constant Yis                          : natural       := 0;
  constant Zis                          : natural       := 0;
  constant header_incl_in_packet_length : boolean       := true;
  constant port_exist                   : integer_vec   := (int_local, int_north, int_east, int_up);
  constant vc_num_vec                   : integer_vec   := (2, 2, 2, 2);
  constant vc_num_out_vec               : integer_vec   := (2, 2, 2, 2);
  constant vc_depth_array               : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4));
  constant vc_depth_out_array           : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4));
  constant rout_algo                    : string        := "XYZ";

  -- component ports
  --- inputs
  signal data_rx : flit_vector(port_num-1 downto 0) :=
    (others => (others => '0'));
  signal vc_write_rx_vec : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0) :=
    (others => '0');
  signal incr_rx_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0)
    := (others => '0');
  --- outputs
  signal data_tx_pl         : flit_vector(port_num-1 downto 0);
  signal vc_write_tx_pl_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
  signal incr_tx_pl_vec     : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  -- clock
  signal clk : std_logic := '1';
  -- rest
  signal rst : std_logic := RST_LVL;

begin  -- architecture testbench

  -- component instantiation
  DUT : entity work.router_pl
    generic map (
      port_num                     => port_num,
      Xis                          => Xis,
      Yis                          => Yis,
      Zis                          => Zis,
      header_incl_in_packet_length => header_incl_in_packet_length,
      port_exist                   => port_exist,
      vc_num_vec                   => vc_num_vec,
      vc_num_out_vec               => vc_num_out_vec,
      vc_depth_array               => vc_depth_array,
      vc_depth_out_array           => vc_depth_out_array,
      rout_algo                    => rout_algo)
    port map (
      clk                => clk,
      rst                => rst,
      data_rx            => data_rx,
      vc_write_rx_vec    => vc_write_rx_vec,
      incr_rx_vec        => incr_rx_vec,
      data_tx_pl         => data_tx_pl,
      vc_write_tx_pl_vec => vc_write_tx_pl_vec,
      incr_tx_pl_vec     => incr_tx_pl_vec);

  -- clock generation
  clk <= not clk after 10 ns;

  -- reset generatrion
  process
  begin
    wait until falling_edge(clk);   -- wait for first falling edge
    rst <= not(RST_LVL);
    wait;
  end process;

  -- waveform generation
  SingleDataTrans : process
  begin
    -- packet of maximim lengths that wants to go to 3,3,3
    data_rx(0)         <= (others => '1');
    -- over first virtual channel
    vc_write_rx_vec(0) <= '1';
    wait;
  end process SingleDataTrans;

end architecture testbench2;

-------------------------------------------------------------------------------
-- Testbench3: Arrival of two packages at the local & north port of a router at 
-- the corner of the NOC (position 0,0,0). The packages must be both forwarded 
-- to the EAST-port. If both processes INCR1 & INCR2 are commented, no incr. is
-- received, so the transmission has to stop at a point. If INCR1 is active one
-- input vc in the adjacent router can forward the package, why the full pack-
-- age can be transmitted. In INCR2, both packages can be transmitted ---------
-------------------------------------------------------------------------------




architecture testbench3 of router_pl_tb is

  -- component generics
  constant port_num                     : integer       := 4;
  constant Xis                          : natural       := 0;
  constant Yis                          : natural       := 0;
  constant Zis                          : natural       := 0;
  constant header_incl_in_packet_length : boolean       := true;
  constant port_exist                   : integer_vec   := (int_local, int_north, int_east, int_up);
  constant vc_num_vec                   : integer_vec   := (2, 2, 2, 2);
  constant vc_num_out_vec               : integer_vec   := (2, 2, 2, 2);
  constant vc_depth_array               : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4));
  constant vc_depth_out_array           : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4));
  constant rout_algo                    : string        := "XYZ_ref";

  -- component ports

  -- Inputs
  signal data_rx : flit_vector(port_num-1 downto 0) :=
    (others => (others => '0'));
  signal vc_write_rx_vec : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0) :=
    (others => '0');
  signal incr_rx_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0)
    := (others => '0');
  -- Outputs
  signal data_tx_pl         : flit_vector(port_num-1 downto 0);
  signal vc_write_tx_pl_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
  signal incr_tx_pl_vec     : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);

  -- clock
  signal clk : std_logic := '1';

  -- rest
  signal rst : std_logic := RST_LVL;

begin  -- architecture testbench

  -- component instantiation
  DUT : entity work.router_pl
    generic map (
      port_num                     => port_num,
      Xis                          => Xis,
      Yis                          => Yis,
      Zis                          => Zis,
      header_incl_in_packet_length => header_incl_in_packet_length,
      port_exist                   => port_exist,
      vc_num_vec                   => vc_num_vec,
      vc_num_out_vec               => vc_num_out_vec,
      vc_depth_array               => vc_depth_array,
      vc_depth_out_array           => vc_depth_out_array,
      rout_algo                    => rout_algo)
    port map (
      clk                => clk,
      rst                => rst,
      data_rx            => data_rx,
      vc_write_rx_vec    => vc_write_rx_vec,
      incr_rx_vec        => incr_rx_vec,
      data_tx_pl         => data_tx_pl,
      vc_write_tx_pl_vec => vc_write_tx_pl_vec,
      incr_tx_pl_vec     => incr_tx_pl_vec);

  -- clock generation
  clk <= not clk after 10 ns;

  -- reset generatrion
  process
  begin
    wait until falling_edge(clk);               -- wait for first falling edge
    rst <= not(RST_LVL);
    wait;
  end process;

  -- waveform generation
  VcDataTrans : process
  begin
    -- packet of maximim lengths that wants to go to 3,3,3
    data_rx(0)                                 <= (others => '1');
    data_rx(1)                                 <= (others => '1');
    data_rx(1)(flit_size-1 downto flit_size-3) <= (others => '0');
    -- over first virtual channel of port 1 and port 2
    vc_write_rx_vec(0)                         <= '1';
    vc_write_rx_vec(2)                         <= '1';
    wait;
  end process VcDataTrans;

--- Maximum one of the two incr processes can be uncommented
  
  --INCR1: process -- only one output is available after a while
  --begin
  --  wait until or_reduce(vc_write_tx_pl_vec)='1';
  --  wait for 60 ns; -- minimum time for the first flit to leave the next router
  --  incr_rx_vec(4)<= '1';
  --  wait for 20 ns;
  --  incr_rx_vec(4)<= '0';
  --  wait for 20 ns;
  --  incr_rx_vec(4)<= '1';
  --  wait;
  --end process;  

  INCR2: process 
  begin
    wait until or_reduce(vc_write_tx_pl_vec)='1';
    incr_rx_vec(4)<= '1';
    incr_rx_vec(5)<= '0';
    wait for 20 ns; --wait a clock cycle
    incr_rx_vec(4)<= '0';
    incr_rx_vec(5)<= '1';
    wait for 20 ns; --wait a clock cycle
  end process;  
  



  
end architecture testbench3;



-------------------------------------------------------------------------------
-- Testbench4: As Testbench3. but in Local there is only one input VC, and in 
-- direction east there is only one output V-----------------------------------
-------------------------------------------------------------------------------



architecture testbench4 of router_pl_tb is

  -- component generics
  constant port_num                     : integer       := 4;
  constant Xis                          : natural       := 0;
  constant Yis                          : natural       := 0;
  constant Zis                          : natural       := 0;
  constant header_incl_in_packet_length : boolean       := true;
  constant port_exist                   : integer_vec   := (int_local, int_north, int_east, int_up);
  constant vc_num_vec                   : integer_vec   := (1, 2, 2, 2);
  constant vc_num_out_vec               : integer_vec   := (2, 2, 1, 2);
  constant vc_depth_array               : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4));
  constant vc_depth_out_array           : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4));
  constant rout_algo                    : string        := "XYZ_ref";

  -- component ports

  -- Inputs
  signal data_rx : flit_vector(port_num-1 downto 0) :=
    (others => (others => '0'));
  signal vc_write_rx_vec : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0) :=
    (others => '0');
  signal incr_rx_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0)
    := (others => '0');
  -- Outputs
  signal data_tx_pl         : flit_vector(port_num-1 downto 0);
  signal vc_write_tx_pl_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
  signal incr_tx_pl_vec     : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);

  -- clock
  signal clk : std_logic := '1';

  -- rest
  signal rst : std_logic := RST_LVL;

begin  -- architecture testbench

  -- component instantiation
  DUT : entity work.router_pl
    generic map (
      port_num                     => port_num,
      Xis                          => Xis,
      Yis                          => Yis,
      Zis                          => Zis,
      header_incl_in_packet_length => header_incl_in_packet_length,
      port_exist                   => port_exist,
      vc_num_vec                   => vc_num_vec,
      vc_num_out_vec               => vc_num_out_vec,
      vc_depth_array               => vc_depth_array,
      vc_depth_out_array           => vc_depth_out_array,
      rout_algo                    => rout_algo)
    port map (
      clk                => clk,
      rst                => rst,
      data_rx            => data_rx,
      vc_write_rx_vec    => vc_write_rx_vec,
      incr_rx_vec        => incr_rx_vec,
      data_tx_pl         => data_tx_pl,
      vc_write_tx_pl_vec => vc_write_tx_pl_vec,
      incr_tx_pl_vec     => incr_tx_pl_vec);

  -- clock generation
  clk <= not clk after 10 ns;

  -- reset generatrion
  process
  begin
    wait until falling_edge(clk);               -- wait for first falling edge
    rst <= not(RST_LVL);
    wait;
  end process;

  -- waveform generation
  VcDataTrans : process
  begin
    -- packet of maximim lengths that wants to go to 3,3,3
    data_rx(0)                                 <= (others => '1');
    data_rx(1)                                 <= (others => '1');
    data_rx(1)(flit_size-1 downto flit_size-3) <= (others => '0');
    -- over first virtual channel of port 1 and port 2
    vc_write_rx_vec(0)                         <= '1';
    vc_write_rx_vec(1)                         <= '1';
    wait;
  end process VcDataTrans;


  
  INCR1: process -- first packet can be tranmitted till the end, second not
  begin
    wait until or_reduce(vc_write_tx_pl_vec)='1';
    wait for 60 ns; -- minimum time for the first flit to leave the next router
    incr_rx_vec(4)<= '1';
    wait;
  end process;  

  

  
end architecture testbench4;






-------------------------------------------------------------------------------

configuration router_pl_tb_testbench_cfg of router_pl_tb is
  for testbench1
  end for;
end router_pl_tb_testbench_cfg;

-------------------------------------------------------------------------------
