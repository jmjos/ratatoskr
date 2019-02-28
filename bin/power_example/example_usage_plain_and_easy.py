from interconnect import Interconnect, Driver

# # typical standard digital technology
ic2D = Interconnect(16, 0.6e-6, 0.3e-6)  # 16b interconect (spacing 0.6u, width, 0.3u)
ic3D = Interconnect(16, 0.6e-6, 0.3e-6, TSVs=True)  # same plus TSVs (typ dim)
max_l = ic2D.max_metal_wire_length(1e9)  # max 2D length for 1 GHZ clock
max_l2 = ic3D.max_metal_wire_length(1e9)  # max wire length for 3D link
ic2D_spec = ic2D.metal_wire_length(1e-3)  # Ex. 1mm 2D link
ic3D_spec = ic3D.metal_wire_length(0.8e-3)  # Ex. 3D link with 0.8mm wires
E_2D = ic2D_spec.E()  # energy per flit transmission
E_3D = ic3D_spec.E()  # energy per flit transmission
A_3D = ic3D_spec.area_3D  # would return zero for 2D link

# # MS technology
driver_ms = Driver.predefined('comm_ms180nm', 2)  # strength two (max) driver
ic2D_ms = Interconnect(16, 1.2e-6, 0.6e-6, Driver=driver_ms)
ic3D_ms = Interconnect(16, 1.2e-6, 0.6e-6, TSVs=True, Driver=driver_ms)
max_l_ms = ic2D_ms.max_metal_wire_length(1e9)
max_l2_ms = ic3D_ms.max_metal_wire_length(1e9)
ic2D_spec_ms = ic2D_ms.metal_wire_length(1e-3)
ic3D_spec_ms = ic3D_ms.metal_wire_length(0.8e-3)
E_2D_ms = ic2D_spec_ms.E()
E_3D_ms = ic3D_spec_ms.E()
# A_3D_ms = ic3D_spec_ms.area_3D  # is the same as for digital
