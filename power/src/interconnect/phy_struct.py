'''
This module defines several functions used to represent
the physical/electrical characterisitics of metal wire
and TSV interconnect structures
'''
import numpy as np
import scipy.io as sio
import scipy.constants as const
from scipy.interpolate import griddata
from math import sqrt
import csv

# # # ---- GENERAL FUNCTIONS (not required but allow to read MATLAB data) -----


def read_cap_matrix_from_mat(file_name):
    '''
    returns spice capacitance matrix from q3d extractor
    saved in a ".mat" file and stored under "../q3d_res"
    '''
    mat_contents = sio.loadmat("../q3d_res/%s" % file_name)
    return mat_contents('spicecapMatrix')


def read_maxwell_cap_matrix_from_mat(file_name):
    '''
    returns maxwell capacitance matrix from q3d extractor
    saved in a ".mat" file and stored under "../q3d_res"
    '''
    mat_contents = sio.loadmat("../q3d_res/%s" % file_name)
    return mat_contents('capMatrix')


# # # ---------- 2D METAL WIRE FUNCTIONS --------------------------------------


def metal_wire_cap_model_coeffs(spacing, width, layer=4):
    '''
    generates the cap matrix coeffs for commercial global metal wire buses
    in dependence of the wire "spacing", "width"
    OPTIONAL:
        layer = metal layer of the wires (default is 4). Since we consider
                global wires, the metal layer has to be bigger than 3
    (!TESTED!  WITH MAX. INTERPOLATION ERROR SMALLER THAN 0.3%)
    '''
    lay = min(layer, 5)  # from layer 5 on paras constant
    width_ref = []
    spacing_ref = []
    C_c_ref = []
    C_t_ref = []
    with open('./interconnect/parasitics/metal_wire_info_m%d.csv' % lay) as f:
        reader = csv.reader(f, delimiter=',')
        for row in reader:
            width_ref.append(row[0])
            spacing_ref.append(row[1])
            C_c_ref.append(row[2])  # values normalized per 100 um
            C_t_ref.append(row[3])  # values normalized per 100 um
    width_ref = [float(x) for x in width_ref[1:]]
    spacing_ref = [float(x) for x in spacing_ref[1:]]
    C_c_ref = [float(x) for x in C_c_ref[1:]]
    C_t_ref = [float(x) for x in C_t_ref[1:]]
    C_t_ref = np.array(C_t_ref)
    C_c_ref = np.array(C_c_ref)
    C_0_ref = C_t_ref - 2*C_c_ref
    var_ref = np.zeros((len(width_ref), 2))
    var_ref[:, 0] = width_ref  # x_values to interpolate (width)
    var_ref[:, 1] = spacing_ref  # y_values to interpolate (TSV pitch)
    C_c = griddata(var_ref, C_c_ref, (width, spacing), method='cubic')
    C_0 = griddata(var_ref, C_0_ref, (width, spacing), method='cubic')
    coeffs = dict()
    coeffs["C_0"] = C_0/10  # this value is given per 10um wire  length
    coeffs["kappa"] = C_c/C_0  # value is wire length independent
    return coeffs


def metal_wire_cap_matrix(length, B, spacing, width, **options):
    '''
    generates the spice cap matrix for a "B"-bit commercical global metal
    wire bus in dependence of the wire "spacing", "width" and "length"
    OPTIONS:
        layer = metal layer of the wires (default is 4). Since we consider
                global wires, the metal layer has to be bigger than 3
        coeffs = if coeffs have been interpolated before with function
                "metal_wire_cap_model_coeffs"  this option allows to pass
                the "coeffs" which  avoids to redo the interpolation to
                speed up the executiontime
    (!TESTED!  WITH MAX. INTERPOLATION ERROR SMALLER THAN 0.3%)
    '''
    if not options.get("layer"):
        layer = 4
    else:
        layer = options.get("layer")
    if not options.get("coeffs"):
        coeffs = metal_wire_cap_model_coeffs(spacing, width, layer)
    else:
        coeffs = options.get("coeffs")
    C_0 = coeffs["C_0"]*length/(10e-6)
    C_c = coeffs["kappa"]*C_0
    C = np.zeros((B, B))
    C[list(range(B)), list(range(B))] = C_0
    C[list(range(B-1)), list(range(1, B))] = C_c
    C[list(range(1, B)), list(range(B-1))] = C_c
    return C


