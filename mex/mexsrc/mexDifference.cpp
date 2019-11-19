/*
   Copyright (C) 2018 HIT
   Written by Hao Zhang, Dept. Mathematics.
*/
#include "mex.h"
#include "basic_inc.hpp"
#include "vhead.hpp"
#include "io.hpp"
using namespace std;
extern void _main();

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if(nrhs != 4)
		mexErrMsgTxt("Number of inputs should be 4");
	if(nlhs > 1)
		mexErrMsgTxt("No output required");

	char* fnIn1;
	char* fnIn2;
	char* fnOut;
	int maxTraces;
	fnIn1 = mxArrayToString(prhs[0]);
	fnIn2 = mxArrayToString(prhs[1]);
	fnOut = mxArrayToString(prhs[2]);
	maxTraces = int(mxGetScalar(prhs[3]));

	__int64 i64 = 0;
	__int64 i64out = 0;

	char buf1[1];
	char buf3600[3600];
	char bufHeader[240];
	int statusError;

	FILE * streamIn1;
	FILE * streamIn2;
	FILE * streamOut;

	streamIn1 = fopen(fnIn1, "rb");
	if(streamIn1 == NULL)
	{
		mexPrintf("Error: Failed in opening file \"%s\". Please check it.\n", fnIn1);
		return;
	}

	streamIn2 = fopen(fnIn2, "rb");
	if(streamIn2 == NULL)
	{
		mexPrintf("Error: Failed in opening file \"%s\". Please check it.\n", fnIn2);
		return;
	}

	_fseeki64(streamIn1, 0, SEEK_END);
	__int64 fileSize1 = _ftelli64(streamIn1);
	_fseeki64(streamIn1, 0, SEEK_SET);

	_fseeki64(streamIn2, 0, SEEK_END);
	__int64 fileSize2 = _ftelli64(streamIn2);
	_fseeki64(streamIn2, 0, SEEK_SET);	

	if(fileSize1 != fileSize2)
	{
		mexPrintf("Error: Size of \n");
		mexPrintf("\"%s\" and \n", fnIn1);
		mexPrintf("\"%s\" \n", fnIn2);
		mexPrintf("Do not match.\n");	
		return;	
	}

	fread(buf3600, sizeof(char), 3600, streamIn1);
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
		mexPrintf("Segy File: %s volume parsing out: \n", fnIn1);
		mexPrintf("Trace Samples: %d \n", nSample);
		mexPrintf("Sample Rate: %.5f s\n", rSample);
		mexPrintf("Sample Byte: %d\n", bSample);
	}

	streamOut = fopen(fnOut, "wb");
	fwrite(buf3600, sizeof(char), 3600, streamOut);
	i64 += 3600;

	char *buf4;
	buf4 = new char[bSample];
	char *bufTraceData;
	double *bufData1;
	double *bufData2;
	double *bufRes;
	int charsPerTrace = nSample * bSample;
	bufTraceData = new char[charsPerTrace];
	bufData1 = new double[nSample];
	bufData2 = new double[nSample];
	bufRes = new double[nSample];

	for(int i = 0; i < maxTraces; i++)
	{
		fread(buf1, sizeof(char), 1, streamIn1);
		if(feof(streamIn1))
			break;
		else
			_fseeki64(streamIn1, -1, SEEK_CUR);		

		_fseeki64(streamIn1, i64, SEEK_SET);
		fread(bufHeader, sizeof(char), 240, streamIn1);
		i64 += 240;
		fread(bufTraceData, sizeof(char), charsPerTrace, streamIn1);
		for(int j = 0; j < nSample; j++)
		{
			for(int k = 0; k < bSample; k++)
				buf4[k] = bufTraceData[j * bSample + k];
			bufData1[j] = (double)singleIBM2float(buf4);
		}
		_fseeki64(streamIn2, i64, SEEK_SET);
		fread(bufTraceData, sizeof(char), charsPerTrace, streamIn2);
		for(int j = 0; j < nSample; j++)
		{
			for(int k = 0; k < bSample; k++)
				buf4[k] = bufTraceData[j * bSample + k];
			bufData2[j] = (double)singleIBM2float(buf4);
			bufRes[j] = bufData1[j] - bufData2[j];
		}
		i64 += charsPerTrace;

		_fseeki64(streamOut, i64out, SEEK_SET);
		fwrite(bufHeader, sizeof(char), 240, streamOut);
		i64out += 240;
		for(int j = 0; j < nSample; j++)
		{
			float2singleIBM(buf4, (float) bufRes[j]);
			for(int k = 0; k < bSample; k++){
				bufTraceData[j * bSample + k] = buf4[k];
				buf4[k] = 0x00;
			}
		}
		_fseeki64(streamOut, i64out, SEEK_SET);
		fwrite(bufTraceData, sizeof(char), charsPerTrace, streamOut);
		i64out += charsPerTrace;
	}

	delete[] bufTraceData;
	delete[] bufData1;
	delete[] bufData2;
	delete[] bufRes;
	delete[] buf4;

	fclose(streamIn1);
	fclose(streamIn2);
	fclose(streamOut);	
	mxFree(fnIn1);
	mxFree(fnIn2);
	mxFree(fnOut);
}