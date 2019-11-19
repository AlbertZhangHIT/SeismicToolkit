/*
   Copyright (C) 2018 HIT
   Written by Hao Zhang, Dept. Mathematics.
*/

#include "mex.h"
#include "reader.hpp"

using namespace std;
extern void _main();

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if(nrhs != 5)
		mexErrMsgTxt("Number of inputs should be 5");
	if(nlhs != 1)
		mexErrMsgTxt("1 output required");

	int numTraces, numSamples;
	int offset;
	int bit;
	char *fileName;
	FILE* pFile;
	int charTraceHeader = 240;
	int charVolumeHeader = 3600;	

	fileName = mxArrayToString(prhs[0]);
	numTraces = int(mxGetScalar(prhs[1]));
	numSamples = int(mxGetScalar(prhs[2]));
	offset = (int)(mxGetScalar(prhs[3]));
	bit = int(mxGetScalar(prhs[4]));

	pFile = fopen(fileName, "rb");
	if(pFile == NULL){
		mexErrMsgTxt("File did not open.\n");
		return;
	}

	_fseeki64(pFile, 0, SEEK_END);
	__int64 fileSize = _ftelli64(pFile);
	_fseeki64(pFile, 0, SEEK_SET);
	if(fileSize < offset){
		mexPrintf("Error: Offset %d exceeds the file size %d.\n", offset, fileSize);
		fclose(pFile);
		return;
	}

	plhs[0] = mxCreateDoubleMatrix(numSamples, numTraces, mxREAL);	
	double* Data = (double *)mxGetData(plhs[0]);

	//mexPrintf("File %s", fileName);
	// call reader
	int statusError;
	statusError = readCertainTraces(Data, pFile, numTraces, numSamples, offset, bit);
	if(statusError){
		mexErrMsgTxt("Error occurs when reading.\n");
		fclose(pFile);
		return;
	}
	fclose(pFile);
	mxFree(fileName);
}

