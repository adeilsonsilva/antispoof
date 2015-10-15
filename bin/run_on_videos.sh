#!/bin/bash

for i in {1..7}
do
	for j in {1..5}
	do
	    # C = color ; D = depth
	    im_type="C"
        folder_name="Mask"
        session="03"

	    folder="${folder_name}/0${i}_${session}_0${j}_${im_type}.avi"

		./faceAntispoof ${folder}
        #echo ${folder}
        wait
	done
done
