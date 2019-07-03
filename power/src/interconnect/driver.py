'''
Defines a class for the used driver
'''
import numpy as np
from scipy.interpolate import interp1d


class Driver():
    def __init__(self, R_on, t_0, C_in, boundaries=[1, 1, 1],
                 E_toggle=0, V_dd=1, area=0, id=None):
        if np.array(R_on).size == np.array(t_0).size == 1:
            print('Warning: for accurate modeling pass three "R_on" and "t_on"'
                  + ' values (including the boundaries), to model the diff'
                  + 'operation regions')
            self._R_on = [R_on, R_on, R_on]
            self._t_0 = [t_0, t_0, t_0]
        else:
            self._R_on, self._t_0 = R_on, t_0
        self.__E_toggle = E_toggle
        self.C_in = C_in   # eq capactiance of the input
        self.id = id
        self.__boundaries = boundaries
        self.V_dd = V_dd  # power supply voltage
        self.area = area  # silicon area

    def R_on(self, C_load=10e-15):
        '''
        Returns R_on for the given load capacitance
        (linear interpolation between given valuea
        '''
        fu = interp1d(self.__boundaries, self._R_on)
        return float(
            fu(min(C_load, self.__boundaries[len(self.__boundaries)-1])))

    def t_0(self, C_load=10e-15):
        '''
        Returns R_on for the given load capacitance
        (linear interpolation between given valuea
        '''
        fu = interp1d(self.__boundaries, self._t_0)
        return float(
            fu(min(C_load, self.__boundaries[len(self.__boundaries)-1])))

    def mean_power(self, toggle_activity=0.5, f=1e9, C_load=10e-15):
        '''
        returns the man power consumption of the driver
        independence of the toggle activity and the clock
        frequeny f
        '''
        fu = interp1d(self.__boundaries, self.__E_toggle)
        E_toggle = fu(min(C_load, self.__boundaries[len(self.__boundaries)-1]))
        return float(E_toggle)*f*toggle_activity

    @classmethod
    def predefined(cls, technology='comm_45nm', driver_strength=4):
        '''
        if driver parameters for any given technology have been stored
        in the directory "driver_parameters" this class method is used
        to use them
        '''
        par = np.load('./interconnect/driver_parameters/driver_%s_d%d.npz'
                      % (technology, driver_strength))
        return cls(R_on=par['R_on'], t_0=par['t_0'], C_in=float(par['C_in']),
                   boundaries=par['boundaries'],
                   E_toggle=par['E_toggle'], V_dd=float(par['V_dd']),
                   area=float(par['A']), id=str(par['id']))
