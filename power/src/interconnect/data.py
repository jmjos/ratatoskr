'''
defines the data stream classes as well as important
functions for general pattern  analysis
'''

import numpy as np
import scipy.signal
import random
import math
import itertools
import copy

# # # ------ CLASS INDEPENDENT FUNCTIONS --------------------------------------


def de2bis(data, B):
    '''
    performs decimal to bin conversion for unsigned
    and signed numbers (automatically done)
    '''
    if (    # Check if it fits into the specified bit width
            (min(data) < 0 and max(data) >= (1 << (B-1))) or
            (max(data) >= (1 << B)) or (min(data) < -(1 << (B-1)))):
        raise ValueError('de2bi: Bit width %i is to small' % B)
    else:
        return [np.binary_repr(x, width=B) for x in data]


def uniform_data(N, B, ro=0):
    '''
    generates N fullly random (uniform distributed) B-bit pattterns
    (signed) with a sample corelation of ro
    '''
    N = int(N)
    data = np.random.uniform(-(1 << B-1), (1 << B-1)-1, N)  # random signal
    data = data - np.mean(data)
    if ro != 0:
        y = math.sqrt(1-ro**2)*scipy.signal.lfilter([1, 0], [1, -ro], data)
        y = y - np.mean(y)  # remove mean
    else:
        y = data
    data = np.clip(np.round(y), -(1 << B-1), (1 << B-1)-1)
    return np.array(data).astype(int)


def gaussian_data(N, B, log2_std, mu, ro=0, is_signed=True):
    '''
    generates N gaussian distributed B-bit patterns
    with a std derivation (sigma) of 2**std (std given in bits/log2),
    a mean of mu, and a rel. pattern correlation of ro
    OPTIONAL:
       is_signed = if true according bit vectors are unsigned
                  DEFAULT: True
    '''
    N = int(N)
    x = np.random.normal(0, 1, N)  # mean free gaussian signal
    x = x - np.mean(x)
    if ro != 0:  # corr
        y = math.sqrt(1-ro**2)*scipy.signal.lfilter([1, 0], [1, -ro], x)
        y = y - np.mean(y)
    else:
        y = x
    y = y*(2**log2_std) + mu  # add mean and standard derivation
    if is_signed:
        data = np.clip(np.round(y), -(1 << (B-1)), (1 << (B-1))-1)
    else:
        data = np.clip(np.round(y), 0, (1 << B)-1)
    return data.astype(int)


def lognormal_data(N, B, log2_std, mu, ro=0, is_signed=True):
    '''
    generates N lognornmal distributed B-bit patterns
    with a std derivation (sigma) of 2**std (std given in bits/log2),
    a mean of mu, and a rel. pattern correlation of ro
    OPTIONAL:
       is_signed = if true according bit vectors are unsigned
                  DEFAULT: True
    '''
    N = int(N)
    x = np.random.lognormal(0, 1, N)  # mean free lognormal signal
    x = x - np.mean(x)
    if ro != 0:  # corr
        y = math.sqrt(1-ro**2)*scipy.signal.lfilter([1, 0], [1, -ro], x)
        y = y - np.mean(y)
    else:
        y = x
    y = y*(2**log2_std) + mu  # add mean and standard derivation
    if is_signed:
        data = np.clip(np.round(y), -(1 << (B-1)), (1 << (B-1))-1)
    else:
        data = np.clip(np.round(y), 0, (1 << B)-1)
    return data.astype(int)


def autocorr(data, k=1):
    '''
    calculates the k-th value of the autocorrelation coefficients
    of the data stream
    '''
    data_mf = np.array(data) - np.mean(data)  # remove mean
    y = data_mf[k:]*data_mf[:len(data_mf)-k]
    autocorr = y.mean()/data_mf.var()
    return autocorr


