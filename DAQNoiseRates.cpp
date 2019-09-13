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
Modified: 12SEP2019
Version: 1.0
Changelog:
	v 1.0:	Added random number generators for normal distribution of
		noise frequencies. Tails of the Gaussian are uniformly distributed.
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
#include <random>
#include <chrono>

using namespace std;

// constants
const double nomRate(3000.0); // nominal dark current rate in Hz
const double maxRate(10000.0); // max dark current rate in Hz
const double lower(1500.0); // upper bound of the lowest uniform interval
const double upper(4500.0); // lower bound of the highest uniform interval
const double sig(750.0); // standard deviation of frequency range
const int N(3553); // index of highest PMT starting at 0. Includes veto. Change to 3257 for inner PMT's only

// function declarations
double freq(double mean, double sd);
double tails();
double doLower();
double doUpper();

// main function
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
				    std::cout << "Continue (y/n)? ";
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
    oFile << line1 << line2 << line3 << line4 << line5 << nRate << "PMTnoise: [";
	
	
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
		oFile << ", ";
	} // end if
	else
	{
		oFile << "],\n" << next2last << last;
		std::cout << fName << " has been populated. Terminating program.\n"; // debug message
	} // end else
    } // end for loop
    
    oFile.close();
    return 0;
} // end main function

// Function to generate a modified Gaussian
double freq(double mean, double sd)
{
	double x;
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen1(seed1);
	std::normal_distribution<double> dist1(mean,sd);
	x=dist1(gen1);
	if (x<mean-2*sd||x>mean+2*sd) // for more than 2 sigma away from nominal, use a uniform distribution
	{
		x=tails();
	} // end if
	dist1.reset();
	return x;
} // end freq function

// Function to put uniformly distributed freqencies for any number in the tails of the calling Gaussian distribution
double tails()
{
	int p;
	unsigned seed2 = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen2(seed2);
	std::uniform_int_distribution<int> dist2(1,4);
	p=dist2(gen2);
	double george;
	if (p==1) // Put in the lower uniform distribution on a 1
		george=doLower();
	else // otherwise put in the upper distribution
		george=doUpper();
	dist2.reset();
	return george;
} // end tails function

// Function to generate a uniform distribution for the lower tail of the calling Gaussian
double doLower()
{
	double bob;
	unsigned seed3 = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen3(seed3);
	std::uniform_real_distribution<double> dist3(0,lower);
	bob=dist3(gen3);
	dist3.reset();
	return bob;
} // end doLower function
// Function to generate a uniform distribution for the upper tail of the calling Gaussian
double doUpper()
{
	double alice;
	unsigned seed4 = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine gen4(seed4);
	std::uniform_real_distribution<double> dist4(upper,maxRate);
	alice=dist4(gen4);
	dist4.reset();
	return alice;
} // end doUpper function

