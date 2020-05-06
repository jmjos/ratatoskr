
import csv
import os
import pandas as pd
import configparser

def setupDirectory(arch, bd, vc, bench):
    print("Generating dirs")
    print(os.path)
    dirName = arch+'_vc_'+str(vc)+'_bd_'+str(bd)+'_'+bench
    os.system('mkdir ' + dirName)
    os.system('cp -r ../origin/sim ' + dirName)
    os.system('mkdir ' + dirName +'/config')
    os.system('cp -r ../origin/config/ntConfig.xml ' + dirName+'/config')
    os.system('cp -r ../origin/config/'+arch+'.xml ' + dirName+'/config/network.xml')

def execute(arch, bd, vc, bench):
    dirName = arch + '_vc_' + str(vc) + '_bd_' + str(bd) + '_' + bench
    os.chdir(dirName)
    os.system('./sim')
    os.chdir('..')


def modifyFiles(arch, bd, vc, bench):
    dirName = arch + '_vc_' + str(vc) + '_bd_' + str(bd) + '_' + bench
    os.chdir(dirName+'/config')
    #set benchmark
    filedata = None
    with open('ntConfig.xml', 'r') as file:
        filedata = file.read()
        file.close()
    filedata = filedata.replace('BENCHMARK', '../../benchmarks/'+bench)
    with open('ntConfig.xml', 'w') as file:
        file.write(filedata)
        file.close()

    #set vc
    # set bd
    filedata = None
    with open('network.xml', 'r') as file:
        filedata = file.read()
        file.close()
    if (arch == 'heteroSynch'):
        filedata = filedata.replace('<bufferDepth value=\"4\" />', '<bufferDepth value=\"'+str(bd)+'\"/>')
        filedata = filedata.replace('<vcCount value=\"4\" />', '<vcCount value=\"' + str(vc) + '\"/>')
    else:
        filedata = filedata.replace('<bufferDepth value=\"4\"/>', '<bufferDepth value=\"'+str(bd)+'\"/>')
        filedata = filedata.replace('<vcCount value=\"4\"/>', '<vcCount value=\"' + str(vc) + '\"/>')
    with open('network.xml', 'w') as file:
        file.write(filedata)
        file.close()

    #set clock speed
    os.chdir('../..')

def gather_results(arch, bd, vc, bench):
    dirName = arch + '_vc_' + str(vc) + '_bd_' + str(bd) + '_' + bench
    latencies = get_latencies(dirName + '/report_Performance.csv')
    flit_latency = latencies[0]
    return flit_latency

def get_latencies(latencies_results_file):
    """
    Read the resulting latencies from the csv file.
    Parameters:
        - results_file: the path to the result file.
    Return:
        - A list of the filt, packet and network latencies.
    """
    latencies = []
    try:
        with open(latencies_results_file, newline='') as f:
            spamreader = csv.reader(f, delimiter=' ', quotechar='|')
            for row in spamreader:
                latencies.append(float(row[1]))
    except Exception:
        # Add dummy values to latencies, -1.
        latencies.append(-1)
        latencies.append(-1)
        latencies.append(-1)
    return (latencies)



architectures = ['homoSynch', 'homoAsynch', 'pseudo', 'heteroSynch']
bds = [4, 8]
vcs = [4, 8]
benchmarks = ['blackscholes_64c_simmedium', 'bodytrack_64c_simlarge', 'canneal_64c_simmedium',
              'dedup_64c_simmedium', 'ferret_64c_simmedium', 'fluidanimate_64c_simmedium',
              'fluidanimate_64c_simmedium', 'vips_64c_simmedium', 'x264_64c_simmedium']

resultsIndex = benchmarks
resultsColumns = []
for arch in architectures:
    for bd in bds:
        for vc in vcs:
            resultsColumns.append(arch + '_vc_' + str(vc) + '_bd_' + str(bd))
results = pd.DataFrame(index=resultsIndex, columns=resultsColumns)
results = results.fillna(0.0) # with 0s rather than NaNs
print(results)

for arch in architectures:
    for bd in bds:
        for vc in vcs:
            for bench in benchmarks:
                setupDirectory(arch, bd, vc, bench)
                modifyFiles(arch, bd, vc, bench)
                execute(arch, bd, vc, bench)
                flitLatency = gather_results(arch, bd, vc, bench)
                setting = arch + '_vc_' + str(vc) + '_bd_' + str(bd)
                if arch == 'homoAsynch':
                    flitLatency = flitLatency / 2
                results.at[bench, setting] = flitLatency

print(results)
results.to_csv("parsec.csv")
