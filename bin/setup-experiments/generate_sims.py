import csv
import os
import configparser

class simDirWriter:
    def __init__(self):
        self.fileName = 'experiments-RQ1-new.csv'
        writer = self.readCsvFile()
        self.generateDirs(writer)

    def readCsvFile(self):
        csvfile = open(self.fileName, newline='')
        reader = csv.DictReader(csvfile)
        return reader

    def generateDirs(self, structure):
        print("Generating dirs")
        for row in structure:
            dirName = row['Test']
            os.system('rm -rf '+dirName)
            os.system('mkdir '+dirName)
            self.writeConfigFile(dirName+'/'+'config.ini', row['Routing'],
                '['+row['Delay-1']+', '+row['Delay-2']+']',
                '['+row['bufferDepth-1']+', '+row['bufferDepth-2']+']',
                '['+row['VC-1']+', '+row['VC-2']+']',
                )
            os.system('cp origin/* '+dirName)
            

    def writeConfigFile(self, path, routing, clockDelays, bufferDepths, vcCount):
        config = configparser.ConfigParser()
        config['Config'] = {}
        config['Config']['simulationTime'] = str(10000)
        config['Config']['flitsPerPacket'] =  str(16)
        config['Config']['bitWidth'] = str(32)
        config['Config']['benchmark'] = 'synthetic'
        config['Config']['libDir'] = 'config'

        config['Synthetic'] = {}
        config['Synthetic']['simDir']= 'sim'
        basedir = os.getcwd()
        config['Synthetic']['restarts'] = str(12)
        config['Synthetic']['warmupStart'] = str(100)
        config['Synthetic']['warmupDuration'] = str(900)
        config['Synthetic']['warmupRate'] = str(0.02)
        config['Synthetic']['runRateMin'] = str(0.002)
        config['Synthetic']['runRateMax'] = str(0.032)
        config['Synthetic']['runRateStep'] = str(0.002)
        config['Synthetic']['runStartAfterWarmup'] = str(10)
        config['Synthetic']['runDuration'] = str(10000)
        config['Synthetic']['numCores'] = str(-1)
        config['Report'] = {}
        config['Report']['bufferReportRouters'] = '[5, 6]'

        config['Hardware'] = {}
        config['Hardware']['x'] = '[8, 8]'
        config['Hardware']['y'] = '[8, 8]'
        config['Hardware']['z'] = str(2)
        config['Hardware']['routing'] = routing
        config['Hardware']['clockDelay'] = clockDelays
        config['Hardware']['bufferDepthType'] = 'single'
        config['Hardware']['bufferDepth'] = bufferDepths
        config['Hardware']['buffersDepths'] = '[10,20,30,40]'
        config['Hardware']['vcCount'] = vcCount
        config['Hardware']['topologyFile'] = 'network'
        config['Hardware']['flitSize'] = str(64)
        config['Hardware']['portNum'] = str(7)

        with open(path, 'w') as configfile:
            config.write(configfile)

    def cleanDirs(self, structure):
        print("Cleaning dirs")
        for row in structure:
            dirName = row['Test']
            os.system('rm -rf ' + dirName)


if __name__ == "__main__":
    writer = simDirWriter()

