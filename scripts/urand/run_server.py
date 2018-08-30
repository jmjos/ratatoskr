# Fast and convienent script for urand traffic benchmarks

""" Import the necessary libraries """
import sys
import os
import shutil
import subprocess
import xml.etree.ElementTree as ET
import csv
import numpy as np
import matplotlib.pyplot as plt
from joblib import Parallel, delayed
import multiprocessing
import pickle
import configparser

##################################################################################################

""" Read the confgiuration file config.ini """
config = configparser.ConfigParser()
config.read("config.ini")

topologyFile = config['DEFAULT']['topologyFile']
print(topologyFile)

libdir = config['DEFAULT']['libdir']
simdir = config['DEFAULT']['simdir']
basedir = os.getcwd() 

simulation_time = int(config['DEFAULT']['simulation_time'])
restarts = int(config['DEFAULT']['restarts'])

baseclock = int(config['DEFAULT']['baseclock'])
flitsPerPacket = int(config['DEFAULT']['flitsPerPacket'])

warmup_start = int(config['DEFAULT']['warmup_start'])
warmup_duration = int(config['DEFAULT']['warmup_duration'])
warmup_rate = float(config['DEFAULT']['warmup_rate'])

run_rate_min = float(config['DEFAULT']['run_rate_min'])
run_rate_max = float(config['DEFAULT']['run_rate_max'])
run_rate_step = float(config['DEFAULT']['run_rate_step'])
run_start_after_warmup = int(config['DEFAULT']['run_start_after_warmup'])
run_start = warmup_start + warmup_duration + run_start_after_warmup
run_duration = int(config['DEFAULT']['run_duration'])

num_cores = int(config['DEFAULT']['num_cores'])
if (num_cores == -1):
    num_cores = multiprocessing.cpu_count()

##################################################################################################

def write_config_file(configFileSrc, configFileDst, injectionRate):
    """ Write the configuration file for the urand simulation """
    try:
        configTree = ET.parse(configFileSrc)
    except Exception as e:
        raise
        
    configTree.find("noc/nocFile").text = "config/" + topologyFile + ".xml"
        
    configTree.find("general/simulationTime").set("value", str(simulation_time))
    configTree.find("general/outputToFile").set("value", "true")
    configTree.find("general/outputToFile").text = "report"
    
    for elem in list(configTree.find("application/synthetic").iter()):
        if elem.get("name") == "warmup":        
            elem.find("start").set("min",str(warmup_start))
            elem.find("start").set("max",str(warmup_start))
            elem.find("duration").set("min",str(warmup_start + warmup_duration))
            elem.find("duration").set("max",str(warmup_start + warmup_duration))
            sendPacketInterval = round(baseclock/warmup_rate, 2)
            elem.find("interval").set("min", str(sendPacketInterval)) #send every sendPacketInterval NS a packet with 10 flits
            elem.find("interval").set("max", str(sendPacketInterval)) 
        if elem.get("name") == "run":        
            elem.find("start").set("min",str(run_start))
            elem.find("start").set("max",str(run_start))
            elem.find("duration").set("min",str(run_start + run_duration))
            elem.find("duration").set("max",str(run_start + run_duration))            
            sendPacketInterval = round(baseclock/injectionRate, 2)
            elem.find("interval").set("min", str(sendPacketInterval)) 
            elem.find("interval").set("max", str(sendPacketInterval))
            repeatLength = round(run_duration/sendPacketInterval, 2)
            elem.find("repeat").set("min", str(repeatLength)) 
            elem.find("repeat").set("max", str(repeatLength))       
    configTree.write(configFileDst)

##################################################################################################

def write_sim_files(simdir):
    """ Write the files that are associated with each run of the simulation (the executable sim + the configuration file) """
    confdir = simdir+"/config"
    shutil.rmtree(simdir, ignore_errors=True)
    try:
        os.makedirs(simdir)
        os.makedirs(confdir)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise
    
    #simulator
    shutil.copy("sim", simdir)
    #topology file
    shutil.copy(libdir+"/networks/"+topologyFile+".xml", confdir)

##################################################################################################
   
