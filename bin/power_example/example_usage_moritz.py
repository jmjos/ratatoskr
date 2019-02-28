'''
!!!!!! NOTE - WRITTEN WITH PYTHON3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
In this document you will find some examples which show you how to
use the functions of the module I provided for you. In this document
I only descripe fucntions which are relevant for you. However,
the true functionality of the module "interconnect" is way bigger
and if desired I can give you an indetail explanation,
but it is a lot of boring physical stuff.
'''
# # # # # # # 0 IMPORT MODULE CLASSES  # # # # # # # # # # # # #
from interconnect import Interconnect, Driver, DataStream, DataStreamProb

# # # # # # # # # # 1 PARAMETERS YOU  CAN SET # # # # # # # # # # #
# -----------------TECH DEPENDENT PARAMETERS-----------------------------------
# 1.1 interconnects (examplary we define only one geometric variation)
f_clk_dig = 1e9  # clock freq. of digital layer
f_clk_ms = 300e6  # clock freq. of ms layer
phit_width = 16  # transmitted bits per link (incl. flow-control, ECC, etc.)
wire_spacing, wire_width = 0.6e-6, 0.3e-6
TSV_pitch, TSV_radius = 8e-6, 2e-6
metal_layer = 5  # not important as it plays a minor role
TSV_length = 50e-6  # in contrast to the wire length this is a tech. constant
ground_ring = False  # structure used to reduce TSV noise (affects power)
KOZ = 2*TSV_pitch
# 1.1 Drivers/process-nodes (examplary 45nm digital and 180nm mixed signal)
driver_40nm_d4 = Driver.predefined()  # comm. 40nm driver - strength 0
                                      # (more predefined drivers in the making,
                                      # but you can also create your own driver
                                      # via "Driver(...)"; or use the script
                                      # ".//interconnect/driver_parameters/
                                      # generate_new_parameter_file.py

driver_40nm_d0 = Driver.predefined('comm_45nm', 4)
driver_ms180nm_d2 = Driver.predefined('comm_ms180nm', 2)
driver_ms180nm_d0 = Driver.predefined('comm_ms180nm', 0)

# -------------------------------------------------------------------------------

# # # # # # # 1 INITIALIZATION PHASE (USED TECHNOLOGY) # # # # # # # # # # # #


# create class inst. for 2D/3D interconnects in digital and mixed-signal layer
interconnect_2D_dig = Interconnect(B=phit_width, wire_spacing=wire_spacing, wire_width=wire_width,
                                   metal_layer=metal_layer, Driver=driver_40nm_d4)

interconnect_2D_ms = Interconnect(B=phit_width, wire_spacing=wire_spacing, wire_width=wire_width,
                                  metal_layer=metal_layer, Driver=driver_ms180nm_d2)

interconnect_3D_dig = Interconnect(B=phit_width, wire_spacing=wire_spacing, wire_width=wire_width,
                                   metal_layer=metal_layer, Driver=driver_40nm_d4,
                                   TSVs=True, TSV_radius=TSV_radius,
                                   TSV_pitch=TSV_pitch, TSV_length=TSV_length,
                                   KOZ=KOZ, ground_ring=ground_ring)

interconnect_3D_ms = Interconnect(B=phit_width, wire_spacing=wire_spacing, wire_width=wire_width,
                                  metal_layer=metal_layer, Driver=driver_ms180nm_d2,
                                  TSVs=True, TSV_radius=TSV_radius,
                                  TSV_pitch=TSV_pitch, TSV_length=TSV_length,
                                  KOZ=KOZ, ground_ring=ground_ring)

# # # # # # # 2 GET REQ PARAMETERS TO BUILD SYSTEM # # # # # # # # # # # #
l_max_2D_dig = interconnect_2D_dig.max_metal_wire_length(f_clk_dig)
l_max_3D_dig = interconnect_3D_dig.max_metal_wire_length(f_clk_dig)
l_max_2D_ms = interconnect_2D_ms.max_metal_wire_length(f_clk_ms)
l_max_3D_ms = interconnect_3D_ms.max_metal_wire_length(f_clk_ms)
TSV_area = interconnect_3D_dig.area_3D


# Results
print("\n\n")
print("-----------------ALLOWED MAX. METAL WIRE LENGTHS----------------------")
print("\nIn the digital technology, for a pure 2D-Link,\n"
      + "the maximum allowed metal wire length is"
      + (" %.2f mm \n(link divided in %d segments)"
         % (l_max_2D_dig['Max Wire Length']*1e3, l_max_2D_dig['Segments'])))
print("\nIn the mixed-signal technology, for a pure 2D-Link,\n"
      + "the maximum allowed metal wire length is"
      + (" %.2f mm \n(link divided in %d segments)"
         % (l_max_2D_ms['Max Wire Length']*1e3, l_max_2D_ms['Segments'])))
print("\nIn the digital technology, for a 3D-Link,\n"
      + "the maximum allowed metal wire length is"
      + (" %.2f mm \n(link divided in %d segments)"
         % (l_max_3D_dig['Max Wire Length']*1e3, l_max_3D_dig['Segments'])))