def metal_wire_resistance(length, spacing, width, layer=4):
    '''
    generates the resitance for commercial  global metal wires in dependence
    of the wire "spacing", "width" and "length"
    OPTIONS:
        layer = metal layer of the wires (default is 4). Since we consider
                global wires, the metal layer has to be bigger than 3
    (!TESTED!  WITH MAX. INTERPOLATION ERROR SMALLER THAN 0.4%)
    '''
    layer = min(layer, 5)  # from layer 5 on paras constant
    width_ref = []
    spacing_ref = []
    res_ref = []
    with open('./interconnect/parasitics/metal_wire_info_m%d.csv'
              % layer) as f:
        reader = csv.reader(f, delimiter=',')
        for row in reader:
            width_ref.append(row[0])
            spacing_ref.append(row[1])
            res_ref.append(row[4])  # values normalized per 100 um
    width_ref = [float(x) for x in width_ref[1:]]
    spacing_ref = [float(x) for x in spacing_ref[1:]]
    res_ref = [float(x) for x in res_ref[1:]]
    res_ref = np.array(res_ref)
    var_ref = np.zeros((len(width_ref), 2))
    var_ref[:, 0] = width_ref  # x_values to interpolate (width)
    var_ref[:, 1] = spacing_ref  # y_values to interpolate (TSV pitch)
    res = griddata(var_ref, res_ref, (width, spacing), method='cubic')
    return float(res*length/100e-6)


# # # ---------- 3D TSV FUNCTIONS (TESTED) ------------------------------------


def get_TSV_cap_model_coeffs(C):
    '''
    returns the edge effect aware model coefficients
    for a given TSV capacitance matric C
    (this function is later used in "TSV_cap_model_coeffs)
    '''
    # model non considered capas by bigger ground capas
    C_c0 = sum(C[0, :])-2*C[0, 1]-2*C[0, 2]-C[0, 6]
    C_e0 = sum(C[2, :])-2*C[2, 1]-2*C[2, 6]-2*C[2, 0]-C[2, 7]
    coeffs = dict()
    coeffs['C_n'] = C[11, 12]  # this value is TSV length specific
    coeffs['lambda_d'] = [C[6, 12]/C[7, 12], C[1, 5]/C[7, 12]]
    coeffs['lambda_c'] = [C_c0/C[7, 12], C[0, 1]/C[7, 12], C[0, 2]/C[7, 12]]
    coeffs['lambda_e'] = [C_e0/C[7, 12], C[1, 2]/C[7, 12], C[1, 3]/C[7, 12]]
    return coeffs


