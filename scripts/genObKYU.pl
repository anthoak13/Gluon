#!/usr/bin/perl

$num_args = $#ARGV + 1;

if ($num_args != 0) {
    print << "Error";
There are  $num_args inputed! Expected 0
Error
   exit;
}
#/home/yangyibo/2464_005_04_nohyp/rbc_conf_2464_m0.005_0.04_$conf

$confDir = "/home/yangyibo/2464_005_04_nohyp";
@files = <$confDir/*>;

#Loop through directory with conf in
foreach $file (@files) {
    #make sure it is a conf
    if($file =~ m{txt}) {
	next;
    }

    print "$file \n";
}
