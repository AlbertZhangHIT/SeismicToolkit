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
	char* fnIn = mxArrayToString(prhs[0]);
	char* fnOut = mxArrayToString(prhs[1]);
	int leftTraceIndex = (int)mxGetScalar(prhs[2]);
	int rightTraceIndex = (int)mxGetScalar(prhs[3]);
	int maxTracePerGather = (int)mxGetScalar(prhs[4]);
	int maxGatherNumber = (int)mxGetScalar(prhs[5]);	

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
	preReader2D(streamIn, preParam, maxTracePerGather, nSample, bSample, gatherCount, tracesCount);

	mexPrintf("--------------- Summary Information -------------\n"); 
	mexPrintf("#Traces    \t\t : %d\n", tracesCount);
	mexPrintf("#Gathers     \t\t : %d\n", gatherCount);

	/*Check whether the input parameters are valid*/
	if (leftTraceIndex > rightTraceIndex)
	{
		mexPrintf("Invalid. leftTraceIndex should not be larger than rightTraceIndex\n");
		return;
	}
	if (leftTraceIndex < 0 | leftTraceIndex > tracesCount | rightTraceIndex < 0 | rightTraceIndex > tracesCount)
	{
		mexPrintf("Max trace index: %d, the input trace indexes are invalid.\n", tracesCount);
		return;
	}	
			
	/*Locate */
	long long leftPos, rightPos, curPos, outPos;

	int charPerTrace = bSample*nSample + 240; 
	char bufTrace[charPerTrace];
	leftPos = (long long)((leftTraceIndex-1)*charPerTrace + 3600);
	rightPos = (long long)(rightTraceIndex*charPerTrace + 3600);
	curPos = leftPos;
	outPos = 3600;

	streamOut = fopen(fnOut, "wb");
	fwrite(buf3600, sizeof(char), 3600, streamOut);

	mexPrintf("Writing traces from %d to %d to file:\n\t \"%s\"\n", leftTraceIndex, rightTraceIndex, fnOut);
	while(curPos < rightPos)
	{
		fseeko(streamIn, curPos, SEEK_SET);
		fread(bufTrace, sizeof(char), charPerTrace, streamIn);
		fseeko(streamOut, outPos, SEEK_SET);
		fwrite(bufTrace, sizeof(char), charPerTrace, streamOut);
		curPos += charPerTrace;
		outPos += charPerTrace;
	}	
	mexPrintf("Well done!\n");
	fclose(streamIn);
	fclose(streamOut);
}