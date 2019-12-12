-------------------------------------------------------------------------------
-- Title      : Input buffer when virtual channels are used
--              (for credit based flow-control) 
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : vc_input_buffer.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-05-24
-- Last update: 2018-11-28
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: An input buffer consists of vc_num (number of virtual channels)
--              paralell input buffers, whose depth is defined via "vc_depth".
--              The one hot encoded signal vc_write_rx determines in which VC
--              data is written (max. 1). vc_transfer determines from which VC
--              data is transfered to the next router (max. 1).
--              The LSBs of the next flit are forwarded to the the centralized
--              arbiter, as they containing the information req. to route the
--              package of the network if the flit is a head-flit. Also the
--              information which VC contains valid data is provided for the
--              centralized arbiter.
-------------------------------------------------------------------------------
-- Copyright (c) 2018 
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2018-05-24  1.0      bamberg Created
-------------------------------------------------------------------------------

library ieee;
use ieee.math_real.all;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;
use ieee.std_logic_misc.all;
use work.NOC_3D_PACKAGE.all;

entity vc_input_buffer is
  generic(vc_num   : positive    := 4;  -- Virtual channels (VC)
          vc_depth : integer_vec := (4 ,4 ,4 ,4 ));  -- Buff depth of each VC
  port(clk           : in  std_logic;
       rst           : in  std_logic;
       data_rx       : in  flit;
       vc_write_rx   : in  std_logic_vector(vc_num-1 downto 0);  -- Write EN VC
       vc_transfer   : in  std_logic_vector(vc_num-1 downto 0);  -- Read EN VC
       valid_data_vc : out std_logic_vector(vc_num-1 downto 0);  --
       data_transfer : out flit;        --
       -- Information from the header that are required for path-finding
       -- and channel allocation (Destination Address & Packet-Length)
       header        : out header_inf_vector(vc_num-1 downto 0)
       );
end entity vc_input_buffer;


architecture rtl of vc_input_buffer is
  signal buffer_out_vector : flit_vector(vc_num-1 downto 0);

begin
  -----------------------------------------------------------------------------
  ------------- Structural Part - Generate FIFOs for each VC ------------------
  -----------------------------------------------------------------------------
  buffer_gen : for i in 0 to vc_num-1 generate
    fifo_i : entity work.fifo generic map(buff_depth => vc_depth(vc_depth'left+i))
      port map (data_in    => data_rx,
                write_en   => vc_write_rx(i),
                read_en    => vc_transfer(i),
                clk        => clk,
                rst        => rst,
                data_out   => buffer_out_vector(i),
                valid_data => valid_data_vc(i)
                );
  end generate buffer_gen;



  -----------------------------------------------------------------------------
  ------------- Logic Part - Calculate outputs --------------------------------
  -----------------------------------------------------------------------------
  OUTPUT_MUX : if vc_num > 1 generate
    process(buffer_out_vector, vc_transfer)
    begin
      data_transfer <= (others => '-');
      if or_reduce(vc_transfer) = '1' then
        data_transfer <= buffer_out_vector(one_hot2int(vc_transfer));
      end if;
    end process;
  end generate;

  OUTPUT_PASS : if vc_num = 1 generate
    data_transfer <= buffer_out_vector(0);
  end generate;

  HEADER_GEN : for i in 0 to vc_num-1 generate
    header(i) <= get_header_inf(buffer_out_vector(i));
  end generate;

end rtl;