print("\nIn the mixed-signal technology, for a 3D-Link,\n"
      + "the maximum allowed metal wire length is"
      + (" %.2f mm \n(link divided in %d segments)"
         % (l_max_3D_ms['Max Wire Length']*1e3, l_max_3D_ms['Segments'])))
print("----------------------------------------------------------------------\n\n")
print("--------------------AREA OCCUPATION-----------------------------------")
print("\n-The silicon area for a TSV array is %.2f [um*um]" % (TSV_area*1e12))
print("----------------------------------------------------------------------\n\n")




# # # # # # # # 3 After your determined your system # # # # #
'''
imagine a 1x2x2 Noc;
your toll wants the following interconnects:
    - two 2D links of length 1mm and 0.8mm
      (1. digital-layer; 2. MS-layer)
    - two 3D links starting in the
      1. digital-layer; 2. MS-layer
      with a metal-wire length of 0.2 and 0.1 mm
'''
interconnect_2D_ds_real = interconnect_2D_dig.metal_wire_length(1e3)  # here I neglect the segments!
interconnect_2D_ms_real = interconnect_2D_ms.metal_wire_length(0.8e3)
interconnect_3D_ds_real = interconnect_3D_dig.metal_wire_length(0.2e3)
interconnect_3D_ms_real = interconnect_3D_ms.metal_wire_length(0.1e3)


# # now you have a lot of options you can do with the interconnect:

# example energy per transmitted phit (random data)
E_mean = interconnect_2D_ds_real.E()
# example prob delay (random data)
prop_delay = interconnect_2D_ds_real.prop_delay()


print("---------------PERFORMANCE - NO HOLD - RANDOM DATA--------------------")
print("Interconnect DS-2D consumes on average %.2f [fJ] per clock cycle,\n"
      % (E_mean*1e15)
      + "and has a propagation delay of %.2f [ps]" % (prop_delay*1e12))
print("----------------------------------------------------------------------\n\n")




# # # # 4 GET DATA STREAM SPECIFIC ENERGY QUANTITIES

ex_samp = list(range(200))
ds1 = DataStream(ex_samp, B=16, is_signed=False)  # 16b data stream from specific samples
ds2 = DataStream.from_stoch(N=1000, B=16, uniform=1, ro=0.4)  # random dist. ro := correlation (1000 samples)
ds3 = DataStream.from_stoch(1000, 16, uniform=0, ro=0.95, mu=2, log2_std=8)  # gaussian
E_mean_ds1 = interconnect_2D_ds_real.E(ds1)
E_mean_ds2 = interconnect_2D_ds_real.E(ds2)
E_mean_ds3 = interconnect_2D_ds_real.E(ds3)
print("----------------ENERGY - NO HOLD - SPECIFIC DATA----------------------")
print("DS1: Interconnect DS-2D consumes on average %.2f [fJ] per clock cycle"
      % (E_mean_ds1*1e15))
print("DS2: Interconnect DS-2D consumes on average %.2f [fJ] per clock cycle"
      % (E_mean_ds2*1e15))
print("DS3: Interconnect DS-2D consumes on average %.2f [fJ] per clock cycle"
      % (E_mean_ds3*1e15))
print("----------------------------------------------------------------------")


ds22 = DataStream.from_stoch(1000, 15, uniform=1, ro=0.4)
E_mean_ds2_coded = interconnect_2D_ds_real.E(ds22.bus_invert())
print("-----------------ENERGY - NO HOLD - SPECIFIC CODED DATA---------------")
print("DS2: Interconnect DS-2D consumes on average %.2f [fJ] per clock cycle"
      % (E_mean_ds2_coded*1e15))
print("----------------------------------------------------------------------\n\n")


ds2_pr = [[0.8, 0.1],  # DS --> DS, H
          [0.05, 0.05]]  # H --> DS, H
E_mean_ds2_with_hold = interconnect_2D_ds_real.E(ds2, ds2_pr)
print("-----------------ENERGY - HOLD - SPECIFIC DATA------------------------")
print("DS2: Interconnect DS-2D consumes on average %.2f [fJ] per clock cycle"
      % (E_mean_ds2_with_hold*1e15))
print("----------------------------------------------------------------------\n\n")


mux = [[0.25832583, 0.14271427, 0.09540954, 0],  # DS1 --> DS1, DS3, H1, H3
       [0.16891689, 0.14241424, 0, 0.03830383],  # DS3 --> DS1, DS3, H1, H3
       [0.06450645, 0.03080308,  0.01050105, 0],  # H1 --> DS1, DS3, H1, H3
       [0.00460046, 0.03370337, 0, 0.00980098]]  # H3 --> DS1, DS3, H1, H3
E_mean_mux_ds1_ds3 = interconnect_2D_ds_real.E([ds1, ds3], mux)
print("-----------------ENERGY - MUX/HOLD - SPECIFIC DATA--------------------")
print("DS1+DS3: Interconnect DS-2D consumes on average %.2f [fJ] per clock cycle"
      % (E_mean_mux_ds1_ds3*1e15))
print("----------------------------------------------------------------------\n\n")
