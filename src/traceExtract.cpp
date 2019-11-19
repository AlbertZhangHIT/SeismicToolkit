/*
	Copyright (C) 2017 HIT
	Written by Hao Zhang
*/
#include "basic_inc.hpp"
#include "preReader.hpp"
#include "vhead.hpp"

using namespace std;

int optionsCreate(const char* optfile, map<string, string>& options)
{
	options.clear();
	ifstream fin(optfile); assert(fin.good());
	string name; fin>>name;
	while(fin.good()) {
		char cont[100];
		fin.getline(cont, 99);
		options[name] = string(cont);
		fin>>name;
	}
	fin.close();
	return 0;
}

int main(int argc, char* argv[])
{
	int charTraceHeader = 240;
	int charVolumeHeader = 3600;	
	// get options 
	map<string, string> opts; optionsCreate(argv[1], opts);
	map<string, string>::iterator mIter;

	char fnIn[1000];
	mIter = opts.find("-infile"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second);	ss >> fnIn;}

	char fnOut[1000];
	mIter = opts.find("-outfilePrefix"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second);	ss >> fnOut;}

	int maxTracePerShot = 100000;
	mIter = opts.find("-maxTracePerShot");	assert(mIter != opts.end());
	{istringstream ss((*mIter).second);	ss >> maxTracePerShot;}

	int leftTraceIndex = 3;
	mIter = opts.find("-leftTraceIndex"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> leftTraceIndex;}

	int rightTraceIndex = 1000;
	mIter = opts.find("-rightTraceIndex"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> rightTraceIndex;}	

	int maxShotNumber = 1000000;
	mIter = opts.find("-maxShotNumber"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> maxShotNumber;}
 
	long long i64 = 0;
	long long fileSize;
	char buf1[1];
	char buf3600[charVolumeHeader];
	int statusError;

	FILE * streamIn;
	FILE * streamOut;

	streamIn = fopen(fnIn, "rb");

	if(streamIn == NULL)
	{
		cout << "Error: Failed in opening file \"" << fnIn <<"\". Please check it." << endl;
		return 1;
	}

	fseeko(streamIn, 0, SEEK_END);
	fileSize = ftello64(streamIn);
	fseeko(streamIn, 0, SEEK_SET);

	fread(buf3600, sizeof(char), charVolumeHeader, streamIn);
	i64 += charVolumeHeader;

	statusError = vheadChecker(buf3600);
	if (statusError)
	{
		cout << "Failure...The input file may not have a volume text. " << endl;
		return 1;
	}

	/* segy file volume header parsing*/
	int nSample, bSample;
	double rSample;
	statusError = vheadReader(buf3600, bSample, nSample, rSample);
	if(statusError)
	{
		cout << "Error: Failed in parsing volume header." << endl;
		return 1;
	} else{
		cout << "Segy File: " << fnIn << " volume parsing out: \n"
			 << "Trace Samples: " << nSample << "\n"
			 << "Sample Rate: " << rSample << " s\n" 
			 << "Sample Byte: " << bSample << "\n"
			 << endl;
	}

	/* Get summary information of the input segy file */
	int shotsCount, tracesCount;
	long long **preParam;
	preParam = new long long *[maxShotNumber];
	for (int i = 0; i < maxShotNumber; i++)
		preParam[i] = new long long[3];
	preReader2D(streamIn, preParam, maxTracePerShot, nSample, bSample, shotsCount, tracesCount);

	cout << "--------------- Summary Information -------------\n" 
		 << "#Traces    \t\t : " << tracesCount << "\n"
		 << "#Shots     \t\t : " << shotsCount << "\n"
		 << "First FFID \t\t : " << preParam[0][0] << "\n"
		 << "Last  FFID \t\t : " << preParam[shotsCount-1][0] << "\n"
		 << endl;

	/*Check whether the input parameters are valid*/
	if (leftTraceIndex > rightTraceIndex)
	{
		cout << "Invalid. leftTraceIndex should not be larger than rightTraceIndex" << endl;
		return 1;
	}
	if (leftTraceIndex < 0 | leftTraceIndex > tracesCount | rightTraceIndex < 0 | rightTraceIndex > tracesCount)
	{
		cout << "The assigned trace indexes are invalid. Please check your input\n" 
			 << " Max trace index: " << tracesCount
			 << endl;
		return 1;
	}

	/*Locate */
	long long leftPos, rightPos, curPos, outPos;

	int charPerTrace = bSample*nSample + charTraceHeader; 
	char bufTrace[charPerTrace];
	leftPos = (long long)((leftTraceIndex-1)*charPerTrace + charVolumeHeader);
	rightPos = (long long)(rightTraceIndex*charPerTrace + charVolumeHeader);
	curPos = leftPos;
	outPos = 3600;

	char fnTemp[1000];
	sprintf(fnTemp, "%s_traces_%d_%d.sgy", fnOut, leftTraceIndex, rightTraceIndex);
	streamOut = fopen(fnTemp, "wb");
	fwrite(buf3600, sizeof(char), charVolumeHeader, streamOut);
	cout << "Extracting traces indexed: " << leftTraceIndex << "--" << rightTraceIndex << "\n"
		 << "Writing to File: " << fnTemp << endl;
	while(curPos < rightPos)
	{
		fseeko(streamIn, curPos, SEEK_SET);
		fread(bufTrace, sizeof(char), charPerTrace, streamIn);
		fseeko(streamOut, outPos, SEEK_SET);
		fwrite(bufTrace, sizeof(char), charPerTrace, streamOut);
		curPos += charPerTrace;
		outPos += charPerTrace;
	}
	cout << "Well done!" << endl;
	fclose(streamOut);

	
	return 0;
}