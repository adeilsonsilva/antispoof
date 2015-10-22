#!/bin/bash

folder1=Histograms
folder2=Real_w2
i=1
j=36
while [ $i -le 35 ]; 
    do 
        cp -T "${folder1}/${folder2}/histograms${i}.xml" "./histograms${j}.xml"
        true $(( i++ ))
        true $(( j++ ))
    done


