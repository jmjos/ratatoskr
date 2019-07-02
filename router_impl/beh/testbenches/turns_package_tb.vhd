-------------------------------------------------------------------------------
-- Title      : NOC_3D_PACKAGE testbench
-- Project    : 
-------------------------------------------------------------------------------
-- File       : package_tb.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-10-29
-- Last update: 2018-11-01
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: Tests some function of NOC_3D_PACKAGE but not all
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-10-29  1.0      bamberg Created
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.TURNS_3D_NOC.all;




entity turns_package_tb is
  generic(
    routing_3D : string := "XXX";
    routing_2D : string := "XY"
    );
end entity;


architecture testbench of turns_package_tb is
  constant poss_routes_3D : turn_table_3D := routes_3D(routing_3D);
  constant poss_routes_2D : turn_table_2D := routes_2D(routing_2D);

  type array5x5 is array(4 downto 0) of std_logic_vector(4 downto 0);
  type array7x7 is array(6 downto 0) of std_logic_vector(6 downto 0);

  signal x3D : array7x7 := (others => (others => '1'));
  signal x2D : array5x5 := (others => (others => '1'));
  signal y3D : array7x7;
  signal y2D : array5x5;
  -- Only for VCD required as it can not show array
  signal y3D_0, y3D_1, y3D_2, y3D_3, y3D_4, y3D_5, y3D_6
    : std_logic_vector(6 downto 0);
  signal y2D_0, y2D_1, y2D_2, y2D_3, y2D_4
    : std_logic_vector(4 downto 0);
begin

  y3D_0 <= y3d(0);
  y3D_1 <= y3d(1);
  y3D_2 <= y3d(2);
  y3D_3 <= y3d(3);
  y3D_4 <= y3d(4);
  y3D_5 <= y3d(5);
  y3D_6 <= y3d(6);
  y2D_0 <= y2d(0);
  y2D_1 <= y2d(1);
  y2D_2 <= y2d(2);
  y2D_3 <= y2d(3);
  y2D_4 <= y2d(4);
  process
  begin
    wait for 1 ns;
    x3D <= (others => (others => '0'));
    x2D <= (others => (others => '0'));
    wait for 1 ns;
    x3D <= (others => (others => '1'));
    x2D <= (others => (others => '1'));
    wait for 2 ns;
    assert false report "NORMAL COMPLETION OF SIMULATION"
      severity failure;
  end process;

  process(x3D)
  begin
    GEN_AND1 : for i in 0 to 6 loop
      GEN_END2 : for j in 0 to 6 loop
        if poss_routes_3D(i)(j) then
          y3D(i)(j) <= x3D(i)(j);
        else
          y3D(i)(j) <= '0';
        end if;
      end loop;
    end loop;
  end process;






  process(x2D)
  begin
    GEN_AND1 : for i in 0 to 4 loop
      GEN_END2 : for j in 0 to 4 loop
        if poss_routes_2D(i)(j) then
          y2D(i)(j) <= x2D(i)(j);
        else
          y2D(i)(j) <= '-';
        end if;
      end loop;
    end loop;
  end process;
end architecture;
