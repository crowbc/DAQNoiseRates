/*
This script generates a file called DAQ.ratdb for use in RATPAC which
stores the default settings in DAQ.ratdb, plus adds lines for average
noise rate and turning on noise. It generates a normal distribution 
for noise rates on the PMT's, plus some high rate outliers. The normal
distribution is centered at the nominal noise rate of 3 kHz and the 
max is 10 kHz. IMPORTANT: back up any copy of DAQ.ratdb stored in the 
ratpac/data folder before executing this script.
Author: Brian Crow
Institution: University of Hawaii at Manoa
Created: 10SEP2019
Modified: 12SEP2019
Version: 0.1.1
Changelog:
	v 0.1.1 Revised initial version now populates the correct number
		of PMT's for WATCHMAN.
	v 0.1:  Initial version - code populates the array with the nominal
    		noise rate.
*/

// import statements
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

using namespace std;

// constants
const double nomRate(3000.0); // nominal dark current rate in Hz
const double maxRate(10000.0); // max dark current rate in Hz
const int N(3553); // index of highest PMT, starting at 0 includes veto. change to 3257 for inner PMT's only

// function declarations

// can later add option for distribution types as inputs
int main()
{
    std::string fName = "DAQ.ratdb";
    std::string line1 = "{\n";
    std::string line2 = "name: \"DAQ\",\n";
    std::string line3 = "valid_begin: [0, 0],\n";
    std::string line4 = "valid_end: [0, 0],\n";
    std::string line5 = "channel_efficiency: 1.0,\n";
    std::string nRate = "noise_rate: 3000.0d,\n";
    std::string next2last = "noise_flag: 2,\n";
    std::string last = "}\n";
    std::ofstream oFile;
    // double noiseRate(3000.0);
    // do stuff to the output file
    oFile.open(fName);
    oFile << line1 << line2 << line3 << line4 << line5 << nRate << "PMTnoise: [";
	
	
    // make the array of noise frequencies and store in output file
    for (int i = 0; i <= N; i++)
    {
        /*
            generate a Gaussian distribution of frequencies centered
            at 3 kHz, with a standard deviation of 1 kHz. Make a 
            uniform distribution for frequencies outside the range of
            1 kHz - 5 kHz and populate both ends uniformly (i.e. put
            roughly 0.83% in each kHz bin outside the range of 1 to 5
            kHz.)
        */
	oFile << nomRate;
	if (i<N)
	{
		oFile << ", ";
	} // end if
	else
	{
		oFile << "],\n" << next2last << last;
	} // end else
    } // end for loop
    
    oFile.close();
    return 0;
} // end main
