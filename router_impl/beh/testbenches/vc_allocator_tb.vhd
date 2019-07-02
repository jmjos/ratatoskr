-------------------------------------------------------------------------------
-- Title      : Testbench for design "vc_allocator"
-- Project    : 
-------------------------------------------------------------------------------
-- File       : vc_allocator_tb.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-11-13
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
-- 2018-11-13  1.0      bamberg Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.NOC_3D_PACKAGE.all;
use work.TURNS_3D_NOC.all;

-------------------------------------------------------------------------------

entity vc_allocator_tb is

end entity vc_allocator_tb;

-------------------------------------------------------------------------------

architecture testbench1 of vc_allocator_tb is

  -- component generics
  constant port_num                     : positive    := 3;
  constant port_exist                   : integer_vec := (int_local, int_up, int_down);
  constant Xis                          : natural     := 1;
  constant Yis                          : natural     := 1;
  constant Zis                          : natural     := 1;
  constant header_incl_in_packet_length : boolean     := true;
  constant rout_algo                    : string      := "DXYU";
  constant vc_num_vec                   : integer_vec := (2, 2, 2);
  constant vc_num_out_vec               : integer_vec := (2, 2, 2);

  type dec_array is array(port_num-1 downto 0) of std_logic_vector(1 downto 0);
  type status_array is array(port_num-1 downto 0, 1 downto 0) of integer;


  type vc_status_array_type is record
    assigned   : dec_array;
    input_port : status_array;
    vc_num     : status_array;
  end record vc_status_array_type;
  signal vc_status_array : vc_status_array_type;


  -- component ports
  signal header           : header_inf_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal enr_vc           : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal valid_data_vc    : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal input_vc_in_use  : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal output_vc_in_use : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
  signal crossbar_ctrl_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)*
                                              bit_width(port_num-1)-1 downto 0);
  signal vc_sel_enc : vc_status_array_enc(int_vec_sum(vc_num_out_vec)-1 downto 0);


  -- clock
  signal clk : std_logic := '1';

  -- reset
  signal rst : std_logic := RST_LVL;

begin  -- architecture testbench1

  -- component instantiation
  DUT : entity work.vc_allocator
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
      enr_vc            => enr_vc,
      valid_data_vc_vec => valid_data_vc,
      input_vc_in_use   => input_vc_in_use,
      crossbar_ctrl_vec => crossbar_ctrl_vec,
      vc_sel_enc_vec    => vc_sel_enc,
      output_vc_in_use  => output_vc_in_use);

  -- clock generation
  clk <= not clk after 10 ns;

  -- reset generation
  process
  begin
    wait until clk = '0';
    rst <= not rst;
    wait;
  end process;

  -- waveform generation
  WaveGen_Proc : process
  begin
    -- insert signal assignments here
    header(0).packet_length <= (others => '1');  -- maximum packet length
    header(1).packet_length <= (others => '1');
    header(2).packet_length <= (others => '1');
    header(3).packet_length <= (others => '1');
    header(4).packet_length <= (others => '1');
    header(5).packet_length <= (others => '1');
    -- Every X and Y address is equal to the current X and Y address to results
    -- in a routing only between the local and the two vertical ports
    header(0).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(1).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(2).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(3).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(4).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(5).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(0).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(1).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(2).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(3).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(4).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(5).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    -- First assume that both VCs in both ports UP and DOWN want to go to local
    header(2).z_dest        <= std_logic_vector(to_unsigned(Zis, z_addr_width));
    header(3).z_dest        <= std_logic_vector(to_unsigned(Zis, z_addr_width));
    header(4).z_dest        <= std_logic_vector(to_unsigned(Zis, z_addr_width));
    header(5).z_dest        <= std_logic_vector(to_unsigned(Zis, z_addr_width));
    -- The VCs in input local require an UP routing
    header(0).z_dest        <= std_logic_vector(to_unsigned(Zis+1, z_addr_width));
    header(1).z_dest        <= std_logic_vector(to_unsigned(Zis+1, z_addr_width));
    enr_vc                  <= (others => '0');  -- inititally no assignment so
    -- no read
    valid_data_vc           <= (others => '1');  -- every vc has valid data
    wait for 200 ns;
    enr_vc(2 downto 0)      <= (others => '1');  -- the two granted local ports, plus
                                        -- the granted up port are read
    enr_vc(4)               <= '1';  -- same for the granted down port
    wait until (input_vc_in_use(0) = '0');  -- wait until all packets of equal
                                            -- length are transmitted
    enr_vc                  <= (others => '0');  -- set the read again to zero
    wait for 200 ns;
    assert false report "NORMAL COMPLETION OF SIMULATION" severity failure;
  end process WaveGen_Proc;


  process(crossbar_ctrl_vec, output_vc_in_use, vc_sel_enc)
  begin
    vc_status_array.assigned(0)  <= output_vc_in_use(1 downto 0);
    vc_status_array.assigned(1)  <= output_vc_in_use(3 downto 2);
    vc_status_array.assigned(2)  <= output_vc_in_use(5 downto 4);
    vc_status_array.vc_num(0, 0) <= slv2int(vc_sel_enc(0));
    vc_status_array.vc_num(0, 1) <= slv2int(vc_sel_enc(1));
    vc_status_array.vc_num(1, 0) <= slv2int(vc_sel_enc(2));
    vc_status_array.vc_num(1, 1) <= slv2int(vc_sel_enc(3));
    vc_status_array.vc_num(2, 0) <= slv2int(vc_sel_enc(4));
    vc_status_array.vc_num(2, 1) <= slv2int(vc_sel_enc(5));
    if crossbar_ctrl_vec(0) = '0' then
      vc_status_array.input_port(0, 0) <= 1;
    elsif crossbar_ctrl_vec(0) = '1' then
      vc_status_array.input_port(0, 0) <= 2;
    end if;
    if crossbar_ctrl_vec(1) = '0' then
      vc_status_array.input_port(0, 1) <= 1;
    elsif crossbar_ctrl_vec(1) = '1' then
      vc_status_array.input_port(0, 1) <= 2;
    end if;

    if crossbar_ctrl_vec(2) = '0' then
      vc_status_array.input_port(1, 0) <= 2;
    elsif crossbar_ctrl_vec(2) = '1' then
      vc_status_array.input_port(1, 0) <= 0;
    end if;
    if crossbar_ctrl_vec(3) = '0' then
      vc_status_array.input_port(1, 1) <= 2;
    elsif crossbar_ctrl_vec(3) = '1' then
      vc_status_array.input_port(1, 1) <= 0;
    end if;

    if crossbar_ctrl_vec(4) = '0' then
      vc_status_array.input_port(2, 0) <= 0;
    elsif crossbar_ctrl_vec(4) = '1' then
      vc_status_array.input_port(2, 0) <= 1;
    end if;
    if crossbar_ctrl_vec(5) = '0' then
      vc_status_array.input_port(2, 1) <= 0;
    elsif crossbar_ctrl_vec(5) = '1' then
      vc_status_array.input_port(2, 1) <= 1;
    end if;

  end process;