def TSV_cap_model_coeffs(r, d, **options):
    '''
    interpolates the edge effect aware TSV capacitance matric coefficients
    for a TSV radius "r" and a TSV pitch "d"
    from a given extracted set of capacitance matrices
    (this function is later used in "TSV_cap_matrix")
    OPTIONS:
        f = significant frequency of the TSV signals (default is 6GHZ)
        ground_ring := if set to 1 than a p+ groundring around the array
                 is assumed (diffusion depth 1um)
        mv = if  "vdd" => mean TSV voltage equal to 1V (assumed as Vdd)
             if  "gnd" => mean TSV voltage eqaul to 0V (assumed as Vss)
    '''
    if options.get("mv"):
        mv = "_"+options.get("mv")
    else:
        mv = ""
    if not options.get("f"):
        f = 6e9
    else:
        f = options.get("f")
    if options.get("ground_ring"):
        nm = ('./interconnect/parasitics/gnd_ring_f%d' % (f/1e9))
    else:
        nm = ('./interconnect/parasitics/no_gnd_ring_f%d' % (f/1e9))
    r_ref = [0.5, 0.5, 1.0, 1.0, 2.0, 2.0, 2.0, 2.5, 2.5, 4.0]  # given in um
    d_ref = [2.0, 2.5, 4.0, 4.5, 5.5, 8.0, 8.5, 7.0, 10, 15]  # given in um
    C_n = []
    lambda_d = []
    lambda_dc = []  # diagonal over corner
    lambda_c0 = []
    lambda_c1 = []
    lambda_c2 = []
    lambda_e0 = []
    lambda_e1 = []
    lambda_e2 = []
    for x in range(len(r_ref)):
        fl_string = '%s_r%d_d%d%s.csv' % (nm, (10*r_ref[x]), (10*d_ref[x]), mv)
        C = np.zeros((25, 25))
        check_flag = 0
        i = 0
        with open(fl_string) as fl:
            reader = csv.reader(fl, delimiter=',')
            for row in reader:
                if check_flag == 1:
                    if row == []:
                        reader = []
                        check_flag = 0
                    else:
                        if row[1] != 'TSV1':
                            C[i, 0:] = row[1:26]
                            i += 1
                if row == ['Spice Capacitance Matrix']:
                    check_flag = 1
        coeffs = get_TSV_cap_model_coeffs(C)
        C_n.append(coeffs["C_n"])
        lambda_d.append(coeffs["lambda_d"][0])
        lambda_dc.append(coeffs["lambda_d"][1])
        lambda_c0.append(coeffs["lambda_c"][0])
        lambda_c1.append(coeffs["lambda_c"][1])
        lambda_c2.append(coeffs["lambda_c"][2])
        lambda_e0.append(coeffs["lambda_e"][0])
        lambda_e1.append(coeffs["lambda_e"][1])
        lambda_e2.append(coeffs["lambda_e"][2])
    var_ref = np.zeros((len(r_ref), 2))
    var_ref[:, 0] = r_ref  # x_values to interpolate (radius)
    var_ref[:, 1] = d_ref  # y_values to interpolate (TSV pitch)
    var_ref = var_ref*1e-6  # get rid of [um] nominalization
    C_n = griddata(var_ref, C_n, (r, d), method='cubic')
    lambda_d = griddata(var_ref, lambda_d, (r, d), method='cubic')
    lambda_dc = griddata(var_ref, lambda_dc, (r, d), method='cubic')
    lambda_c0 = griddata(var_ref, lambda_c0, (r, d), method='cubic')
    lambda_c1 = griddata(var_ref, lambda_c1, (r, d), method='cubic')
    lambda_c2 = griddata(var_ref, lambda_c2, (r, d), method='cubic')
    lambda_e0 = griddata(var_ref, lambda_e0, (r, d), method='cubic')
    lambda_e1 = griddata(var_ref, lambda_e1, (r, d), method='cubic')
    lambda_e2 = griddata(var_ref, lambda_e2, (r, d), method='cubic')
    coeffs = dict()
    coeffs['C_n'] = C_n/5  # this value is given per 10um TSV length
    coeffs['lambda_d'] = [lambda_d, lambda_dc]  # lambda value length indep
    coeffs['lambda_c'] = [lambda_c0, lambda_c1, lambda_c2]
    coeffs['lambda_e'] = [lambda_e0, lambda_e1, lambda_e2]
    return coeffs


