'''
This script is used to generate the results for the analysis
of the low power coding
AUTHOR= @LENNARTBAMBERG (bamberg@uni-bremen.de)
'''
from interconnect import DataStream, Interconnect, DataStreamProb
import matplotlib.pyplot as plt
from matplotlib import interactive
import numpy as np
from matplotlib2tikz import save as tikz_save

plt.clf()
plt.cla()
plt.close()
# ------------------VARIABLES--------------------------------
# #  DATA VARIABLES # #
muxProbReal = np.arange(0, 1.2, 0.2)
muxProbMod = np.arange(0, 1.05, 0.05)
B = 16  # number of bits
N = 1e6  # number of analyzed samples
# # DATA STREAMS # #
# TWO CORRELATED DATA STREAMS
uniformC1, uniformC2 = True, True  # pattern dist
roC1, roC2 = 0.996, 0.996
mu1, mu2 = 0, 0  # mean (only important if not uniform)
ldStd1, ldStd2 = 8, 8  # std dev in bit (")
id1, id2 = "corr1", " corr2"
codingC = 2  # 0: NEGK0; 1: NEGK1; 2: NEG_CORR;
            # 3: NEG(K0+CORR); 4 NEG(K1+CORR)
# TWO UNIFORM DISTRIBUTED DATA STREAMS
uniformR1, uniformR2 = True, True  # pattern dist
roR1, roR2 = 0, 0  # assume no patt corr
# # INTERCONNECTS
ic2D = Interconnect(B=16, wire_spacing=0.3e-6,  # 2D IC
                    wire_width=0.3e-6, wire_length=100e-6)
ic3D = Interconnect(16, 0.6e-6, 0.3e-6, wire_length=0,  # 3D IC
                    TSVs=True, TSV_radius=2e-6, TSV_pitch=8e-6)
# -----------------------------------------------------------

# ----------------MAIN---------------------------------------
# correlated data streams
dsC1 = DataStream.from_stoch(N, B, uniformC1, roC1, mu1, ldStd1)
dsC2 = DataStream.from_stoch(N, B, uniformC2, roC2, mu2, ldStd2)
# coding correlated data streams
if codingC == 0:
    dsC1Coded = dsC1.k0_encoded().invert
    dsC2Coded = dsC2.k0_encoded().invert
elif codingC == 1:
    dsC1Coded = dsC1.k1_encoded().invert
    dsC2Coded = dsC2.k1_encoded().invert
elif codingC == 2:
    dsC1Coded = dsC1.corr_encoded().invert
    dsC2Coded = dsC2.corr_encoded().invert
elif codingC == 3:
    dsC1Coded = dsC1.corr_encoded().invert
    dsC2Coded = dsC2.corr_encoded().invert
elif codingC == 4:
    dsC1Coded = dsC1.k0_encoded().corr_encoded().invert
    dsC2Coded = dsC2.k0_encoded().corr_encoded().invert
else:
    dsC1Coded = dsC1.k1_encoded().corr_encoded().invert
    dsC2Coded = dsC2.k1_encoded().corr_encoded().invert
# uncorrelated data streams
dsR1 = DataStream.from_stoch(N, B, uniformR1, roR1)
dsR2 = DataStream.from_stoch(N, B, uniformR1, roR2)
# coding uncorrelated data streams
dsR1r = DataStream.from_stoch(N, B-1, uniformR1, roR1)  # red B
dsR2r = DataStream.from_stoch(N, B-1, uniformR1, roR2)  # red B
dsR1TradCoded = dsR1r.bus_invert()  # reduced switching activity
dsR2TradCoded = dsR2r.bus_invert()  # reduced switching activities
dsR1NewCoded = dsR1r.prob_invert()  # increased one bit prob
dsR2NewCoded = dsR2r.prob_invert()  # increased one bit prob

# interleaved data streams
dsRRmux = []  # 1: two random
dsCCmux = []  # 2: two corr
dsRCmux = []  # 3: one random, one corr
dsRRmuxCoded = []  # 1: two random
dsCCmuxCoded = []  # 2: two corr
dsRCmuxCoded = []  # 3: one random, one corr

