//Main program to use O_bs and C2pt to construct 3pt and <x>
//Assumes O_b and 2pt are shifted back to t=0.
#include <algorithm>
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
void jackknife(vec2dDouble& data);
void avgSig(const vec2dDouble& data, vecDouble& avg, vecDouble& sig);

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
    const bool runTest = false;
    
    bool do2pt = true;

    
    if(argc > 3 || argc < 2)
	return usage();
    if(argc == 2)
	do2pt = false;

    std::cout << "Argc = " << argc <<std::endl;

    
    /** Load in the 2pt correlator and Ob values **/
    vec2dDouble O_b, corr2pt;
    readFile(O_b, argv[1]);
    if(do2pt)
	readFile(corr2pt, argv[2]);

    /** Get information on size, etc. **/
    int nT = O_b.back().size();
    int nConf = O_b.size();
    int subT = nT/numSrc;


    /**Test for exploring effect of adding more sources to data**/
    if(runTest)
    {
	//Find O_b with four sources
	vecDouble avg1, sig1;
	jackknife(O_b, avg1, sig1);
	writeFile("O_b4src.txt",avg1,sig1);
	vec2dDouble multiSrc;
	for(int conf = 0; conf < nConf; conf++)
	{
	    for(int t = 0; t < subT; t++)
	    {
		multiSrc.push_back(O_b.at(conf));
		std::rotate(multiSrc.back().begin(), multiSrc.back().begin()+t, multiSrc.back().end());
		if(conf == 0)
		{
		    for( auto&& elem : multiSrc.back())
			std::cout << elem << std::endl;
		    std::cout << std::endl;
		}
	    }
	}

	jackknife(multiSrc,avg1,sig1);
	writeFile("O_b64src.txt",avg1, sig1);

	vec2dDouble multiSrc2;
	for(int conf = 0; conf < nConf; conf++)
	{
	    
		multiSrc2.push_back(O_b.at(conf));
		int rotateBy = conf % 16;
		std::rotate(multiSrc2.back().begin(), multiSrc2.back().begin()+rotateBy, multiSrc2.back().end());
		if(conf == 0)
		{
		    for( auto&& elem : multiSrc2.back())
			std::cout << elem << std::endl;
		    std::cout << std::endl;
		}
	    
	}

	jackknife(multiSrc2,avg1,sig1);
	writeFile("O_b64srcConstT.txt",avg1, sig1);
	
	return 0;
    } //End run test
    

    std::cout << "Comparing size" << std::endl;
    //Make sure number of srcs passed is same for O_b and 2pt
    if( do2pt && O_b.size() != corr2pt.size())
    {
	std::cout << "Error! Ob (" << O_b.size() << ") and 2pt ("
		  << corr2pt.size() << ") not same size!" << std::endl;
	return -1;
    }
    std::cout << "Using " << O_b.size() << " files with " << numSrc
	      << " sources per file." << std::endl;

    //Data structures
    vecDouble avgOb, avg2pt; //of size nT
    auto O_bjk = O_b;
    auto corr2ptjk = corr2pt;
    
    /** Find the jackknife distribution of O_b and the avg/sig **/
    vecDouble avg, sig;
    jackknife(O_bjk);
    avgSig(O_bjk, avg, sig);
    avgOb = avg;
    
        
    /** write out the average O_b **/
    writeFile("O_bAvg.txt",avg,sig);
    
    /** write out the compressed, averaged O_b **/
    jackknife(compress(O_b,0,numSrc), avg, sig);
    writeFile("O_bAvgCompr.txt", avg, sig);

    //If no 2pt was passed, end processing here
    if(!do2pt)
	return 0;

    /** Find the jackknife distribution of 2pt and the avg/sig **/
    jackknife(corr2ptjk);
    avgSig(corr2ptjk, avg, sig);
    avg2pt = avg;

    /** write out the average O_b **/
    writeFile("corr2ptAvg.txt",avg,sig);
    
    /** write out the compressed, averaged O_b **/
    jackknife(compress(corr2pt,0,numSrc), avg, sig);
    writeFile("corr2ptAvgCompr.txt", avg, sig);



    /** Find 2pt/<2pt> using jackknife distribution 
	(Equiv to finding 2pt/<2pt> and then jackknifing) **/
    vec2dDouble norm2pt;
    norm2pt.resize(nConf);
    for(int conf = 0; conf < nConf; conf++)
	for(int t = 0; t < nT; t++)
	    norm2pt[conf].push_back(corr2ptjk[conf][t]/avg2pt[t]);

    avgSig(norm2pt, avg, sig);
    writeFile("norm2pt.txt", avg, sig);


    //Get average 2pt over the four sources
    //per configuration

    vec2dDouble avg2pt2;
    for(int conf = 0; conf < nConf; conf++)
    {
	avg2pt2.push_back(vecDouble(subT,0));
	for(int t = 0; t < subT; t++)
	    for(int i = 0; i < numSrc; i++)
		avg2pt2.back()[t] += corr2pt[conf][t+i*subT]/numSrc;
    }

    
    /** find three point elements **/ //t_1 associated with O_b
    vec2dDouble three;
    three.resize(nConf);
    for(int conf = 0; conf < nConf; conf++)
    {
	three.at(conf) = vecDouble(subT,0);
	
	for(int t = 0; t < subT; t++)
	    for(int t_1 = 1; t_1 < t; t_1++)
		for(int i = 0; i < numSrc; i++) //i is src
		    //three.at(conf).at(t) += (O_b.at(conf).at(i*subT + t_1) - avgOb.at(i*subT + t_1))
		    //	* (corr2pt.at(conf).at(t+i*subT) - avg2pt.at(t+i*subT) )/4.0;
		    three.at(conf).at(t) += O_b.at(conf).at(i*subT + t_1)
		    	* (corr2pt.at(conf).at(t+i*subT) - avg2pt2[conf][t])/4.0;
	//Does vacuum subtraction on 2pt but not O_b (Because it's what yi-bo did in his comparison code)
	
    }
    
    //Do jackknife one 3pt
    jackknife(three);
    jackknife(avg2pt2);

   for(int conf = 0; conf < nConf; conf++)
	for(int t = 0; t < subT; t++)
	    three[conf][t] /= avg2pt2[conf].at(t);

    avgSig(three, avg, sig);
    writeFile("x.txt",avg,sig);
      
}