def corr(data1, data2, k=0):
    '''
    calculates the k-th value of the correlation coeffs of the two data streams
    (!TESTED!)
    '''
    n = min(len(data1), len(data2))
    data1_mf = np.array(data1)-np.mean(np.array(data1))  # remove mean
    data2_mf = np.array(data2)-np.mean(np.array(data2))
    y = data1_mf[k:n]*data2_mf[:n-k]
    corr = y.mean()/(data1_mf.std()*data2_mf.std())
    return corr

# # # ------ CLASS FOR DATA STREAM --------------------------------------------


class DataStream():
    '''
    class for concrete data_stream.
    TWO POSSIBLE INSTANTIATIONS:
       # 1 data_stream_module.data_stream( ...)
       # 2 data_stream_module.data_stream.from_Stoch( ...)
         (used if samples/samples of the data_stream are not known,
          but the stochastic properties --> see according docstring)
    VARS FOR #1: samples = (int) samples of the data stream
                  B     =  bit width
                fit_to_B = if the specified samples can not be presented by
                           B-bit, the LSBs are removed until it fits
               is_signed = signed  binary repr ?
                id       = identifier string
    '''

    version = "0.1"  # class version

    def __init__(self, samples, B=8, fit_to_B=False, is_signed=False, id=None):
        samples = np.array(samples).astype(int)  # samples = np int array
        if is_signed:
            v_range = [-(1 << (B-1)), (1 << (B-1))-1]  # sample range
            B_pos = B - 1    # how many bits are counted positve
        else:
            if np.min(samples) < 0:
                raise ValueError("Negative samples can't be unsigned")
            v_range = [0, (1 << B)-1]
            B_pos = B
        if (np.max(samples) > v_range[1]):
            if fit_to_B:
                req_bits = math.ceil(math.log2(np.max(samples)+1))
                samples = samples >> (req_bits-B_pos)
            else:
                raise ValueError("Values range %d to %d to large for %d bits"
                                 % (np.min(samples), np.max(samples), B))
        if (np.min(samples) < v_range[0]):
            if fit_to_B:
                req_bits = math.ceil(math.log2(-np.min(samples)))  # always pos
                samples = samples >> (req_bits-B_pos)
            else:
                raise ValueError("Values range %d to %d to large for %d bits"
                                 % (np.min(samples), np.max(samples), B))
        self.B = B
        self.samples = samples
        self.is_signed = is_signed
        self.fitted = fit_to_B
        self.id = id  # identifier to find specific data streams

    # # -------------------- MAGIC METHODS ------------------------------------
    def __getitem__(self, idx):
        samples = self.samples[idx]
        return DataStream(B=self.B, samples=samples, is_signed=self.is_signed,
                          fit_to_B=self.fitted)  # NEW ID!

    def __setitem__(self, idx, newsamples):
        samples = self.samples
        samples[idx] = newsamples
        return DataStream(B=self.B, samples=samples, is_signed=self.is_signed,
                          fit_to_B=self.fitted, id=self.id)  # KEEP ID!

    def __len__(self):
        ''' number of patterns '''
        return len(self.samples)

    def __eq__(self, ds2):
        return (self.B == ds2.B) and np.array_equal(self.samples, ds2.samples)\
            and (self.is_signed == ds2.is_signed)

    def __and__(self, ds2):
        '''
        logical ANDs the patterns of the two data streams and writes result in
        new data stream (use as ds1 & ds2)
        '''
        self.check_comp(ds2)
        samples = self.samples & ds2.samples
        return DataStream(B=self.B, samples=samples,
                          is_signed=self.is_signed, fit_to_B=self.fitted,
                          id=("(%s) AND (%s)" % (self.id, ds2.id)))  # NEW ID!

    def __or__(self, ds2):
        '''
        logical ORs the patterns of the two data streams and writes result in
        new data stream (use as ds1 | ds2)
        '''
        self.check_comp(ds2)
        samples = self.samples | ds2.samples
        return DataStream(B=self.B, samples=samples,
                          is_signed=self.is_signed, fit_to_B=self.fitted,
                          id=("(%s) OR (%s)" % (self.id, ds2.id)))  # NEW ID!

    def __xor__(self, ds2):
        '''
        logical XORs the patterns of the two data streams and writes result in
        new data stream (use as ds1 ^ ds2)
        '''
        self.check_comp(ds2)
        samples = self.samples ^ ds2.samples
        return DataStream(B=self.B, samples=samples,
                          is_signed=self.is_signed, fit_to_B=self.fitted,
                          id=("(%s) XOR (%s)" % (self.id, ds2.id)))  # NEW ID!

    def __add__(self, ds2):
        '''
        adds the patterns of the two data streams and writes result in
        new data stream (use as ds1 + ds2)
        (when results dont fit in bitrange, reports error if "self.is_fitted"
        is False else it fits the values by removing the LSB)
        '''
        self.check_comp(ds2)
        samples = self.samples + ds2.samples
        return DataStream(B=self.B, samples=samples,
                          is_signed=self.is_signed, fit_to_B=self.fitted,
                          id=("(%s) + (%s)" % (self.id, ds2.id)))  # NEW ID!

    # # ------------------ REGULAR METHODS ------------------------------------

    def append(self, ds2):
        ''' append data stream 2 (ds2) '''
        if isinstance(ds2, type(self)):
            self.check_B_and_type(ds2)
            app_samples = np.copy(ds2.samples)
        else:
            app_samples = np.array(ds2)
        samples = np.append(self.samples, app_samples)
        self.__init__(samples, self.B, fit_to_B=self.fitted,
                      is_signed=self.is_signed, id=self.id)

    def pattern_corr(self, k=1):
        '''
        calculates the k-th autocorrelation coeff of the data stream
        '''
        return autocorr(self.samples, k)

    def cross_corr(self, ds2, k=0):
        '''
        calculates the k-th correlation of the two data streams
        '''
        self.check_comp(ds2)
        return corr(self.samples, ds2.samples, k)

    def check_B(self, ds2, ref=None):
        if ref is None:
            ref = self.B
        if not isinstance(ds2, type(self)):
            raise ValueError("Operand is not a data stream")
        if ref != self.B or ref != ds2.B:
            raise ValueError("Data streams %s and %s have different bitwidths"
                             % (self.id, ds2.id))

    def check_B_and_type(self, ds2):
        self.check_B(ds2)
        if self.is_signed != ds2.is_signed:
            raise ValueError("Different signs for data streams %s and %s"
                             % (self.id, ds2.id))

    def check_comp(self, ds2):
        self.check_B_and_type(ds2)
        if len(self.samples) != len(ds2.samples):
            raise ValueError("Data streams %s and %s have different lengths"
                             % (self.id, ds2.id))

    # LOW POWER CODING
    def k0_encoded(self, LSB_untouched=False):
        '''
        k0 encoder (XOR adjacent bit), while
        leaving MSB untouched (--> GRAY Encoding) (DEFAULT)
        IF "LSB_untouched" the LSB is untouched
        '''
        samples = np.copy(self.as_unsigned.samples)  # bit vecs as unsigned
        if LSB_untouched is False:
            samples = samples ^ (samples >> 1)
        else:
            samples = samples ^ np.clip((samples << 1), 0, ((1 << self.B) - 1))
        return DataStream(B=self.B, samples=samples, fit_to_B=self.fitted,
                          id=(" KO-coded(%s), with LSB untouched is  (%s)"
                              % (self.id, LSB_untouched)))  # NEW ID!

    def k1_encoded(self):
        '''
        k1 encoder (XOR MSB)
        '''
        samples = np.copy(self.as_unsigned.samples)  # bit vecs  as unsigned
        samples = samples ^ ((samples >> self.B-1)*((1 << self.B-1)-1))
        return DataStream(B=self.B, samples=samples, fit_to_B=self.fitted,
                          id=(" K1-coded(%s)" % self.id))  # NEW ID!

    def corr_encoded(self, k=1):
        '''
        corr encoder (!TESTED!)
        y[i]  = x[i] xor x[i-k] (!TESTED!)
        '''
        samples = np.copy(self.as_unsigned.samples)  # bit vecs as unsigned
        samples[k:] = samples[k:] ^ samples[:len(samples)-k]  # FIR FILTER
        return DataStream(B=self.B, samples=samples, fit_to_B=self.fitted,
                          id=(" Corr-coded(%s)" % self.id))  # NEW ID!

    def decorr(self, k=1):
        '''
        corr decoder (decorr)
        y[i]  = x[i] xor y[i-k]
        '''
        samples = self.as_unsigned.samples  # bit vecs interpreted as unsigned
        for i in range(k, len(samples)):
            samples[i] = samples[i] ^ samples[i-k]   # IIR Filter
        return DataStream(B=self.B, samples=samples, fit_to_B=self.fitted,
                          id=("Decorr (%s)" % self.id))  # NEW ID!

    def bus_invert(self):
        '''
        classical bus invert as proposed by Stan
        '''
        samples = np.copy(self.as_unsigned.samples)  # bit vecs as unsigned
        for i in range(1, len(samples)):
            toggles = bin(samples[i] ^ samples[i-1])[2:].count("1")
            if (toggles << 1) >= self.B+1:  # check if more than (B+1)/2 switch
                samples[i] = samples[i] ^ ((1 << self.B+1)-1)
                # xor with just ones. Length B+1 to automatic add invert bit
        return DataStream(B=self.B+1, samples=samples, fit_to_B=self.fitted,
                          id=("Bus Invert (%s)" % self.id))  # NEW ID!

    def prob_invert(self):
        '''
        bus invert to increase number of ones
        '''
        samples = np.copy(self.as_unsigned.samples)  # bit vecs as unsigned
        for i in range(1, len(samples)):
            ones = bin(samples[i])[2:].count("1")
            if (ones << 1) <= self.B+1:  # check if more than (B+1)/2 are zero
                samples[i] = samples[i] ^ ((1 << self.B+1)-1)
                # xor with just ones. Length B+1 to automatic add invert bit
        return DataStream(B=self.B+1, samples=samples, fit_to_B=self.fitted,
                          id=("Prob. Invert (%s)" % self.id))  # NEW ID!

    def interleaving(self, other_ds, prob):
        '''
        This method (randomly) interleaves the current DataStream instance
        with other instances from DataStream (1 or more), and "prob" defines
        the probabilities of interleaving (example 2 two data streams:
        prob=1.0 --> sample by sample interleaving of the samples.
        prob=0.5 --> on average after a sample of dsI in 50% another dsI
                     sample follows, in 50% a ds2 sample
        output DataStream has the length of the shortest input data stream
        '''
        if not isinstance(prob, (int, float)):
            raise ValueError('"prob" has to be a float')
        # # # NO REAL ITNERLEAVING AS PROB IS ZERO
        if prob == 0:  # no multiplexing
            DS = copy.deepcopy(self)
            if isinstance(other_ds, (list, tuple)):
                for i in range(len(other_ds)):
                    DS.append(other_ds[i])
            else:
                DS.append(other_ds)
            return DS
        # # # REAL INTERLEAVING (PROB NON-ZERO)
        stack = [list(self.samples)]
        N = len(self)  # length of output data stream
        if isinstance(other_ds, (list, tuple)):
            for i in range(len(other_ds)):
                self.check_B_and_type(other_ds[i])
                N = min(N, len(other_ds[i]))
                stack.append(list(other_ds[i].samples))
        else:
            self.check_B_and_type(other_ds)
            N = min(N, len(other_ds))
            stack.append(list(other_ds.samples))
        n_ds = len(stack)  # number of data streams
        curr_ds = 0  # current data stream (start with first)
        other_ds = list(list(range(0, curr_ds)) + list(range(curr_ds+1, n_ds)))
        samples = []
        for i in range(N):
            if random.random() < prob:  # change data stream
                curr_ds = random.choice(other_ds)
                other_ds = list(list(range(0, curr_ds))
                                + list(range(curr_ds+1, n_ds)))
            samples.append(stack[curr_ds].pop(0))
        return DataStream(B=self.B, samples=samples, fit_to_B=self.fitted,
                          is_signed=self.is_signed,
                          id=("Interleaved with prob %.2f" % prob))  # NEW ID!
    # !!

    # # ----------------------- PROPERTIES ------------------------------------
    @property
    def mean(self):
        return np.mean(self.samples)

    @property
    def max(self):
        '''max sample'''
        return np.max(self.samples)

    @property
    def min(self):
        '''min sample'''
        return np.min(self.samples)

    @property
    def bit_width(self):
        return self.B

    @property
    def binary_samples(self):
        '''
        returns a vector of strings with the binary repr as
        '''
        return [np.binary_repr(x, width=self.B) for x in self.samples]

    @property
    def binary_samples_mat(self):
        '''
        NxB matrix containining the N b-bit binary samples
        '''
        mat = np.zeros((len(self), self.B))
        y = self.binary_samples
        for i, j in itertools.product(range(len(self)), range(self.B)):
                mat[i, j] = int(y[i][j])
        return mat.astype(bool)  # return as int for readability?

    @property
    def std_log2(self):
        '''
        log to the base of to of the std dev (sigma)
        '''
        std = np.std(self.samples)
        return math.log2(std)

    @property
    def as_unsigned(self):
        '''
        returns a data streams with the same binary samples as "self" with the
        exception that the bit vectors are interpreted as unsigned
        '''
        if self.is_signed is False:
            samples = self.samples
        else:
            samples = [self.samples[x] -
                       (self.samples[x] >> (self.B-1))*(1 << self.B)
                       for x in range(len(self.samples))]
        return DataStream(B=self.B, samples=samples,
                          is_signed=False, fit_to_B=self.fitted,
                          id="%s as UNSIGNED" % self.id)  # NEW ID!

    @property
    def as_signed(self):
        '''
        returns a data streams with the same binary samples as "self" with the
        exception that the bit vectors are interpreted as signed
        '''
        if self.is_signed is True:
            samples = self.samples
        else:
            samples = [self.samples[x] -
                       (self.samples[x] >> (self.B-1))*(1 << self.B)
                       for x in range(len(self.samples))]
        return DataStream(B=self.B, samples=samples,
                          is_signed=True, fit_to_B=self.fitted,
                          id="%s as SIGNED" % self.id)  # NEW ID!

    @property
    def invert(self):
        '''
        logical neagation of the patterns of the data stream
        '''
        ones = -1 if self.is_signed is True else (1 << self.B)-1
        samples = [self.samples[i] ^ ones for i in
                   range(len(self.samples))]
        return DataStream(B=self.B, samples=samples,
                          is_signed=self.is_signed, fit_to_B=self.fitted,
                          id=("NOT (%s)" % self.id))  # NEW ID!

    @property
    def bit_prob_vec(self):
        '''
        returns a vector containing the one-bit probabilities
        of the bits
        '''
        bm = self.binary_samples_mat
        return np.array([sum(bm[:, x]) for x in range(self.B)])/len(self)

    @property
    def bit_prob_mat(self):
        '''
        returns a vector containing the one-bit probabilities
        of the bits
        '''
        bm = self.binary_samples_mat
        pr = np.zeros((self.B, self.B))
        for i, j in itertools.product(range(self.B), repeat=2):
            pr[i, j] = np.mean(bm[:, i]*bm[:, j])
        return pr

    @property
    def bit_prob(self):
        '''
        returns the one-bit probability
        of the data stream
        '''
        return np.mean(self.binary_samples_mat)

    @property
    def toggle_prob_vec(self):
        '''
        returns a vector containing the one-bit probabilities
        of the data stream
        '''
        delta = self[1:] ^ self[:len(self.samples)-1]
        bm = delta.binary_samples_mat
        return np.array([sum(bm[:, x]) for x in range(self.B)])/len(delta)

    @property
    def toggle_prob(self):
        '''
        returns the one-bit probability
        of the data stream
        '''
        return sum(self.toggle_prob_vec)/self.B

    @property
    def corr_switching_mat(self):
        '''
        returns the correlated switching probabilities E{\Delta b_i\Delta b_j}
        as a matrix
        '''
        bm = self.binary_samples_mat
        delta = bm.astype(int)[1:, :]-bm.astype(int)[:len(self)-1, :]
        M_coup = np.zeros((self.B, self.B))
        for i in range(self.B):
            for j in range(i+1, self.B):
                M_coup[i, j] = M_coup[j, i] = np.mean(delta[:, i]*delta[:, j])
        return M_coup

    @property
    def T(self):
        '''
        returns the T matrix where the diagonal is equal to the toggle
        activities, while entry i,j is equal to the toggle activity of line i
        minus the correlated switching probability of the lines i and j
        '''
        Ts = self.toggle_prob_vec
        Tc = self.corr_switching_mat
        I_nxn = np.ones(Tc.shape)
        return (Ts*I_nxn).transpose() - Tc

    # # ----------------CLASS METHODS------------------------------------------
    @classmethod
    def from_stoch(cls, N, B, uniform=1, ro=0, mu=0, log2_std=4,
                   is_signed=True, id=None, lognormal=None):
        '''
        instead of samples, the stochastic properties are assigned here.
        VARS: N = number of samples (generated synthetically using the stoch.)
              B = bit width
              uniform = IF set (DEFAULT) uniform distributed data
                       ELSE GAUSSIAN DISTRIBUTED:
                         ro, mu = correlation and mean of the patterns
                         log2_std = std deviation (std) in bit (log2)
        '''
        if uniform:
            samples = uniform_data(N, B, ro)
            is_signed = True
        else:
            if lognormal is None:
                samples = gaussian_data(N, B, log2_std, mu, ro,
                                        is_signed=is_signed)
            else:
                samples = lognormal_data(N, B, log2_std, mu, ro,
                                         is_signed=is_signed)
        return cls(samples=samples, B=B, is_signed=is_signed, id=id)

    # # ----------------STATIC METHODS-----------------------------------------


