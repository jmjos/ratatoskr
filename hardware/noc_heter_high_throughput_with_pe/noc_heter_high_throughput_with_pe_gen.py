1#title			:full_noc_gen.py
#description	:generate a full noc
#author			:Behnam Razi Perjikolaei
#date			:20190604
#version		:1
#usage			:python full_noc_gen.py
#notes			:
#python_version	:3
#==============================================================================

import os
import glob
import subprocess
import itertools
import string
import func
import numpy as np
from string import Template
out_file = 'noc_heter_high_throughput_with_pe.vhd'
#########################################################################################
# 	Network size
#########################################################################################
noc_x=3
noc_y=2
noc_z=2
#########################################################################################
# 	Virtual Channel Number
#########################################################################################
vc_xy=[2, 2, 2]    # Horizontal Ports VC numbers (array size = noc_z)
depth_xy=[4, 4, 8] # Horizontal Ports buffer depth (array size = noc_z)
vc_z =[2, 2]       # Up and Down ports VC numbers (array size = noc_z-1)
depth_z=[8, 8]     # Up and Down ports buffer depth (array size = noc_z-1)
cf_vec=(1, 2, 4) # Clock factor in each layer (this has to be a list)
#########################################################################################
# 	Routing Algorithm (character " is known as \")
#########################################################################################
flit_size= 32
max_vc_num=max(max(vc_xy),max(vc_z))
max_vc_num_out=max(max(vc_xy),max(vc_z))
max_x_dim=noc_x
max_y_dim=noc_y
max_z_dim=noc_z
max_packet_len=31 #(2^N-1)
max_port_num=7
rst_lvl=0
max_buffer_depth=max(max(depth_xy),max(depth_z))
cf=cf_vec[1]
vc_depth_array = func.ret_2D_int_array(max_port_num, max_vc_num, max_buffer_depth)
vc_depth_array_minus= func.ret_2D_int_array(max_port_num-1, max_vc_num, max_buffer_depth)
vc_depth_out_array = func.ret_2D_int_array(max_port_num, max_vc_num_out, max_buffer_depth)
vc_depth_out_array_minus=func.ret_2D_int_array(max_port_num-1, max_vc_num_out, max_buffer_depth)
max_port_exist = "(0, 1, 2, 3, 4, 5, 6)"
port_exist_wo_down="(0, 1, 2, 3, 4, 5)"
port_exist_wo_up="(0, 1, 2, 3, 4, 6)"
max_vc_num_vec=func.ret_int_array(max_port_num, max_vc_num)
max_vc_num_out_vec=func.ret_int_array(max_port_num, max_vc_num_out)
vc_num_vec_minus=func.ret_int_array(max_port_num-1, max_vc_num)
vc_num_out_vec_minus=func.ret_int_array(max_port_num-1, max_vc_num_out)
max_vc_depth=func.ret_int_array(max_vc_num, max_buffer_depth)
rout_algo="\"DXYU\""
clk_period= 2   # clk in ns
rate_percent= [1,1,1] # injection rate percent for each layer
run_time= 2000 # runtime in ns
layer_prob= [(60, 40, 0), (70, 30, 0), (30, 30, 40)] #Probability of injection to the
                                                       #layers for nodes in each layer
credit_num= func.ret_int_array(max_vc_num,max_buffer_depth)
#########################################################################################
# 	MAKE VHDL FILES FROM TXT FILES 
#########################################################################################
subs={ 'flit_size': flit_size, 'max_vc_num': max_vc_num,'max_vc_num_out': max_vc_num_out,
       'max_x_dim': max_x_dim, 'max_y_dim': max_y_dim, 'max_z_dim': max_z_dim,
       'max_packet_len': max_packet_len, 'max_port_num': max_port_num, 'rst_lvl': rst_lvl,
       'max_buffer_depth': max_buffer_depth, 'cf': cf, 'vc_depth_array': vc_depth_array,
       'vc_depth_array_minus': vc_depth_array_minus, 'vc_depth_out_array': vc_depth_out_array,
       'vc_depth_out_array_minus': vc_depth_out_array_minus, 'max_port_exist': max_port_exist,
       'port_exist_wo_down': port_exist_wo_down, 'port_exist_wo_up': port_exist_wo_up,
       'max_vc_num_vec': max_vc_num_vec, 'max_vc_num_out_vec': max_vc_num_out_vec,
       'vc_num_vec_minus': vc_num_vec_minus, 'vc_num_out_vec_minus': vc_num_out_vec_minus,
       'max_vc_depth': max_vc_depth, 'rout_algo': rout_algo, 'cf_vec': str(cf_vec),
       'clk_period': clk_period, 'rate_percent': rate_percent[0], 'run_time': run_time,
       'layer_prob': layer_prob[0], 'N': flit_size, 'credit_num': credit_num}
