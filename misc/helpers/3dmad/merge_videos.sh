#!/bin/bash

for j in {1..17}
do
    # C = color ; D = depth
    im_type="C"

    # Case $j < 10
    week1_1="../../Data_week1/0${j}_01_01_${im_type}.avi ../../Data_week1/0${j}_01_02_${im_type}.avi ../../Data_week1/0${j}_01_03_${im_type}.avi ../../Data_week1/0${j}_01_04_${im_type}.avi ../../Data_week1/0${j}_01_05_${im_type}.avi"
    week2_1="../../Data_week2/0${j}_02_01_${im_type}.avi ../../Data_week2/0${j}_02_02_${im_type}.avi ../../Data_week2/0${j}_02_03_${im_type}.avi ../../Data_week2/0${j}_02_04_${im_type}.avi ../../Data_week2/0${j}_02_05_${im_type}.avi"
    week3_1="../../Data_week3/0${j}_03_01_${im_type}.avi ../../Data_week3/0${j}_03_02_${im_type}.avi ../../Data_week3/0${j}_03_03_${im_type}.avi ../../Data_week3/0${j}_03_04_${im_type}.avi ../../Data_week3/0${j}_03_05_${im_type}.avi"
    out1="../../merged_videos/vid0${j}_${im_type}.avi"

    # Case $j > 10
    week1_2="../../Data_week1/${j}_01_01_${im_type}.avi ../../Data_week1/${j}_01_02_${im_type}.avi ../../Data_week1/${j}_01_03_${im_type}.avi ../../Data_week1/${j}_01_04_${im_type}.avi ../../Data_week1/${j}_01_05_${im_type}.avi"
    week2_2="../../Data_week2/${j}_02_01_${im_type}.avi ../../Data_week2/${j}_02_02_${im_type}.avi ../../Data_week2/${j}_02_03_${im_type}.avi ../../Data_week2/${j}_02_04_${im_type}.avi ../../Data_week2/${j}_02_05_${im_type}.avi"
    week3_2="../../Data_week3/${j}_03_01_${im_type}.avi ../../Data_week3/${j}_03_02_${im_type}.avi ../../Data_week3/${j}_03_03_${im_type}.avi ../../Data_week3/${j}_03_04_${im_type}.avi ../../Data_week3/${j}_03_05_${im_type}.avi"
    out2="../../merged_videos/vid${j}_${im_type}.avi"

	if [ $j -lt 10 ]; then
		avimerge -i $week1_1  $week2_1 $week3_1 -o $out1
	else
        avimerge -i $week1_2  $week2_2 $week3_2 -o $out2
	fi
done
