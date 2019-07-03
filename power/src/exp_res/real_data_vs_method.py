import numpy as np
import interconnect
import copy


# # VARIABLES
N = 3001  # max clock cycles +1
FW = 16  # flit width
FPP = 32  # flits per packet


def get_header(FW=16):
    '''
    generates a random header for a flit-width of FW)
    '''
    return np.random.random_integers(0, (1 << FW)-1)


# data, day = np.load('./videos/traffic_pictures_day.npz'), 1
data, day = np.load('./videos/traffic_pictures_night.npz'), 0
# data = np.load('./videos/traffic_features.npz')
sim = np.load('./res_simulator/sensors_to_memory.npz')
mux = sim['mux_matrices']
DHs = [get_header(16) for i in range(int(N))]
D0s = data['pic1'].astype(int)  # pixel samples
D1s = data['pic2'].astype(int)
D2s = data['pic3'].astype(int)
D3s = data['pic4'].astype(int)
D4s = data['pic5'].astype(int)
D5s = data['pic6'].astype(int)

D0s = np.add(D0s[0::2, :], (1 << 8)*D0s[1::2, :])  # attach two for flit
D1s = np.add(D1s[0::2, :], (1 << 8)*D1s[1::2, :])
D2s = np.add(D2s[0::2, :], (1 << 8)*D2s[1::2, :])
D3s = np.add(D3s[0::2, :], (1 << 8)*D3s[1::2, :])
D4s = np.add(D4s[0::2, :], (1 << 8)*D4s[1::2, :])
D5s = np.add(D5s[0::2, :], (1 << 8)*D5s[1::2, :])

ic2D = interconnect.Interconnect(B=16, wire_spacing=0.3e-6,  # 2D IC
                                 wire_width=0.3e-6, wire_length=100e-6)
ic3D = interconnect.Interconnect(16, 0.6e-6, 0.3e-6, wire_length=0,  # 3D IC
                                 TSVs=True, TSV_radius=2e-6, TSV_pitch=8e-6)

E3dLink0bitlevel = []
E2dLink1bitlevel = []
E2dLink2bitlevel = []
E3dLink0highlevel = []
E2dLink1highlevel = []
E2dLink2highlevel = []
E3dLink0ref = []
E2dLink1ref = []
E2dLink2ref = []
# # MAIN PART

for coding in range(8):
    # run the simulation for
    # 0: NO-CODING; 1: NEGK1; 2: NEGK0
    # 3: NEGCORR; 4:NEG(K0+CORR); 5:NEG(K1+CORR)
    D_true = []
    cD = []  # counter for the different data types
    DH = interconnect.DataStream(np.copy(DHs), 16)  # headers not coded
    D0 = interconnect.DataStream(D0s.flatten()[:N], 16)  # DATA STREAMS UNCO
    D1 = interconnect.DataStream(D1s.flatten()[:N], 16)
    D2 = interconnect.DataStream(D2s.flatten()[:N], 16)
    D3 = interconnect.DataStream(D3s.flatten()[:N], 16)
    D4 = interconnect.DataStream(D4s.flatten()[:N], 16)
    D5 = interconnect.DataStream(D5s.flatten()[:N], 16)
    # coding correlated data streams
    if coding == 1:
        D0, D1 = D0.k0_encoded().invert, D1.k0_encoded().invert
        D2, D3 = D2.k0_encoded().invert, D3.k0_encoded().invert
        D4, D5 = D4.k0_encoded().invert, D5.k0_encoded().invert
    elif coding == 2:
        D0, D1 = D0.k1_encoded().invert, D1.k1_encoded().invert
        D2, D3 = D2.k1_encoded().invert, D3.k1_encoded().invert
        D4, D5 = D4.k1_encoded().invert, D5.k1_encoded().invert
    elif coding == 3:
        D0, D1 = D0.corr_encoded().invert, D1.corr_encoded().invert
        D2, D3 = D2.corr_encoded().invert, D3.corr_encoded().invert
        D4, D5 = D4.corr_encoded().invert, D5.corr_encoded().invert
    elif coding == 4:
        D0 = D0.k0_encoded().corr_encoded().invert
        D1 = D1.k0_encoded().corr_encoded().invert
        D2 = D2.k0_encoded().corr_encoded().invert
        D3 = D3.k0_encoded().corr_encoded().invert
        D4 = D4.k0_encoded().corr_encoded().invert
        D5 = D5.k0_encoded().corr_encoded().invert
    elif coding == 5:
        D0 = D0.k1_encoded().corr_encoded().invert
        D1 = D1.k1_encoded().corr_encoded().invert
        D2 = D2.k1_encoded().corr_encoded().invert
        D3 = D3.k1_encoded().corr_encoded().invert
        D4 = D4.k1_encoded().corr_encoded().invert
        D5 = D5.k1_encoded().corr_encoded().invert
    elif coding == 6:
        D0 = D0.corr_encoded().k0_encoded().invert
        D1 = D1.corr_encoded().k0_encoded().invert
        D2 = D2.corr_encoded().k0_encoded().invert
        D3 = D3.corr_encoded().k0_encoded().invert
        D4 = D4.corr_encoded().k0_encoded().invert
        D5 = D5.corr_encoded().k0_encoded().invert
    elif coding == 7:
        D0 = D0.corr_encoded().k1_encoded().invert
        D1 = D1.corr_encoded().k1_encoded().invert
        D2 = D2.corr_encoded().k1_encoded().invert
        D3 = D3.corr_encoded().k1_encoded().invert
        D4 = D4.corr_encoded().k1_encoded().invert
        D5 = D5.corr_encoded().k1_encoded().invert
    # # #
    for i in range(len(sim['links'])):
        d_link = [0]  # data going over the link (init val 0)
        # copy of single data streams as list
        h = np.copy(DH.samples).tolist()
        d0, d1 = np.copy(D0.samples).tolist(), np.copy(D1.samples).tolist()
        d2, d3 = np.copy(D2.samples).tolist(), np.copy(D3.samples).tolist()
        d4, d5 = np.copy(D4.samples).tolist(), np.copy(D5.samples).tolist()
        d_list = [h, d0, d1, d2, d3, d4, d5]
        counter = [0, 0, 0, 0, 0, 0, 0]
        seq = sim['true_values'][i].astype(int)  # pattern sequence
        for j in range(1, len(seq)):
            if seq[j] < 7:
                d_link.append(d_list[seq[j]].pop(0))
                counter[seq[j]] += 1
            else:
                d_link.append(d_link[-1])
        cD.append(counter)
        D_true.append(interconnect.DataStream(d_link, 16))

    D_mux0 = interconnect.DataStreamProb([DH[:cD[0][0]], D0[:cD[0][1]],
                                          D1[:cD[0][2]], D2[:cD[0][3]],
                                          D3[:cD[0][4]], D4[:cD[0][5]],
                                          D5[:cD[0][6]]], mux[0])
    D_mux1 = interconnect.DataStreamProb([DH[:cD[1][0]], D0,  # D0-D2 not trans
                                          D1, D2,
                                          D3[:cD[1][4]], D4[:cD[1][5]],
                                          D5[:cD[1][6]]], mux[1])
    D_mux2 = interconnect.DataStreamProb([DH[:cD[2][0]], D0,
                                          D1, D2,  # only D3 transmitted
                                          D3[:cD[2][4]], D4, D5], mux[2])

    D_noMux0 = copy.deepcopy(DH[:cD[0][0]])
    D_noMux0.append(D0[:cD[0][1]])
    D_noMux0.append(D1[:cD[0][2]])
    D_noMux0.append(D2[:cD[0][3]])
    D_noMux0.append(D3[:cD[0][4]])
    D_noMux0.append(D4[:cD[0][5]])
    D_noMux0.append(D5[:cD[0][6]])
    D_noMux1 = copy.deepcopy(DH[:cD[1][0]])
    D_noMux1.append(D3[:cD[1][4]])
    D_noMux1.append(D4[:cD[1][5]])
    D_noMux1.append(D5[:cD[1][6]])
    D_noMux2 = copy.deepcopy(DH[:cD[2][0]])
    D_noMux2.append(D3[:cD[2][4]])

    # golden values (bit-level sim)
    E3dLink0bitlevel.append(ic3D.E(D_true[0]))
    E2dLink1bitlevel.append(ic2D.E(D_true[1]))
    E2dLink2bitlevel.append(ic2D.E(D_true[2]))

    # proposed high-level model
    E3dLink0highlevel.append(ic3D.E(D_mux0))
    E2dLink1highlevel.append(ic2D.E(D_mux1))
    E2dLink2highlevel.append(ic2D.E(D_mux2))

    # ref bit level
    E3dLink0ref.append(ic3D.E(D_noMux0))
    E2dLink1ref.append(ic2D.E(D_noMux1))
    E2dLink2ref.append(ic2D.E(D_noMux2))

