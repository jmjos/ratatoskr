-------------------------------------------------------------------------------
-- Title      : SRL fifo 
-- Project    : NoC testbench generator
-------------------------------------------------------------------------------
-- File       : srl_fifo.vhd
-------------------------------------------------------------------------------
-- Copyright (c)
-- Andrew Mulcock, amulcock@opencores.org
-- Copyright (C) 2008 Authors and OPENCORES.ORG
-- This source file may be used and distributed without
-- restriction provided that this copyright statement is not
-- removed from the file and that any derivative work contains
-- the original copyright notice and the associated disclaimer.
-------------------------------------------------------------------------------
-- Additional copyright (c): 
-- This file has been edited by Seyed Nima Omidsajedi
-- for the purpose of Traffic_Gen project
-------------------------------------------------------------------------------
-- Vesion     : 1.1.0
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.NOC_3D_PACKAGE.all;

entity srl_fifo is
    generic ( buffer_depth : integer := 8 );
    port( 
        data_in      : in     flit;
        data_out     : out    flit;
        rst          : in     std_logic;
        write_en     : in     std_logic;
        read_en      : in     std_logic;
        buffer_full  : out    std_logic;
        buffer_empty : out    std_logic;
        clk          : in     std_logic
    );

end entity ;

architecture rtl of srl_fifo is

constant pointer_vec       : positive := bit_width(buffer_depth);    -- set to number of bits needed to store pointer = log2(buffer_depth)

type	srl_array	is array (buffer_depth-1 downto 0) of flit;
signal	fifo_store		   : srl_array;

signal  pointer            : integer range 0 to buffer_depth - 1;

signal pointer_zero        : std_logic;
signal pointer_full        : std_logic;
signal valid_write         : std_logic;
signal half_full_int       : std_logic_vector( pointer_vec - 1 downto 0);

signal empty               : std_logic := '1';
signal valid_count         : std_logic ;

begin


-- Valid write_en, high when valid to write_en data to the store.
valid_write <= '1' when ( read_en = '1' and write_en = '1' )  
                    or  ( write_en = '1' and pointer_full = '0' ) else '0';

-- data store SRL's
data_srl :process( clk )
begin
    if rising_edge( clk ) then
        if valid_write = '1' then
            fifo_store <= fifo_store( fifo_store'left - 1 downto 0) & data_in;
        end if;
    end if;
end process;
    
data_out <= fifo_store( pointer );


process(clk)
begin
    if rising_edge( clk ) then
        if rst = RST_LVL then
            empty <= '1';
        elsif empty = '1' and write_en = '1' then
            empty <= '0';
        elsif pointer_zero = '1' and read_en = '1' and write_en = '0' then
            empty <= '1';
        end if;
    end if;
end process;



--	W	R	Action
--	0	0	pointer <= pointer
--	0	1	pointer <= pointer - 1	read_en, but no write_en, so less data in counter
--	1	0	pointer <= pointer + 1	write_en, but no read_en, so more data in fifo
--	1	1	pointer <= pointer		read_en and write_en, so same number of words in fifo

valid_count <= '1' when (
                             (write_en = '1' and read_en = '0' and pointer_full = '0' and empty = '0' )
                        or
                             (write_en = '0' and read_en = '1' and pointer_zero = '0' )
                         ) else '0';
process( clk )
begin
    if rising_edge( clk ) then
        if valid_count = '1' then
            if write_en = '1' then
                pointer <= pointer + 1;
            else
                pointer <= pointer - 1;
            end if;
        end if;
    end if;
end process;


-- Detect when pointer is zero and maximum
pointer_zero <= '1' when pointer = 0 else '0';
pointer_full <= '1' when pointer = buffer_depth - 1 else '0';

process(pointer_full)
begin
	if (pointer_full = '1') then
		report "Internal Buffer is Full!" severity failure;
	end if;
end process;

-- assign internal signals to outputs
buffer_full <= pointer_full;  
buffer_empty <= empty;

end rtl;

------------------------------------------------------------------------------------
--
------------------------------------------------------------------------------------


