#!/bin/python

# Copyright 2018 Jan Moritz Joseph

# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
###############################################################################
import os
import sys
sys.path.insert(0, '..')
from configure import Configuration
###############################################################################


def copy_folders(source_folder, destination_folder, folders):
    """ Copy the vhdl folders from source to destination """
    copy = 'cp -r'
    for folder in folders:
        os.system(copy + ' ' + source_folder + folder + ' ' +
                  destination_folder)
###############################################################################


def replace_brackets(string):
    string = string.replace('[', '(')
    string = string.replace(']', ')')
    return string
###############################################################################


def increasing_vec(length):
    """ Create a vector starting with values from 0 to length-1 """
    string = str(list(range(0, length)))
    return replace_brackets(string)
###############################################################################


def repeat_value(value, length, filename):
    """ Create a vector of length-1 with a repeated value """
    if filename == 'router_pl.vhd':
        lst = [value] * length
        lst[0] = 1
    else:
        lst = [value] * length
    return replace_brackets(str(lst))
###############################################################################


def depth_mtx(rows, columns, value):
    """ Create a matrix of rows and columns with a repeated value """
    row = [value] * columns
    mtx = [row] * rows
    return replace_brackets(str(mtx))
###############################################################################


def read_content(path):
    f = open(path, 'r')
    content = f.read()
    f.close()
    return content
###############################################################################


def write_content(content, path):
    f = open(path, 'w')
    f.write(content)
    f.close()
###############################################################################


def update_NOC_vhd(content, path, config):
    if 'constant flit_size      : positive := 64' in content:
        content = content.replace('64', str(config.flitSize))
    if 'constant max_vc_num     : positive := 2' in content:
        content = content.replace('2', str(config.vcCount))
    if 'constant max_x_dim      : positive := 4' in content:
        content = content.replace('4', str(config.x))
    if 'constant max_y_dim      : positive := 4' in content:
        content = content.replace('4', str(config.y))
    if 'constant max_z_dim      : positive := 4' in content:
        content = content.replace('4', str(config.z))
    if 'constant max_packet_len : positive := 15' in content:
        content = content.replace('15', str(config.flitsPerPacket))
    write_content(content, path)
###############################################################################


def update_router_vhd(content, path, config, filename):
    if 'port_num                     : integer       := 7' in content:
        content = content.replace('7', str(config.portNum))
    if 'Xis                          : natural       := 1' in content:
        content = content.replace('1', str(config.Xis))
    if 'Yis                          : natural       := 1' in content:
        content = content.replace('1', str(config.Yis))
    if 'Zis                          : natural       := 1' in content:
        content = content.replace('1', str(config.Zis))
    if 'port_exist                   : integer_vec   := (0, 1, 2, 3, 4, 5, 6)' in content:
        content = content.replace('(0, 1, 2, 3, 4, 5, 6)', increasing_vec(config.portNum))
    if 'vc_num_vec                   : integer_vec   := (2, 2, 2, 2, 2, 2, 2)' in content:
        repeated_str = repeat_value(config.vcCount, config.portNum, filename)
        content = content.replace('(2, 2, 2, 2, 2, 2, 2)', repeated_str)
    if 'vc_num_out_vec               : integer_vec   := (2, 2, 2, 2, 2, 2, 2)' in content:
        content = content.replace('(2, 2, 2, 2, 2, 2, 2)', repeated_str)
    if 'vc_depth_array               : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4))' in content:
        depth_str = depth_mtx(config.portNum, config.vcCount, config.bufferDepth)
        content = content.replace('((4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4))', depth_str)
    if 'vc_depth_out_array           : vc_prop_int_array := ((4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4));' in content:
        content = content.replace('((4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4), (4, 4))', depth_str)
    if 'rout_algo                    : string        := "XYZ_ref"' in content:
        content = content.replace('XYZ_ref', config.routAlgo)
    write_content(content, path)
###############################################################################


