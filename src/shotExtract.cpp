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
		char cont[1000];
		fin.getline(cont, 999);
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

	int leftShotID = 3;
	mIter = opts.find("-leftShotID"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> leftShotID;}

	int rightShotID = 1000;
	mIter = opts.find("-rightShotID"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> rightShotID;}	

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
	int minShotID = preParam[0][0];
	int maxShotID = preParam[shotsCount-1][0];
	int tempID = minShotID;
	if (minShotID > maxShotID)
	{
		tempID = maxShotID;
		maxShotID = minShotID;
		minShotID = tempID;
	} 
/*	if (leftShotID > rightShotID)
	{
		cout << "Invalid. leftShotID should not be larger than rightShotID" << endl;
		return 1;
	}*/
	if (leftShotID < minShotID | leftShotID > maxShotID | rightShotID < minShotID | rightShotID > maxShotID)
	{
		cout << "The assigned shot IDs are invalid. Please check your input\n" 
			 << "Min shot index: " << minShotID << "\n"
			 << "Max shot index: " << maxShotID
			 << endl;
		return 1;
	}

	/*Locate */
	long long leftPos, rightPos, curPos, outPos;
	int charPerTrace = bSample*nSample + charTraceHeader; 
	char bufTrace[charPerTrace];

	leftPos = -1;
	rightPos = -1;
	for (int i = 0; i < shotsCount; i++)
	{
		if(preParam[i][0] == leftShotID)
			leftPos = preParam[i][1];
		if(preParam[i][0] == rightShotID)
			rightPos = preParam[i][2];
	}
	if (leftPos == -1)
	{
		cout << "Invalid. There is no shot indexed leftShotID: " << leftShotID << endl;
		return 1;
	}
	if (rightPos == -1)
	{
		cout << "Invalid. There is no shot indexed rightShotID: " << rightShotID << endl;
		return 1;
	}

	curPos = leftPos;
	outPos = 3600;

	char fnTemp[1000];
	sprintf(fnTemp, "%s_shotID_%d_%d.sgy", fnOut, leftShotID, rightShotID);
	streamOut = fopen(fnTemp, "wb");
	fwrite(buf3600, sizeof(char), charVolumeHeader, streamOut);
	cout << "Extracting shot indexed: " << leftShotID << "--" << rightShotID << "\n"
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