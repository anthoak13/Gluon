#!/usr/bin/perl

$num_args = $#ARGV + 1;

if ($num_args != 3) {
    print << "Error";
There are $num_args inputed, but expected 2
Usage: test.pl 2ptFile truncRadius hypSteps
Error
   exit;
}
$r = $ARGV[1];
$hypStep = $ARGV[2];

#Split the file up to extract sources
@file= split('/', $ARGV[0]);
@file= split('_', @file[-1]);
@file= split('\.', @file[-1]);
$conf = @file[1];
@srcs= split('\+', @file[0]);


$data_loc = "data/truncObSmeared/Ob/${conf}r$r.xml";


#Create xml-file to generate OB around location

print << "EOF";
<?xml version="1.0"?>
<chroma>
  <Param>
    <InlineMeasurements>

      <elem>
	<Name>PLAQUETTE</Name>
	<Frequency>1</Frequency>
	<Param>
	  <version>2</version>
	</Param>
	<NamedObject>
	  <gauge_id>default_gauge_field</gauge_id>
	</NamedObject>
      </elem>
      
      <elem>
	<Name>GMF_O_b</Name>
	<Frequency>1</Frequency>
        <Multi_Src>
EOF


#Loop through the sources and populate multi_Src
for( $i = 0; $i < 4; $i = $i + 1 )
{
    #src has x,y,z,t as 0,1,2,3 
    @src= split('[txyz]', @srcs[$i]);
    shift @src;
    @src= reverse @src;
    $tend = @src[-1] + 16;

    print << "EOF";
          <elem>
            <t_src>@src</t_src>
            <t_start>@src[-1]</t_start>
            <t_start>$tend</t_start>
          </elem>

EOF
}
    
print << "EOF";
        </Multi_Src>
        <radius>$r</radius>
        <NamedObject>
          <gauge_id>default_gauge_id</gauge_id>
        </NamedObject>
        <xml_file>${data_loc}</xml_file>
      </elem>
    </InlineMeasurements>
    
    <nrow>24 24 24 64</nrow>

  </Param>

  <Cfg>
    <cfg_type>SCIDAC</cfg_type>
    <cfg_file>/mnt/home/antho121/lqcd/hypcfgs/l2464f211b600m0102m0509m635a.hyp.$conf.${hypStep}</cfg_file>
  </Cfg>
</chroma>

EOF
#"2pt_l2464f211b600m00507m0507m628a_t9z9y22x9+t25z15y4x15+t41z9y22x9+t57z15y4x15.3105.dat.xml";
