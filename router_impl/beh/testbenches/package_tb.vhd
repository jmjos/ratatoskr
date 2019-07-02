-------------------------------------------------------------------------------
-- Title      : NOC_3D_PACKAGE testbench
-- Project    : 
-------------------------------------------------------------------------------
-- File       : package_tb.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-10-29
-- Last update: 2018-11-15
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
use work.NOC_3D_PACKAGE.all;
use ieee.math_real.all;



entity package_tb is
  generic(bit_width      : integer := 32;
          bit_width_log2 : integer := 5);
end entity;


architecture testbench of package_tb is
  signal int_vec        : integer_vec(4 downto 0) :=(1,2,3,4,5);
  signal sum1, sum2, sum3, sum4, sum5 : integer;
  signal x_int                  : integer := 0;
  signal x_one_hot              : std_logic_vector(bit_width-1 downto 0)
    := (others => '0');
  signal one_hot_decoded_int    : integer := 0;
  signal one_hot_decoded_binary : std_logic_vector(bit_width_log2-1 downto 0);
begin 

  -- test sum
  sum5 <= int_vec_sum(int_vec(0 downto 0));
  sum1 <= int_vec_sum(int_vec(1 downto 0));
  -- Generate Stimuli
  process
  begin
    wait for 1 ns;
    x_int <= x_int+1;
  end process;


  process(x_int)
  begin
    x_one_hot <= (others => '0');
    if x_int < bit_width then
    x_one_hot(x_int) <= '1';
    end if;
  end process;
  -- Use Functions
  one_hot_decoded_int    <= one_hot2int(x_one_hot);
  one_hot_decoded_binary <= one_hot2slv(x_one_hot);

  -- Check results
  process(one_hot_decoded_binary)
  begin
    assert one_hot_decoded_int = x_int report "Bug in one_hot2int"
      severity warning;
    assert slv2int(one_hot_decoded_binary) = x_int report "Bug in one_hot2int"
      severity warning;
  end process;


  -- Check Stop Time
  process
  begin
    wait until (x_int = 32);
    assert false
      report "NORMAL COMPLETION OF SIMULATION"
      severity failure;
  end process;


    --process(x_int)
    --begin
    --  if x_int > 1 then
    --    bit_width_test <= bit_width(x_int);
    --  end if;
    --end process;


end architecture;
