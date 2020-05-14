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
echo "add path to simulation folder: "
read SIMDIR
SIM="/home/tzschoppe/$SIMDIR"
VAR=true
while [[ $VAR ]]
do
	echo -e "is the path '$SIM' correct? (y/n)"
	read -n1 answer
	if [[ $answer == n || $answer == N ]]
	then
		echo -e "\nreenter path to simulation folder: "
		read SIMDIR
		SIM="/home/tzschoppe/$SIMDIR"
	else
		VAR=false
		break
	fi
done

#SIM="/home/tzschoppe/base2"

SIM1="${SIM##*/}"
echo -e "\n\nrun simulation $SIM1"
echo "add correct configuration in config.ini? (y/n)"
read -n1 answer

if [[ $answer == n || $answer == N ]]
then
	vim $SIM/config.ini
fi

echo -e "\nstart simulation? (y/n)"
read -n1 answer
if [[ $answer == n || $answer == N ]]
then
	echo ""
	exit 0	
fi

#echo -e "\n\nsource source_me.sh"
cd $SIM/
#source source_me.sh &

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
