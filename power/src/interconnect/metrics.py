'''
This module defines several functions used for the power
delay and silicon area estimation in 2D and 3D links
(silicon area of metal wires (2D) is zero) and for TSVs
(3D) the area is only required in the dies where the TSV
traverses the silicon substrate
'''
import numpy as np


# # # -------- POWER ESTIMATION -----------------------------------------------

def mean_norm_power(Ts, Tc, C):
    '''
    this function calculates mean dynamic power consumption,
    normalized by Vdd²f_clk/2,
    for an N-bit  interconnect structure with capacitance matrix C
    transmitting data with the switching properties Ts and Tc
    '''
    if Tc.shape != C.shape:
        raise ValueError("Data and interconnect bit width do not match")
    I_nxn = np.ones(C.shape)  # NxN matrix of ones
    T = (Ts*I_nxn).transpose() - Tc
    P_mat = np.multiply(T, C)
    return P_mat.sum()


def mean_norm_power2(T, C):
    '''
    this function calculates mean dynamic power consumption,
    normalized by Vdd²f_clk/2,
    for an N-bit  interconnect structure with capacitance matrix C
    transmitting data with the switching properties Ts and C
    '''
    if T.shape != C.shape:
        raise ValueError("Data and interconnect bit width do not match")
    P_mat = np.multiply(T, C)
    return P_mat.sum()


def mean_power(Ts, Tc, C,  Vdd, f_clk, C_driver=0):
    '''
    this function calculates mean dynamic power consumption
    for an N-bit  interconnect structure with spice capacitance matrix C
    transmitting data with the switching properties Ts and Tc
    '''
    C_i = np.copy(C)
    C_i[range(len(C_i)), range(len(C_i))] = C_i.diagonal()+C_driver
    P_norm = mean_norm_power(Ts, Tc, C_i)
    return P_norm*0.5*(Vdd**2)*f_clk


def mean_power2(C, Vdd=1, f_clk=1e9, C_driver=0, **options):
    '''
    this function calculates mean dynamic power consumption
    for an N-bit  interconnect structure with capacitance matrix C
    OPTIONS:
         T = matrix with the switching probalities
             (if not given random data is assumed)
    '''
    if options.get("T"):
        T = options.get("T")
    else:
        T = 0.5*np.ones((len(C), len(C)))
    C_i = np.copy(C)
    C_i[range(len(C_i)), range(len(C_i))] = C_i.diagonal()+C_driver
    P_norm = mean_norm_power2(T, C_i)
    return P_norm*0.5*(Vdd**2)*f_clk


def mean_energy(Ts, Tc, C, Vdd, C_driver=0):
    '''
    this function calculates mean dynamic energy disipation per clock cycle
    for an N-bit  interconnect structure with spice capacitance matrix C
    transmitting data with the switching properties Ts and Tc
    '''
    C_i = np.copy(C)
    C_i[range(len(C_i)), range(len(C_i))] = C_i.diagonal()+C_driver
    P_norm = mean_norm_power(Ts, Tc, C_i)
    return P_norm*0.5*(Vdd**2)


def mean_energy2(C, Vdd, C_driver=0, **options):
    '''
    this function calculates mean dynamic energy disipation per clock cycle
    for an N-bit  interconnect structure with capacitance matrix C
    OPTIONS:
         T = matrix with the switching probalities
             (if not given random data is assumed)
    '''
    if options.get("T"):
        T = options.get("T")
    else:
        T = 0.5*np.ones((len(C), len(C)))
    C_i = np.copy(C)
    C_i[range(len(C_i)), range(len(C_i))] = C_i.diagonal()+C_driver
    P_norm = mean_norm_power2(T, C_i)
    return P_norm*0.5*(Vdd**2)


# # # -------- DELAY ESTIMATION -----------------------------------------------


def delay(C, R, t_0=0, C_in=0, R_on=0, **options):
    '''
    this function estimates the elmore-delay for an
    interconnect architecture  with capacitance matrix c,
    interconnect resistance R and load capacitance C_l.
    Driver modeled by eq. input capacitance (C_in), offset delay (t_o)
    and the on resitance of the transistors (R_on)
    OPTIONS:
         delay = "propagation", "risetime", or "elmore"
                 (calculate either the propagation time (0->50%),
                  the elmore delay (0->63.2%) or the rise time (10%->90%)
                  DEFAULT: "propagation"
        wc_switching = matrix containing the worst case switching/coupling
                       for CAC data (DEFAULT: no CAC coding)
    '''
    delay = options.get("delay")
    if options.get("wc_switching"):
        M_wc = options.get("wc_switching")
        if M_wc.shape != C.shape:
            raise ValueError("Data and interconnect bit width do not match")
    else:
        M_wc = 2*np.ones((len(C), len(C)))
        M_wc[range(len(M_wc)), range(len(M_wc))] = 1
    C_wc = np.max(np.diag(np.dot(M_wc, C)))  # max effec. interconnect capa
    if delay == "risetime":
        t1, t2 = [0.9*C_wc+2.2*C_in, 2.2*(C_in+C_wc)]
    elif delay == "elmore":
        t1, t2 = [0.5*C_wc+C_in, C_in+C_wc]
    else:
        t1, t2 = [0.38*(C_wc+2*C_in), 0.69*(C_in+C_wc)]
    return R*t1+R_on*t2+t_0


def delay_mult_no_buf(C, R, t_0=0, C_in=0, R_on=0, **options):
    '''
    this function estimates the elmore-delay for serialized
    interconnect architecture  with capacitance matrices C[i],
    interconnect resistance R[i]. Thereby, no buffers between the interconnects
    are assumed. For the delay whith buffer insertion, just calculate the delay
    values of the single interconnects via the function "delay" and add the
    results up.
    Driver modeled by eq. input capacitance (C_in), offset delay (t_o)
    and the on resitance of the transistors (R_on).
    OPTIONS:
         delay = "propagation", "risetime", or "elmore"
                 (calculate either the propagation time (0->50%),
                  the elmore delay (0->63.2%) or the rise time (10%->90%)
                  DEFAULT: "propagation"
        wc_switching = matrix containing the worst case switching/coupling
                       for CAC data (DEFAULT: no CAC coding)
    '''
    if len(R) != len(C):
        raise ValueError("R and C have to be of the same length")
    delay = options.get("delay")
    if options.get("wc_switching"):
        M_wc = options.get("wc_switching")
        if M_wc.shape != C.shape:
            raise ValueError("Data and interconnect bit width do not match")
    else:
        M_wc = 2*np.ones(C[0].shape)
        M_wc[range(len(M_wc)), range(len(M_wc))] = 1
    C_wc = [np.max(np.diag(np.dot(M_wc, C[i]))) for i in range(len(C))]
    if delay == "risetime":
        c = 2.2
    elif delay == "elmore":
        c = 1
    else:
        c = 0.69
    t = 0
    for i in range(len(R)):
        t += t_0+c*(R_on+sum(R[:i])+0.5*R[i])*C_wc[i]
    t += c*(R_on+sum(R[:i]))*C_in
    return t


# # # ------ TSV ARRAY AREA ---------------------------------------------------

def TSV_array_area(d, m, n, w_koz):
    '''
    calculates the TSV area occupation for a m x n TSV array
    with TSV pitch d and KOZ w_koz
    '''
    area = (m*d + 2*w_koz)*(n*d + 2*w_koz)
    return area
