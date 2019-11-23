/*
This script generates a file called DAQ.ratdb for use in RATPAC which
stores the default settings in DAQ.ratdb, plus adds lines for average
noise rate and turning on noise. It generates a normal distribution 
for noise rates on the PMT's, plus some low and high rate outliers. 
The normal distribution is centered at the nominal noise rate of 3 kHz 
and the max is 10 kHz. IMPORTANT: select only the options provided in 
the file handler to process existing DAQ.ratdb file. Otherwise the 
program terminates.
Author: Brian Crow
Institution: University of Hawaii at Manoa
Created: 10SEP2019
Modified: 22NOV2019
Version: 1.0.4
Changelog:
	v1.0.4: Corrected the uniform distribution on the low end and high end to be truly uniform.
	v1.0.3:	Restored "d" to the end of average noise entry. 
	v1.0.2:	Removed "d" from array and from average noise entry..
	v1.0.1:	Added "d" string to doubles for each frequency to go in output array.
		Declared all non-variable strings at the beginning of the main function
	v1.0.0:	Added random number generators for normal distribution of
		noise frequencies. Tails of the Gaussian are uniformly distributed.
	v0.2.0:	Added file handler for existing DAQ.ratdb files.
	v0.1.1:	Revised initial version now populates the correct number
		of PMT's for WATCHMAN.
	v0.1.0:	Initial version - code populates the array with the nominal
    		noise rate.
*/

// Include Statements
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <iomanip>
#include <cstdio>
#include <random>
#include <chrono>

using namespace std;

// Constants
const double nomRate(3000.0); // nominal dark current rate in Hz
const double maxRate(10000.0); // max dark current rate in Hz
const double sig(750.0); // standard deviation of frequency range
const double lower(nomRate-2*sig); // upper bound of the lowest uniform interval, should be nominal - 2*sig
const double upper(nomRate+2*sig); // lower bound of the highest uniform interval, should be nominal + 2*sig
const int N(3553); // index of highest PMT starting at 0. 3553 includes veto. Change to 3257 for inner PMT's only

// Function Declarations
double freq(double mean, double sd);
double tails();
double doLower();
double doUpper();

// main Function
int main()
{
    std::string fName = "DAQ.ratdb";
    std::string line1 = "{\n";
    std::string line2 = "name: \"DAQ\",\n";
    std::string line3 = "valid_begin: [0, 0],\n";
    std::string line4 = "valid_end: [0, 0],\n";
    std::string line5 = "channel_efficiency: 1.0,\n";
    std::string nRate = "noise_rate: ";
    std::string nRateEndl = "d,\n";
    std::string arrayStart = "PMTnoise: [";
    std::string delim = ", ";
    std::string arrayEnd = "],\n";
    std::string next2last = "noise_flag: 2,\n";
    std::string last = "}\n";
    std::ifstream iFile;
    std::ofstream oFile;
    double rate;

    // Check to see if the file already exists
    iFile.open(fName);
    if(iFile.fail())
    {
	    cout << "DAQ.ratdb does not exist. Creating file.\n";
	    oFile.open(fName);
    } // end if
    else
    {
	    char chr;
	    std::string fred;
	    cout << "DAQ.ratdb already exists. Select option: b)ackup existing file";
	    cout << " (WARNING! This will overwrite existing backups!), o)verwrite, or n)ew filename?";
	    cin >> fred;
	    cout << std::endl;
	    chr = fred[0];
	    switch(chr)
	    {
		    case 'b':
			    {
				    std::string name = fName+".backup";
				    oFile.open(name,ios::trunc);
				    std::string line;
				    while(getline(iFile,line))
				    {
					    oFile << line << std::endl;
				    } // end while for reading file and writing to backup
				    iFile.close();
				    oFile.close();
				    oFile.open(fName,ios::trunc); // open in truncate mode to overwrite DAQ.ratdb
				    break;
			    }
		    case 'o':
			    {
				    // open in truncate mode to overwrite DAQ.ratdb
				    iFile.close();
				    oFile.open(fName,ios::trunc);
				    break;
			    }
		    case 'n':
			    {
				    std::string name;
				    iFile.close();
				    fred.clear();
				    std::cout << "WARNING! Using this option will overwrite matching filenames! ";
				    std::cout << "Continue (y/n, n terminates program)? ";
				    std::cin >> fred;
				    if(fred[0]=='y')
				    {
					    std::cout << "Type the name of the file you wish to create: ";
					    std::cin >> name;
					    fName = name;
					    oFile.open(fName,ios::trunc);
				    }
				    else
					    exit(0);
				    break;
			    }
		    default:
			    {
				    std::cout << "Invalid input. Use 'b', 'o', or 'n'. Terminating program.\n";
				    iFile.close();
				    exit(1);
			    }
	    } // end switch
    } // end else

    // Write to the output file
    oFile << line1 << line2 << line3 << line4 << line5 << nRate << nomRate << nRateEndl << arrayStart;
	
	
    // make the array of noise frequencies and store in output file
    for (int i = 0; i <= N; i++)
    {
        /*
            Generate a Gaussian distribution of frequencies centered
            at 3 kHz, with a standard deviation of .75 kHz. Make a 
            uniform distribution for frequencies outside the range of
            1.5 kHz - 4.5 kHz and populate both ends uniformly. See 
	    	freq(mean, sd), tails(), doLower(), and doUpper() functions.
        */
	rate = freq(nomRate, sig);
	oFile << rate;
	if (i<N)
	{
		oFile << delim;
	} // end if
	else
	{
		oFile << arrayEnd << next2last << last;
		std::cout << fName << " has been populated. Terminating program.\n"; // debug message
	} // end else
    } // end for loop
    
    oFile.close();
    return 0;
} // End main Function

// freq Function
// generates a modified Gaussian
double freq(double mean, double sd)
{
	double x;
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen1(seed1);
	std::normal_distribution<double> dist1(mean,sd);
	x=dist1(gen1);
	if (x<lower||x>upper) // for more than 2 sigma away from nominal, use a uniform distribution
	{
		x=tails();
	} // end if
	dist1.reset();
	return x;
} // End freq Function

// tails Function
// puts uniformly distributed freqencies for any number in the tails of the calling Gaussian distribution
double tails()
{
	int p;
	unsigned seed2 = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen2(seed2);
	// create a distribution of 14 bins of 500 Hz each.
	std::uniform_int_distribution<int> dist2(1,14);
	p=dist2(gen2);
	double george;
	// Put in the lower uniform distribution on a 1-3, representing 0-1500Hz
	if (p<4)
		george=doLower();
	else // otherwise put in the upper distribution of 4500-10000Hz
		george=doUpper();
	dist2.reset();
	return george;
} // End tails Function

// doLower Function 
// generates a uniform distribution for the lower tail of the calling Gaussian
double doLower()
{
	double bob;
	unsigned seed3 = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen3(seed3);
	std::uniform_real_distribution<double> dist3(0,lower);
	bob=dist3(gen3);
	dist3.reset();
	return bob;
} // End doLower Function

// doUpper Function
// generates a uniform distribution for the upper tail of the calling Gaussian
double doUpper()
{
	double alice;
	unsigned seed4 = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen4(seed4);
	std::uniform_real_distribution<double> dist4(upper,maxRate);
	alice=dist4(gen4);
	dist4.reset();
	return alice;
} // End doUpper Function