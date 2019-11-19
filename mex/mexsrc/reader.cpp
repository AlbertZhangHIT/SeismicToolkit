#include "io.hpp"
#include "reader.hpp"


int readCertainTraces(double* Data, FILE* streamIn, int numTraces, int numSamples, int offset, int bit)
{


	int charsPerTrace = numSamples * bit;

	int startOffSet = offset;

	int check = (int)((startOffSet - 3600)%(numSamples*bit + 240));
	if(check != 0)
		startOffSet -= check;

	char buf1[1];
	char bufHeader[240];
	char *buf4;
	buf4 = new char[bit];
	char ** bufData;
	bufData = new char *[numTraces];
	for(int i = 0; i < numTraces; i++)
		bufData[i] = new char[charsPerTrace];

	int traceCount = 0;
	_fseeki64(streamIn, startOffSet, SEEK_SET);
	for(int i = 0; i < numTraces; i++)
	{
		fread(buf1, sizeof(char), 1, streamIn);
		if(feof(streamIn))
			break;
		else
			_fseeki64(streamIn, -1, SEEK_CUR);
		_fseeki64(streamIn, startOffSet, SEEK_SET);
		fread(bufHeader, sizeof(char), 240, streamIn);
		startOffSet += 240;
		fread(bufData[i], sizeof(char), charsPerTrace, streamIn);
		startOffSet += charsPerTrace;
		traceCount++;
	}

	// Initialize
	for(int i = 0; i < numTraces; i++)
		for(int j = 0; j < numSamples; j++)
			Data[i*numSamples + j] = 0.;

	for(int i = 0; i < traceCount; i++)
	{
		for(int j = 0; j < numSamples; j++)
		{
			for(int k = 0; k < bit; k++)
				buf4[k] = bufData[i][j * bit + k];
			Data[i*numSamples + j] = (double)singleIBM2float(buf4);
		}
	}

	for(int i = 0; i < numTraces; i++)
		delete[] bufData[i];
	delete[] bufData;
	delete[] buf4;

	return 0;
}