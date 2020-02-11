-------------------------------------------------------------------------------
-- Title      : Routing calc at position (Xis,Yis,Zis) in a
--              A-3D NOC  
-- Project    : modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : routing_calc.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : ITEM.ids, University of Bremen
-- Created    : 2018-04-03
-- Last update: 2018-11-14
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

entity routing_calc is
  generic(
    Xis       : natural := 1;
    Yis       : natural := 1;
    Zis       : natural := 1;
    rout_algo : string  := "DXYU");
  port(
    address : in  address_inf;
    enable  : in  std_logic;
    routing : out std_logic_vector(6 downto 0));
end routing_calc;

architecture rtl of routing_calc is
begin

  ROUTING_DXYU : if rout_algo = "DXYU" generate
    dxyu_routing_1 : entity work.dxyu_routing
      generic map (
        Xis => Xis,
        Yis => Yis,
        Zis => Zis)
      port map (
        address => address,
        enable  => enable,
        routing => routing);
  end generate;


  ROUTING_UXYD : if rout_algo = "UXYD" generate
    dxyu_routing_1 : entity work.uxyd_routing
      generic map (
        Xis => Xis,
        Yis => Yis,
        Zis => Zis)
      port map (
        address => address,
        enable  => enable,
        routing => routing);
  end generate;

  ROUTING_XYZ : if rout_algo = "XYZ" or rout_algo = "XYZ_ref"
  or rout_algo = "XYZ_REF" generate
    dxyu_routing_1 : entity work.xyz_routing
      generic map (
        Xis => Xis,
        Yis => Yis,
        Zis => Zis)
      port map (
        address => address,
        enable  => enable,
        routing => routing);
  end generate;

  ROUTING_ZXY : if rout_algo = "ZXY" generate
    dxyu_routing_1 : entity work.zxy_routing
      generic map (
        Xis => Xis,
        Yis => Yis,
        Zis => Zis)
      port map (
        address => address,
        enable  => enable,
        routing => routing);
  end generate;

end architecture;


