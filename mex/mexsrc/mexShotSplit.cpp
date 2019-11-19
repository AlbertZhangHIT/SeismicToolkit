/*
   Copyright (C) 2018 HIT
   Written by Hao Zhang, Dept. Mathematics.
*/
#include "mex.h"
#include "basic_inc.hpp"
#include "preReader.hpp"
#include "vhead.hpp"
using namespace std;
extern void _main();

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if(nrhs != 5)
	{
		mexErrMsgTxt("5 inputs required!");
		return;
	}
	if(nlhs > 0)
	{
		mexErrMsgTxt("no output required!");
		return;
	}
	char* fnIn;
	char* fnOut;
	int maxTracePerShot;
	int shotPerSubfile;
	int maxShotNumber;

	fnIn = mxArrayToString(prhs[0]);
	fnOut = mxArrayToString(prhs[1]);
	maxTracePerShot = int(mxGetScalar(prhs[2]));
	shotPerSubfile = int(mxGetScalar(prhs[3]));
	maxShotNumber = int(mxGetScalar(prhs[4]));

	__int64 i64 = 0;

	char buf1[1];
	char buf3600[3600];
	int statusError;

	FILE * streamIn;
	FILE * streamOut;

	streamIn = fopen(fnIn, "rb");
	if(streamIn == NULL)
	{
		mexPrintf("Error: Failed in opening file \"%s\". Please check it.\n", fnIn);
		return;
	}

	_fseeki64(streamIn, 0, SEEK_END);
	__int64 fileSize = _ftelli64(streamIn);
	_fseeki64(streamIn, 0, SEEK_SET);

	fread(buf3600, sizeof(char), 3600, streamIn);
	i64 += 3600;

	statusError = vheadChecker(buf3600);
	if (statusError)
	{
		mexPrintf("Failure...The input file may not have a volume text.\n");
		return;
	}

	/* segy file volume header parsing*/
	int nSample, bSample;
	double rSample;
	statusError = vheadReader(buf3600, bSample, nSample, rSample);
	if(statusError)
	{
		mexPrintf("Error: Failed in parsing volume header.\n");
		return;
	} else{
		mexPrintf("Segy File: %s volume parsing out: \n", fnIn);
		mexPrintf("Trace Samples: %d \n", nSample);
		mexPrintf("Sample Rate: %.5f s\n", rSample);
		mexPrintf("Sample Byte: %d\n", bSample);
	}

	/* Get summary information of the input segy file */
	int shotsCount, tracesCount;
	long long **preParam;
	preParam = new long long *[maxShotNumber];
	for (int i = 0; i < maxShotNumber; i++)
		preParam[i] = new long long[3];
	preReader2D(streamIn, preParam, maxTracePerShot, nSample, bSample, shotsCount, tracesCount);

	mexPrintf("--------------- Summary Information -------------\n"); 
	mexPrintf("#Traces    \t\t : %d\n", tracesCount);
	mexPrintf("#Shots     \t\t : %d\n", shotsCount);
	mexPrintf("First FFID \t\t : %d\n", preParam[0][0]);
	mexPrintf("Last  FFID \t\t : %d\n", preParam[shotsCount-1][0]);

	/* Check whether the input parameters are valid*/
	if(shotPerSubfile > shotsCount)
	{
		mexPrintf("Parameter shotPerSubfile: %d is larger than number of shots: %d. Please reset.\n", shotPerSubfile, shotsCount); 
		return;
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
	char* bufTrace;
	bufTrace = new char[byteTrace];
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
			_fseeki64(streamIn, posCur, SEEK_SET);
			fread(bufTrace, sizeof(char), byteTrace, streamIn);
			_fseeki64(streamOut, posOut, SEEK_SET);
			fwrite(bufTrace, sizeof(char), byteTrace, streamOut);
			posCur += byteTrace;
			posOut += byteTrace;
			groupTraces += 1;
		}
		mexPrintf("Task %d: Writing to %s \n \t\t | Traces %d | First ShotID %d | Last ShotID %d | \n", i, fnTemp, groupTraces, shotIDleft, shotIDright);
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
		_fseeki64(streamIn, posCur, SEEK_SET);
		fread(bufTrace, sizeof(char), byteTrace, streamIn);
		_fseeki64(streamOut, posOut, SEEK_SET);
		fwrite(bufTrace, sizeof(char), byteTrace, streamOut);
		posCur += byteTrace;
		posOut += byteTrace;
		groupTraces += 1;
	}
	mexPrintf("Task %d: Writing to %s \n \t\t | Traces %d | First ShotID %d | Last ShotID %d | \n", groups, fnTemp, groupTraces, shotIDleft, shotIDright);
	}
	fclose(streamOut);
	mexPrintf("Well done!\n");

	delete[] bufTrace;
}