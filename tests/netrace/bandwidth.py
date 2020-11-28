import argparse
import csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np


def movingAverage(dataReader):
    timeDataDict = {}
    totalTimeBandwidthDict = {}
    movingWindowLength = 100 #SC_NS

    for row in dataReader:
        time = float(row[0])/1000.0 #SC_NS
        value = float(row[1])
        if(time in timeDataDict):
            timeDataDict[time] += value
        else:
            timeDataDict[time] = value
        #clean Dict
        minTime = time-movingWindowLength
        timeDataDict = {k:v for (k,v) in timeDataDict.items() if k > minTime} #k = time, v = dataAmount(in time slot) #remove old/(not in Window) time-value pairs

        #save averaged in finale Dict
        deltaT = max(timeDataDict.keys())-min(timeDataDict.keys())
        if((time > 100) & (deltaT > 0)):
            dataAmount = sum(timeDataDict.values())
            bandwidth = dataAmount/deltaT
            totalTimeBandwidthDict[time] = bandwidth
    return totalTimeBandwidthDict


def generatePDF(title):
    parser = argparse.ArgumentParser()
    parser.add_argument('--version', action='version', version='%(prog)s 1.0')
    parser.add_argument("-v", "--verbose", help="increase output verbosity",
                    action="store_true")
    parser.add_argument("-d", "--debug", help="enable debug mode",
                    action="store_true")
    parser.add_argument("-pf", "--pdf", help="print to pdf",
                    action="store_true")
    parser.add_argument("-dp", "--display", help="display plots",
                    action="store_true")
    parser.add_argument("-f", "--file",
                    help="path to input file")
    parser.add_argument("-c", "--comparefile",
                    help="path to compare file")
    args = parser.parse_args()

    csvFilePath = args.file
    csvFilePath = 'report_Bandwidth_Input.csv'

    csvCompareFilePath = args.comparefile
    csvCompareFilePath = 'report_Bandwidth_Output.csv'

    csvfile = open(csvFilePath,  newline='')
    bandwidthReader1 = csv.reader(csvfile, delimiter=',')
    next(bandwidthReader1) #skip header line in File

    csvComparefile = open(csvCompareFilePath,  newline='')
    bandwidthReader2 = csv.reader(csvComparefile, delimiter=',')
    next(bandwidthReader2) #skip header line in File


    timeBandwidthDict1 = movingAverage(bandwidthReader1)
    timeBandwidthDict2 = movingAverage(bandwidthReader2)

    fig, ax = plt.subplots()
    ax.plot(timeBandwidthDict1.keys(), timeBandwidthDict1.values())
    ax.plot(timeBandwidthDict2.keys(), timeBandwidthDict2.values())
    ax.legend(["input bandwidth", "output bandwidth"])

    ax.set(xlabel='time (ns)', ylabel='bandwidth (Gbit/s)',
        title='Bandwidth of '+title+ 'simulation')
    ax.grid()


    #if (argparse.pdf):
    fig.savefig("bandwidth_"+title+".pdf")
    #if (argparse.display):
    #plt.show()


    #for row1, row2 in zip(bandwidthReader, bandwidthReader2):
    #    print(row1)
