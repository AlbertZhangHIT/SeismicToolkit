/*
	Copyright (C) 2017 HIT
	Written by Hao Zhang
*/
#include "basic_inc.hpp"
#include "io.hpp"
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
	mIter = opts.find("-outfile"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second);	ss >> fnOut;}

	int ShotID = 3;
	mIter = opts.find("-ShotID"); assert(mIter != opts.end());
	{istringstream ss((*mIter).second); ss >> ShotID;}


 
	long i64 = 0;
	long i64out = 0;
	long fileSize;
	char buf1[1];
	char buf4[4];
	char bufTraceHeader[240];
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

	fseek(streamIn, 0, SEEK_END);
	fileSize = ftell(streamIn);
	fseek(streamIn, 0, SEEK_SET);

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

	streamOut = fopen(fnOut, "wb");
	fwrite(buf3600, sizeof(char), charVolumeHeader, streamOut);
	i64out += charVolumeHeader;

	int byteTrace = nSample * bSample;
	char bufTraceData[byteTrace];
	int shotId, i; 
	while(!feof(streamIn) && i64<fileSize) {
		fseek(streamIn, i64, SEEK_SET);
		// check whether reach the end
		fread(buf1, sizeof(char), 1, streamIn);
		if(feof(streamIn))
			break;
		else
			fseek(streamIn, -1, SEEK_CUR);
		// read trace header
		fread(bufTraceHeader, sizeof(char), 240, streamIn);
		i64 += 240;
		// extract shot FFID
		for(i = 0; i < 4; i++)
			buf4[i] = bufTraceHeader[8 + i];
		stack_mov_4(buf4);	
		shotId = *((int *) buf4);	

		// read trace data
		fseek(streamIn, i64, SEEK_SET);
		fread(bufTraceData, sizeof(char), byteTrace, streamIn);
		i64 += byteTrace;

		if(shotId == ShotID){
			// This trace should be output
			fseek(streamOut, i64out, SEEK_SET);
			fwrite(bufTraceHeader, sizeof(char), 240, streamOut);
			i64out += 240;
			fseek(streamOut, i64out, SEEK_SET);
			fwrite(bufTraceData, sizeof(char), byteTrace, streamOut);
			i64out += byteTrace;
		}
	}

	cout << "Well done!" << endl;
	fclose(streamIn);
	fclose(streamOut);

	return 0;
}