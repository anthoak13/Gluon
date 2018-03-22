#!/bin/bash

topdir="/home/antho121/lqcd"
tempdir="$topdir/temp"
curdir=`pwd`

increment=460
min=1
max=460

cd $tempdir
oldi=1
for i in `seq $min $increment $max`;
do
    i=$(( $i-1 ))
    
    sed -n $oldi,${i}p file.txt > file_list$oldi.txt
    for j in `seq 1 1`;
    do
    $topdir/GMF/scripts/genObScript.pl $oldi 0 0 0 > runOb.sh
    qsub runOb.sh
    done
    
    oldi=$(( $i+1 ))
    
done

sed -n $oldi,${max}p file.txt > file_list$oldi.txt
for j in `seq 1 1`;
do
$topdir/GMF/scripts/genObScript.pl $oldi 0 0 0 > runOb.sh
qsub runOb.sh

done
rm runOb.sh


cd $curdir

