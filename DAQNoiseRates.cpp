/*
This script generates a file called DAQ.ratdb for use in RATPAC which
stores the default settings in DAQ.ratdb, plus adds lines for average
noise rate and turning on noise. It generates a normal distribution 
for noise rates on the PMT's, plus some high rate outliers. The normal
distribution is centered at the nominal noise rate of 3 kHz and the 
max is 10 kHz. IMPORTANT: select only the options provided in the file 
handler to process existing DAQ.ratdb file. Otherwise the program 
terminates.
Author: Brian Crow
Institution: University of Hawaii at Manoa
Created: 10SEP2019
Modified: 12SEP2019
Version: 0.1.1
Changelog:
	v 0.2:	Added file handler for existing DAQ.ratdb files.
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
    std::ifstream iFile;
    std::ofstream oFile;
    char chr;
    // double noiseRate(3000.0);
    // Check to see if the file already exists
    iFile.open(fName);
    if(iFile.fail())
    {
	    cout << "DAQ.ratdb does not exist. Creating file.\n";
	    oFile.open(fName);
    } // end if
    else
    {
	    cout << "DAQ.ratdb already exists. Select option: b)ackup existing file";
	    cout << " (WARNING! This will overwrite existing backups!), o)verwrite, or n)ew filename?";
	    cin >> chr;
	    cout << std::endl;
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
				    std::cout << "WARNING! Using this option will overwrite matching filenames!";
				    std::cout << "Type the name of the file you wish to create: ";
				    std::cin >> name;
				    // name = name - "\n";
				    oFile.open(name,ios::trunc);
				    break;
			    }
		    otherwise:
			    {
				    std::cout << "Invalid input. Terminating program.\n";
				    iFile.close();
				    exit(1);
			    }
	    } // end switch
    } // end else

    // Write to the output file
    // oFile.open(fName);
    oFile << line1 << line2 << line3 << line4 << line5 << nRate << "PMTnoise: [";
	
	
    // make the array of noise frequencies and store in output file
    for (int i = 0; i <= N; i++)
    {
        /*
            Generate a Gaussian distribution of frequencies centered
            at 3 kHz, with a standard deviation of .75 kHz. Make a 
            uniform distribution for frequencies outside the range of
            1.5 kHz - 4.5 kHz and populate both ends uniformly (i.e. put
            roughly 0.017 % of frequencies below the 2.5th percentile and 
	    above the 97.5th percentile in each .1 kHz bin outside the 
	    range of 1.5 to 4.5 kHz.)
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
