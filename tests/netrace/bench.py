from subprocess import call
import shutil
import os
import bandwidth
import xml.etree.ElementTree as ET
import time
from joblib import Parallel, delayed


files = [
    "report_Bandwidth_Input.csv",
    "report_Bandwidth_Output.csv",
    "report_Links.csv",
    "report_Performance.csv",
    "report_Routers_Power.csv",
    "report.txt",
    ]
    
def cleanDir():
    for file in files:
        if os.path.exists(file):
            os.remove(file)
###############################################################################

def write_sim_files(trace, simdir):
    confdir = simdir + '/config'
    
    shutil.rmtree(simdir, ignore_errors=True)
    try:
        os.makedirs(simdir)
        os.makedirs(confdir)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

    shutil.copy('sim', simdir)
    shutil.copy( trace + '.tra.bz2', simdir)
    shutil.copy('config/config.xml'	 , confdir)
    shutil.copy('config/ntConfig.xml', confdir)
    shutil.copy('config/network.xml' , confdir)
###############################################################################


def run_individual_sim(trace, time, queuePos, currentQueuePos):
    #setup Folder
    simdir = trace
    write_sim_files(trace, simdir)
    os.chdir(simdir)

    #start simulation
    command = "./sim --simTime " +  str(time) + " --netraceTraceFile " + str(trace + ".tra.bz2") + " --netraceVerbosity none"
    print('start Simulation with ntraces-trace: ' + trace + '\n\t' + command)
    call(command, shell=True)
    bandwidth.generatePDF(trace)
    
    os.chdir('..')	
###############################################################################

def run_all_sims(traceNames, simTimes):
    call(["sh","./bench_build.sh"])
    
    #download traces
    for trace in traceNames:
        if( not os.path.exists(trace + ".tra.bz2") ) :
            httpAddr = "https://www.cs.utexas.edu/~netrace/download/" + trace + ".tra.bz2"
            call(["wget", httpAddr])    


    print('Starting Sims with -1 processes')
    currentQueuePos = 0
    Parallel(n_jobs=-1)(delayed(run_individual_sim)
        (trace, time, queuePos, currentQueuePos) for trace, time, queuePos in zip(traceNames, simTimes, range(len(simTimes))) )
###############################################################################

def main():


    traceNames = [
        "blackscholes_64c_simsmall"
        ,"fluidanimate_64c_simsmall"
        ,"x264_64c_simsmall"
        ,"bodytrack_64c_simlarge"
        #,"canneal_64c_simmedium"
        #,"dedup_64c_simmedium"
        #,"ferret_64c_simmedium"
        #,"swaptions_64c_simlarge"
        #,"vips_64c_simmedium"
        ]

    simTimes = [100000
        ,100000
        ,100000
        ,100000
        ]

    ###################################################


    os.environ['SYSTEMC_DISABLE_COPYRIGHT_MESSAGE'] = "1"
    run_all_sims(traceNames, simTimes)
    #build program


    #for trace, time in zip(traceNames, simTimes):
        #cleanDir()

        #if( not os.path.exists(trace + ".tra.bz2") ) :
            #httpAddr = "https://www.cs.utexas.edu/~netrace/download/" + trace + ".tra.bz2"
            #call(["wget", httpAddr])

        #command = "./sim --simTime " +  str(time) + " --netraceTraceFile " + str(trace + ".tra.bz2") + " --netraceVerbosity none"
        #print(command)
        #call(command, shell=True)

        #bandwidth.generatePDF(trace)

        #os.makedirs(os.path.dirname(trace +"/"), exist_ok=True)
        #if not("bandwidth_"+trace+".pdf" in files): files.append("bandwidth_"+trace+".pdf")
        #for file in files:
        #	if os.path.exists(file):
        #		shutil.move(file, trace + "/" + file)


if __name__ == "__main__":
    main()
