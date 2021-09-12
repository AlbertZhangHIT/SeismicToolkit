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

	int maxTracePerGather = 10000;
	mIter = opts.find("-maxTracePerGather");	assert(mIter != opts.end());
	{istringstream ss((*mIter).second);	ss >> maxTracePerGather;}

	int gatherPerSubfile = 3;
	mIter = opts.find("-gatherPerSubfile"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> gatherPerSubfile;}

	int maxGatherNumber = 10000;
	mIter = opts.find("-maxGatherNumber"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> maxGatherNumber;}
 
 	int byte_shift = 0;
 	mIter = opts.find("-byteshift");
 	if(mIter != opts.end()){
 		istringstream ss((*mIter).second); ss >> maxGatherNumber;
 	} else{
 		cout << "No byte_shift assigned ... default to byte_shift=8 (8=FFID/16=SP)." << endl;
 		byte_shift = 8;
 	}

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
	int gathersCount, tracesCount;
	long long **preParam;
	preParam = new long long *[maxGatherNumber];
	for (int i = 0; i < maxGatherNumber; i++)
		preParam[i] = new long long[3];
	preReader2D(streamIn, preParam, maxTracePerGather, nSample, bSample, gathersCount, tracesCount, byte_shift);

	cout << "--------------- Summary Information -------------\n" 
		 << "#Traces    \t\t : " << tracesCount << "\n"
		 << "#Gathers     \t\t : " << gathersCount << "\n"
		 << endl;

	/* Check whether the input parameters are valid*/
	if(gatherPerSubfile > gathersCount)
	{
		cout << "Parameter gatherPerSubfile: " << gatherPerSubfile << " is larger than number of gathers: " 
			 << gathersCount << "\n"
			 << "Please reset." 
			 << endl;
		return 1;
	}

	int rema = gathersCount%gatherPerSubfile; 
	int groups;
	if(rema == 0)
		groups = (int)(gathersCount/gatherPerSubfile);
	else
		groups = (int)(gathersCount/gatherPerSubfile) + 1;

	char fnTemp[1000];
	int gatherIDleft, gatherIDright, groupTraces;
	int byteTrace = nSample*bSample + 240;
	char bufTrace[byteTrace];
	long long posLeft, posRight, posCur, posOut;
	for(int i = 1; i < groups; i++)
	{
		groupTraces = 0;
		posLeft = preParam[(i-1)*gatherPerSubfile][1];
		posRight = preParam[i*gatherPerSubfile-1][2];
		posCur = posLeft;
		gatherIDleft = preParam[(i-1)*gatherPerSubfile][0];
		gatherIDright = preParam[i*gatherPerSubfile-1][0];
		if(gatherIDleft != gatherIDright){
			sprintf(fnTemp, "%s_gatherID_%d_%d.sgy", fnOut, gatherIDleft, gatherIDright);
		} else{
			sprintf(fnTemp, "%s_gatherID_%d.sgy", fnOut, gatherIDleft);
		}
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
			 << endl;
		fclose(streamOut);
	}
	// write the last group
	{
	groupTraces = 0;
	if (rema == 0)
	{
		posLeft = preParam[(groups-1)*gatherPerSubfile][1];
		posRight = preParam[groups*gatherPerSubfile-1][2];
		gatherIDleft = preParam[(groups-1)*gatherPerSubfile][0];
		gatherIDright = preParam[groups*gatherPerSubfile-1][0];
	} else{
		posLeft = preParam[(groups-1)*gatherPerSubfile][1];
		posRight = preParam[gathersCount-1][2];
		gatherIDleft = preParam[(groups-1)*gatherPerSubfile][0];
		gatherIDright = preParam[gathersCount-1][0];
	}
	sprintf(fnTemp, "%s_gatherID_%d_%d.sgy", fnOut, gatherIDleft, gatherIDright);
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
		 << endl;
	}

	cout << "Well done!" << endl;
	fclose(streamIn);
	fclose(streamOut);
	
	return 0;
}