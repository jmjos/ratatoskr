#title                  :full_noc_gen.py
#description    :generate a full noc
#author                 :Behnam Razi Perjikolaei
#date                   :20190604
#version                :1
#usage                  :python full_noc_gen.py
#notes                  :
#python_version :3
#==============================================================================
import sys
sys.path.insert(0, '../bin')
import configparser
import os
import glob
import subprocess
import itertools
import string
import func
import numpy as np
from string import Template
out_file = 'noc_heter_with_pe.vhd'

class Configuration:
    """ The main configuration """
    def __init__(self, path):
        self.path = path
        config = configparser.ConfigParser()
        try:
            config.read(self.path)
        except Exception:
            raise
        self.simulationTime = int(config['Config']['simulationTime'])
        self.flitsPerPacket = int(config['Config']['flitsPerPacket'])
        self.benchmark = config['Config']['benchmark']
        self.libDir = config['Config']['libDir']

        self.simDir = config['Synthetic']['simDir']
        self.basedir = os.getcwd()
        self.restarts = int(config['Synthetic']['restarts'])
        self.warmupStart = int(config['Synthetic']['warmupStart'])
        self.warmupDuration = int(config['Synthetic']['warmupDuration'])
        self.warmupRate = float(config['Synthetic']['warmupRate'])
        self.runRateMin = float(config['Synthetic']['runRateMin'])
        self.runRateMax = float(config['Synthetic']['runRateMax'])
        self.runRateStep = float(config['Synthetic']['runRateStep'])
        self.runStartAfterWarmup = int(config['Synthetic']['runStartAfterWarmup'])
        self.runStart = self.warmupStart + self.warmupDuration + self.runStartAfterWarmup
        self.runDuration = int(config['Synthetic']['runDuration'])
        
        self.bufferReportRouters = config['Report']['bufferReportRouters']
        try:
                self.bufferReportRouters = self.bufferReportRouters[1:len(self.bufferReportRouters)-1]
        except Exception:
                raise

        self.x = config['Hardware']['x']
        self.y = config['Hardware']['y']
        try:
                self.x = self.x[1:len(self.x)-1]
                self.x = self.x.split(',')
                self.x = [ int(x) for x in self.x ]
                self.y = self.y[1:len(self.y)-1]
                self.y = self.y.split(',')
                self.y = [ int(y) for y in self.y ]
        except Exception:
                raise
        self.z = int(config['Hardware']['z'])
        self.routing = config['Hardware']['routing']
        self.clockDelay = config['Hardware']['clockDelay']
        try:
                self.clockDelay = self.clockDelay[1:len(self.clockDelay)-1]
                self.clockDelay = self.clockDelay.split(',')
                self.clockDelay = [ int(cd) for cd in self.clockDelay ]
        except Exception:
                raise
        self.bufferDepthType = config['Hardware']['bufferDepthType']
        self.bufferDepth = int(config['Hardware']['bufferDepth'])
        self.buffersDepths = config['Hardware']['buffersDepths']
        self.buffersDepths = self.buffersDepths[1:len(self.buffersDepths)-1]
        self.vcCount = int(config['Hardware']['vcCount'])
        self.topologyFile = config['Hardware']['topologyFile']
        self.flitSize = int(config['Hardware']['flitSize'])
        self.portNum = int(config['Hardware']['portNum'])
###############################################################################

