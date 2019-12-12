# Project    : Traffic generator for a NoC
# -----------------------------------------
# Description: Generate random data then convert it to uint8 and make further
# processes for using the converted data (in the binary system) in a real 
# digital system. The system supports multiple header structure
# -----------------------------------------
# File       : data2file.py
# Author     : Seyed Nima Omidsajedi  <nima@omidsajedi.com>
# Company    : University of Bremen
# -----------------------------------------
# Copyright (c) 2019
# -----------------------------------------
# Vesion     : 2.1.0
# -----------------------------------------
# Header structure:
# -----------------------------------------------------------------------------------------------
# |              |           |       |       |       |        |        |        |               |
# | Flit_padding | Packet_id | Z_src | Y_src | X_src | Z_dest | Y_dest | X_dest | Packet_length |
# |              |           |       |       |       |        |        |        |               |
# -----------------------------------------------------------------------------------------------

import os
import sys
import math
import random
import numpy as np
from PIL import Image

# *********************************** Define functions


# Define flit division

def data2flit(s, n):
    for start in range(0, len(s), n):
        yield s[start:start+n]


# Define bit conversion


def bit_length(n):
    if n == 1:
        return 1
    elif n > 1:
        return math.ceil(math.log(n) / math.log(2))


# Define bit conversion


def int2binary(w, z):
    return bin(w)[2:].zfill(z)


# *********************************** Generic variables

output_file = "data_flits.txt"   # Output file will be in binary (Unsigned number)
output_packet_length_file = "packet_length.txt"
output_inj_time_file = "injection_time.txt"  # Injection time in C.C
output_data_header_file = "data_header.txt"
output_packet_header_length = "packet_header_length.txt"
header_included = True  # header_included in the packet structure
max_x_dim = 4  # starting from 1 //  Must be the same in both files + VHDL files
max_y_dim = 4  # starting from 1 //  Must be the same in both files + VHDL files
max_z_dim = 3  # starting from 1 //  Must be the same in both files + VHDL files
flit_width = 32  # Must be the same in both files + VHDL files
max_packet_len = 31  # (number of flits + header_included) in a packet //  Must be the same in both files + VHDL files
src_address = (0, 2, 1)  # Z Y X  //   starting from 0
dest_address = (0, 0, 1)  # Z Y X   //  starting from 0
lower_range_packet_length = 3  # Lower range of packet_length for random function
upper_range_packet_length = 3  # Upper range of packet_length for random function
mu, sigma = 30, 5  # Mean and Standard deviation for the injection time
data_num = 6776  # The number of generated random data


# *********************************** Internal variables

data_flits = ""
data_save = ""
packet_num = 0  # Should not be changed
packet_line_counter = 0  # Should not be changed
final_inj_time = 0  # Should not be changed
data_packet_length = ""
data_inj_time = ""
flit_padding_width = flit_width / 16  # Should not be changed
packet_length_line_counter = 0  # Should not be changed
packet_header_length = 0  # Should not be changed
packet_id = 0  # Should not be changed
data_line_counter = 0  # Should not be changed
header = []
header_temp = ""
data2save = ""
packet_header2save = ""
flit_padding_bin = ""

# *******************************************************************************
# ********************************************************************* Main body

# ******************************************************************
# ************************************************** Data Generation

data_rand = [random.randint(0, 2**flit_width) for _ in range(data_num)]
for i in range(data_num):
    data_save += int2binary(data_rand[i], flit_width) + "\n"

if os.path.exists(output_file):
    os.remove(output_file)

f = open(output_file, 'w')
f.write(data_save)
f.close()

# ******************************************************************
# ******************************************************** Data info

# Determine packet length
line_num = data_num
while packet_num <= line_num:
    packet_length_temp = random.randint(lower_range_packet_length, upper_range_packet_length)
    if packet_num + packet_length_temp < line_num:
        packet_num = packet_length_temp + packet_num
        packet_line_counter += 1
        data_packet_length += str(packet_length_temp) + "\n"
    else:
        packet_length_temp = line_num - packet_num
        data_packet_length += str(packet_length_temp)
        packet_line_counter += 1
        break

if os.path.exists(output_packet_length_file):
    os.remove(output_packet_length_file)

f = open(output_packet_length_file, 'w')
f.write(data_packet_length)
f.close()

# Data injection time

