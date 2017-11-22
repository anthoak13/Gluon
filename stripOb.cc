//Main program to extract O_bs from the xml files
//and write to a file shifting back to t = 0
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include "qdp.h"
#include "inline_Ob.h"

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
    //and dublicate data stucture used by O_b for srcs
    vec2dDouble O_b;
    multi1d<Chroma::InlineObEnv::InlineObParams::Src_t> srcs;
    
    O_b.resize(numFiles);    
    
    for(int i = 0; i < numFiles; i++)
    {
	XMLReader xml_in;
	
	//try to read in xml
	try
	{
	    std::cout << "Reading in file: " << argv[i+(argc - numFiles)] << std::endl;
	    
	    xml_in.open(argv[i + (argc-numFiles)]);

	    read(xml_in, "/GMF_O_b/O_b", O_b[i]);
	    read(xml_in, "/GMF_O_b/Input/Multi_Src", srcs);
	    int newStart = srcs[0].t_start;
	    
//	    std::cout << "New start is " << newStart << std::endl;
//	    std::cout << O_b[i][0] << " " << O_b[i][newStart] << " ";
	    std::rotate(O_b[i].begin(), O_b[i].begin()+newStart, O_b[i].end());
//	    std::cout << O_b[i][0] << std::endl;
	    
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

