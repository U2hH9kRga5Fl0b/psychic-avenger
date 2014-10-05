#!/bin/bash

mkdir objects

for file in $(find $(pwd) -name \*.cc)
do
	echo $file
	outfile=objects/$(basename $file).o
	
        if [[ -e $outfile ]]
        then
                outtime=$(stat -c "%Y" $outfile)
                srctime=$(stat -c "%Y" $file)

                if [[ $srctime -le $outtime ]]
                then
                        echo Already done!
                        continue;
                fi
        fi

        g++ -pthread -g3 -c -std=c++11 -I/usr/include/opencv $file -o $outfile
        if [ $? -ne 0 ]
        then
                exit;
        fi
        ar rcs objects/archive.a $outfile
done


g++  -pthread objects/archive.a -o tsp -lopencv_core -lopencv_highgui
#rm *.o
#rm archive.a
