-------------------------------------------------------------------------------
-- Title      : ZXY routing for a router at position (Xis,Yis,Zis) in a
--              A-3D NOC  
-- Project    : modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : zxy_routing.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : ITEM.ids, University of Bremen
-- Created    : 2018-04-03
-- Last update: 2018-11-13
-- Platform   : Linux Debian 8
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-04-03  1.0      bamberg Created
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.all;
use ieee.numeric_std.all;
use work.NOC_3D_PACKAGE.all;

entity zxy_routing is
  generic(
    Xis : natural := 1;
    Yis : natural := 1;
    Zis : natural := 1);
  --port_num : positive := 7;)
  port(
    address : in  address_inf;
    enable  : in  std_logic;
    -- in dependence of the possible routes not all bit of "routing" are used
    routing : out std_logic_vector(6 downto 0));
end zxy_routing;

architecture rtl of zxy_routing is
begin

  process(address, enable)
  begin
    routing <= (others => '0');
    if enable = '1' then
      if (to_integer(unsigned(address.z_dest)) < Zis) then
        routing(int_down) <= '1';       -- Route Down
      elsif (to_integer(unsigned(address.z_dest)) > Zis) then
        routing(int_up) <= '1';         -- Route pos. Z
      elsif (to_integer(unsigned(address.x_dest)) < Xis) then
        routing(int_west) <= '1';       -- Route neg. X
      elsif (to_integer(unsigned(address.x_dest)) > Xis) then
        routing(int_east) <= '1';       -- Route pos. X
      elsif (to_integer(unsigned(address.y_dest)) < Yis) then
        routing(int_south) <= '1';      -- Route neg. Y
      elsif (to_integer(unsigned(address.y_dest)) > Yis) then
        routing(int_north) <= '1';      -- Route pos. Y
      else
        routing(int_local) <= '1';      -- Route local
      end if;
    end if;
  end process;
end architecture;


