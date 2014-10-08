#!/bin/bash


archivefile=objects/archive.a
main_method_files=""


#rm -rf objects
mkdir objects
rm -f $archivefile

for file in $(find $(pwd) -name \*.cc)
do
        echo $file
        outfile=objects/$(basename $file).o
        
        c="g++ -flto -pthread -g3 -c -std=c++11 -I/usr/include/opencv $file -o $outfile";
        
        if [[ -e $outfile ]]
        then
                outtime=$(stat -c "%Y" $outfile)
                srctime=$(stat -c "%Y" $file)

                if [[ $srctime -le $outtime ]]
                then
                        echo "Already done!"
                else
                        $c
                        if [ $? -ne 0 ]
                        then
                                exit;
                        fi
                fi
        else
                $c
                if [ $? -ne 0 ]
                then
                        exit;
                fi
        fi

        
        
        main_method=$(readelf -Ws $outfile | grep main)
        if [[ -z $main_method ]]
        then
                ar rcs $archivefile $outfile
        else
                echo "Has a main method."
                main_method_files="$outfile $main_method_files"
        fi
done


g++ objects/get_exe_name.cc.o -o get_exe_name

echo $main_method_files

for file in $main_method_files
do
        exefile=./bin/$(./get_exe_name $file)
        echo $exefile
        g++ -flto $file -pthread $archivefile -o $exefile -lopencv_core -lopencv_highgui
done

rm get_exe_name