def run_simulation(simdir, basedir):
    """ Run a simulation """
    os.chdir(simdir)
    args = ("./sim")
    outfile = open("log","w")
    popen = subprocess.Popen(args, stdout=outfile)#subprocess.PIPE)
    #print(popen.communicate())
    popen.wait()
    #output = popen.stdout.read()
    #print(output.decode('ascii'))
    os.chdir(basedir)

##################################################################################################
  
def get_results(results_file):
    """ Read the resulting latencies from the csv file """
    latencies = list()
    try:
        with open(results_file, newline='') as csvfile:
            spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
            for row in spamreader:
                latencies.append(row[1])
    except:
    	#add dummy values to latencies, -1
    	latencies.append(-1)
    	latencies.append(-1)
    	latencies.append(-1)
    return(latencies)

##################################################################################################

def processInput(restart, inj, injIter):
    """ Run a simulation with a specif injection rate """
    print("Simulation with injection rate: "+ str(inj[injIter])\
            + " restart "+ str(restart))
    currentSimDir = simdir+str(restart)
    currentConfDir = currentSimDir+"/config"    
    write_sim_files(currentSimDir) 
    write_config_file("library/config.xml", currentConfDir+"/config.xml", inj[injIter])
    run_simulation(currentSimDir, basedir)
    
##################################################################################################
   
    
""" Main point of execution """

print("Generating urand simulation with injection rate from " + \
    str(run_rate_min)+ " to " + str(run_rate_max) + " steps " + \
    str(run_rate_step))

# Initialze the variables
injectionRates = np.arange(run_rate_min, run_rate_max, run_rate_step)
injectionRates = [round(elem, 4) for elem in injectionRates]
latenciesFlit = -np.ones((len(injectionRates), restarts))
latenciesPacket = -np.ones((len(injectionRates), restarts))
latenciesNetwork = -np.ones((len(injectionRates), restarts))

# Run the full simulation (for all injection rates)
injIter = 0
for inj in injectionRates:
    print("Starting Sims with "+ str(num_cores)+" processes")
    Parallel(n_jobs=num_cores)(delayed(processInput)\
            (restart, injectionRates,injIter) for restart in range(restarts))  
    print("Executed all sims. Reading Results")
    for restart in range(restarts): 
        currentSimdir = 'sim'+str(restart)
        lat = get_results(currentSimdir+'/reportPerformance.csv')
        latenciesFlit[injIter, restart] = lat[0]
        latenciesPacket[injIter, restart] = lat[1]
        latenciesNetwork[injIter, restart] = lat[2]
        #input("press any key")
        shutil.rmtree(currentSimdir)
    injIter += 1

# After finishing the entire simulation, calculate the mean and standard deviation for each latency
meanLatenciesFlit = np.mean(latenciesFlit, axis=1)
meanLatenciesPacket = np.mean(latenciesPacket, axis=1)
meanLatenciesNetwork = np.mean(latenciesNetwork, axis=1)  
stdLatenciesFlit = np.std(latenciesFlit, axis=1)
stdLatenciesPacket = np.std(latenciesPacket, axis=1)
stdLatenciesNetwork = np.std(latenciesNetwork, axis=1)

# Plot the graph of latencies
fig = plt.figure()
plt.ylabel('latencies in ns', fontsize = 11)
plt.xlabel('injection rate', fontsize = 11)
plt.xlim([0,1])
linestyle = {"linestyle":"--", "linewidth":1, "markeredgewidth":1, "elinewidth":1, "capsize":10}
plt.errorbar(injectionRates, meanLatenciesFlit, yerr = stdLatenciesFlit, color="r", **linestyle, marker = '*')
plt.errorbar(injectionRates, meanLatenciesNetwork, yerr = stdLatenciesNetwork, color="b", **linestyle, marker = 's')
plt.errorbar(injectionRates, meanLatenciesPacket, yerr = stdLatenciesPacket, color="g", **linestyle, marker = '^')
plt.legend(["Flit","Network", "Packet"])
plt.show()

# Save the configuration parameters and the results to a pickle file 
file = "rawResults"
with open(file+".pkl", 'wb') as f: 
    pickle.dump([latenciesFlit, latenciesNetwork, latenciesPacket, injectionRates, simulation_time, restarts, warmup_start, warmup_duration, warmup_rate, run_start_after_warmup, run_duration, topologyFile], f)
f.close()
