#!/bin/bash

topdir="/home/antho121/lqcd/GMF"
stripper="/home/antho121/software/install/chroma_utils/main/strippers/strip_hadspec"
DATADIR="${topdir}/data/nucl_2pt2"
curdir=`pwd`
FILELIST=""
#Configuration form
#l2464f211b600m0102m0509m635a.hyp.1005


#Loop through file list and generate the list of xml files
#to pass to the stripper function

while read line
do
    declare $(echo $line | awk -F \. '{print "conf="$2}')
    FILELIST+=" ${DATADIR}/2pt/hadspec2.test.${conf}.dat.xml"
done < <(cat file_list.txt)

cd $DATADIR/corr
${stripper} $FILELIST
cd $curdir


