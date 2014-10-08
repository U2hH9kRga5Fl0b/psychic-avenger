#!/bin/bash

#rm -rf objects
mkdir objects

main_method_files=""

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
        
        main_method=$(readelf -Ws $outfile | grep main)
        if [[ -z $main_method ]]
        then
                ar rcs objects/archive.a $outfile
        else
                echo "Has a main method."
                main_method_files="$outfile $main_method_files"
        fi
done

echo $main_method_files

for file in $main_method_files
do
        exefile=$(basename $file).out
        echo $exefile
        g++  $file -pthread objects/archive.a -o $exefile -lopencv_core -lopencv_highgui
done

