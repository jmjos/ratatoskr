-------------------------------------------------------------------------------
-- Title      : Package used to define the allowed turns in the used routing
--              algorithms
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : TURNS_3D_NOC.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-10-24
-- Last update: 2018-11-30
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: Package with constant boolean arrays, used to determine which
--              I/0 connections are possible in a router, for a given routing.
--              The package can be easily extended to support more routing al-
--              gorithms, which are then automatically considered for logic
--              minimization (exploits forbidden turns) in the synthesis.
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-10-24  1.0      bamberg Created
-------------------------------------------------------------------------------

library ieee;
use work.NOC_3D_PACKAGE.all;


package TURNS_3D_NOC is

  -- Type used to describe all allowed connections in 2D routers
  -- First index of the array defines the "from" the second the "to"
  type boolean_vector is array(natural range <>) of boolean;
  type turn_table_2D is array(4 downto 0) of boolean_vector(4 downto 0);

  -- Type used to describe all allowed connections in 3D routers
  type turn_table_3D is array(6 downto 0) of boolean_vector(6 downto 0);

  function routes_2D (x : string) return turn_table_2D;
  function routes_3D (x : string) return turn_table_3D;


end package TURNS_3D_NOC;

package body TURNS_3D_NOC is

  function routes_2D(x : string) return turn_table_2D is
    variable y : turn_table_2D := (others => (others => false));
  begin
    if x = "XY" then
      -- From and to "local" always possible, except for U turns
      for i in 0 to 4 loop
        y(i)(int_local) := true;
      end loop;
      y(int_local)(4 downto 0) := (others => true);
      y(int_local)(int_local)  := false;
      -- Path following (no turns) always possible
      y(int_north)(int_south)  := true; y(int_south)(int_north) := true;
      y(int_east)(int_west)    := true; y(int_west)(int_east) := true;
      -- Routing specific remainder
      y(int_west)(int_north)   := true; y(int_west)(int_south) := true;
      y(int_east)(int_north)   := true; y(int_east)(int_south) := true;
    else
      assert false report
        x & " 2D routing not implemented for crossbar & arbiter opt. Implementing full connectivity"
        severity warning;
      y                       := (others => (others => true));
      y(int_local)(int_local) := false; y(int_north)(int_north) := false;
      y(int_east)(int_east)   := false; y(int_south)(int_south) := false;
      y(int_west)(int_west)   := false;
    end if;
    return y;
  end function;


  function routes_3D(x : string) return turn_table_3D is
    variable y : turn_table_3D := (others => (others => false));
  begin
    if (x = "XYZ") or (x = "ZXY") or (x = "DXYU") or (x = "UXYD") then
      -- From and to "local" always possible, except for U-turns
      for i in 0 to 6 loop
        y(i)(int_local) := true;
      end loop;
      y(int_local)(6 downto 0) := (others => true);
      y(int_local)(int_local)  := false;
      -- Path following (no turns) always possible
      y(int_north)(int_south)  := true; y(int_south)(int_north) := true;
      y(int_east)(int_west)    := true; y(int_west)(int_east) := true;
      y(int_down)(int_up)      := true; y(int_up)(int_down) := true;
      -- Routing specific XY
      y(int_west)(int_north)   := true; y(int_west)(int_south) := true;
      y(int_east)(int_north)   := true; y(int_east)(int_south) := true;
      if (x = "XYZ") then
        -- From everywhere to UP and DOWN
        for i in 0 to 6 loop
          y(i)(int_up) := true;
        end loop;
        for i in 0 to 6 loop
          y(i)(int_down) := true;
        end loop;
      elsif (x = "ZXY") then
        -- From UP and DOWN to everywhere 
        y(int_up)(6 downto 0)   := (others => true);
        y(int_down)(6 downto 0) := (others => true);
      elsif (x = "DXYU") then
        -- From UP to everywhere, From Everywhere to UP 
        y(int_up)(6 downto 0) := (others => true);
        for i in 0 to 6 loop
          y(i)(int_up) := true;
        end loop;
      else                              -- UXYD
        -- From DOWN to everywhere, From everywhere to DOWN 
        y(int_down)(6 downto 0) := (others => true);
        for i in 0 to 6 loop
          y(i)(int_down) := true;
        end loop;
      end if;
    else
      assert false report
        x & " 3D routing not implemented for crossbar & arbiter opt. Implementing full connectivity"
        severity warning;
      y                       := (others => (others => true));
      y(int_local)(int_local) := false; y(int_north)(int_north) := false;
      y(int_east)(int_east)   := false; y(int_south)(int_south) := false;
      y(int_west)(int_west)   := false;
    end if;

    -- Always  exclude U-turns for Z-direction
    y(int_up)(int_up)     := false;
    y(int_down)(int_down) := false;

    return y;
  end function;

end package body TURNS_3D_NOC;
