//Main program to use O_bs and C2pt to construct 3pt and <x>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>


//Type aliases
using vecDouble = std::vector<double>;
using vec2dDouble = std::vector<vecDouble>;
using vec3dDouble = std::vector<vec2dDouble>;
using string = std::string;

//Forward declarations
int usage();
int readFile(vec2dDouble& data, string inFile);
void writeFile(const string fName, const vecDouble& a, const vecDouble& b);

void jackknife(const vec3dDouble& data, vec2dDouble& avg, vec2dDouble& sig);
void jackknife(const vec2dDouble& data, vecDouble& avg, vecDouble& sig);

vec2dDouble compress(const vec2dDouble& data, int tStart, int nSource);
void const3pt(vec3dDouble& corr3pt, const vec2dDouble& corr2pt, const vec2dDouble O_b);

//Global variables
string obName = "O_b";
string corr3ptName = "3ptCorr";
string corr2ptName = "2ptCorr";
int numSrc = 4;


//Arguments should be 2ptdatafile ObFile
int main(int argc, char *argv[])
{
    if(argc != 3)
	return usage();
    
    /** Load in the 2pt correlator and Ob values **/
    vec2dDouble O_b, corr2pt;
    readFile(O_b, argv[2]);
    readFile(corr2pt, argv[1]);
    int nT = O_b.back().size();
    int nConf = O_b.size();
    int subT = nT/numSrc;

    //Data structures
    vecDouble avgOb2, avg2pt2; //of size nT
    vecDouble avgOb, avg2pt; //size of subT


    //Make sure number of srcs passed is same for O_b and 2pt
    if(O_b.size() != corr2pt.size())
    {
	std::cout << "Error! Ob (" << O_b.size() << ") and 2pt ("
		  << corr2pt.size() << ") not same size!" << std::endl;
	return -1;
    }
    std::cout << "Using " << O_b.size() << " files with " << numSrc
	      << " number of sources per file." << std::endl;
       
    /** Find the average value of O_b and 2pt at each timeslice **/
    vecDouble avg, sig;
    jackknife(O_b, avg, sig);
    avgOb2 = avg;

    jackknife(corr2pt, avg, sig);
    avg2pt2 = avg;

    
    
    /** Compress the 2pt and O_b and write them out **/
    jackknife(compress(corr2pt,0,numSrc), avg, sig);
    writeFile("2pt.txt",avg,sig);
    avg2pt = avg;
    jackknife(compress(O_b,0,numSrc), avg, sig);
    writeFile("O_b.txt",avg,sig);
    avgOb = avg;

    //** Create C2/<C2> and then jackknife**/
    vec2dDouble norm2pt;
    norm2pt.resize(nConf);
    for(int conf = 0; conf < nConf; conf++)
	for(int t = 0; t < nT; t++)
	    norm2pt[conf].push_back(corr2pt[conf][t]/avg2pt2[t]);

    jackknife(compress(norm2pt,0,numSrc),avg,sig);
    writeFile("norm2pt.txt", avg, sig);


    /** Do vacuum subtraction on O_b and 2pt config by config **/
    for(int conf = 0; conf < nConf; conf++)
	for(int t = 0; t < nT; t++)
	{
	    O_b[conf][t] -= avgOb2[t];
	    corr2pt[conf][t] -= avg2pt2[t];
	}
    

    /** find matrix elements **/ //t_1 associated with O_b
    vec2dDouble x;
    x.resize(nConf);
    for(int conf = 0; conf < nConf; conf++)
    {
	x.at(conf) = vecDouble(subT,0);
	for(int t = 0; t < subT; t++)
	{
	    for(int t_1 = 1; t_1 < t; t_1++)
		for(int i = 0; i < numSrc; i++)
		    x[conf].at(t) += (O_b[conf][i*subT + t_1]*corr2pt[conf][i*subT+t]);
	
	    x[conf][t] /= (double)numSrc;
	    x[conf][t] /= avg2pt[t];
	}
    }

    jackknife(x,avg,sig);
    writeFile("x.txt", avg, sig);
}

int usage()
{
    std::cout << "Invalid number of arguments." << std::endl <<
	"Usage: processOb 2ptFile ObFile" << std::endl;
    return -1;
}

int readFile(vec2dDouble& data, string fName)
{
    data.clear();
    std::ifstream inFile;
    inFile.open(fName);
    if(inFile)
    {
	std::cout << "Reading data from "
		  << fName << std::endl;

	//Read in the header
	int nConf, nT;
	inFile >> nConf;
	inFile >> nT;
	//std::cout << nConf << " " << nT << std::endl;

	//Clear out rest of the header
	string temp;
	std::getline(inFile, temp);

	//Now load in all of the data
	for(int i = 0; i < nConf; i++)
	{
	    //Push back a vector of length 64 (nT)
	    data.push_back(vecDouble{});
	    for(int t = 0; t < nT; t++)
	    {
		int j;
		inFile >> j;
		if (t != j)
		{
		    std::cout << "Error reading file!" << std::endl;
		    return -1;
		}

		double val;
		inFile >> val;
		data.back().push_back(val);

		//Clear out rest of line
		std::getline(inFile, temp);
		
	    }//End loop through time
	} //End loop through configs
	
	inFile.close();
	std::cout << "Finished reading file" << std::endl;
    }
    else
	std::cout << "Error opening file: " << fName << std::endl;
}

