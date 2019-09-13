# DAQNoiseRates
This C++ script will generate a file called DAQ.ratdb for use in RATPAC for the AIT-WATCHMAN project. The DAQ.ratdb file will contain the default options currently on the ait-watchman GitHub, and add an average PMT noise rate, and populate an array for the individual noise rate of the 3554 PMT's. The current version will populate the array with randomly generated noise rates up to the maximum of 10 kHz. All noise rates use double precision floating point numbers as the variable type. v0.2 added a file handler to backup, overwrite, or rename in the case of existing DAQ.ratdb files. v1.0 is the current version.
