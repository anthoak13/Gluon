topdir="/home/antho121/lqcd"
ob_dir="/home/antho121/lqcd/GMF/data/truncObSmeared/Ob"
result_dir="/home/antho121/lqcd/GMF/results"
PT2="$topdir/GMF/data/nucl_2pt_heavy/corr/proton_nr.D-193.DG5p5_S.DG5p5_S.SS" #Medium mass

for i in `seq 8 2 10`;
do
    cd $result_dir
    mkdir r$i
    cd r$i
    
    $topdir/GMF/programs/strip $ob_dir/*r$i.xml
    $topdir/GMF/programs/processOb $PT2 O_b.txt

    cd $result_dir
    
done

	 
