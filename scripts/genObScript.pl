#!/usr/bin/perl

$num_args = $#ARGV + 1;

if ($num_args != 4) {
    print << "Error";
There are only $num_args inputed! Expected i smearnum rmin rmax
Error
   exit;
}
$i=$ARGV[0];
$smear=$ARGV[1];
$rmin=$ARGV[2];
$rmax=$ARGV[3];
   
    
print << "EOF";
#!/bin/bash -login
#PBS -l walltime=04:00:00
#PBS -l nodes=1:ppn=16
#PBS -l mem=6gb
#PBS -N Ob


TOPDIR="/home/antho121/lqcd/GMF"
CHROMADIR="\${TOPDIR}/programs"
OBDIR="\${TOPDIR}/data/ObSmeared/log"
FILELIST="/home/antho121/lqcd/temp/file_list$i.txt"
XMLLOC="/home/antho121/lqcd/temp/in_$i$smear.xml"

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

    #conf smear rmin rmax
    ./scripts/xml/genOb.pl \$conf $smear $rmin $rmax > \$XMLLOC
    
    mpirun -np  16 \$exe  -geom \$geom -qmp-geom \$geom -iogeom \$iogeom -i \${XMLLOC} -o \${OBDIR}/\${conf}.$smear.xml > \${OBDIR}/\${conf}.$smear.log < /dev/null
    
done < <(cat \${FILELIST})

rm \$FILELIST \$XMLLOC

EOF
