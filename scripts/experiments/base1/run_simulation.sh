#!/bin/bash

# update sim:
	# get ratatoskr from github
		# git clone https://github.com/jmjos/ratatoskr.git
	# run CMakeList (delete '-D enable gui' in CMakeList)
		# cd ~/ratatoskr/simulation
		# ./build.sh
	# copy sim
		# cp ~/ratatoskr/simulation/sim ~/'simualtion_folder'

python generate_sims.py

for subdir in simdir/Heter*/
do
	echo -e "start simulation"
	
	cd $subdir

	echo -e "\nrun configure.py"
	python configure.py
	PID=$!
	wait $PID

	echo -e "\nrun run_urand.py with nohub"
	#nohup python -u run_urand.py > nohup_output.log 2>&1 &
	python run_urand.py
	PID=$!
	wait $PID

	echo -e "\nrun generate_plots.py"
	python generate_plots.py
	PID=$!
	wait $PID	

	cd $OLDPWD

	echo -e "\nstop simulation"
done

exit 0