end architecture testbench1;





architecture testbench_high_perf of vc_allocator_tb is

  -- component generics
  constant port_num                     : positive    := 3;
  constant port_exist                   : integer_vec := (int_local, int_up, int_down);
  constant Xis                          : natural     := 1;
  constant Yis                          : natural     := 1;
  constant Zis                          : natural     := 1;
  constant header_incl_in_packet_length : boolean     := true;
  constant rout_algo                    : string      := "DXYU";
  constant vc_num_vec                   : integer_vec := (2, 2, 2);
  constant vc_num_out_vec               : integer_vec := (2, 2, 2);

  type dec_array is array(port_num-1 downto 0) of std_logic_vector(1 downto 0);
  type status_array is array(port_num-1 downto 0, 1 downto 0) of integer;


  type vc_status_array_type is record
    assigned   : dec_array;
    input_port : status_array;
    vc_num     : status_array;
  end record vc_status_array_type;
  signal vc_status_array : vc_status_array_type;


  -- component ports
  signal header           : header_inf_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal enr_vc           : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal valid_data_vc    : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal input_vc_in_use  : std_logic_vector(int_vec_sum(vc_num_vec)-1 downto 0);
  signal output_vc_in_use : std_logic_vector(int_vec_sum(vc_num_out_vec)-1 downto 0);
  signal crossbar_ctrl_vec : std_logic_vector(int_vec_sum(vc_num_out_vec)*
                                              bit_width(port_num-1)-1 downto 0);
  signal vc_sel_enc : vc_status_array_enc(int_vec_sum(vc_num_out_vec)-1 downto 0);


  -- clock
  signal clk : std_logic := '1';

  -- reset
  signal rst : std_logic := RST_LVL;

