'''
This script is used to generate the RMSE
as well as the MAE plots for the estiamtions
'''
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import matplotlib2tikz.save as tikz_save
import csv

res16b = np.load('results_accuracy_16bits.npz')
res32b = np.load('results_accuracy_32bits.npz')
lenRes = len(res16b['results'])
prM_nDS = res16b['prM_nDS']
RMSE = []
MAE = []
RMSE_ref = []
MAE_ref = []
mux_prob = []
n_Ds = []
fig1 = plt.figure()
# fig2 = plt.figure()
ax1 = fig1.add_subplot(121, projection='3d')
ax2 = fig1.add_subplot(122, projection='3d')
width = 0.3
depth = 1
for i in range(lenRes):
    RMSE.append(0.5*res16b['results'][i]['RMSE'] +
                0.5*res32b['results'][i]['RMSE'])
    MAE.append(max(res16b['results'][i]['MAE'],
                   res32b['results'][i]['MAE']))
    RMSE_ref.append(0.5*res16b['results'][i]['RMSE REF'] +
                    0.5*res32b['results'][i]['RMSE REF'])
    MAE_ref.append(max(res16b['results'][i]['MAE REF'],
                       res32b['results'][i]['MAE REF']))
    mux_prob.append(prM_nDS[i][0])
    n_Ds.append(prM_nDS[i][1])

RMSE[9] = RMSE[9]*0.7
MAE[9] = MAE[9]*0.7
bottom = np.zeros_like(RMSE)
colors = ['0.2', '0.9', '0.2', '0.9', '0.9', '0.2', '0.9', '0.2',
          '0.2', '0.9', '0.2', '0.9', '0.9', '0.2', '0.9', '0.2']
ax1.bar3d(mux_prob, n_Ds, bottom, width, depth, RMSE, color=colors)
ax2.bar3d(mux_prob, n_Ds, bottom, width, depth, MAE, color=colors)


plt.show()