if day == 0:
    E3dLink0bitlevel_night = E3dLink0bitlevel
    E2dLink1bitlevel_night = E2dLink1bitlevel
    E2dLink2bitlevel_night = E2dLink2bitlevel

    E3dLink0highlevel_night = E3dLink0highlevel
    E2dLink1highlevel_night = E2dLink1highlevel
    E2dLink2highlevel_night = E2dLink2highlevel

    E3dLink0ref_night = E3dLink0ref
    E2dLink1ref_night = E2dLink1ref
    E2dLink2ref_night = E2dLink2ref
else:
    E3dLink0bitlevel_day = E3dLink0bitlevel
    E2dLink1bitlevel_day = E2dLink1bitlevel
    E2dLink2bitlevel_day = E2dLink2bitlevel

    E3dLink0highlevel_day = E3dLink0highlevel
    E2dLink1highlevel_day = E2dLink1highlevel
    E2dLink2highlevel_day = E2dLink2highlevel

    E3dLink0ref_day = E3dLink0ref
    E2dLink1ref_day = E2dLink1ref
    E2dLink2ref_day = E2dLink2ref

if 'E3dLink0ref_day' in locals() and 'E3dLink0ref_night' in locals():
    packages = 2*sum(cD[0])/32
    E3dLink0ref_tot = (N/packages)*(np.array(E3dLink0ref_day)+np.array(E3dLink0ref_night))
    E2dLink1ref_tot = (N/packages)*(np.array(E2dLink1ref_day)+np.array(E2dLink1ref_night))
    E2dLink2ref_tot = (N/packages)*(np.array(E2dLink2ref_day)+np.array(E2dLink2ref_night))
    E3dLink0bitlevel_tot = (N/packages)*(np.array(E3dLink0bitlevel_day)+np.array(E3dLink0bitlevel_night))
    E2dLink1bitlevel_tot = (N/packages)*(np.array(E2dLink1bitlevel_day)+np.array(E2dLink1bitlevel_night))
    E2dLink2bitlevel_tot = (N/packages)*(np.array(E2dLink2bitlevel_day)+np.array(E2dLink2bitlevel_night))
    E3dLink0highlevel_tot = (N/packages)*(np.array(E3dLink0highlevel_day)+np.array(E3dLink0highlevel_night))
    E2dLink1highlevel_tot = (N/packages)*(np.array(E2dLink1highlevel_day)+np.array(E2dLink1highlevel_night))
    E2dLink2highlevel_tot = (N/packages)*(np.array(E2dLink2highlevel_day)+np.array(E2dLink2highlevel_night))
