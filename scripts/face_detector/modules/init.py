import xml.etree.ElementTree as ET
from xml.dom import minidom
import math
import configparser
#################################################################################
# Global Variables

# configparser object
config = None

# Parameters for calculating the number of tiles (processors) and the probabilites
frame_width = None # S_x
frame_height = None # S_y
tile_size = None # S_VJ
num_tiles = None
#################################################################################
# Parameters of communication

# STEP 1

# Vertical data (camera->proc)
R_i = None # R_i, the rate of reinitializing step 1 and 2
R_min = None # R_min, the rate of reinitializing step 3
max_face_size = None # S_{G}^{max}
num_color_channels = None # N_c
bit_depth = None # D
image_details = None
num = None  # In the pdf, it says this is a number but what number exactly
# min_num_face = None # N_{G}^{min}
# max_num_faces = None # N_{G}^{max}
num_found_faces = None # N_G
position_size = None
likelihood_size = None
winner_data = None

# Horizontal data (proc->proc in layer1)
face_found_size = None
proc_proc_data = None # data sent (proc->proc)

# Data to be sent
camera_proc_rate = None
proc_proc_rate = None
winner_master_rate = None
#################################################################################
# STEP 2

# Vertical data between layer2 and the camera
lay2_cam_data = None # (layer2 -> camera)
cam_lay2_data = None # (camera -> layer2)

# Horizontal data (proc->proc in layer2)
shadow2_master2_data = None
num_found_features = None # An assumption has been made, which is we fixed the number of discovered features for all nodes

# Data to be sent
lay2_cam_rate = None
cam_lay2_rate = None
shadow2_master2_rate = None
#################################################################################
# STEP 3

# Vertical data between layer3 and the camera
N_s = None
lay3_cam_data = None # (layer3 -> camera)
cam_lay3_data = None # (camera -> layer3)
lay3_lay3_data = None # (layer3 -> layer3)

# Data to be sent
lay3_cam_rate = None
cam_lay3_rate = None
lay3_lay3_rate = None

#################################################################################
# The index of the data type to be send and received
data_types = None

# Debug Variables
send_one_pack = True
default_num_packs = 1

# Mapping Variables
camera_pos = None # where the camera should map on the mesh
##################################################################################
# Helper functions to get the size of specific data type
# Please refer to this link to know the number of bits for each data type: https://de.mathworks.com/help/matlab/numeric-types.html
# All functions return the number of bytes, so we divided the number of bits by 8
def getsizeof_bool():
    return 1.0/8
def getsizeof_uint8():
    return 8.0/8
def getsizeof_uint32():
    return 32.0/8
def getsizeof_float():
    return 32.0/8
##################################################################################
def pretty_print(p_elem):
    """ Return a pretty-printed XML string for the Element """
    rough_string = ET.tostring(p_elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")
##################################################################################
def write_xml(p_f, p_data):
    """ Write data to xml file """
    senders_file = open(p_f, 'w')
    senders_file.write(pretty_print(p_data))
    senders_file.close()
##################################################################################
def read_config(p_config_file):
    """ Read configuration file """
    global config; config = configparser.RawConfigParser()
    try:
        config.read(p_config_file)
    except Exception:
        raise Exception
##################################################################################
def init_face_detector():
    """ Initialize Face Detector Parameters """
    global frame_width; frame_width = config.getint('Face_Detector', 'frame_width')
    global frame_height; frame_height = config.getint('Face_Detector', 'frame_height')
    global tile_size; tile_size = config.getint('Face_Detector', 'tile_size')
    global data_types; data_types = config.get('Face_Detector', 'data_types').split(',')
    # K_VJ = 4 * ( (S_x S_y) / (S_VJ)^2 )
    global num_tiles; num_tiles = 4 * ( (frame_width * frame_height) / (tile_size**2) )
    if (type(num_tiles) == float): # Technical issue (if num_tiles is float)
        num_tiles = math.ceil(num_tiles)

    global R_i; R_i = config.getint('Face_Detector', 'R_i')
    global R_min; R_min = config.getint('Face_Detector', 'R_min')
    global max_face_size; max_face_size = config.getint('Face_Detector', 'max_face_size')
    global num_color_channels; num_color_channels = config.getint('Face_Detector', 'num_color_channels')
    global bit_depth; bit_depth = config.getint('Face_Detector', 'bit_depth')
    # 4 (S_VJ)^2 N_C D
    global image_details; image_details = 4 * (max_face_size**2) * num_color_channels * bit_depth

    global num; num = getsizeof_uint8()
    global num_found_faces; num_found_faces = config.getint('Face_Detector', 'num_found_faces')
    global position_size; position_size = 2 * getsizeof_uint32()
    global likelihood_size; likelihood_size = getsizeof_float()
    # Winner = uint8 + 4 (2 uint32 + float)
    global winner_data; winner_data = num + 4 * (position_size + likelihood_size)

    global face_found_size; face_found_size = getsizeof_bool()
    global proc_proc_data; proc_proc_data = face_found_size + position_size + likelihood_size

    global camera_proc_rate; camera_proc_rate = [image_details, 100] # [size of data camera->proc, interval]
    global proc_proc_rate; proc_proc_rate = [proc_proc_data , 100] # [size of data proc->proc, interval]
    global winner_master_rate; winner_master_rate = [winner_data, 100] # [size of data winner_proc->master, interval]

    # lay2_cam_data = 2 * uint32
    global lay2_cam_data; lay2_cam_data = 2 * getsizeof_uint32()
    global lay2_cam_rate; lay2_cam_rate = [lay2_cam_data, 100] # [size of data layer2->camera, interval]

    # cam_lay2_data =  D * N_C * S_{G}^{max} ^ 2
    global cam_lay2_data;  cam_lay2_data = bit_depth * num_color_channels * (max_face_size**2)
    global cam_lay2_rate; cam_lay2_rate = [cam_lay2_data, 100] # [size of data camera->layer2, interval]

    global num_found_features; num_found_features = config.getint('Face_Detector', 'num_found_features')
    # shadow2_master2_data = N_F * uint32
    global shadow2_master2_data; shadow2_master2_data = num_found_features * getsizeof_uint32()
    global shadow2_master2_rate; shadow2_master2_rate = [shadow2_master2_data, 100] # [size of data shadow2->master2, interval]

    global N_s; N_s = config.getint('Face_Detector', 'N_s')
    # lay3_cam_data = N_s * uint32
    global lay3_cam_data; lay3_cam_data = N_s * getsizeof_uint32()
    global lay3_cam_rate; lay3_cam_rate = [lay3_cam_data, 100] # [size of data layer2->camera, interval]

    # cam_lay3_data = N_s * D * N_C
    global cam_lay3_data; cam_lay3_data = N_s * bit_depth * num_color_channels
    global cam_lay3_rate; cam_lay3_rate = [cam_lay3_data, 100] # [size of data camer->layer3, interval]

    # lay3_lay3_data = (N_s + N_F) * uint32 + N_G * float
    global lay3_lay3_data; lay3_lay3_data = (N_s + num_found_features) * getsizeof_uint32() + num_found_faces * getsizeof_float()
    global lay3_lay3_rate; lay3_lay3_rate = [lay3_lay3_data, 100] # [size of data layer3 -> layer3, interval]
##################################################################################
def init_mapping():
    """ Initialize Mapping Parameters """
    global camera_pos; camera_pos = config.getint('Mapping', 'camera_pos')
##################################################################################