def update_routerpl_vhd(content, path, config, filename):
    if 'port_num                     : integer       := 7' in content:
        content = content.replace('7', str(config.portNum))
    if 'Xis                          : natural       := 1' in content:
        content = content.replace('1', str(config.Xis))
    if 'Yis                          : natural       := 1' in content:
        content = content.replace('1', str(config.Yis))
    if 'Zis                          : natural       := 1' in content:
        content = content.replace('1', str(config.Zis))
    if 'port_exist                   : integer_vec   := (0, 1, 2, 3, 4, 5, 6)' in content:
        content = content.replace('(0, 1, 2, 3, 4, 5, 6)', increasing_vec(config.portNum))
    if 'vc_num_vec                   : integer_vec   := (1, 2, 2, 2, 2, 2, 2)' in content:
        repeated_str = repeat_value(config.vcCount, config.portNum, filename)
        content = content.replace('(1, 2, 2, 2, 2, 2, 2)', repeated_str)
    if 'vc_num_out_vec               : integer_vec   := (2, 2, 2, 2, 2, 2, 2)' in content:
        content = content.replace('(1, 2, 2, 2, 2, 2, 2)', repeated_str)
    if 'vc_depth_array               : vc_prop_int_array := ((2, 2), (2, 2), (2, 2), (2, 2), (2, 2), (2, 2), (2, 2))' in content:
        depth_str = depth_mtx(config.portNum, config.vcCount, config.vcCount)
        content = content.replace('((2, 2), (2, 2), (2, 2), (2, 2), (2, 2), (2, 2), (2, 2))', depth_str)
    if 'vc_depth_out_array           : vc_prop_int_array := ((2, 2), (2, 2), (2, 2), (2, 2), (2, 2), (2, 2), (2, 2))' in content:
        content = content.replace('((2, 2), (2, 2), (2, 2), (2, 2), (2, 2), (2, 2), (2, 2))', depth_str)
    if 'rout_algo                    : string        := "DXYU"' in content:
        content = content.replace('DXYU', config.routAlgo)
    write_content(content, path)
###############################################################################


def update_routingcalc_vhd(content, path, config):
    if 'Xis       : natural := 1' in content:
        content = content.replace('1', str(config.Xis))
    if 'Yis       : natural := 1' in content:
        content = content.replace('1', str(config.Yis))
    if 'Zis       : natural := 1' in content:
        content = content.replace('1', str(config.Zis))
    if 'rout_algo : string  := "DXYU"' in content:
        content = content.replace('DXYU', config.routAlgo)
    write_content(content, path)
###############################################################################


def update_routing(content, path, config):
    if 'Xis : natural := 1' in content:
        content = content.replace('1', str(config.Xis))
    if 'Yis : natural := 1' in content:
        content = content.replace('1', str(config.Yis))
    if 'Zis : natural := 1' in content:
        content = content.replace('1', str(config.Zis))
    write_content(content, path)
###############################################################################


def setup(destination_folder, folders, config):
    """ Setup the vhdl files """
    for folder in folders:
        for filename in os.listdir(destination_folder + folder):
            path = destination_folder + folder + '/' + filename
            content = read_content(path)
            if filename == 'NOC_3D_PACKAGE.vhd':
                update_NOC_vhd(content, path, config)
                continue
            if filename == 'router.vhd':
                update_router_vhd(content, path, config, filename)
                continue
            if filename == 'router_pl.vhd':
                update_routerpl_vhd(content, path, config, filename)
                continue
            if filename == 'routing_calc':
                update_routingcalc_vhd(content, path, config)
                continue
            if filename in ['dxyu_routing.vhd', 'uxyd_routing.vhd',
                            'xy_routing.vhd', 'xyz_routing.vhd',
                            'zxy_routing.vhd']:
                update_routing(content, path, config)
                continue
###############################################################################


def main():
    """ Main execution point """
    try:
        source_folder = sys.argv[1]
        destination_folder = sys.argv[2]
    except Exception:
        print('Please provide the path to source and destination folders.')
    else:
        folders = ['packages', 'router', 'routing_algos']
        config = Configuration('../config.ini')
        copy_folders(source_folder, destination_folder, folders)
        setup(destination_folder, folders, config)
###############################################################################


if __name__ == '__main__':
    main()
