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

	char* fnIn = mxArrayToString(prhs[0]);
	char* fnOut = mxArrayToString(prhs[1]);
	int maxTracePerGather = (int)mxGetScalar(prhs[2]);
	int gatherPerSubfile = (int)mxGetScalar(prhs[3]);
	int maxGatherNumber = (int)mxGetScalar(prhs[4]);
	int byte_shift = (int)mxGetScalar(prhs[5]);

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

	if(fileSize < 0){
		mexPrintf("Error! File size %d\n", fileSize);
		return;
	}

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
	int gatherCount, tracesCount;
	long long **preParam;
	preParam = new long long *[maxGatherNumber];
	for (int i = 0; i < maxGatherNumber; i++)
		preParam[i] = new long long[3];
	preReader2D(streamIn, preParam, maxTracePerGather, nSample, bSample, gatherCount, tracesCount, byte_shift);

	mexPrintf("--------------- Summary Information -------------\n"); 
	mexPrintf("#Traces    \t\t : %d\n", tracesCount);
	mexPrintf("#Gathers     \t\t : %d\n", gatherCount);

	/* Check whether the input parameters are valid*/
	if(gatherPerSubfile > gatherCount)
	{
		mexPrintf("Parameter gatherPerSubfile: %d is larger than number of gathers: %d. Please reset.\n", gatherPerSubfile, gatherCount); 
		return;
	}

	int rema = gatherCount%gatherPerSubfile; 
	int groups;
	if(rema == 0)
		groups = (int)(gatherCount/gatherPerSubfile);
	else
		groups = (int)(gatherCount/gatherPerSubfile) + 1;

	char fnTemp[1000];
	int gatherIDleft, gatherIDright, groupTraces;
	int byteTrace = nSample*bSample + 240;
	char* bufTrace;
	bufTrace = new char[byteTrace];
	long long posLeft, posRight, posCur, posOut;
	for(int i = 1; i < groups; i++)
	{
		groupTraces = 0;
		posLeft = preParam[(i-1)*gatherPerSubfile][1];
		posRight = preParam[i*gatherPerSubfile-1][2];
		posCur = posLeft;
		gatherIDleft = preParam[(i-1)*gatherPerSubfile][0];
		gatherIDright = preParam[i*gatherPerSubfile-1][0];
		sprintf(fnTemp, "%s_gatherID_%d_%d.sgy", fnOut, gatherIDleft, gatherIDright);
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
		mexPrintf("Task %d: Writing to %s \n \t\t | Traces %d |\n", i, fnTemp, groupTraces);
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
		posRight = preParam[gatherCount-1][2];
		gatherIDleft = preParam[(groups-1)*gatherPerSubfile][0];
		gatherIDright = preParam[gatherCount-1][0];
	}
	sprintf(fnTemp, "%s_gatherID_%d_%d.sgy", fnOut, gatherIDleft, gatherIDright);
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
	mexPrintf("Task %d: Writing to %s \n \t\t | Traces %d | \n", groups, fnTemp, groupTraces);
	}
	fclose(streamIn);
	fclose(streamOut);
	mexPrintf("Well done!\n");

	delete[] bufTrace;
}