int usage()
{
    std::cout << "Invalid number of arguments." << std::endl <<
	"Usage: processOb  ObFile 2ptfile" << std::endl;
    std::cout << "If only one file is passed, assume it is O_b " <<
	"and do minimal processing." << std::endl;
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

void avgSig(const vec2dDouble& data, vecDouble& avg, vecDouble& sig)
{
    int nConf = data.size();
    int nT = data.back().size();

    if(nConf == 1)
    {
	avg = data.back();
	sig = data.back();
	return;
    }

    avg.clear(); sig.clear();
    avg.resize(nT); sig.resize(nT);

    std::fill(avg.begin(), avg.end(), 0);
    for(int t = 0; t < nT; t++)
    {
	for(int i = 0; i < nConf; i++)
	    avg[t] += data[i][t];
	avg[t] /= nConf;
    }
    
    //Sigma
    for(int i = 0; i < nConf; i++)
	for(int t = 0; t < nT; t++)
	    sig[t] += std::pow(avg[t] - data[i][t],2);
    for(int t = 0; t < nT; t++)
	sig[t] = std::sqrt((nConf - 1)/(double)nConf * sig[t]);
}

void jackknife(vec2dDouble& data)
{
    vec2dDouble subset{data};
    int nConf = data.size();
    int nT = data.back().size();
    if(nConf == 1)
	return;

    vecDouble sum(nT,0);
    for(int i = 0; i < nConf; i++)
	for(int t = 0; t < nT; t++)
	    sum.at(t) += data.at(i).at(t);
    
    for(int i = 0; i < nConf; i++)
	for(int t = 0; t < nT; t++)
	    subset.at(i).at(t) = (sum[t]-data[i][t])/(nConf - 1);
    data = subset;
}

void jackknife(const vec2dDouble& data, vecDouble& avg, vecDouble& sig)
{
    
    vec2dDouble subset{data};

    jackknife(subset);
    avgSig(subset, avg, sig);
        
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
