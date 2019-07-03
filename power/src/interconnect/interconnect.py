'''
defines the actual (top) interconnect class, based
on "phy_struct", "data", and "perf_est"
'''
import interconnect.phy_struct as phs
import interconnect.data as data
import interconnect.metrics as met
import math
from scipy.optimize import fsolve
import numpy as np
from interconnect.driver import Driver
import warnings


class Interconnect():
    '''
    Top level class interconnect, which combines all modules.
    For the metal wires per default no buffer insertion
    is applied (segments=1), if segments is non equal 1,
    an equi-distant buffer insertion is assumed.
    If TSVs is set, an 3D interconnect structure is defined via the metal-wires
    in the source layer (the layer where the TSVs cross the substrate), and the
    TSVs. Possible wires in the destination layer build a new interconnect
    distance. Per default for interconnect.delay at all wire-->TSV and TSV-->
    wire boundaries a buffer is assumed. For no buffers at the bundaries
    '''
    def __init__(self, B, wire_spacing, wire_width, wire_length=1e-3,
                 metal_layer=5, segments=1, Driver=Driver.predefined(),
                 TSVs=False, TSV_radius=1e-6, TSV_pitch=4e-6, TSV_length=50e-6,
                 KOZ=8e-6, n=None, m=None, ground_ring=False):
        if TSVs:
            self.is_3D = True
            if n is None:
                n = round(math.sqrt(B))
                m = math.ceil(B/n)  # assume as quadratic as possible
                if n*m > B:
                    print("----------------------INFO------------------------")
                    print("Virtuell increase of link width from %d to %d bit"
                          % (B, n*m))
                    print("to make it fit into a quadratic array")
                    print("--------------------------------------------------")
                    B = n*m
            elif n*m != B:
                raise ValueError("Metal wire and TSV bit width do not match")
            self.C_3D = phs.TSV_cap_matrix(TSV_length, n, m, TSV_radius,
                                           TSV_pitch, ground_ring=ground_ring)
            # store C_3D_g as private property to avoid recalculations
            self._C_3D_g = phs.TSV_cap_matrix_prob(TSV_length, n, m,
                                                   TSV_radius,  TSV_pitch,
                                                   np.zeros(B),  # worst case
                                                   ground_ring=ground_ring)
            self.R_3D = phs.TSV_resistance(TSV_length, TSV_radius)
            self.TSV_array_n, self.TSV_array_m = n, m
            self.TSV_pitch, self.TSV_radius = TSV_pitch, TSV_radius
            self.TSV_length = TSV_length
            self.KOZ = KOZ
            self.ground_ring = ground_ring
        else:
            self.is_3D = False
        self.B = B
        self.segments = segments
        self._Nb = segments+1  # number of buffers
        # self C_2D is the overall acc. capacitance for all segments acc
        self.wire_length, self.wire_spacing = wire_length, wire_spacing
        self.wire_width = wire_width
        self.C_2D = phs.metal_wire_cap_matrix(wire_length, B, wire_spacing,
                                              wire_width, layer=metal_layer)
        self.R_2D = phs.metal_wire_resistance(wire_length, wire_spacing,
                                              wire_width, layer=metal_layer)
        self.Driver = Driver
        self.metal_layer = metal_layer

    @property
    def area_3D(self):
        '''
        returns the required silicon area
        (0) if no TSVs are used
        '''
        if self.is_3D is False:
            return 0
        else:
            return met.TSV_array_area(self.TSV_pitch, self.TSV_array_m,
                                      self.TSV_array_n, self.KOZ)

    @property
    def power_ratio_3D_over_2D(self):
        '''
        roughly estimates how much power is consumed by the TSVs
        compared to the metal wires by comparing the capacitance matrices
        '''
        return sum(sum(self.C_3D))/sum(sum(self.C_2D))

    def power(self, data_inst=None, mux=None, f=1e9):
        '''
        returns power consumption of the interconnect for the specified
        clock frequency "f". Also the transmitted data can be defined via
        the variable "data_inst", by either:
          #1: assigning instance of class "DataStream" or "DataStreamProbs"
          #2: assigning array of samples (numpy or list/tuple)
          #3: assigning mutliple instances of the the class "DataStream" or
              "DataStreamProbs" via a list/tuple. In this scenarios, the
              probabilities for the interleaving of the single DataStreams
              has to be defined via variable "mux"
        If "data_inst" is 'None', random data is assumed
        '''
        # # VAR1.__t_0 data unspecified (random data no hold assumed)
        if data_inst is None:
            C = self.C_2D + self.C_3D if self.is_3D is True else np.copy(
                self.C_2D)
            return met.mean_power2(C, self.Driver.V_dd, f, self.Driver.C_in)  \
                + self._Nb*self.B*self.Driver.mean_power(f=f)  # C load dep neg
        # # VAR2: data specified
        if isinstance(data_inst, (list, tuple, np.ndarray)):  # samples
                data_inst = data.DataStream(data_inst, self.B)
        data_inst = data.DataStreamProb(data_inst, mux)
        C = np.copy(self.C_2D)
        if self.is_3D is True:
            C = C + phs.TSV_cap_matrix_prob(self.TSV_length, self.TSV_array_n,
                                            self.TSV_array_m, self.TSV_radius,
                                            self.TSV_pitch,
                                            data_inst.bit_prob_vec,
                                            C_r=self.C_3D, C_g=self._C_3D_g,
                                            ground_ring=self.ground_ring)

        return met.mean_power(data_inst.toggle_prob_vec,
                              data_inst.corr_switching_mat, C,
                              self.Driver.V_dd, f, self.Driver.C_in) \
            + self._Nb*sum(
                self.Driver.mean_power(data_inst.toggle_prob_vec, f))

    def E(self, data_inst=None, mux=None):
        '''
        (mean) energy consumptions per clock cycle for the transmission
        of "data_inst" (for more details on "data_inst" and "mux" see
        description of class function "power")
        '''
        return self.power(data_inst=data_inst, mux=mux, f=1)

    def prop_delay(self, wc_switching=None, verbose=False):
        '''
        -- delay of the interconnect structure --
        if CAC coding is applied: "wc_switching" should be defined
        '''
        t_2d_seg = met.delay(self.C_2D/self.segments, self.R_2D/self.segments,
                             self.Driver.t_0(), self.Driver.C_in,
                             self.Driver.R_on(2*max(sum(self.C_2D)
                                                    - self.C_2D[0, 0])),
                             wc_switching=wc_switching)
        if self.is_3D is True:
            t_3d = met.delay(self._C_3D_g, self.R_3D, self.Driver.t_0(),
                             self.Driver.C_in, self.Driver.R_on(
                             2*max(sum(self._C_3D_g) - self._C_3D_g[0, 0])),
                             wc_switching=wc_switching)
        else:
            t_3d = 0
        if verbose is True:
            print("Metal wire delay: %s in [s]" % self.segments*t_2d_seg)
            print("       TSV delay: %s in [s]" % t_3d)
        return self.segments*t_2d_seg+t_3d

    def prop_delay_3D_no_buff(self, wc_switching=None):
        '''
        -- delay of the interconnect structure for no buffers
        between metal wires and TSVs--
        (in pratice not recommended due to signal integrity issues!)
        if CAC coding is applied: "wc_switching" should be defined.
        If you want the delay of the metal wires for no buffers,
        just set segments to '1' and run function 'prob_delay'
        '''
        if (self.is_3D and self.segments == 1) is False:
            raise ValueError("Segment count has to be one (no buff 2D),"
                             " and the interconnect has to be 3D")
        C = [self.C_2D, self._C_3D_g]
        R = [self.R_2D, self.R_2D]
        return met.delay_mult_no_buf(C, R, self.Driver.t_0(),
                                     self.Driver.C_in, self.Driver.R_on(
                             2*max(sum(self._C_3D_g) - self._C_3D_g[0, 0])),
                         wc_switching=wc_switching)

    def max_metal_wire_length(self, f, factor=0.8, max_segments=10,
                              wc_switching=None):
        '''
        function that estimates how long the metal-wires
        of a 2D or 3D interconnect can be if it should run with
        a clock frequency of "f". "factor" smaller than one is
        used to leaf some times for the remaining elements (e.g.
        Flip-Flops).For example, for "factor"=0.8: 20%*of the clock
        cycle is left for the remaining circuit.
        (N_max: maximum buffers inserted)
        '''
        warnings.filterwarnings(
            'ignore', 'The iteration is not making good progress')
        t_left = factor/f
        c = self.C_2D/self.wire_length  # cap per length
        r = self.R_2D/self.wire_length  # res per length
        if self.is_3D is True:
            t_3D = met.delay(self._C_3D_g, self.R_3D, self.Driver.t_0(),
                             self.Driver.C_in, self.Driver.R_on(
                            2*max(sum(self._C_3D_g) - self._C_3D_g[0, 0])),
                             wc_switching=wc_switching)
            t_left -= t_3D
            if t_left <= 0:
                raise ValueError('TSV array delay %f [ns] to big'
                                 % (t_left*1e9))
        else:
            t_3D = 0

        guess_l = self.wire_length*((t_left-self.Driver.t_0())/(
            self.prop_delay()-t_3D-self.Driver.t_0()))  # makes a guess

        def fu(i):
            def f_int(x):
                t_seg = met.delay(c*x/i, r*x/i, self.Driver.t_0(),
                                  self.Driver.C_in,
                                  self.Driver.R_on(2*max(sum(c*x)
                                                         - c[0, 0]*x)),
                                  wc_switching=wc_switching)
                return (t_left-i*t_seg)
            return f_int
        l_max = segs = 0
        for i in range(1, max_segments+1):
            if (i+1)*self.Driver.t_0() <= t_left:  # check if general possible
                var = fsolve(fu(i), guess_l)
                [segs, l_max] = [i, var] if var > l_max else [segs, l_max]
        return {'Max Wire Length': float(l_max), 'Segments': segs}

    def metal_wire_length(self, length, segments=1):
        '''
        creates an copy of the instance of the class "Interconnect",
        with the exception that the metal-wire-length is set to
        "length", and the segments are equal to "segments"
        '''
        if self.is_3D is True:
            return Interconnect(self.B, self.wire_spacing, self.wire_width,
                                self.wire_length, self.metal_layer,
                                segments, self.Driver, True,
                                self.TSV_radius, self.TSV_pitch,
                                self.TSV_length, self.KOZ,
                                self.TSV_array_n, self.TSV_array_m,
                                self.ground_ring)
        else:
            return Interconnect(self.B, self.wire_spacing, self.wire_width,
                                self.wire_length, self.metal_layer,
                                segments, self.Driver, False)