def main():
    config = Configuration('../../bin/config.ini')
    #########################################################################################
    #       Network size
    #########################################################################################
    noc_x=config.x[0] #3
    noc_y=config.y[0] #3
    noc_z=config.z    #3
    #########################################################################################
    #       Virtual Channel Number
    #########################################################################################
    vc_num=4
    vc_xy=[4, 4, 4]    # Horizontal Ports VC numbers (array size = noc_z)
    depth_xy=[4, 4, 4] # Horizontal Ports buffer depth (array size = noc_z)
    vc_z =[4, 4]       # Up and Down ports VC numbers (array size = noc_z-1)
    depth_z=[8, 8]     # Up and Down ports buffer depth (array size = noc_z-1)
    #########################################################################################
    #       Routing Algorithm (character " is known as \")
    #########################################################################################
    flit_size= 32
    max_vc_num=max(max(vc_xy),max(vc_z))
    max_vc_num_out=max(max(vc_xy),max(vc_z))
    max_x_dim=noc_x
    max_y_dim=noc_y
    max_z_dim=noc_z
    max_packet_len=31
    max_port_num=7
    rst_lvl=0
    max_buffer_depth=max(max(depth_xy),max(depth_z))
    cf=4
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
    cf_vec="(1, 1, 1)"
    clk_period= 2   # clk in ns
    rate_percent= [1,1,1] # injection rate percent for each layer
    run_time= 2000 # runtime in ns
    layer_prob= [(10, 40, 50), (30, 40, 30), (30, 30, 40)] #Probability of injection to the
                                                           #layers for nodes in each layer
    credit_num= func.ret_int_array(max_vc_num,max_buffer_depth)
    #########################################################################################
    #       MAKE VHDL FILES FROM TXT FILES
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
           'max_vc_depth': max_vc_depth, 'rout_algo': rout_algo, 'cf_vec': cf_vec,
           'clk_period': clk_period, 'rate_percent': rate_percent[0], 'run_time':run_time,
           'layer_prob': layer_prob[0], 'N': flit_size, 'credit_num': credit_num}
    if not os.path.exists("noc_heter_with_pe"):
        os.makedirs("noc_heter_with_pe")
    if not os.path.exists("reports"):
        os.makedirs("reports")
    for root, dirs, files in os.walk("./template"):
        for file in files:
            if file.endswith(".txt"):
                text_file = os.path.join(root, file)
                text_dir=os.path.dirname(text_file)
                dest_folder="noc_heter_with_pe/"+os.path.basename(text_dir)
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
    #       Number of routers and input/output signals in network entity
    #########################################################################################
    router_num= noc_x * noc_y * noc_z
    io_sig_num=sum(vc_xy)*noc_x*noc_y
    max_vc=max(max(vc_xy),max(vc_z))
    #########################################################################################
    #       Opening the file and writing the entity and top of the architecture
    #########################################################################################
    ft=open(out_file, 'w+')
    ft.write(func.entity.substitute(router_num=str(router_num),io_sig_num=str(io_sig_num)))
    ft.write(func.archi_top.substitute(noc_x=str(noc_x-1), noc_y=str(noc_y-1), noc_z=str(noc_z-1),
                                       vc_num=str(max_vc), router_num=router_num, io_sig_num=io_sig_num))
    #########################################################################################
    #       Writing the input and output connection signals in architecture
    #########################################################################################
    for z in range(noc_z):
        for y in range(noc_y):
            for x in range(noc_x):
                port_num=7
                if (y == noc_y-1):
                    port_num-=1
                if (x == noc_x-1):
                    port_num-=1
                if (y == 0):
                    port_num-=1
                if (x == 0):
                    port_num-=1
                if (z == noc_z-1):
                    port_num-=1
                if (z == 0):
                    port_num-=1
                ft.write(func.data_in_tmp.substitute(x=str(x),y=str(y),z=str(z),port_num=str(port_num)))
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
    for z in range(noc_z):
        for y in range(noc_y):
            for x in range(noc_x):
                func.ftwrite_router(x,y,z,noc_x, noc_y, noc_z, vc_xy, vc_z, vc_num, depth_xy, depth_z, rout_algo,
                                    cf_vec, clk_period, flit_size, credit_num, rate_percent, run_time, layer_prob)
    #########################################################################################
    #       writing end of architecture
    #########################################################################################
    ft=open(out_file,"a")
    ft.write("end architecture structural;")


    os.rename("noc_heter_with_pe.vhd", "noc_heter_with_pe/noc_heter_with_pe.vhd")

if __name__ == '__main__':
    main()
