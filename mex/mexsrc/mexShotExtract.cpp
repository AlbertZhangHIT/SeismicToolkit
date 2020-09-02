/*
   Copyright (C) 2018 HIT
   Written by Hao Zhang, Dept. Mathematics.
*/
#include "mex.h"
#include "basic_inc.hpp"
#include "io.hpp"
#include "vhead.hpp"
using namespace std;
extern void _main();

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if(nrhs != 4)
	{
		mexErrMsgTxt("4 inputs required!");
		return;
	}
	if(nlhs > 0)
	{
		mexErrMsgTxt("no output required!");
		return;
	}

	char* fnIn = mxArrayToString(prhs[0]);
	char* fnOut = mxArrayToString(prhs[1]);
	double* ShotIDs = (double *) mxGetData(prhs[2]);
	int numIDs = (int) mxGetScalar(prhs[3]);


	__int64 i64 = 0;
	__int64 i64out = 0;
	int charTraceHeader = 240;
	int charVolumeHeader = 3600;	

	char buf1[1];
	char buf3600[3600];
	char buf4[4];
	char bufTraceHeader[240];	
	int statusError;

	FILE * streamIn;
	FILE * streamOut;

	streamIn = fopen(fnIn, "rb");
	if(streamIn == NULL)
	{
		mexPrintf("Error: Failed in opening file \"%s\". Please check it.\n", fnIn);
		return;
	} else{
		mexPrintf("Opening file \"%s\" \n", fnIn);
		mexPrintf("\t attempting to extract %d shots of data...\n", fnIn, numIDs);
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
		mexPrintf("Volume parsing out: \n", fnIn);
		mexPrintf("Trace Samples: %d \n", nSample);
		mexPrintf("Sample Rate: %.5f s\n", rSample);
		mexPrintf("Sample Byte: %d\n", bSample);
	}

	streamOut = fopen(fnOut, "wb");
	fwrite(buf3600, sizeof(char), charVolumeHeader, streamOut);
	i64out += charVolumeHeader;

	int byteTrace = nSample * bSample;
	char bufTraceData[byteTrace];
	int shotId, i, k; 
	while(!feof(streamIn)) {
		_fseeki64(streamIn, i64, SEEK_SET);
		// check whether reach the end
		fread(buf1, sizeof(char), 1, streamIn);
		if(feof(streamIn))
			break;
		else
			_fseeki64(streamIn, -1, SEEK_CUR);
		// read trace header
		fread(bufTraceHeader, sizeof(char), 240, streamIn);
		i64 += 240;
		// extract shot FFID
		for(i = 0; i < 4; i++)
			buf4[i] = bufTraceHeader[8 + i];
		stack_mov_4(buf4);	
		shotId = *((int *) buf4);	

		// read trace data
		_fseeki64(streamIn, i64, SEEK_SET);
		fread(bufTraceData, sizeof(char), byteTrace, streamIn);
		i64 += byteTrace;

		for(k = 0; k < numIDs; k++)
			if(shotId == (int)ShotIDs[k]){
				// This trace should be output
				_fseeki64(streamOut, i64out, SEEK_SET);
				fwrite(bufTraceHeader, sizeof(char), 240, streamOut);
				i64out += 240;
				_fseeki64(streamOut, i64out, SEEK_SET);
				fwrite(bufTraceData, sizeof(char), byteTrace, streamOut);
				i64out += byteTrace;
			}
	}
	mexPrintf("Writing data to \"%s\"\n", fnOut);
	mexPrintf("Well done!\n");
	fclose(streamIn);
	fclose(streamOut);
}	
