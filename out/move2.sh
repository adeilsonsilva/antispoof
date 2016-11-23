#!/bin/bash

folder2=Histograms
folder1=Real_w2
folder0=Real_w1
i=1
j=1
while [ $i -le 35 ]; 
    do 
        cp -T "${folder0}/${folder2}/histograms${i}.xml" "./histograms${j}.xml"
        true $(( i++ ))
        true $(( j++ ))
    done
i=1
while [ $i -le 35 ]; 
    do 
        cp -T "${folder1}/${folder2}/histograms${i}.xml" "./histograms${j}.xml"
        true $(( i++ ))
        true $(( j++ ))
    done

