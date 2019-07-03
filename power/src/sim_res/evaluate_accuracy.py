#!/usr/bin/python3
'''
This script is used to evaluate how accurate the switching estimation
of our approach is
'''
from interconnect import data
import random
import time
import os
import numpy as np
import itertools
from multiprocessing.pool import ThreadPool


def determine_error(muxProb, Nds, patCount, simRep, NB):
    '''
    runs the simulation for a multiplexing probability of
    mux_prob, Nds multiplexed data streams with a pattern count
    of 10e3. The simulation is repeated simRep times and
    returned is the RMSE as well as  the MAE
    '''
    for i in range(int(simRep)):
        dsVec = []
        dsProbVec = []
        mse_vec = []
        mse_vec_ref = []
        max_error = 0
        max_error_ref = 0
        T_ref = np.zeros((NB, NB))
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
            T_ref += ds.T
            dsVec.append(ds)
            dsProbVec.append(data.DataStreamProb(ds))
        T_ref /= Nds
        # REAL MULTIPLEXED DATASTREAM
        realMuxedDs = dsVec[0].interleaving(dsVec[1:], muxProb)
        # THEORETICAL (OUR MODEL)
        muxMatrix, diagMatrix = np.zeros((2*Nds, 2*Nds)), np.zeros(2*Nds)
        muxMatrix[0:Nds, 0:Nds] = muxProb/((Nds-1)*Nds)
        diagMatrix[0:Nds] = (1-muxProb)/Nds
        np.fill_diagonal(muxMatrix, diagMatrix)
        if sum(sum(muxMatrix)) != 1:
            print ("AUTO TEST DETECTED PROBLEM IN MUX MATRIX")
        theoMuxedDs = data.DataStreamProb(dsProbVec, muxMatrix)
        mse_vec.append(((theoMuxedDs.T - realMuxedDs.T) ** 2).mean())
        max_error = max(max_error, np.abs(theoMuxedDs.T - realMuxedDs.T).max())
        mse_vec_ref.append(((T_ref - realMuxedDs.T) ** 2).mean())
        max_error_ref = max(max_error_ref, np.abs(T_ref - realMuxedDs.T).max())
    rmse = np.sqrt(np.mean(mse_vec))
    rmse_ref = np.sqrt(np.mean(mse_vec_ref))
    return {"RMSE": rmse, "MAE": max_error, "RMSE REF": rmse_ref,
            "MAE REF": max_error_ref, "DS": dsVec, "muxed": realMuxedDs}


if __name__ == "__main__":
    os.nice(1)
    muxProbs = [0.1, 0.4, 0.7, 1]  # analyzed multiplexing prob.
    maxDs = 5  # maximum number of multiplexed data streams
    patCount = 10e3  # pattern count per data stream
    simRep = 1e2  # how often eachf simulation is repeated
    NBvec = [16, 32, 64]
    pool = ThreadPool(processes=(maxDs-1)*len(muxProbs))
    for NB in NBvec:
        print("%s --> Started Bit-Width %d" % (time.ctime(), NB))
        results = []
        muxProb_Nds = []
        async_results = []
        i = 0
        for muxProb, Nds in itertools.product(muxProbs, range(2, maxDs+1)):
            print("%s --> Started: Prob. %f, #DS: %d" % (time.ctime(),
                                                         muxProb, Nds))
            muxProb_Nds.append([muxProb, Nds])
            async_results.append(pool.apply_async(determine_error,
                                                  (muxProb, Nds, patCount, simRep,
                                                   NB)))
            i += 1
            for i in range((maxDs-1)*len(muxProbs)):
                print("%s --> Wait for results %d" % (time.ctime(), i))
                results.append(async_results[i].get())

    np.savez('results_accuracy_%dbits' % NB,
             id=NB, results=results, prM_nDS=muxProb_Nds)
