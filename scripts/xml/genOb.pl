#!/usr/bin/perl

$num_args = $#ARGV + 1;
if($num_args != 4) {
    print << "EOF";
There are $num_args inputed, extpected 4. Conf# Smearing steps Rmin Rmax
EOF
    exit;
}

$conf = $ARGV[0];
$smear_num = $ARGV[1];
$rMin = $ARGV[2];
$rMax = $ARGV[3];
$gauge_id = "default_gauge_field";
if($smear_num == 0)
{
    $gauge_location = "/mnt/home/antho121/lqcd/hypcfgs/l2464f211b600m0102m0509m635a.hyp.$conf";
} else {
    $gauge_location = "/mnt/home/antho121/lqcd/hypcfgs/l2464f211b600m0102m0509m635a.hyp.$conf.$smear_num";
}

print << "EOF";
<?xml version="1.0"?>
<chroma>
  <Param>
    <InlineMeasurements>

      <!-- Do measurement -->
      <elem>
	<Name>PLAQUETTE</Name>
	<Frequency>1</Frequency>

	<Param>
	  <version>2</version>
	</Param>
	<NamedObject>
	  <gauge_id>$gauge_id</gauge_id>
	</NamedObject>
      </elem>
EOF

for( $r = $rMin; $r <= $rMax; $r = $r + 1 )
{
    $fileName = "";
    if($r != 0) {
	$fileName .= "r$r";
    }
    if($smear_num != 0) {
	$fileName .= "s$smear_num";
    }
    
    print << "EOF";
      <elem>
	<Name>GMF_O_b</Name>
	<Frequency>1</Frequency>

	<Multi_Src>
	  <elem>
	    <t_src>12 4 8 2</t_src>
	    <t_start>2</t_start>
	    <t_end>17</t_end>
	  </elem>
	  <elem>
	    <t_src>4 12 8 18</t_src>
	    <t_start>18</t_start>
	    <t_end>31</t_end>
	  </elem>
	  <elem>
	    <t_src>12 4 8 2</t_src>
	    <t_start>32</t_start>
	    <t_end>49</t_end>
	  </elem>
	  <elem>
	    <t_src>4 12 8 18</t_src>
	    <t_start>50</t_start>
	    <t_end>1</t_end>
	  </elem>
	</Multi_Src>
	<radius>$r</radius>

	<NamedObject>
	  <gauge_id>$gauge_id</gauge_id>
	</NamedObject>

	<xml_file>/mnt/home/antho121/lqcd/GMF/data/Ob/Ob/${conf}$fileName.xml</xml_file>
      </elem>


EOF

}

print << "EOF"
</InlineMeasurements>

    <nrow>24 24 24 64</nrow>
    
  </Param>
  
  <Cfg>
    <cfg_type>SCIDAC</cfg_type>
    <cfg_file>$gauge_location</cfg_file>
  </Cfg>
</chroma>
EOF
    
