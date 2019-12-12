-------------------------------------------------------------------------------
-- Title      : Input buffer when virtual channels are used
--              (for credit based flow-control) 
-- Project    : Modular, heterogenous 3D NoC
-------------------------------------------------------------------------------
-- File       : vc_input_buffer_enc_sel.vhd
-- Author     : Lennart Bamberg  <bamberg@office.item.uni-bremen.de>
-- Company    : 
-- Created    : 2018-05-24
-- Last update: 2018-11-28
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: Reasonable only for more than four VCs (=> NOT USED CURRENTLY!)
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
use work.NOC_3D_PACKAGE.all;

entity vc_input_buffer_enc_sel is
  generic(vc_num   : positive    := 4;  -- Virtual channels (VC)
          vc_depth : integer_vec := (4 ,4 ,4 ,4 ));  -- Array including the buffer 
                                              -- depths of each VC
  port(clk             : in  std_logic;     
       rst             : in  std_logic;     
       data_rx         : in  flit;  
       -- "sel" signal to determine in which VC "data_rx" is written
       vc_sel_write_rx : in  std_logic_vector(bit_width(vc_num)-1 downto 0);  --
       -- "sel" signal to determine from which VC "data_rx" is read 
       vc_sel_transfer : in  std_logic_vector(bit_width(vc_num)-1 downto 0);  --
       -- Indicate if "data_rx" is valid new data
       valid_rx        : in  std_logic;       --
       -- Indicate if a flit from the input buffer can be transfered
       transfer        : in  std_logic;       --
       -- Indicate which VC contains valid data
       valid_data_vc   : out std_logic_vector(vc_num-1 downto 0);  --
       data_transfer   : out flit;      --
       -- Credit counter increment signals (One Hot)
       incr_tx         : out std_logic_vector(vc_num-1 downto 0);
       -- Information from the header that are required for path-finding
       -- and channel allocation (Destination Address & Packet-Length)
       header          : out header_inf_vector(vc_num-1 downto 0)
       );
end entity vc_input_buffer_enc_sel;


architecture rtl of vc_input_buffer_enc_sel is
  -- ENR_VC, ENW_VC AND INCR_TX ARE ONE_HOT ENCODED (all zeros also poss)
  signal enr_vc            : std_logic_vector(vc_num-1 downto 0);
  signal enw_vc            : std_logic_vector(vc_num-1 downto 0);
  signal buffer_out_vector : flit_vector(vc_num-1 downto 0);

begin
  -- Structural Part - Generate FIFOs for each VC
  buffer_gen : for i in 0 to vc_num-1 generate
    fifo_i : entity work.fifo generic map(buff_depth => vc_depth(vc_depth'left+i))
      port map (data_in    => data_rx,
                write_en   => enw_vc(i),
                read_en    => enr_vc(i),
                clk        => clk,
                rst        => rst,
                data_out   => buffer_out_vector(i),
                valid_data => valid_data_vc(i)
                );
  end generate buffer_gen;


  -- Geneate "Write-Enable" signals
  process(vc_sel_write_rx, valid_rx)
  begin
    enw_vc                           <= (others => '0');
    enw_vc(slv2int(vc_sel_write_rx)) <= valid_rx;
  end process;

  -- Geneate "Read-Enable" signals (equal incr_txement hear)
  process(transfer, vc_sel_write_rx)
  begin
    enr_vc                           <= (others => '0');
    enr_vc(slv2int(vc_sel_write_rx)) <= transfer;
  end process;
  incr_tx       <= enr_vc;
  -- Generate "data_out"   
  data_transfer <= buffer_out_vector(slv2int(vc_sel_transfer));


  HEADER_GEN : for i in 0 to vc_num-1 generate
    header(i) <= get_header_inf(buffer_out_vector(i));
  end generate;



end rtl;

