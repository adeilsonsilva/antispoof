#!/bin/bash

i=1
maxAll=7
maxUser=5
folder=Real_w1
session=01
type=C

while [ $i -le $maxAll ]
	do
	   j=1
	   while [ $j -le $maxUser ]
		   do
				command="./faceAntispoof 3DMAD_Train_Set/${folder}/0${i}_${session}_0${j}_${type}.avi"
				echo $command;
				$command;
				true $((j++))
		   done
	   true $(( i++ ))
	   echo "==========================================================================="
	done