def TSV_cap_matrix(length, n, m, r, d,  **options):
    '''
    generates the cap matrix for an n x m global TSV array
    with TSV length "length", radius "r" and min. pitch "d"
    OPTIONS:
        f = significant frequency of the TSV signals (default is 6GHZ)
        ground_ring = if set a ground ring for noise supression around the
                  is assumed (default 0)
        coeffs = if coeffs have been interpolated before with function
                "TSV_cap_model_coeffs" this option allows to pass the
                "coeffs" which  avoids to redo the interpolation to
                speed up the executiontime
        mv = if  "vdd" => mean TSV voltage equal to 1V (assumed as Vdd)
             if  "gnd" => mean TSV voltage eqaul to 0V (assumed as Vss)
    '''
    if not options.get("coeffs"):
        gr = options.get("ground_ring")
        mv = options.get("mv")
        f = options.get("f")
        coeffs = TSV_cap_model_coeffs(r, d, f=f, ground_ring=gr, mv=mv)
    else:
        coeffs = options.get("coeffs")
    B = m*n   # number of TSVs
    C_n = coeffs["C_n"]
    C = np.zeros((B, B))
    # -------  MIDDLE TSVS ----------------------------------------------------
    for row in range(1, (m-1)):  # iterate over all middle TSVs
        for col in range(1, (n-1)):
            i = row*n+col  # index of current TSV in matrix
            C[i, (i-n, i-1, i+1, i+n)] = C[(i-n, i-1, i+1, i+n), i] = 1  # h/v
            C[i, (i-n-1, i-n+1, i+n-1, i+n+1)] = coeffs["lambda_d"][0]  # diag
            C[(i-n-1, i-n+1, i+n-1, i+n+1), i] = coeffs["lambda_d"][0]  # diag
    # ------- EDGE TSVs (corners later overwritten) --------------------------
    # # # following lines: one multiline command for the ground capas
    C[range(n), range(n)] = \
        C[range(B-n, B), range(B-n, B)] = \
        C[range(0, B, n), range(0, B, n)] = \
        C[range(n-1, B, n), range(n-1, B, n)] = \
        coeffs["lambda_e"][0]
    # # # following lines: one multiline command for the hor/vert capas
    C[range(0, n-1), range(1, n)] = \
        C[range(1, n), range(0, n-1)] = \
        C[range(B-n, B-1), range(B-n+1, B)] = \
        C[range(B-n+1, B), range(B-n, B-1)] = \
        C[range(0, B-n, n), range(n, B, n)] = \
        C[range(n, B, n), range(0, B-n, n)] = \
        C[range(n-1, B-n, n), range(2*n-1, B, n)] = \
        C[range(2*n-1, B, n), range(n-1, B-n, n)] = \
        coeffs["lambda_e"][1]
    # # # following lines: one multiline command for 2nd order hor/vert capas
    C[range(0, n-2), range(2, n)] = \
        C[range(2, n), range(0, n-2)] = \
        C[range(B-n, B-2), range(B-n+2, B)] = \
        C[range(B-n+2, B), range(B-n, B-2)] = \
        C[range(0, B-2*n, n), range(2*n, B, n)] = \
        C[range(2*n, B, n), range(0, B-2*n, n)] = \
        C[range(n-1, B-2*n, n), range(3*n-1, B, n)] = \
        C[range(3*n-1, B, n), range(n-1, B-2*n, n)] = \
        coeffs["lambda_e"][2]

    # ------- CORNERS ---------------------------------------------------------
    C[(0, n-1, m*n-n, m*n-1), (0, n-1, m*n-n, m*n-1)] = coeffs["lambda_c"][0]
    # # # following lines: one multiline command for the hor/vert capas
    C[0, (1, n)] = C[(1, n), 0] = \
        C[n-1, (n-2, 2*n-1)] = C[(n-2, 2*n-1), n-1] = \
        C[B-n, (B-2*n, B-n+1)] = C[(B-2*n, B-n+1), B-n] = \
        C[B-1, (B-2, B-n-1)] = C[(B-2, B-n-1), B-1] = \
        coeffs["lambda_c"][1]
    # # # following lines: one multiline command for 2nd order hor/vert capas
    C[0, (2, 2*n)] = C[(2, 2*n), 0] = \
        C[n-1, (n-3, 3*n-1)] = C[(n-3, 3*n-1), n-1] = \
        C[B-n, (B-3*n, B-n+2)] = C[(B-3*n, B-n+2), B-n] = \
        C[B-1, (B-3, B-2*n-1)] = C[(B-3, B-2*n-1), B-1] = \
        coeffs["lambda_c"][2]
    # # #
    C[(1, n-2, (m-2)*n, (m-1)*n-1), (n, 2*n-1, (m-1)*n+1, m*n-2)] = \
        coeffs["lambda_d"][1]
    C[(n, 2*n-1, (m-1)*n+1, m*n-2), (1, n-2, (m-2)*n, (m-1)*n-1)] = \
        coeffs["lambda_d"][1]
    C = C*C_n*length/10e-6  # remove normalization
    return C


def TSV_resistance(length, r, f=6e9):
    '''
    calculates the TSV resitance for a TSV radius r
    (considers the skin effect)
    OPTIONAL:
        f = significant frequency of the TSV signals (default is 6GHZ)
    (!TESTED!)
    '''
    # constants
    rho_metal = 1.68e-8  # resistivity of conductor (copper)
    mu_metal = const.mu_0*0.999990  # permeability of cond (copper)
    square_skin_depth = rho_metal/(const.pi*f*mu_metal)
    skin_depth = min(sqrt(square_skin_depth), r)
    A_eff = const.pi*(r**2 - (r-skin_depth)**2)   # effective cross-section
    res = rho_metal/A_eff*length
    return res


