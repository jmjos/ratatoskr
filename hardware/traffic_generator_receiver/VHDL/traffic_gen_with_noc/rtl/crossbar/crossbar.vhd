-------------------------------------------------------------------------------
-- Title      : Mux-based crossbar
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : crossbar.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-10-24
-- Last update: 2018-11-28
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: Crossbar to connect the inputs to the outputs with the help of
--              multiplexers (U-turns are avoided). Poss routes are exploited.
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-10-24  1.0      bamberg Created
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.all;
use ieee.numeric_std.all;
use work.NOC_3D_PACKAGE.all;
use work.TURNS_3D_NOC.all;

entity crossbar is
  generic(port_num   : positive    := 7;
          port_exist : integer_vec := (0, 1, 2, 3, 4, 5, 6);
          rout_algo  : string      := "DXYU");
  port (crossbar_in   : in  flit_vector(port_num-1 downto 0);
        crossbar_ctrl : in  std_logic_vector(
          port_num*bit_width(port_num-1)-1 downto 0);
        crossbar_out  : out flit_vector(port_num-1 downto 0));
end entity crossbar;

architecture rtl of crossbar is
  constant poss_routes    : turn_table_3D := routes_3D(rout_algo);
  constant port_sel_width : positive      := bit_width(port_num-1);  -- bits for
                                        -- crossbar_ctrl signal of
                                        -- one output port
  type multiplexer_input_type is array (port_num-1 downto 0)
    of flit_vector(port_num-2 downto 0);
  signal multiplexer_input : multiplexer_input_type;
begin

-------------------------------------------------------------------------------
-- Generate only the inputs that are really required. For all others, choose a
-- don't care (in hardware: just wires)----------------------------------------
-------------------------------------------------------------------------------  
  INPUT_GEN : process(crossbar_in)
    variable var_in : natural;
  begin
    multiplexer_input <= (others => (others => (others => '-')));
    for y in 0 to port_num-1 loop       -- For the mulitplexer at output y,
      for x in 0 to port_num-2 loop     -- the x^th input is 
        if y+x < port_num-1 then
          var_in := y+x+1;
        else
          var_in := y+x-port_num+1;
        end if;
        if poss_routes(port_exist(var_in))(port_exist(y)) then
          multiplexer_input(y)(x) <= crossbar_in(var_in);
        end if;
      end loop;
    end loop;
  end process;

-------------------------------------------------------------------------------
-- Generate the muxes----------------------------------------------------------
-------------------------------------------------------------------------------    
  MULT_GEN : for i in 0 to port_num-1 generate
  begin
    crossbar_out(i) <= multiplexer_input(i)(
      to_integer(unsigned(
        crossbar_ctrl((i+1)*port_sel_width-1 downto i*port_sel_width))));
  end generate;




end architecture rtl;
