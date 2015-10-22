#!/bin/bash

i=1
maxAll=7
maxUser=5
folder=Real_w2
session=02
type=C

while [ $i -le $maxAll ]
	do
	   j=1
	   while [ $j -le $maxUser ]
		   do
				command="./faceAntispoof ${folder}/0${i}_${session}_0${j}_${type}.avi"
				echo $command;
				$command;
				true $((j++))
		   done
	   true $(( i++ ))
	   echo "==========================================================================="
	done