for i in muxProbReal:
    dsRRmux.append(dsR1.interleaving(dsR2, i))
    dsCCmux.append(dsC1.interleaving(dsC2, i))
    dsRCmux.append(dsR1.interleaving(dsC1, i))
    dsRRmuxCoded.append(dsR1TradCoded.interleaving(dsR2TradCoded, i))
    dsCCmuxCoded.append(dsC1Coded.interleaving(dsC2Coded, i))
    dsRCmuxCoded.append(dsR1TradCoded.interleaving(dsC1Coded, i))

# # model for interleaving
# make property containers to speed up the simulation
dspR1, dspR2 = DataStreamProb(dsR1), DataStreamProb(dsR2)
dspC1, dspC2 = DataStreamProb(dsC1), DataStreamProb(dsC2)
dspR1Coded = DataStreamProb(dsR1TradCoded)
dspR2Coded = DataStreamProb(dsR2TradCoded)
dspC1Coded = DataStreamProb(dsC1Coded)
dspC2Coded = DataStreamProb(dsC2Coded)
dspRRmux = []  # 1: two random
dspCCmux = []  # 2: two corr
dspRCmux = []  # 3: one random, one corr
dspRRmuxCoded = []  # 1: two random
dspCCmuxCoded = []  # 2: two corr
dspRCmuxCoded = []  # 3: one random, one corr
mux = np.zeros((4, 4))  # including the hold state
for j in muxProbMod:
    mux[1, 0] = mux[0, 1] = j/2
    mux[0, 0] = mux[1, 1] = (1-j)/2
    dspRRmux.append(DataStreamProb([dspR1, dspR2], mux))
    dspCCmux.append(DataStreamProb([dspC1, dspC2], mux))
    dspRCmux.append(DataStreamProb([dspR1, dspC1], mux))
    dspRRmuxCoded.append(DataStreamProb(
        [dspR1Coded, dspR2Coded], mux))
    dspCCmuxCoded.append(DataStreamProb(
        [dspC1Coded, dspC2Coded], mux))
    dspRCmuxCoded.append(DataStreamProb(
        [dspR1Coded, dspC1Coded], mux))
power2dRRreal = [ic2D.E(dsRRmux[i]) for i in range(len(dsRRmux))]
power3dRRreal = [ic3D.E(dsRRmux[i]) for i in range(len(dsRRmux))]
power2dCCreal = [ic2D.E(dsCCmux[i]) for i in range(len(dsCCmux))]
power3dCCreal = [ic3D.E(dsCCmux[i]) for i in range(len(dsCCmux))]
power2dRCreal = [ic2D.E(dsRCmux[i]) for i in range(len(dsRCmux))]
power3dRCreal = [ic3D.E(dsRCmux[i]) for i in range(len(dsRCmux))]
power2dRRrealCoded = [ic2D.E(dsRRmuxCoded[i]) for i in range(len(dsRRmux))]
power3dRRrealCoded = [ic3D.E(dsRRmuxCoded[i]) for i in range(len(dsRRmux))]
power2dCCrealCoded = [ic2D.E(dsCCmuxCoded[i]) for i in range(len(dsCCmux))]
power3dCCrealCoded = [ic3D.E(dsCCmuxCoded[i]) for i in range(len(dsCCmux))]
power2dRCrealCoded = [ic2D.E(dsRCmuxCoded[i]) for i in range(len(dsRCmux))]
power3dRCrealCoded = [ic3D.E(dsRCmuxCoded[i]) for i in range(len(dsRCmux))]
power2dRRmod = [ic2D.E(dspRRmux[i]) for i in range(len(dspRRmux))]
power3dRRmod = [ic3D.E(dspRRmux[i]) for i in range(len(dspRRmux))]
power2dCCmod = [ic2D.E(dspCCmux[i]) for i in range(len(dspCCmux))]
power3dCCmod = [ic3D.E(dspCCmux[i]) for i in range(len(dspCCmux))]
power2dRCmod = [ic2D.E(dspRCmux[i]) for i in range(len(dspRCmux))]
power3dRCmod = [ic3D.E(dspRCmux[i]) for i in range(len(dspRCmux))]
power2dRRmodCoded = [ic2D.E(dspRRmuxCoded[i]) for i in range(len(dspRRmux))]
power3dRRmodCoded = [ic3D.E(dspRRmuxCoded[i]) for i in range(len(dspRRmux))]
power2dCCmodCoded = [ic2D.E(dspCCmuxCoded[i]) for i in range(len(dspCCmux))]
power3dCCmodCoded = [ic3D.E(dspCCmuxCoded[i]) for i in range(len(dspCCmux))]
power2dRCmodCoded = [ic2D.E(dspRCmuxCoded[i]) for i in range(len(dspRCmux))]
power3dRCmodCoded = [ic3D.E(dspRCmuxCoded[i]) for i in range(len(dspRCmux))]


