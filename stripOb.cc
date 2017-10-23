//Main program to extract O_bs from the xml files
//and write to a file shifting back to t = 0
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include "qdp.h"

//Type aliases
using vecDouble = std::vector<double>;
using vec2dDouble = std::vector<vecDouble>;
using string = std::string;
using namespace QDP;

//Forward declarations
void usage();
void writeFile(const vec2dDouble& yVec, std::ofstream& oFile, const string fName);


//Global variables
string obName = "O_b.txt";


//Arguments include what was used to call function ie "./strip test" returns
//argv[0] = "./strip"
//argv[1] = "test"

//Arguments should be file0 file1 ... fileN
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
	usage();
	return -1;
    }
    
    //Initialize QDP for access to XML classes
    int temp = 0;
    QDP_initialize(&temp, &argv);
    int numFiles = argc - 1;

    std::cout << "Examining " << numFiles << " files." << std::endl;

    /** Open the output file **/
    std::ofstream oFile;
    oFile.open(obName);
    if(!oFile)
    {
	std::cout << "Output file: " << obName <<
	    " could not be opened." << std::endl;
	return 1;
    }
    
    //Create 2D vector to hold all values of O_b
    vec2dDouble O_b;
    O_b.resize(numFiles);    
    
    for(int i = 0; i < numFiles; i++)
    {
	XMLReader xml_in;
	
	//try to read in xml
	try
	{
	    std::cout << "Reading in file: " << argv[i+(argc - numFiles)] << std::endl;
	    xml_in.open(argv[i + (argc-numFiles)]);

	    //Get the dimmensions of the lattice from the
	    //xml file.
	    multi1d<int> lattSize;
	    read(xml_in,"/chroma/Input/chroma/Param/nrow", lattSize);
	    int t_start = 0;
	    read(xml_in,"/chroma/Input/chroma/Param/InlineMeasurements/elem/t_start", t_start);
	    std::cout << "Starting at: " << t_start << std::endl;

	    //Loop through and copy values from xmlFile into
	    //O_b
	    for(int t = 0; t < lattSize[3]; t++)
	    {
		double val = 0;
		int time = (t - t_start + lattSize[3]) % lattSize[3];
		//std::cout << t << " " << time << std::endl;
		read(xml_in, "/chroma/InlineObservables/elem/GMF_O_b/t_" +
		     std::to_string((t-t_start)%lattSize[3]) + "/O_b", val);
		O_b.at(i).push_back(val);
	    }
	}
	catch(const std::string& e)
	{
	    std::cout << "Error reading in file: " <<
		argv[i] << std::endl;
	    std::cout << "Error: " << e << std::endl;
	    return 2;
	}
	catch(...)
	{
	    std::cout << "A generic error was caught reading in: " <<
		argv[i] << std::endl;
	    return 3;
	} //End try-catch block for processing xml file
    } //End loop over files

    //Write out just the O_b values
    writeFile(O_b, oFile, obName);
    oFile.close();
}

void usage()
{
    std::cout << "Invalid number of arguments." << std::endl <<
	"Usage: strip  inFile1 inFile2 ... inFileN" << std::endl;
}

//yVec[config][time]
void writeFile(const vec2dDouble& yVec, std::ofstream& oFile, const string fName)
{
    std::cout << "Writing output file " << fName << std::endl;
    int nConf = yVec.size();
    int nT = yVec.back().size();
    
    //Print the header
    oFile << nConf << " " << nT << " 1 24 1" << std::endl;
    
    for(int i = 0; i < nConf; i++)
	for(int t = 0; t < nT; t++)
	oFile << t << " " << yVec[i][t] <<  std::endl;

    std::cout << "Done writing " << fName << std::endl;
}