begin  -- architecture testbench_high_perf

  -- component instantiation
  DUT : entity work.vc_allocator_high_perf
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
      enr_vc            => enr_vc,
      valid_data_vc_vec => valid_data_vc,
      input_vc_in_use   => input_vc_in_use,
      crossbar_ctrl_vec => crossbar_ctrl_vec,
      vc_sel_enc_vec    => vc_sel_enc,
      output_vc_in_use  => output_vc_in_use);

  -- clock generation
  clk <= not clk after 10 ns;

  -- reset generation
  process
  begin
    wait until clk = '0';
    rst <= not rst;
    wait;
  end process;

  -- waveform generation
  WaveGen_Proc : process
  begin
    -- insert signal assignments here
    header(0).packet_length <= (others => '1');  -- maximum packet length
    header(1).packet_length <= (others => '1');
    header(2).packet_length <= (others => '1');
    header(3).packet_length <= (others => '1');
    header(4).packet_length <= (others => '1');
    header(5).packet_length <= (others => '1');
    -- Every X and Y address is equal to the current X and Y address to results
    -- in a routing only between the local and the two vertical ports
    header(0).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(1).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(2).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(3).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(4).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(5).x_dest        <= std_logic_vector(to_unsigned(Xis, x_addr_width));
    header(0).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(1).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(2).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(3).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(4).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    header(5).y_dest        <= std_logic_vector(to_unsigned(Yis, y_addr_width));
    -- First assume that both VCs in both ports UP and DOWN want to go to local
    header(2).z_dest        <= std_logic_vector(to_unsigned(Zis, z_addr_width));
    header(3).z_dest        <= std_logic_vector(to_unsigned(Zis, z_addr_width));
    header(4).z_dest        <= std_logic_vector(to_unsigned(Zis, z_addr_width));
    header(5).z_dest        <= std_logic_vector(to_unsigned(Zis, z_addr_width));
    -- The VCs in input local require an UP routing
    header(0).z_dest        <= std_logic_vector(to_unsigned(Zis+1, z_addr_width));
    header(1).z_dest        <= std_logic_vector(to_unsigned(Zis+1, z_addr_width));
    enr_vc                  <= (others => '0');  -- inititally no assignment so
    -- no read
    valid_data_vc           <= (others => '1');  -- every vc has valid data
    wait for 200 ns;
    enr_vc(2 downto 0)      <= (others => '1');  -- the two granted local ports, plus
                                        -- the granted up port are read
    enr_vc(4)               <= '1';     -- same for the granted down port
    wait until (input_vc_in_use(0) = '0');  -- wait until all packets of equal
                                            -- length are transmitted
    enr_vc                  <= (others => '0');  -- set the read again to zero
    wait for 200 ns;
    assert false report "NORMAL COMPLETION OF SIMULATION" severity failure;
  end process WaveGen_Proc;


  process(crossbar_ctrl_vec, output_vc_in_use, vc_sel_enc)
  begin
    vc_status_array.assigned(0)  <= output_vc_in_use(1 downto 0);
    vc_status_array.assigned(1)  <= output_vc_in_use(3 downto 2);
    vc_status_array.assigned(2)  <= output_vc_in_use(5 downto 4);
    vc_status_array.vc_num(0, 0) <= slv2int(vc_sel_enc(0));
    vc_status_array.vc_num(0, 1) <= slv2int(vc_sel_enc(1));
    vc_status_array.vc_num(1, 0) <= slv2int(vc_sel_enc(2));
    vc_status_array.vc_num(1, 1) <= slv2int(vc_sel_enc(3));
    vc_status_array.vc_num(2, 0) <= slv2int(vc_sel_enc(4));
    vc_status_array.vc_num(2, 1) <= slv2int(vc_sel_enc(5));
    if crossbar_ctrl_vec(0) = '0' then
      vc_status_array.input_port(0, 0) <= 1;
    elsif crossbar_ctrl_vec(0) = '1' then
      vc_status_array.input_port(0, 0) <= 2;
    end if;
    if crossbar_ctrl_vec(1) = '0' then
      vc_status_array.input_port(0, 1) <= 1;
    elsif crossbar_ctrl_vec(1) = '1' then
      vc_status_array.input_port(0, 1) <= 2;
    end if;

    if crossbar_ctrl_vec(2) = '0' then
      vc_status_array.input_port(1, 0) <= 2;
    elsif crossbar_ctrl_vec(2) = '1' then
      vc_status_array.input_port(1, 0) <= 0;
    end if;
    if crossbar_ctrl_vec(3) = '0' then
      vc_status_array.input_port(1, 1) <= 2;
    elsif crossbar_ctrl_vec(3) = '1' then
      vc_status_array.input_port(1, 1) <= 0;
    end if;

    if crossbar_ctrl_vec(4) = '0' then
      vc_status_array.input_port(2, 0) <= 0;
    elsif crossbar_ctrl_vec(4) = '1' then
      vc_status_array.input_port(2, 0) <= 1;
    end if;
    if crossbar_ctrl_vec(5) = '0' then
      vc_status_array.input_port(2, 1) <= 0;
    elsif crossbar_ctrl_vec(5) = '1' then
      vc_status_array.input_port(2, 1) <= 1;
    end if;

  end process;

end architecture testbench_high_perf;