codEff2dRRreal = (1-(16*np.array(power2dRRrealCoded)) /
                  (15*np.array(power2dRRreal)))*100
codEff2dCCreal = (1-np.array(power2dCCrealCoded)/np.array(power2dCCreal))*100
codEff2dRCreal = (1-(32*np.array(power2dRCrealCoded)) /
                  (31*np.array(power2dRCreal)))*100
codEff3dRRreal = (1-(16*np.array(power3dRRrealCoded)) /
                  (15*np.array(power3dRRreal)))*100
codEff3dCCreal = (1-np.array(power3dCCrealCoded)/np.array(power3dCCreal))*100
codEff3dRCreal = (1-(32*np.array(power3dRCrealCoded)) /
                  (31*np.array(power3dRCreal)))*100

codEff2dRRmod = (1-(16*np.array(power2dRRmodCoded)) /
                 (15*np.array(power2dRRmod)))*100
codEff2dCCmod = (1-np.array(power2dCCmodCoded)/np.array(power2dCCmod))*100
codEff2dRCmod = (1-(32*np.array(power2dRCmodCoded)) /
                 (31*np.array(power2dRCmod)))*100
codEff3dRRmod = (1-(16*np.array(power3dRRmodCoded)) /
                 (15*np.array(power3dRRmod)))*100
codEff3dCCmod = (1-np.array(power3dCCmodCoded)/np.array(power3dCCmod))*100
codEff3dRCmod = (1-(32*np.array(power3dRCmodCoded)) /
                 (31*np.array(power3dRCmod)))*100


# ---------------PLOT 1--------------------------------------------------------
plt.figure(1)
plt.title('2D - Effect of Coding')
plt.plot(muxProbReal, np.array(power2dRRreal)/2, 'ro')
plt.plot(muxProbReal, np.array(power2dCCreal)/2, 'bs')
plt.plot(muxProbReal, np.array(power2dRCreal)/2, 'gv')
plt.plot(muxProbReal, np.array(power2dRRrealCoded)/1.875, 'rx')
plt.plot(muxProbReal, np.array(power2dCCrealCoded)/2, 'b*')
plt.plot(muxProbReal, np.array(power2dRCrealCoded)/1.9375, 'g^')
plt.plot(muxProbMod, np.array(power2dRRmod)/2, 'r-')
plt.plot(muxProbMod, np.array(power2dCCmod)/2, 'b-')
plt.plot(muxProbMod, np.array(power2dRCmod)/2, 'g-')
plt.plot(muxProbMod, np.array(power2dRRmodCoded)/1.875, 'r--')
plt.plot(muxProbMod, np.array(power2dCCmodCoded)/2, 'b--')
plt.plot(muxProbMod, np.array(power2dRCmodCoded)/1.9375, 'g--')
plt.ylabel("Energy/Byte")
plt.xlabel("Mux Probability")
plt.legend(['Random+Random', 'Corr+Corr', 'Random+Corr',
            'Random+Random Coded', 'Corr+Corr Coded', 'Random+Corr Coded'],
           bbox_to_anchor=(0.3, 1.1))
