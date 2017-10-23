#ifndef __inline_Ob_h__
#define __inline_Ob_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"

namespace Chroma 
{ 
  // A namespace for this particular   measurement
  namespace InlineObEnv 
  {
    extern const std::string name;
    bool registerAll();
  

    //! Parameter structure
    struct InlineObParams 
    {
	// Default constructor -- forward declaration
	InlineObParams();
	
	// Construct from XML -- forward declaration
	InlineObParams(XMLReader& xml_in, const std::string& path);
	
	// Write myself out
	void write(XMLWriter& xml_out, const std::string& path);
	
	// How often should I measure me in an HMC run
	unsigned long frequency;

	struct NamedObject_t
	{
	    std::string gauge_id;
	} named_obj;

	int t_start;
	int t_length;
	
	// Parameters to pull from the xml file
	std::string xml_file; //optional xml output

		
    }; // struct
  }; // namespace InlineObEnv

  class InlineMyMeas : public AbsInlineMeasurement 
  {
  public:
      // Constructor -- default -- do nothing
      ~InlineMyMeas() {}
      
      // Constructor -- from param struct -- copy param struct inside me
      InlineMyMeas(const InlineObEnv::InlineObParams& p) : params(p) {}
      
      // Constructor -- copy constructor -- copy param struct from argument
      InlineMyMeas(const InlineMyMeas& p) : params(p.params) {}

    // Boiler plate
    unsigned long getFrequency(void) const {return params.frequency;}

    //! Do the measurement
    void operator()(const unsigned long update_no,
		    XMLWriter& xml_out); 


  private:
    void func(const unsigned long update_no,
	      XMLWriter& xml_out);

    //function to get the time direction
    static int tDir() { return Nd-1;}

    //Returns unnormalized O_b at coordinates coords
    //Also returns E^2 and B^2 at that point
    Double getO_b(const multi1d<int>& coords,
		  const multi2d<LatticeColorMatrix>& plane_plaq,
		  Double &E, Double &B);
    
    //Vector contains O_b found at each time t. Assumes tDir == 3
    void getO_b(std::vector<Double>& O_b,
		std::vector<Double>& E,
		std::vector<Double>& B, int tStart,
		const multi2d<LatticeColorMatrix>& plane_plaq);

    ColorMatrix get_G(const multi1d<int>& coords, int mu, int nu,
		      const multi2d<LatticeColorMatrix>& P);
    

    //private parameters
    InlineObEnv::InlineObParams params;
    
  };

};

#endif
