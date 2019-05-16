# Requirements

The python script requires python3. Please check if it's installed on your machine by running:
```
> python --version
```

If it is not installed, please go to the offcial <a href="https://www.python.org/downloads/" > website</a> and follow the instructions.

# What is in this folder?

## Makefile:
Contains the instructions to install a python virtual environment with the necessary libraries to run the simulation.

## config folder:
- config.xml: this is a general template which will be used to generate a special config file for each simulation.
- network.xml: this is the file that contains the topolgy of the network you want to use.

## requirments.txt:
Contains the names of the required python libraries. This file will be used as a source to pip command when executing the Makefile.

## run_urand.py:
The python script that you execute to run the simulation.

## source_me.sh:
When sourced, it will activate the python virtual library.


# Instructions to run the simulation:

## 1- Run the following commands to install and activate python virtual environment:
```
> make
> source source_me.sh
```
## 2- Run the python script:
```
> python run_urand.py
```
## 3- Plot the graphs:
```
> python generate_plots.py
```
# Verfication

After the simulation is done you should see rawResults.pkl file. This file contains the latencies (the result of the simulation) and the general configuration that produced these results (basically, the config.ini file).

Also, to make sure the simulator runs the urand simulation correctly, you should get a latencies graph similar to the following one.

<img src="https://github.com/jmjos/ratatoskr/raw/master/docs/misc/latencies.png">
