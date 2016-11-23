#!/bin/bash

folder2=Histograms
folder1=Real_w2
i=1
for d in ./*/ ; 
    do 
        if [ -d "$d" ]; then
            cd "$d"
            cp -T histograms.xml "../${folder1}/${folder2}/histograms${i}.xml"
            #cp -T results.txt "../${folder1}/${folder2}/results${i}.txt"
            cd ..
            true $(( i++ ))
        fi
    done


