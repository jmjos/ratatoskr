#!/bin/bash

folder=$1
x=$2
y=$3
z=$4
faultnum=$5
concount=$6
count=$7

mkdir $folder

i=0
while [ $i -lt $count ]
do
	echo $i
	./genFaultyMesh.bash ${folder}/${x}x${y}x${z}_4VCS_${faultnum}Fault_${i}.xml $x $y $z $faultnum $concount
	i=$(($i + 1))
done
