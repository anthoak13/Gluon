#!/usr/bin/perl

$num_args = $#ARGV + 1;

if ($num_args != 3) {
    print << "Error";
There are only $num_args inputed! 
Expected index smearMin smearMax
Error
   exit;
}
$i=$ARGV[0];
$smearMin = $ARGV[1];
$smearMax = $ARGV[2];


print << "EOF";
#!/bin/bash -login
#PBS -l walltime=04:00:00
#PBS -l nodes=1:ppn=16
#PBS -l mem=32gb
#PBS -N smear


TOPDIR="/home/antho121/lqcd/GMF"
CHROMADIR="\${TOPDIR}/programs"
FILELIST="/home/antho121/lqcd/temp/temp/file_listSmear$i.txt"
XMLLOC="/home/antho121/lqcd/temp/temp/inSmear_$i.xml"
datadir="/home/antho121/lqcd/hypcfgs/log"

exe=\${CHROMADIR}/chroma
geom="2 2 2 2" # n16
iogem=\$geom

#Configuration form
#l2464f211b600m0102m0509m635a.hyp.1005


#Loop through file list and run the measurement of O_b for
#each configuration in file_list.txt
cd \$TOPDIR

while read line
do
    echo "Processing \${line}"
    
    #Get the configuration to use and store in conf
    declare \$(echo \$line | awk -F \\. '{print "conf="\$2}')

    ./scripts/xml/stepSmear.pl \$conf $smearMin $smearMax > \$XMLLOC
    
    mpirun -np  16 \$exe  -geom \$geom -qmp-geom \$geom -iogeom \$iogeom -i \${XMLLOC} -o \${datadir}/\${conf}.xml > \${datadir}/\${conf}.log < /dev/null
    
done < <(cat \${FILELIST})

rm \$FILELIST \$XMLLOC

EOF