void writeFile(const string fName, const vecDouble& a, const vecDouble& b)
{

    if(a.size() != b.size())
    {
	std::cout << "Vectors are different sizes! Skip writing" << fName << std::endl;
	return;
    }
    std::ofstream oFile;
    oFile.open(fName);
    if(oFile)
    {
	//Write out header
	oFile << "1 " << a.size() << " 1 24 1" << std::endl;
	//Write out the vector a/b
	for(int t = 0; t < a.size(); t++)
	    oFile << t << " " << a[t] << " " << b[t] << std::endl;
    }
    else
    {
	std::cout << "Can't open " << fName << "!" << std::endl;
	return;
    }
    oFile.close();

}

void jackknife(const vec2dDouble& data, vecDouble& avg, vecDouble& sig)
{
    int nConf = data.size();
    int nT = data.at(0).size();

    avg.clear(); sig.clear();
    avg.resize(nT); sig.resize(nT);
    vec2dDouble subset{data};

    //Find the average value for each time in each subset
    for(int i = 0; i < nConf; i++)
	for(int t = 0; t < nT; t++)
	    avg.at(t) += data.at(i).at(t);
    
    for(int i = 0; i < nConf; i++)
	for(int t = 0; t < nT; t++)
	    subset.at(i).at(t) = (avg[t]-data[i][t])/(nConf - 1);

    //Use subsets to get central value and sigma
    std::fill(avg.begin(), avg.end(), 0);
    for(int i = 0; i < nConf; i++)
	for(int t = 0; t < nT; t++)
	    avg[t] += subset[i][t];
    for(int t = 0; t < nT; t++)
	avg[t] /= nConf;
    
    //Sigma
    for(int i = 0; i < nConf; i++)
	for(int t = 0; t < nT; t++)
	    sig[t] += std::pow(avg[t] - subset[i][t],2);
    for(int t = 0; t < nT; t++)
	sig[t] = std::sqrt((nConf - 1)/(double)nConf * sig[t]);
        
}
//data[config][x][y] avg[x][y] sig[x][y]
void jackknife(const vec3dDouble& data, vec2dDouble& avg, vec2dDouble& sig)
{
    int numConfig = data.size();
    int nT = data.at(0).size();
    int nT_1 = data.at(0).at(0).size();

    //std::cout << numConfig << " " << nT << " " << nT_1 << std::endl;

    //For each timeslice, copy each config to a temp vector
    //and pass that to the 2d jackknife function
    for(int t = 0; t < nT; t++)
    {
	vec2dDouble temp;
	temp.resize(numConfig);
	for(int i = 0; i < numConfig; i++)
	{
	    temp.at(i).resize(nT_1);
	    for(int tt = 0; tt < nT_1; tt++)
		temp.at(i).at(tt) = data.at(i).at(t).at(tt);
	}
	avg.push_back(vecDouble{});
	sig.push_back(vecDouble{});
	jackknife(temp, avg.back(), sig.back());
    }
}

vec2dDouble compress(const vec2dDouble& data, int tStart, int nSrc)
{
    int subT = data.back().size()/(double)nSrc;
    int nConf = data.size();

    vec2dDouble ret(nConf);
    for(int conf = 0; conf < nConf; conf++)
    {
	ret[conf] = vecDouble(subT,0);
	for(int t = 0; t < subT; t++)
	{
	    for(int i = 0; i < numSrc; i++)
		ret[conf][t] += data[conf][i*subT + t];
	    
	    ret[conf][t] /= numSrc;
	}
    }
    return ret;
}

void const3pt(vec3dDouble& data,
	      const vec2dDouble& corr2pt, const vec2dDouble O_b)
{
    //Add check that nT is same for all configurations?
    if(corr2pt.size() != O_b.size() || corr2pt.size() < 1)
    {
	std::cout << "Number of configurations don't match!" << std::endl;
	return;
    }
    
    int nConf = corr2pt.size();
    int nT = corr2pt.back().size();

    //Get data to be the right size
    data.resize(nConf);
    for(auto&& elem : data)
    {
	elem.resize(nT);
	for(auto&& vec : elem)
	    vec.resize(nT);
    }

    
    for(int conf = 0; conf < nConf; conf++)
	for(int t = 0; t < nT; t++)
	    for(int t_1 = 0; t_1 < nT; t_1++)
		data.at(conf).at(t).at(t_1) = corr2pt[conf][t] * O_b[conf][t_1];
}
