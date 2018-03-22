#!/bin/bash

topdir="/home/antho121/lqcd/GMF"
filedir="/home/antho121/lqcd/temp/file_list1.txt"
DATADIR="${topdir}/data/ObSmeared/Ob"
curdir=`pwd`
FILELIST=""
#Configuration form
#l2464f211b600m0102m0509m635a.hyp.1005


#Loop through file list and generate the list of xml files
#to pass to the stripper function

while read line
do
    declare $(echo $line | awk -F \. '{print "conf="$2}')
    while read file
    do
	FILELIST+=" ${DATADIR}/${conf}r10000s9.xml"
    done < <(ls -1 /mnt/home/antho121/lqcd/2pt/xxx5/LP/2pt | grep .$conf. | grep -v BWD)
	
done < <($filedir )

cd $DATADIR
#${topdir}/programs/strip $FILELIST
echo $FILELIST
cd $curdir


