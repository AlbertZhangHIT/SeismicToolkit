/*
   Copyright (C) 2018 HIT
   Written by Hao Zhang, Dept. Mathematics.
*/

#include "mex.h"
#include "io.hpp"
using namespace std;
extern void _main();

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if(nrhs > 1)
		mexErrMsgTxt("Only segy file path needed.");
	if(nlhs != 6)
		mexErrMsgTxt("6 outputs required.");

	char *fileName;
	FILE* pFile;
	__int64 i64 = 0;
	int charTraceHeader = 240;
	int charVolumeHeader = 3600;

	int samplePerTrace, totalTraces, sampleFormat, bit;
	int tracesPerGather;
	double sampleRate;

	char buf3600[3600];
	char buf2[2];
	char buf1[1];

	fileName = mxArrayToString(prhs[0]);
	pFile = fopen(fileName, "rb");
	if(pFile == NULL){
		mexErrMsgTxt("File did not open.\n");
		return;
	}
	_fseeki64(pFile, 0, SEEK_END);
	__int64 fileSize = _ftelli64(pFile);
	_fseeki64(pFile, 0, SEEK_SET);

	fread(buf3600, sizeof(char), 3600, pFile);
	i64 += 3600;

	// number of traces per gather
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3212 + i];
	stack_mov_2(buf2);	
	tracesPerGather = (int) (*((short *) (buf2)));

	// sample rate
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3216 + i];
	stack_mov_2(buf2);
	short sample = *(short *) (buf2);

	sampleRate = (double)sample * 0.000001;

	// number of samples per trace
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3220 + i];
	stack_mov_2(buf2);
	samplePerTrace = (int) (*((short *) (buf2)));

	// sample format
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3224 + i];
	stack_mov_2(buf2);
	sampleFormat = (int) (*((short *) (buf2)));

	if (sampleFormat == 1 || sampleFormat == 2 || sampleFormat == 4 || sampleFormat == 5)
		// 4-byte IBM floating-point, 4-byte two's complement integer, 
		// 4-byte fixed point with gain (obsolete), % 4-byte IEEE floating-point
		bit = 4;
	else if (sampleFormat == 3) 
		// 2-byte two's complement integer 
		bit = 2;
	else if (sampleFormat == 8)
		// 1-byte two's complement integer
		bit = 1;
	else 
	{
		mexPrintf("Warning: Unrecognized data sample format code %d, default to 1 \n", sampleFormat);
		sampleFormat = 1;
		bit = 4;
	}

	// number of total traces	
	char *buf4;
	buf4 = new char[bit];
	char *bufTrace;
	int charPerTrace = samplePerTrace*bit + charTraceHeader;
	bufTrace = new char[charPerTrace];
	int traceCount = 0;
	
/*	while( !feof(pFile) && i64 < fileSize )
	{
		_fseeki64(pFile, i64, SEEK_SET);	
		fread(bufTrace, sizeof(char), charPerTrace, pFile);
		traceCount++;
		i64 += charPerTrace;
		fread(buf1, sizeof(char), 1, pFile);	
	}
	totalTraces = traceCount;*/
	totalTraces = (int)(fileSize - charVolumeHeader)/charPerTrace;
	fclose(pFile);
	mxFree(fileName);

	mexPrintf("----------Summary Information----------\n");
	mexPrintf("File: %s\n", fileName);
	mexPrintf("# Traces: \t %d\n", totalTraces);
	mexPrintf("# Gather Traces: %d\n", tracesPerGather);
	mexPrintf("# Trace Samples: %d\n", samplePerTrace);
	mexPrintf("Sample Format: \t %d\n", sampleFormat);
	mexPrintf("Sample Interval (s):  %f\n", sampleRate);

	plhs[0] = mxCreateDoubleScalar(totalTraces);
	plhs[1] = mxCreateDoubleScalar(tracesPerGather);
	plhs[2] = mxCreateDoubleScalar(samplePerTrace);
	plhs[3] = mxCreateDoubleScalar(sampleFormat);
	plhs[4] = mxCreateDoubleScalar(sampleRate);
	plhs[5] = mxCreateDoubleScalar(bit);

}