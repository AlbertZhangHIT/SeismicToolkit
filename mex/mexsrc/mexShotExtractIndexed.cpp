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
	if(nrhs != 6)
	{
		mexErrMsgTxt("6 inputs required!");
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
	int leftShotIndex;
	int rightShotIndex;
	int maxShotNumber;

	fnIn = mxArrayToString(prhs[0]);
	fnOut = mxArrayToString(prhs[1]);
	leftShotIndex = int(mxGetScalar(prhs[2]));
	rightShotIndex = int(mxGetScalar(prhs[3]));
	maxTracePerShot = int(mxGetScalar(prhs[4]));
	maxShotNumber = int(mxGetScalar(prhs[5]));

	__int64 i64 = 0;
	int charTraceHeader = 240;
	int charVolumeHeader = 3600;	

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

	int minShotIndex = 1;
	int maxShotIndex = shotsCount;

	if (leftShotIndex > rightShotIndex)
	{
		mexPrintf("Invalid. leftShotIndex should not be larger than rightShotIndex.\n");
		return;
	}
	if (leftShotIndex < minShotIndex)
	{
		mexPrintf("The left shot index is invalid. It will be defaulted as %d.\n", minShotIndex); 
		leftShotIndex = minShotIndex;
	}
	if (rightShotIndex > maxShotIndex)
	{
		mexPrintf("The right shot index is invalid. It will be defaulted as %d.\n", maxShotIndex); 
		rightShotIndex = maxShotIndex;
	}

	/*Locate */
	long long leftPos, rightPos, curPos, outPos;
	int charPerTrace = bSample*nSample + charTraceHeader; 
	char *bufTrace;
	bufTrace = new char[charPerTrace];

	leftPos = preParam[leftShotIndex-1][1];
	rightPos = preParam[rightShotIndex-1][2];

	curPos = leftPos;
	outPos = 3600;

	char fnTemp[1000];
	sprintf(fnTemp, "%s_shotIndex_%d_%d.sgy", fnOut, leftShotIndex, rightShotIndex);
	streamOut = fopen(fnTemp, "wb");
	fwrite(buf3600, sizeof(char), charVolumeHeader, streamOut);
	mexPrintf("Extracting shot indexed: %d -- %d \n.", leftShotIndex, rightShotIndex);
	mexPrintf("Writing to File: %s\n.", fnTemp);
	while(curPos < rightPos)
	{
		_fseeki64(streamIn, (__int64)curPos, SEEK_SET);
		fread(bufTrace, sizeof(char), charPerTrace, streamIn);
		_fseeki64(streamOut, (__int64)outPos, SEEK_SET);
		fwrite(bufTrace, sizeof(char), charPerTrace, streamOut);
		curPos += charPerTrace;
		outPos += charPerTrace;
	}
	mexPrintf("Well done!\n");
	fclose(streamOut);
	delete[] bufTrace;
}	
