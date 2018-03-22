#!/usr/bin/perl

$num_args = $#ARGV + 1;
if($num_args != 1) {
    print << "EOF";
There are $num_args inputed, extpected 4. Conf# Smearing steps Rmin Rmax
EOF
    exit;
}

$conf = $ARGV[0];
$gauge_id = "default_gauge_field";
$gauge_location = "/home/yangyibo/2464_005_04_nohyp/rbc_conf_2464_m0.005_0.04_$conf";


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

	<NamedObject>
	  <gauge_id>$gauge_id</gauge_id>
	</NamedObject>

	<xml_file>/mnt/home/antho121/lqcd/GMF/data/ObKYU/Ob/${conf}.xml</xml_file>
      </elem>

</InlineMeasurements>

    <nrow>24 24 24 64</nrow>
    
  </Param>
  
  <Cfg>
    <cfg_type>KYU</cfg_type>
    <cfg_file>$gauge_location</cfg_file>
    <parallel_io>true</parallel_io>
  </Cfg>
</chroma>
EOF
    