def read_TSV_cap_matrix(r, d, **options):
    '''
    this function is used to read the original extracted 5x5 TSV array
    capacitance matrices. (TSV length "length", radius "r" and min. pitch "d")
    OPTIONS:
        f = significant frequency of the TSV signals (default is 6GHZ)
        ground_ring = if set than a p+ groundring around the array
                       is assumed (diffusion depth 1um)
        mv = if  "vdd" => mean TSV voltage equal to 1V (assumed as Vdd)
             if  "gnd" => mean TSV voltage eqaul to 0V (assumed as Vss)
    (!TESTED! - but some extractions for ground rings are still faulty)
    '''
    if options.get("mv"):
        mv = "_"+options.get("mv")
    else:
        mv = ""
    if not options.get("f"):
        f = 6e9
    else:
        f = options.get("f")
    if options.get("ground_ring"):
        nm = ('./parasitics/gnd_ring_f%d' % (f/1e9))
    else:
        nm = ('./parasitics/no_gnd_ring_f%d' % (f/1e9))
    C = np.zeros((25, 25))
    check_flag = 0
    i = 0
    with open('%s_r%d_d%d%s.csv' % (nm, (10e6*r), (10e6*d), mv)) as fl:
            reader = csv.reader(fl, delimiter=',')
            for row in reader:
                if check_flag == 1:
                    if row == []:
                        reader = []
                        check_flag = 0
                    else:
                        if row[1] != 'TSV1':
                            C[i, 0:] = row[1:26]
                            i += 1
                if row == ['Spice Capacitance Matrix']:
                    check_flag = 1
    return C


def TSV_cap_matrix_prob(length, n, m, r, d, bit_prob_vec,  **options):
    '''
    generates the cap matrix for an n x m global TSV array
    with TSV length "length", radius "r" and min. pitch "d".
    Thereby, it considers the one bit probabilities ("bit_prob_vec")
    OPTIONS:
        f = significant frequency of the TSV signals (default is 6GHZ)
        ground_ring = if set a ground ring for noise supression around the
                  is assumed (default 0)
        C_g/C_r = if the TSV capacitances for random data ("C_r") as well as
                  for just zero bits ("C_g") have been determined before they
                  can be  passed which  avoids to redo the interpolation to
                  speed up the executiontime
    '''
    eps = bit_prob_vec - 0.5
    f = options.get("f")
    gr = options.get("ground_ring")
    if isinstance(options.get("C_g"), np.ndarray):
        C_g = options.get("C_g")
    else:
        C_g = TSV_cap_matrix(
            length, n, m, r, d, f=f, ground_ring=gr, mv="gnd")
    if isinstance(options.get("C_r"), np.ndarray):
        C_r = options.get("C_r")
    else:
        C_r = TSV_cap_matrix(
            length, n, m, r, d, f=f, ground_ring=gr)
    dC = C_r - C_g
    uv = np.ones(len(eps))  # vector of ones
    eps_mat = np.outer(eps, uv) + np.outer(eps, uv)  # episilon matrix
    C = C_r + (eps_mat*dC)
    return C


def TSV_cap_matrix_eps(length, n, m, r, d, eps,  **options):
    '''
    generates the cap matrix for an n x m global TSV array
    with TSV length "length", radius "r" and min. pitch "d".
    Thereby, it considers the set probalities of the bits pr
    (eps[i] = pr[i] - 0.5)
    OPTIONS:
        f = significant frequency of the TSV signals (default is 6GHZ)
        ground_ring = if set a ground ring for noise supression around the
                  is assumed (default 0)
        C_g/C_r = if the TSV capacitances for random data ("C_r") as well as
                  for just zero bits ("C_g") have been determined before they
                  can be  passed which  avoids to redo the interpolation to
                  speed up the executiontime
    (WRITTEN FOR LATER PURPOSES AS IT IS REQUIRED FOR INVERT CODING)
    '''
    f = options.get("f")
    gr = options.get("ground_ring")
    if isinstance(options.get("C_g"), np.ndarray):
        C_g = options.get("C_g")
    else:
        C_g = TSV_cap_matrix(
            length, n, m, r, d, f=f, ground_ring=gr, mv="gnd")
    if isinstance(options.get("C_r"), np.ndarray):
        C_r = options.get("C_r")
    else:
        C_r = TSV_cap_matrix(
            length, n, m, r, d, f=f, ground_ring=gr)
    dC = C_r - C_g
    uv = np.ones(len(eps))  # vector of ones
    eps_mat = np.outer(eps, uv) + np.outer(eps, uv)  # episilon matrix
    C = C_r + (eps_mat*dC)
    return C
