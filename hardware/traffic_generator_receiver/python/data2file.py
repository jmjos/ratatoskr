# Project    : Traffic generator for a NoC
# -----------------------------------------
# Description: Convert back the recieved data from the NoC into the initial givnen 
# data for further analysis, line by line comparision of data and publish reports
# Merge the input data and header flits. The system supports multiple header structure
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

import math
import os

# *********************************** Define functions

# Define bit conversion


def bit_length(n):
    if n == 1:
        return 1
    elif n > 1:
        return math.ceil(math.log(n) / math.log(2))


# Define bit conversion


def int2binary(w, z):
    return bin(w)[2:].zfill(z)


# Define bit conversion

def binary_str2int(str_input):
    a = 0
    for x in range(0, len(str_input)):
        b = int(str_input[len(str_input) - x - 1]) * int(2 ** x)
        a += b
    return a


# ****** Define bit to str conversion *******


def bits2a(b):
    return ''.join(chr(int(''.join(x), 2)) for x in zip(*[iter(b)]*8))


# *********************************** Generic variables

output_file = "lena_converted.jpg"  # Must be the same file type in both files
output_file_binary = "data_flits_noc.txt"
output_file_header = "report.txt"  # Header parsing report
output_file_mismatch_report = "mismatch_report.txt"  # Flit mismatch report
input_data_file_noc = "data_header_noc.txt"  # Received data+header from NoC
input_data_file_python = "data_header.txt"  # Initial converted data+header from Python
input_packet_length_file = "packet_header_length.txt"
header_included = True  # header_included in the packet structure
max_x_dim = 4  # starting from 1  //  Must be the same in both files
max_y_dim = 4  # starting from 1  //  Must be the same in both files
max_z_dim = 3  # starting from 1  //  Must be the same in both files
flit_width = 32  # Must be the same in both files
max_packet_len = 31  # (number of flits + header_included) in a packet  //  Must be the same in both files

# *********************************** Internal variables

flit_padding_width = flit_width / 16  # Should not be changed
packet_length_line_counter = 0  # Should not be changed
f1_line_counter = 0  # Should not be changed
f2_line_counter = 0  # Should not be changed
line_counter = 0  # Should not be changed
data_line_counter = 1  # Should not be changed
total_packet_length = 0  # Should not be changed
data2save = ""
header2save = ""
header = ""
mismatch_report = ""
data_reconversion = []


# *******************************************************************************
# ********************************************************************* Main body

# ******************************************************************
# *********************************** Header parsing and make report

with open(input_data_file_noc) as f:
    input_data = f.readlines()

with open(input_packet_length_file) as f:
    packet_length = f.readlines()

line_num = sum(1 for line in open(input_packet_length_file))
x_width = bit_length(max_x_dim)
y_width = bit_length(max_y_dim)
z_width = bit_length(max_z_dim)
packet_length_width = bit_length(max_packet_len)
packet_id_width = math.ceil(math.log(line_num) / math.log(2))

header_total = int(flit_padding_width + packet_id_width + (2 * z_width) + (2 * y_width) + (2 * x_width) + packet_length_width)
header_num = math.ceil(header_total / flit_width)

flit_padding_lsb = (flit_width * header_num) - (packet_id_width + (2 * z_width) + (2 * y_width) + (2 * x_width) + packet_length_width)
flit_padding_msb = (flit_width * header_num) - (int(flit_padding_width) + packet_id_width + (2 * z_width) + (2 * y_width) + (2 * x_width) + packet_length_width)

packet_id_lsb = (flit_width * header_num) - ((2 * z_width) + (2 * y_width) + (2 * x_width) + packet_length_width)
packet_id_msb = (flit_width * header_num) - (packet_id_width + (2 * z_width) + (2 * y_width) + (2 * x_width) + packet_length_width)

