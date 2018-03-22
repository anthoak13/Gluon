#!/usr/bin/perl


# Gets configurations to use from O_b directory
# combines them with correct configuration from
# the 2pt directory.
#
## Usage
# stripAll.pl --cluster=r --2pt "2ptDir" --Ob "ObDir"

## Assumtions on file structure for O_b ##

# O_b files without cluster decomoposition
#    "${config}r${r}$s{smear}.xml"
#    "1005r0s5.xml"
# O_b files with cluster decomposition
#    "Ob_l2464f211b600m00507m0507m628a_t9z9y8x9+t25z15y14x15+t41z9y8x9+t57z15y14x15.3545r0s5.xml"
# File name is essentially the same except cluster decomposition has added part with extra info

use Getopt::Long;

#By defualt assume no cluster decomposition
my $radius = '';
my $hypStep = '';
my $corrdir = "";
my $Obdir = "";
my $calc2pt = '1';
my @ObFiles;
my @corrFiles;


GetOptions ("cluster=i" => \$radius,
	    "smear=i"   => \$hypStep,
	    "2pt=s"     => \$corrdir,
	    "Ob=s"      => \$Obdir,
	    "do2pt"     => \$calc2pt)
    or die("Error in command line arguments");

#If radius is passed assume that means cluster decomp is used as well
if ($radius) {
    print "Using cluster decomp with radius $radius" . "\n";
} else {
    print "Not using cluster decomp" . "\n";
}
if ($hypStep) {
    print "Using $hypStep steps of HYP smearing" . "\n";
} else {
    print "Not using HYP smearing" . "\n";
}

#Make sure directories were passed
if(!$corrdir) {
    die("2pt directory was not passed with flag --2pt \n");
}
if(!$Obdir) {
    die("Ob directory was not passed with flag --Ob \n");
}

@files = <$Obdir/*>;

foreach $file (@files) {
    #Make sure file is an xml file
    #print "$file";
    if(! ($file =~ m{xml})){ 
	next;
    }
    
    my $fileName;
    my @srcs;

    
    #$file is now an xml file need to get just the file name
    # and remove the .xml tag
    #Stored in $fileName
    
    if($radius) {
	my @file= split('/', $file);
	@file= split('_', @file[-1]);
	@file= split('\.',@file[-1]);
	$fileName = @file[1];
	@srcs= split('\+', @file[0]);
    } else {
	my @file= split('/', $file);
	print "@file[-1]" . "\n";
	@file= split('\.', @file[-1]);
	$fileName = @file[0];
    }
    
    #If hypStep was passed pull out just matching files otherwise
    #just use all files
    if($hypStep && $fileName !~ /s$hypStep/) {
	print "Rejecting $fileName \n";
	next;
    }
    
    #Get the configuration used
    @temp = split("[sr]", $fileName);
    my $conf = @temp[0];


    #print $conf . "\n";
    #if($conf < 1005 || $conf >= 1105)
    #{
#	next;
 #   }
    
	#Loop through 2pt directory and look for
    #matching configurations
    
    #No cluster decomp used, so just look for matching
    #configuration number
    
    print "Looking for files matching $conf.\n";
    foreach $corrFile (<$corrdir/*>) {
	if ($corrFile !~ /BWD/ && $corrFile =~ /\.${conf}\./) {
	    
	    #Have a valid 2pt fucntion
	    #print "Found $corrFile \n";
	    push @corrFiles, $corrFile;
	    push @ObFiles, "$Obdir/$fileName.xml";
	    #push @ObFiles, "/mnt/home/antho121/lqcd/GMF/data/Ob/Ob/$conf.xml"; 
	    
	    
	}
    } #Get all matching 2pt functions
	
} #Done with file
my $curDir = `pwd`;
chdir "/mnt/home/antho121/lqcd/temp/temp";
system("pwd");

#push @ObFiles, "1>prog.stdout 2>prog.stderr";
my $ret = system("/mnt/home/antho121/lqcd/GMF/programs/stripOb", @ObFiles);
print "Ob status: $ret \n";

if($calc2pt) {
    $ret = system("/mnt/home/antho121/lqcd/GMF/programs/strip_nucl_2pt_proj", @corrFiles);
}

$corrFile="proton_nr.D-749.DG5p5_S.DG5p5_S.SS";
$ObFile="O_b.txt";
system("/mnt/home/antho121/lqcd/GMF/programs/processOb", $ObFile, $corrFile);

chdir $curDir;

print @corrFiles[2500] . "\n";
print @ObFiles[2500] . "\n";

print @corrFiles[6943] . "\n";
print @ObFiles[6943] . "\n";

print "Status: $ret \n";