class DataStreamProb():
    '''
    a class which only includes the data stream properties (Ts, Tc, pr, B)
    and no actual samples
    THREE POSSIBLE INSTANTIATIONS:
        # 1: "dsp" = "DataStreamProb(ds)", where "ds"  is an instance of the
                     class "DataStream" defined in this submodule "data"
        # 2: "dsp" = "DataStreamProb([ds(p)], MUX)", where "[ds(p)]" is
                     an list consiting of either "DataStream" or
                     "DataStream_properties" instances, and "MUX" is a matrix
                     which defines the interleaving probabilities of the single
                     datastreams (SEE DOCSTRING OF SUBMETHOD ("MUX"))
       # 3: "dsp" = DataStreamProb.from_probs(Ts, Tc, pr) where Ts, Tc, and
                    pr are the self-switching(-vector), the coupling-switching-
                    (matrix) and the 1-bit probabilities-(vector) of the data
    '''
    def __init__(self, data_inst, mux=None):
        if isinstance(data_inst, (list, tuple)) is True:
            mux = np.array(mux)
            if mux.shape == (2*len(data_inst), 2*len(data_inst)) is False:
                raise IndexError("Mux has to be a quadratic matrix with a row "
                                 + "column count of 2*len(data_inst)")
            if np.all([data_inst[x].B == data_inst[0].B
                       for x in range(1, len(data_inst))]) is False:
                raise ValueError("Data streams have different bit widths")
            self.B = data_inst[0].B
            toggle_mux = np.zeros((
                len(data_inst), len(data_inst), self.B))
            corr_mux = np.zeros((
                len(data_inst), len(data_inst), self.B, self.B))
            for d1 in range(len(data_inst)):
                toggle_mux[d1, d1, :] = data_inst[d1].toggle_prob_vec
                corr_mux[d1, d1, :, :] = np.copy(
                    data_inst[d1].corr_switching_mat)
                for d2 in range(d1+1, len(data_inst)):
                    p1 = data_inst[d1].bit_prob_mat
                    p2 = data_inst[d2].bit_prob_mat
                    toggle_mux[d1, d2, :] = toggle_mux[d2, d1, :] = \
                        np.diag(p1)*(1-np.diag(p2))+np.diag(p2)*(1-np.diag(p1))
                    for i, j in itertools.permutations(range(self.B), 2):
                        corr_mux[d1, d2, i, j] = corr_mux[d2, d1, i, j] = \
                           p1[i, j]+p2[i, j] - \
                           p1[i, i]*p2[j, j] - p2[i, i]*p1[j, j]
            self.corr_switching_mat = self.bit_prob_mat = \
                np.zeros((self.B, self.B))
            self.toggle_prob_vec = np.zeros(self.B)
            for d1, d2 in itertools.product(range(len(data_inst)), repeat=2):
                var = (mux[d1, d2]+mux[d1+len(data_inst), d2])  # d1 or h1->d2
                self.corr_switching_mat = self.corr_switching_mat + \
                    var*np.array(corr_mux[d1, d2, :, :])  # += DOES NOT WORK!!!
                self.toggle_prob_vec += var*toggle_mux[d1, d2, :]
                # prob (d1 to d2)+ (d1_hold to d2) and (d1 to d2_hold)
                self.bit_prob_mat = self.bit_prob_mat + \
                    (var+mux[d1, d2+len(data_inst)])*data_inst[d2].bit_prob_mat
        else:
            if mux is not None:  # mux defines how often value is hold
                mux = np.array(mux)
                if mux.shape != (2, 2):
                    raise ValueError('"mux" has wrong shape')
                factor = mux[0, 0] + mux[1, 0]  # pr(DS--> DS or H --> DS)
            else:
                factor = 1
            self.toggle_prob_vec = factor*(data_inst.toggle_prob_vec)
            self.corr_switching_mat = factor*(data_inst.corr_switching_mat)
            self.bit_prob_mat = np.copy(data_inst.bit_prob_mat)
            self.B = data_inst.B
        self.bit_prob_vec = np.copy(np.diag(self.bit_prob_mat))

    @property
    def T(self):
        '''
        returns the T matrix where the diagonal is equal to the toggle
        activities, while entry i,j is equal to the toggle activity of line i
        minus the correlated switching probability of the lines i and j
        '''
        Ts = self.toggle_prob_vec
        Tc = self.corr_switching_mat
        I_nxn = np.ones(Tc.shape)
        return (Ts*I_nxn).transpose() - Tc

    @classmethod
    def from_probs(cls, self_switching, corr_switching, bit_prob, B=8):
        '''
        see descriptions of CLASS for classmethod descriptions
        '''
        class DataProbContainer():
            def __init__(self, Ts, Tc, pr, B):
                self.toggle_prob_vec = Ts
                self.corr_switching_mat = Tc
                self.bit_prob_mat = pr
                self.B = B
        if isinstance(self_switching, (int, float)) is True:
            if (np.size(corr_switching)
               == np.size(self_switching) == 1):
                self_switching = self_switching*np.ones(B)
                corr_switching = corr_switching*np.ones((B, B))
                bit_prob = bit_prob*np.ones(B)
            else:
                raise IndexError("All inputs have to be either int or arrays")
        else:
            if ((np.array(bit_prob).shape == np.array(corr_switching).shape
                 == (B, B)) and (np.size(self_switching) == B)) is False:
                raise IndexError("All inputs have to be either int or arrays")
        return cls(DataProbContainer(
            self_switching, corr_switching, bit_prob, B))