for inj_time in range(packet_line_counter):
    random_inj_time = abs(round(np.random.normal(mu, sigma)))
    if random_inj_time < lower_range_packet_length:
        random_inj_time = random_inj_time + lower_range_packet_length + 1
    final_inj_time = final_inj_time + random_inj_time
    data_inj_time += str(final_inj_time) + "\n"

if os.path.exists(output_inj_time_file):
    os.remove(output_inj_time_file)

g = open(output_inj_time_file, 'w')
g.write(str(data_inj_time))
g.close()

# ******************************************************************
# ************************************************* Data_header_info

x_width = bit_length(max_x_dim)
y_width = bit_length(max_y_dim)
z_width = bit_length(max_z_dim)
packet_id_width = math.ceil(math.log(packet_line_counter) / math.log(2))
packet_length_width = bit_length(max_packet_len)

header_total = int(flit_padding_width + packet_id_width + (2 * z_width) + (2 * y_width) + (2 * x_width) + packet_length_width)
header_num = math.ceil(header_total / flit_width)


if (src_address[0] >= max_x_dim) | (dest_address[0] >= max_x_dim):
    sys.exit('Fatal error: the router address exceeded the maximum router number!')
if (src_address[1] >= max_y_dim) | (dest_address[1] >= max_y_dim):
    sys.exit('Fatal error: the router address exceeded the maximum router number!')
if (src_address[2] >= max_z_dim) | (dest_address[2] >= max_z_dim):
    sys.exit('Fatal error: the router address exceeded the maximum router number!')
if upper_range_packet_length + header_num > max_packet_len:
    sys.exit('Fatal error: the entered packet length exceeded the maximum "packet + header" number!')

with open(output_file) as f:
    input_data = f.readlines()

input_data_line_num = sum(1 for line in open(output_file))
last_line_data = input_data[input_data_line_num - 1]

with open(output_packet_length_file) as f:
    packet_length = f.readlines()

for x in range(0, len(packet_length)):

    if x < len(packet_length) - 1:
        flit_padding = 0
    elif x == len(packet_length) - 1:
        flit_padding_bit = flit_width - len(last_line_data) + 1
        flit_padding = flit_padding_bit / 8

    header_flit = int2binary(int(flit_padding), int(flit_padding_width)) + \
                  int2binary(packet_id, packet_id_width) + \
                  int2binary(src_address[0], z_width) + \
                  int2binary(src_address[1], y_width) + \
                  int2binary(src_address[2], x_width) + \
                  int2binary(dest_address[0], z_width) + \
                  int2binary(dest_address[1], y_width) + \
                  int2binary(dest_address[2], x_width) + \
                  int2binary(int(packet_length[packet_length_line_counter]) + int(header_included) + header_num - 1, packet_length_width)

    packet_id = packet_id + 1
    packet_length_line_counter = packet_length_line_counter + 1
    packet_header_length = int(packet_length[x]) + header_num
    packet_header2save += str(packet_header_length) + "\n"

    if len(header_flit) > flit_width:
        header_flit = header_flit.rjust(flit_width*header_num, '1')
        header_temp = header_flit[(header_num - 1) * flit_width: len(header_flit)]
        data2save += header_temp + "\n"
        for i in range(header_num-1, 0, -1):
            header_temp = header_flit[flit_width * (i-1): flit_width * i]
            data2save += header_temp + "\n"
    else:
        header_flit = header_flit.rjust(flit_width, '1')
        data2save += header_flit + "\n"

    if int(packet_length[x]) > max_packet_len:
        sys.exit('Fatal error: the packet_length exceeded the maximum legal number!')

    for y in range(0, int(packet_length[x])):
        data2save += str(input_data[data_line_counter])
        data_line_counter = data_line_counter + 1

if os.path.exists(output_data_header_file):
    os.remove(output_data_header_file)

f = open(output_data_header_file, 'w')
f.write(data2save)
f.close()

if os.path.exists(output_packet_header_length):
    os.remove(output_packet_header_length)

f = open(output_packet_header_length, 'w')
f.write(packet_header2save)
f.close()


# Reports
print(type(input_data))
print("******************************")
print("Number of flits: %i" % line_num)
print("******************************")
print("Number of packets: %i" % packet_line_counter)
print("******************************")
print("Number of headers: %i" % header_num)
print("******************************")
print("Required bits for packet_id: %i" % packet_id_width)
print("******************************")