source_add_lsb = (flit_width * header_num) - (z_width + y_width + x_width + packet_length_width)
source_add_msb = (flit_width * header_num) - ((2 * z_width) + (2 * y_width) + (2 * x_width) + packet_length_width)

dest_add_lsb = (flit_width * header_num) - packet_length_width
dest_add_msb = (flit_width * header_num) - (z_width + y_width + x_width + packet_length_width)

packet_length_lsb = (flit_width * header_num)
packet_length_msb = (flit_width * header_num) - packet_length_width

counter = 0
for y in range(0, line_num):
    packet_length_temp = packet_length[y]
    for x in range(total_packet_length, (total_packet_length + int(packet_length_temp))):
        if x == total_packet_length:
            for z in range(header_num, 0, -1):
                header += input_data[x+z-1]
            header = header.replace("\n", "")
            counter += 1
            flit_padding = binary_str2int(header[flit_padding_msb: flit_padding_lsb])
            packet_id = binary_str2int(header[packet_id_msb: packet_id_lsb])
            source_add = header[source_add_msb: source_add_lsb]
            dest_add = header[dest_add_msb: dest_add_lsb]
            packet_length_header = binary_str2int(header[packet_length_msb: packet_length_lsb]) - (header_num + int(header_included) - 1)
            header2save += "Flit Padding (Byte): " + str(flit_padding).ljust(10) + "| " + "Packet ID: " + \
                           str(packet_id).ljust(10) + "| " + "Source add (ZYX): " + str(source_add).ljust(15) + "| " + \
                           "Dest add (ZYX): " + str(dest_add).ljust(15) + "| " + "Packet length: " + \
                           str(packet_length_header).ljust(10) + "| " + "Header: " + header + "\n"
            header = ""
        elif (x > total_packet_length + header_num - 1) and (x < ((total_packet_length + int(packet_length_temp)) - 1)):
            data2save += str(input_data[x])
        elif x == (total_packet_length + int(packet_length_temp)) - 1:
            last_line = input_data[x]
            data2save += str(last_line[(flit_padding * 8): len(last_line) - 1]) + "\n"

    total_packet_length += int(packet_length_temp)

if os.path.exists(output_file_header):
    os.remove(output_file_header)

f = open(output_file_header, 'w')
f.write(header2save)
f.close()

if os.path.exists(output_file_binary):
    os.remove(output_file_binary)

f = open(output_file_binary, 'w')
f.write(data2save)
f.close()

# ******************************************************************
# ************************************************ Data Reconversion

input_file_2 = open(output_file_binary, 'r')
lines = input_file_2.read()

myList = [item for item in lines.split('\n')]
newString = ''.join(myList)

for i in range(0, len(newString), 8):
    a = binary_str2int(newString[i:i+8])
    data_reconversion.extend([a])
data_reconversion_bytes = bytes(data_reconversion)

if os.path.exists(output_file):
    os.remove(output_file)

f = open(output_file, 'wb')
f.write(data_reconversion_bytes)
f.close()

# ******************************************************************
# *************************** Compare line by line - Mismatch report

f1 = open(input_data_file_noc, "r")
f2 = open(input_data_file_python, "r")
for line1 in f1:
    f1_line_counter += 1
    for line2 in f2:
        f2_line_counter += 1
        if line1 != line2:
            mismatch_report += "Data mismatch at flit: " + str(f1_line_counter) + "\n"
            mismatch_report += "Input data flit from NoC:    " + str(line1)
            mismatch_report += "Input data flit from Python: " + str(line2)
            mismatch_report += "----------------------------------------------------------------------" + "\n"
        break

if mismatch_report == "":
    mismatch_report = "No mismatch in data!"
if f1_line_counter != f2_line_counter:
    print("Data line counter mismatch!")
f1.close()
f2.close()

if os.path.exists(output_file_mismatch_report):
    os.remove(output_file_mismatch_report)

f = open(output_file_mismatch_report, 'w')
f.write(mismatch_report)
f.close()
