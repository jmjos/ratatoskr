from subprocess import call
import shutil
import os
import bandwidth
import xml.etree.ElementTree as ET

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

def main():
	os.environ['SYSTEMC_DISABLE_COPYRIGHT_MESSAGE'] = "1"

	traceNames = [
		"blackscholes_64c_simsmall"
		#,"fluidanimate_64c_simsmall"
		#,"x264_64c_simsmall"
		#,"bodytrack_64c_simlarge"
		#,"canneal_64c_simmedium"
		#,"dedup_64c_simmedium"
		#,"ferret_64c_simmedium"
		#,"swaptions_64c_simlarge"
		#,"vips_64c_simmedium"
		]
	simTimes = [10000
		#,10000
		#,10000
		]

	assert 	len(traceNames) == len(simTimes)

	#build program
	call(["sh","./bench_build.sh"])

	for trace, time in zip(traceNames, simTimes):
		cleanDir()

		if( not os.path.exists(trace + ".tra.bz2") ) :
			httpAddr = "https://www.cs.utexas.edu/~netrace/download/" + trace + ".tra.bz2"
			call(["wget", httpAddr])

		command = "./sim --simTime " +  str(time) + " --netraceTraceFile " + str(trace + ".tra.bz2") + " --netraceVerbosity none"
		print(command)
		call(command, shell=True)

		bandwidth.generatePDF(trace)

		#os.makedirs(os.path.dirname(trace +"/"), exist_ok=True)
		#if not("bandwidth_"+trace+".pdf" in files): files.append("bandwidth_"+trace+".pdf")
		#for file in files:
		#	if os.path.exists(file):
		#		shutil.move(file, trace + "/" + file)


if __name__ == "__main__":
    main()
