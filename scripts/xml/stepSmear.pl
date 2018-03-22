#!/usr/bin/perl

$num_args = $#ARGV + 1;

if ($num_args != 3) {
    print << "Error";
There are $num_args inputed, but expected 1
Usage: test.pl conf# smearMin smearMax
Error
   exit;
}
$conf = $ARGV[0];
$smearMin = $ARGV[1];
$smearMax = $ARGV[2];
print << "EOF";
<?xml version="1.0"?>
<chroma>
  <Param>
    <InlineMeasurements>
   
EOF


#Print out first 2 smear
print << "EOF";
    <elem>
    <Name>LINK_SMEAR</Name>
      <Frequency>1</Frequency>
      <Param>
        <version>5</version>
        <LinkSmearingType>HYP_SMEAR</LinkSmearingType>
        <alpha1>0.75</alpha1>
        <alpha2>0.6</alpha2>
        <alpha3>0.3</alpha3>
        <num_smear>$smearMin</num_smear>
        <no_smear_dir>-1</no_smear_dir>
        <BlkMax>100</BlkMax>
        <BlkAccu>1.0e-7</BlkAccu>
      </Param>
      <NamedObject>
        <gauge_id>default_gauge_field</gauge_id>
        <linksmear_id>hyp_gauge_field$smearMin</linksmear_id>
      </NamedObject>
    </elem>

    <elem>
      <Name>QIO_WRITE_NAMED_OBJECT</Name>
      <Frequency>1</Frequency>
      <NamedObject>
        <object_id>hyp_gauge_field$smearMin</object_id>
	<object_type>Multi1dLatticeColorMatrix</object_type>
      </NamedObject>
      <File>
	<file_name>/mnt/home/antho121/lqcd/hypcfgs/l2464f211b600m0102m0509m635a.hyp.$conf.$smearMin</file_name>
        <file_volfmt>SINGLEFILE</file_volfmt>
      </File>
    </elem>

EOF

   
for( $i = $smearMin; $i < $smearMax; $i = $i + 1 )
{
    $newI = $i + 1;
    print << "EOF";
    <elem>
      <Name>LINK_SMEAR</Name>
      <Frequency>1</Frequency>
      <Param>
        <version>5</version>
        <LinkSmearingType>HYP_SMEAR</LinkSmearingType>
        <alpha1>0.75</alpha1>
        <alpha2>0.6</alpha2>
        <alpha3>0.3</alpha3>
        <num_smear>1</num_smear>
        <no_smear_dir>-1</no_smear_dir>
        <BlkMax>100</BlkMax>
        <BlkAccu>1.0e-7</BlkAccu>
      </Param>
      <NamedObject>
        <gauge_id>hyp_gauge_field$i</gauge_id>
        <linksmear_id>hyp_gauge_field$newI</linksmear_id>
      </NamedObject>
    </elem>

    <elem>
      <Name>QIO_WRITE_NAMED_OBJECT</Name>
      <Frequency>1</Frequency>
      <NamedObject>
        <object_id>hyp_gauge_field$newI</object_id>
	<object_type>Multi1dLatticeColorMatrix</object_type>
      </NamedObject>
      <File>
	<file_name>/mnt/home/antho121/lqcd/hypcfgs/l2464f211b600m0102m0509m635a.hyp.$conf.$newI</file_name>
        <file_volfmt>SINGLEFILE</file_volfmt>
      </File>
    </elem>

EOF
}




print << "EOF";

    </InlineMeasurements>
    
    <nrow>24 24 24 64</nrow>

  </Param>

  <Cfg>
    <cfg_type>SCIDAC</cfg_type>
    <cfg_file>/mnt/home/antho121/lqcd/hypcfgs/l2464f211b600m0102m0509m635a.hyp.$conf</cfg_file>
  </Cfg>
</chroma>
EOF


#"2pt_l2464f211b600m00507m0507m628a_t9z9y22x9+t25z15y4x15+t41z9y22x9+t57z15y4x15.3105.dat.xml";
