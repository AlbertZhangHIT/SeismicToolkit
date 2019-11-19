/*Author Hao Zhang, MathGeo Center, Harbin Institute of Technology*/

/*This file pre-reads the trace headers of segy files to establish an index
	mapping between FFIDs and data locations in segy file
*/
#include "preReader.hpp"
#include "io.hpp"
/*void stack_mov_4(char * buf) {
	//stack_mov_4(char *buf):
	// 4 bytes of char buf[4], change the stack memory sequence;
	// used for reading binary file.

	char buf_temp;

	buf_temp = buf[0];
	buf[0] = buf[3];
	buf[3] = buf_temp;

	buf_temp = buf[1];
	buf[1] = buf[2];
	buf[2] = buf_temp;
}*/

void preReader2D(FILE* streamIn, long long **preParam, int maxTraces,
  		int numSample, int byteSample, int& shotCount, int& tracesCount)
{
	char bufHeader[240];
	char buf4[4];
	char buf1[1];
	int shotTraceCount;

	int shotId_n = 0;
	int shotId_p = 0;
	int numShot;

	int byteTraceData = numSample*byteSample;
	char *bufData;
	bufData = new char[byteTraceData];

	long long fileSize;
	_fseeki64(streamIn, 0, SEEK_END);
	fileSize = _ftelli64(streamIn);
	_fseeki64(streamIn, 0, SEEK_SET);

	long long pos = 0;
	pos += 3600;	// skip volumn header
	long long startPos;


	int i, k;

	numShot = -1;
	tracesCount = 0;

	while( !feof(streamIn) && (pos < fileSize) ) // may add some other condition
	{
		shotTraceCount = 0;
		startPos = pos;
		
		for(k = 0; k < maxTraces; k++)
		{
			
			fread(buf1, sizeof(char), 1, streamIn);
			if (feof(streamIn))
				break;
			else
				_fseeki64(streamIn, -1, SEEK_CUR);
			
			shotId_p = shotId_n;
			_fseeki64(streamIn, pos, SEEK_SET);
			fread(bufHeader, sizeof(char), 240, streamIn); // read trace header
			pos += 240;

			// extract shot id 9-12 byte
			for(i = 0; i < 4; i++)
				buf4[i] = bufHeader[8 + i];
			stack_mov_4(buf4);
			shotId_n = *((int *) buf4);

			if(k != 0 && shotId_p != shotId_n){
				_fseeki64(streamIn, -240, SEEK_CUR);
				pos -= 240;
				break;
			}
			// pass over shot data
			_fseeki64(streamIn, pos, SEEK_SET);
			fread(bufData, sizeof(char), byteTraceData, streamIn);
			pos += byteTraceData;
			shotTraceCount++;
		}

		numShot++;
		tracesCount += shotTraceCount;
		//cout << "Shot " << numShot << " Traces: " << shotTraceCount << endl;
		
		preParam[numShot][0] = (long long)shotId_p;
		preParam[numShot][1] = startPos;
		preParam[numShot][2] = pos;

		//fread(buf1, sizeof(char), 1, streamIn);
	}
	shotCount = numShot + 1;
	_fseeki64(streamIn, 3600, SEEK_SET);
	delete[] bufData;
}
