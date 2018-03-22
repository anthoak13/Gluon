#!/bin/bash

topdir="/home/antho121/lqcd"
tempdir="$topdir/temp/temp"
curdir=`pwd`

increment=100
min=1
max=100
smearMin=1
smearMax=2

cd $tempdir
oldi=$min
for i in `seq $increment $increment $((max-1))`;
do
    sed -n $oldi,${i}p ../file.txt > file_listSmear$oldi.txt
    $topdir/GMF/scripts/genSmearScript.pl $oldi $smearMin $smearMax > runSmear.sh
    qsub runSmear.sh
    
    oldi=$(( $i+1 ))
    
done

sed -n $oldi,${max}p ../file.txt > file_listSmear$oldi.txt
$topdir/GMF/scripts/genSmearScript.pl $oldi $smearMin $smearMax > runSmear.sh
qsub runSmear.sh

rm runSmear.sh

cd $curdir

