#!/bin/bash

for i in {1..3}
	do
		for j in {1..17}
			do
				for k in {1..5}
					do
						if [ $j -lt 10 ]; then
							./bin/python raw_to_AVI.py ../../Data_week${i}/0${j}_0${i}_0${k}.hdf5
						else
							./bin/python raw_to_AVI.py ../../Data_week${i}/${j}_0${i}_0${k}.hdf5
						fi
					done
			done
	done
