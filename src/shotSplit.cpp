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

	int shotPerSubfile = 3;
	mIter = opts.find("-shotPerSubfile"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> shotPerSubfile;}

	int maxShotNumber = 1000000;
	mIter = opts.find("-maxShotNumber"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> maxShotNumber;}
 
	long long i64 = 0;
	long long fileSize;
	char buf1[1];
	char buf3600[3600];
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

	fread(buf3600, sizeof(char), 3600, streamIn);
	i64 += 3600;

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

	/* Check whether the input parameters are valid*/
	if(shotPerSubfile > shotsCount)
	{
		cout << "Parameter shotPerSubfile: " << shotPerSubfile << " is larger than number of shots: " 
			 << shotsCount << "\n"
			 << "Please reset." 
			 << endl;
		return 1;
	}

	int rema = shotsCount%shotPerSubfile; 
	int groups;
	if(rema == 0)
		groups = (int)(shotsCount/shotPerSubfile);
	else
		groups = (int)(shotsCount/shotPerSubfile) + 1;

	char fnTemp[1000];
	int shotIDleft, shotIDright, groupTraces;
	int byteTrace = nSample*bSample + 240;
	char bufTrace[byteTrace];
	long long posLeft, posRight, posCur, posOut;
	for(int i = 1; i < groups; i++)
	{
		groupTraces = 0;
		posLeft = preParam[(i-1)*shotPerSubfile][1];
		posRight = preParam[i*shotPerSubfile-1][2];
		posCur = posLeft;
		shotIDleft = preParam[(i-1)*shotPerSubfile][0];
		shotIDright = preParam[i*shotPerSubfile-1][0];
		sprintf(fnTemp, "%s_shotID_%d_%d.sgy", fnOut, shotIDleft, shotIDright);
		streamOut = fopen(fnTemp, "wb");
		fwrite(buf3600, sizeof(char), 3600, streamOut);
		posOut = 3600;
		while(posCur < posRight)
		{
			fseeko(streamIn, posCur, SEEK_SET);
			fread(bufTrace, sizeof(char), byteTrace, streamIn);
			fseeko(streamOut, posOut, SEEK_SET);
			fwrite(bufTrace, sizeof(char), byteTrace, streamOut);
			posCur += byteTrace;
			posOut += byteTrace;
			groupTraces += 1;
		}
		cout << "Task " << i << ": Writing to " << fnTemp << "\n" 
			 << "\t\t | Traces " << groupTraces << " | "
			 << "First ShotID " << shotIDleft << " | "
			 << "Last ShotID " << shotIDright << " | "
			 << endl;
		fclose(streamOut);
	}
	// write the last group
	{
	groupTraces = 0;
	if (rema == 0)
	{
		posLeft = preParam[(groups-1)*shotPerSubfile][1];
		posRight = preParam[groups*shotPerSubfile-1][2];
		shotIDleft = preParam[(groups-1)*shotPerSubfile][0];
		shotIDright = preParam[groups*shotPerSubfile-1][0];
	} else{
		posLeft = preParam[(groups-1)*shotPerSubfile][1];
		posRight = preParam[shotsCount-1][2];
		shotIDleft = preParam[(groups-1)*shotPerSubfile][0];
		shotIDright = preParam[shotsCount-1][0];
	}
	sprintf(fnTemp, "%s_shotID_%d_%d.sgy", fnOut, shotIDleft, shotIDright);
	streamOut = fopen(fnTemp, "wb");
	fwrite(buf3600, sizeof(char), 3600, streamOut);
	posOut = 3600;
	while(posCur < posRight)
	{
		fseeko(streamIn, posCur, SEEK_SET);
		fread(bufTrace, sizeof(char), byteTrace, streamIn);
		fseeko(streamOut, posOut, SEEK_SET);
		fwrite(bufTrace, sizeof(char), byteTrace, streamOut);
		posCur += byteTrace;
		posOut += byteTrace;
		groupTraces += 1;
	}
	cout << "Task " << groups << ": Writing to " << fnTemp << "\n" 
		 << "\t\t | Traces " << groupTraces << " | "
		 << "First ShotID " << shotIDleft << " | "
		 << "Last ShotID " << shotIDright << " | "
		 << endl;
	}
	fclose(streamOut);
	cout << "Well done!" << endl;
	return 0;
}