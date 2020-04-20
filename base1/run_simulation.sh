#!/bin/bash
# skript to run ratatoskr simultion

# update sim:
	# get ratatoskr from github
		# git clone https://github.com/jmjos/ratatoskr.git
	# run CMakeList (delete '-D enable gui' in CMakeList)
		# cd ~/ratatoskr/simulation
		# ./build.sh
	# copy sim
		# cp ~/ratatoskr/simulation/sim ~/'simualtion_folder'
	
# add path to simulation folder
SIM="/home/tzschoppe/base2"

SIM1="${SIM##*/}"
echo "run simulation $SIM1"
echo "add correct configuration in config.ini? (y/n)"
read -n1 answer

if [[ $answer == n || $answer == N ]]
then
	vim $SIM/config.ini
fi

echo -e "\n\nsource source_me.sh"
cd $SIM/
source source_me.sh &

echo -e "\nrun configure.py"
python configure.py
PID=$!
wait $PID

echo -e "\nrun run_urand.py with nohub"
nohup python -u run_urand.py > nohup_output.log 2>&1 &
PID=$!
wait $PID

echo -e "\nrun generate_plots.py"
python generate_plots.py
PID=$!
wait $PID

echo -e "\nsimulation done"

exit 0