plt.grid(True)
tikz_save(
    'coding_2D.tex',
    figureheight='\\figureheight',
    figurewidth='\\figurewidth'
    )
interactive(True)
plt.show()

plt.figure(2)
plt.title('3D - Effect of Coding')
plt.plot(muxProbReal, np.array(power3dRRreal)/2, 'ro')
plt.plot(muxProbReal, np.array(power3dCCreal)/2, 'bs')
plt.plot(muxProbReal, np.array(power3dRCreal)/2, 'gv')
plt.plot(muxProbReal, np.array(power3dRRrealCoded)/1.875, 'rx')
plt.plot(muxProbReal, np.array(power3dCCrealCoded)/2, 'b*')
plt.plot(muxProbReal, np.array(power3dRCrealCoded)/1.9375, 'g^')
plt.plot(muxProbMod, np.array(power3dRRmod)/2, 'r-')
plt.plot(muxProbMod, np.array(power3dCCmod)/2, 'b-')
plt.plot(muxProbMod, np.array(power3dRCmod)/2, 'g-')
plt.plot(muxProbMod, np.array(power3dRRmodCoded)/1.875, 'r--')
plt.plot(muxProbMod, np.array(power3dCCmodCoded)/2, 'b--')
plt.plot(muxProbMod, np.array(power3dRCmodCoded)/1.9375, 'g--')
plt.ylabel("Energy/Byte")
plt.xlabel("Mux Probability")
plt.legend(['Random+Random', 'Corr+Corr', 'Random+Corr',
            'Random+Random Coded', 'Corr+Corr Coded', 'Random+Corr Coded'],
           bbox_to_anchor=(0.3, 1.1))
plt.grid(True)
tikz_save(
    'coding_3D.tex',
    figureheight='\\figureheight',
    figurewidth='\\figurewidth'
    )
plt.show()

plt.figure(3)
plt.title('2D - Coding Efficiency')
plt.plot(muxProbReal, np.array(codEff2dRRreal), 'ro')
plt.plot(muxProbReal, np.array(codEff2dCCreal), 'bs')
plt.plot(muxProbReal, np.array(codEff2dRCreal), 'gv')
plt.plot(muxProbMod, np.array(codEff2dRRmod), 'r-')
plt.plot(muxProbMod, np.array(codEff2dCCmod), 'b-')
plt.plot(muxProbMod, np.array(codEff2dRCmod), 'g-')
plt.ylabel("Energy reduction [%]")
plt.xlabel("Mux probability")
plt.legend(['Random+Random', 'Corr+Corr', 'Random+Corr'],
           bbox_to_anchor=(0.3, 1.1))
plt.grid(True)
tikz_save(
    'coding_eff_2D.tex',
    figureheight='\\figureheight',
    figurewidth='\\figurewidth'
    )
plt.show()

plt.figure(4)
plt.title('3D - Coding Efficiency')
plt.plot(muxProbReal, np.array(codEff3dRRreal), 'ro')
plt.plot(muxProbReal, np.array(codEff3dCCreal), 'bs')
plt.plot(muxProbReal, np.array(codEff3dRCreal), 'gv')
plt.plot(muxProbMod, np.array(codEff3dRRmod), 'r-')
plt.plot(muxProbMod, np.array(codEff3dCCmod), 'b-')
plt.plot(muxProbMod, np.array(codEff3dRCmod), 'g-')
plt.ylabel("Energy reduction [%]")
plt.xlabel("Mux probability")
plt.legend(['Random+Random', 'Corr+Corr', 'Random+Corr'],
           bbox_to_anchor=(0.3, 1.1))
plt.grid(True)
tikz_save(
    'coding_eff_3D.tex',
    figureheight='\\figureheight',
    figurewidth='\\figurewidth'
    )
interactive(False)
plt.show()
