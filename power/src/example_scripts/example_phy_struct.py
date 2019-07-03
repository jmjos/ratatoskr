# # # this script exempalry shows how to get the capacitance matrice
# # # for: an n x m TSV array of length l_3D with a TSV radius of r_3d
# # # and a minimum TSV pitch of d_3d, a 2D NB-bit metal wire bus of
# # # length l_2D with a wire width and spacing of w_2D and s_2D

import interconnect.phy_struct as psm
import interconnect.perf_est as pem
# # Variables
# 3D (TSVs) - One Layer
n = 4
m = 4
l_3D = 50e-6
r_3D = 2e-6
d_3D = 8e-6
ground_ring = False  # optional (not required)
# 2D (Metal Wire one segment)
w_2D = 0.3e-6
s_2D = 0.3e-6
l_2D = 200e-6
metal_layer = 5         # optional not required
# Technology
w_koz = 2*d_3D  # TSV array KOZ
R_on = 4e3  # on restitance of the used drivers
C_in = 1e-15  # eq. resitance for the driver input (2*C_gate)
C_out = 1e-15  # eq. resitance for the driver input (2*C_ds)
Vdd = 1
# # Derived Variables
B = m*n  # number of bits


# Main part
C_2D = psm.metal_wire_cap_matrix(l_2D, B, s_2D, w_2D, layer=metal_layer)
R_2D = psm.metal_wire_resistance(l_2D, s_2D, w_2D, metal_layer)
C_3D = psm.TSV_cap_matrix(l_3D, n, m, r_3D, d_3D,  ground_ring=ground_ring)
R_3D = psm.TSV_resistance(l_3D, r_3D)

# Performance (unencoded/random data)
t_prop_3D = pem.delay_estimation(C_3D, R_3D, C_in, C_out, R_on)
    # see the function descriptions for the other capabilities of the function
t_prop_2D = pem.delay_estimation(C_2D, R_2D, C_in, C_out, R_on)
P_3D_per_GHz = pem.mean_power2(C_3D, Vdd, 1e9, (C_in+C_out))
    # see the function descriptions for the other capabilities of the function
P_2D_per_GHz = pem.mean_power2(C_2D, Vdd, 1e9, (C_in+C_out))
P_total_per_GHz = P_2D_per_GHz+P_3D_per_GHz
t_prop_total = t_prop_2D + t_prop_3D
A_3D = pem.TSV_array_area(d_3D, m, n, w_koz)
