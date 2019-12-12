-------------------------------------------------------------------------------
-- Title      : Round Robin arbiter
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : NOC_3D_PACKAGE.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-10-24
-- Last update: 2018-11-28
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: Package including the constants, types, function and components 
--              required for the modular, heterogenous 3D NoC 
-------------------------------------------------------------------------------
-- HUGE PARTS OF THIS FILE ARE ADOPTED FROM A FILE BY BENJAMIN KRILL, DISTRI-
-- BUTED USING THE FOLLOWING COPYRIGHT NOTE:
-- Copyright (c) 2009 Benjamin Krill <benjamin@krll.de>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2009-XX-XX  0.0      krll    Created
-- 2018-10-24  1.0      bamberg Modified for 3D NoC project
-------------------------------------------------------------------------------

-- -----------------------------------------------------------------------------
-- -----------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.NOC_3D_PACKAGE.all;

entity rr_arbiter is
  generic (CNT : integer := 7);
  port (
    clk : in std_logic;
    rst : in std_logic;

    req   : in  std_logic_vector(CNT-1 downto 0);
    ack   : in  std_logic;
    grant : out std_logic_vector(CNT-1 downto 0)
    );
end;

architecture rr_arbiter of rr_arbiter is
  signal grant_q  : std_logic_vector(CNT-1 downto 0);
  signal pre_req  : std_logic_vector(CNT-1 downto 0);
  signal sel_gnt  : std_logic_vector(CNT-1 downto 0);
  signal isol_lsb : std_logic_vector(CNT-1 downto 0);
  signal mask_pre : std_logic_vector(CNT-1 downto 0);
  signal win      : std_logic_vector(CNT-1 downto 0);
begin
  grant    <= grant_q;
  -- Mask off previous winners
  mask_pre <= req and not (std_logic_vector(unsigned(pre_req) - 1) or pre_req);
  -- Select new winner
  sel_gnt  <= mask_pre and std_logic_vector(unsigned(not(mask_pre)) + 1);
  -- Isolate least significant set bit.
  isol_lsb <= req and std_logic_vector(unsigned(not(req)) + 1);
  win      <= sel_gnt when mask_pre /= (CNT-1 downto 0 => '0') else isol_lsb;

  process (clk, rst)
  begin
    if rst = RST_LVL then
      pre_req <= (others => '0');
      grant_q <= (others => '0');
    elsif rising_edge(clk) then
      --grant_q <= grant_q; -- WHY SHOULD IT BE NESSECARY??
      --pre_req <= pre_req; -- WHY SHOULD IT BE NESSECARY??
      if grant_q = (CNT-1 downto 0 => '0') or ack = '1' then
        --if win /= (CNT-1 downto 0 => '0') then -- maybe comment it to reduce
                -- complexity, then, if no
                        -- request is left, the mask
                                -- would be just reset. This
                                        -- will even increse flit/packet level
                                        -- fairness (decr. port fairness)
        pre_req <= win;
        --end if;
        grant_q <= win;
      end if;
    end if;
  end process;

end rr_arbiter;
