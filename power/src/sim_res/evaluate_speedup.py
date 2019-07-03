#!/usr/bin/python3
'''
This script is used to evaluate how fast our high-level
approach is compared to a bit-level approach
'''
from interconnect import data
import random
import timeit
import numpy as np


def determine_speedup(muxProb, NB, Nds, patCount):
    '''
    runs the simulation for a multiplexing probability of
    mux_prob, Nds multiplexed data streams with a pattern count
    of pat_count.
    '''
    # Generate single data streams
    dsVec = []
    dsProbVec = []
    for j in range(Nds):
        dsType = random.randint(1, 3)
        if dsType == 1:
            print('got a random')
            ds = data.DataStream.from_stoch(patCount, NB)
        elif dsType == 2:
            print('got a gaussian')
            std = (random.uniform(0.1, 1))*(NB - 1)  # random std deviation
            ro = random.random()
            ds = data.DataStream.from_stoch(patCount, NB, 0, ro, 0, std)
        else:
            print('got a lognormal')
            std = (random.uniform(0.1, 1))*(NB - 1)  # random std deviation
            ro = random.random()
            ds = data.DataStream.from_stoch(patCount, NB, 0, ro, 0, std,
                                            lognormal=True)
        dsVec.append(ds)
        dsProbVec.append(data.DataStreamProb(ds))

    # Bit level timing
    def bit_level_T():
        realMuxedDs = dsVec[0].interleaving(dsVec[1:], muxProb)
        T_bl = realMuxedDs.T
        return T_bl
    timeBitLevel = timeit.timeit(bit_level_T, number=10)

    muxMatrix, diagMatrix = np.zeros((2*Nds, 2*Nds)), np.zeros(2*Nds)
    muxMatrix[0:Nds, 0:Nds] = muxProb/((Nds-1)*Nds)
    diagMatrix[0:Nds] = (1-muxProb)/Nds
    np.fill_diagonal(muxMatrix, diagMatrix)

    # High level timing
    def high_level_T():
        theoMuxedDs = data.DataStreamProb(dsProbVec, muxMatrix)
        T_hl = theoMuxedDs.Tx
        return T_hl
    timeHighLevel = timeit.timeit(high_level_T, number=10)
    return timeBitLevel/timeHighLevel


if __name__ == "__main__":
    print(determine_speedup(0.5, 16, 4, 1e5))
    print(determine_speedup(0.5, 32, 4, 1e5))
