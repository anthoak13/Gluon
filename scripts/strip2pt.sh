#!/bin/bash

topdir="/home/antho121/lqcd/GMF"
filedir="/home/antho121/lqcd/temp/file_list1.txt"
DATADIR="/mnt/home/antho121/lqcd/2pt/xxx5/LP/2pt"
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
	FILELIST+=" ${DATADIR}/$file"
    done < <(ls -1 /mnt/home/antho121/lqcd/2pt/xxx5/LP/2pt | grep .$conf. | grep -v BWD)
	
done < <( cat $filedir)

cd $topdir/../temp/2pt/light
echo "Loaded fileList"
#${topdir}/programs/strip_nucl_2pt_proj $FILELIST
echo $FILELIST
cd $curdir


