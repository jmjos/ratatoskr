'''
This script is used to generate figure X.X in the paper.
The figure is used to illustrate the effect of multiplexing
AUTHOR= @LENNARTBAMBERG (bamberg@uni-bremen.de)
'''

# # ATTENTION WILL NOT WORK ANYMORE AS  RANDOM DATA IS NOW INTERPRETED
# # AS SIGNED
from interconnect import DataStream, Interconnect
import copy
import matplotlib.pyplot as plt
import numpy as np
# ------------------VARIABLES--------------------------------
# #  DATA VARIABLES # #
B = 16  # number of bits
N = 3e5  # number of analyzed samples
# # DATA STREAM STOCHASTIC PROPERTIES # #
uniform1, uniform2 = False, False  # pattern distributin
ro1, ro2 = 0.99, 0.99
mu1, mu2 = 0, 0  # mean (only important if not uniform)
ld_std1, ld_std2 = 7, 7  # std dev in bit (")
id1, id2 = "gauss", "uniform_corr"
ic2D = Interconnect(16, 0.6e-6, 0.3e-6, wire_length=100e-6)  # 16b interconect (spacing 0.6u, width, 0.3u)
ic3D = Interconnect(16, 0.6e-6, 0.3e-6, wire_length=0, TSVs=True)  # same plus TSVs (typ dim)

# -----------------------------------------------------------

# ----------------MAIN---------------------------------------
ds1 = DataStream.from_stoch(N, B, uniform1, ro1, mu1, ld_std1)
ds2 = DataStream.from_stoch(N, B, uniform2, ro2, mu2, ld_std2)
#ds1c = DataStream.from_stoch(N, B-1, uniform1, ro1, mu1, ld_std1)
#ds2c = DataStream.from_stoch(N, B-1, uniform2, ro2, mu2, ld_std2)
ds1c = DataStream.from_stoch(N, B-1)
ds2c = DataStream.from_stoch(N, B-1)
# ds2 = ds2.as_unsigned
# ds1 = ds1.as_unsigned
ds1_coded = ds1c.bus_invert()
ds2_coded = ds2c.bus_invert()
ds3 = copy.deepcopy(ds1)
ds3_coded = copy.deepcopy(ds1_coded)
ds3.append(ds2)  # data stream for no mux
ds3_coded.append(ds2_coded)
ds_mux = [ds3]
ds_mux_coded = [ds3_coded]

for i in (0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0):  # mux propbabilites
    ds_mux.append(ds1.interleaving(ds2, i))
    ds_mux_coded.append(ds1_coded.interleaving(ds2_coded, i))


power2D = [ic2D.E(ds_mux[i]) for i in range(len(ds_mux))]
power3D = [ic3D.E(ds_mux[i]) for i in range(len(ds_mux))]
power2D_coded = [ic2D.E(ds_mux_coded[i]) for i in range(len(ds_mux))]
power3D_coded = [ic3D.E(ds_mux_coded[i]) for i in range(len(ds_mux))]

ds_ref = DataStream(ds1c.samples, B)
power2D_ref = ic2D.E(ds_ref)
power3D_ref = ic3D.E(ds_ref)
coding_gain_2D = (1-power2D_coded/power2D_ref)*100
coding_gain_3D = (1-power3D_coded/power3D_ref)*100




# ---------------PLOT--------------------------------------------------------
probs = 0.1*np.array(range(11))


plt.title('Correlated Data')
plt.plot(probs, power2D, 'ro-')
plt.plot(probs, power3D, 'bs-')
plt.ylabel("Energy/Pattern")
plt.xlabel("Mux Probability")
plt.legend(['2D Link', '3D Link'])
plt.grid(True)
plt.show()
'''
from matplotlib2tikz import save as tikz_save
tikz_save(
    'effect_mux_1.tex',
    figureheight='\\figureheight',
    figurewidth='\\figurewidth'
    )
plt.clf()
plt.cla()
plt.close()
plt.title('BI Coded Uncorrelated Data')
plt.plot(probs, coding_gain_2D, 'ro-')
plt.plot(probs, coding_gain_3D, 'bs-')
plt.ylabel("Coding gain in %")
plt.xlabel("Mux Probability")
plt.legend(['2D Link', '3D Link'])
plt.grid(True)
tikz_save(
    'effect_mux_2.tex',
    figureheight='\\figureheight',
    figurewidth='\\figurewidth'
    )
'''