if not os.path.exists("noc_heter_high_throughput_with_pe"):
    os.makedirs("noc_heter_high_throughput_with_pe")
if not os.path.exists("reports"):
    os.makedirs("reports")
for root, dirs, files in os.walk("./template"):
    for file in files:
        if file.endswith(".txt"):
            text_file = os.path.join(root, file)
            text_dir=os.path.dirname(text_file)
            dest_folder="noc_heter_high_throughput_with_pe/"+os.path.basename(text_dir)
            if not os.path.exists(dest_folder):
                os.makedirs(dest_folder)
            text_file_vhd=text_file[:-3]
            text_file_vhd+='vhd'
            temp_file= open( text_file )
            scr = Template(temp_file.read())
            ft=open(text_file_vhd, 'w')
            ft.write(scr.substitute(subs))
            ft.close()
            dest_file_vhd=dest_folder+"/"+os.path.basename(text_file_vhd)
            os.rename(text_file_vhd, dest_file_vhd)

#########################################################################################
# 	Number of routers and input/output signals in network entity
#########################################################################################
router_num= noc_x * noc_y * noc_z
io_sig_num=sum(vc_xy)*noc_x*noc_y # Number of local ports in layers is equal noc_x*noc_y
                                  # Each local port in layer i has vc_xy[i] VC
data_sig_num=flit_size * noc_x * noc_y * sum(cf_vec) # Each local port in layer i has
                                               # flit_size*cf_vec[i] signal size

max_vc=max(max(vc_xy),max(vc_z))
max_cf=max(cf_vec)
#########################################################################################
#       Opening the file and writing the entity and top of the architecture
#########################################################################################
ft=open(out_file, 'w+')
ft.write(func.entity.substitute(data_sig_num=str(data_sig_num),io_sig_num=str(io_sig_num),cf_num=str(len(cf_vec))))
ft.write(func.archi_top.substitute(noc_x=str(noc_x-1), noc_y=str(noc_y-1), noc_z=str(noc_z-1),
                                   max_cf=str(max_cf), vc_num=str(max_vc)))
#########################################################################################
#       Writing the input and output connection signals in architecture
#########################################################################################
for z in range(noc_z):
  for y in range(noc_y):
    for x in range(noc_x):
      port_num=func.ret_port_num(noc_x, noc_y, noc_z, x, y, z)
      port_len=func.ret_port_len(cf_vec, noc_z, z, port_num)
      ft.write(func.data_in_fast_tmp.substitute(x=str(x),y=str(y),z=str(z),
                                                port_num= str(port_len),flit_size=flit_size))
      vc_num_vec=func.ret_sum_vc(z=z, vc_xy=vc_xy, vc_z=vc_z, noc_z=noc_z, port_num=port_num)
      vc_num_out_vec=func.ret_sum_vc(z=z, vc_xy=vc_xy, vc_z=vc_z, noc_z=noc_z, port_num=port_num)
      ft.write(func.vc_write_rx_vec_tmp.substitute(x=str(x),y=str(y),z=str(z),sum_vc=vc_num_vec))
      ft.write(func.incr_rx_vec_tmp.substitute(x=str(x),y=str(y),z=str(z),sum_vc=vc_num_out_vec))
      ft.write(func.vc_write_tx_pl_vec_tmp.substitute(x=str(x),y=str(y),z=str(z),sum_vc=vc_num_out_vec))
      ft.write(func.incr_tx_pl_vec_tmp.substitute(x=str(x),y=str(y),z=str(z),sum_vc=vc_num_vec))
#########################################################################################
#       writing the begin of the architecture
#########################################################################################
ft.write("""
begin
""")
ft.close()
#########################################################################################
#       writing each router code
#########################################################################################
local_lb=0
for z in range(noc_z):
    for y in range(noc_y):
        for x in range(noc_x):
            local_lb=func.ret_local_lb(x,y,z, noc_x, noc_y, noc_z, cf_vec, flit_size)
            port_num=func.ret_port_num(noc_x, noc_y, noc_z, x, y, z)
            func.ftwrite_router(x,y,z,noc_x, noc_y, noc_z, vc_xy, vc_z, depth_xy, depth_z,
                                rout_algo, cf_vec, local_lb, flit_size)
            func.ftwrite_pe(x, y, z, noc_x, noc_y, noc_z, vc_xy, vc_z, depth_xy, depth_z,
                            rout_algo, cf_vec, local_lb, flit_size, rate_percent, run_time,
                            layer_prob, clk_period)
#########################################################################################
#       writing end of architecture
#########################################################################################
ft=open(out_file,"a")
ft.write("end architecture structural;")
#########################################################################################
#       move full_noc.vhd file to the full_noc folder
#########################################################################################
os.rename("noc_heter_high_throughput_with_pe.vhd", "noc_heter_high_throughput_with_pe/noc_heter_high_throughput_with_pe.vhd")
