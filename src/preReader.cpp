/*Author Hao Zhang, MathGeo Center, Harbin Institute of Technology*/

/*This file pre-reads the trace headers of segy files to establish an index
	mapping between gathers and data locations in segy file
*/
#include "preReader.hpp"
#include "io.hpp"
using namespace std;

void preReader2D(FILE* streamIn, long long **preParam, int maxTraces,
  		int numSample, int byteSample, int& gatherCount, int& tracesCount, int byte_shift)
{
	char bufHeader[240];
	char buf4[4];
	char buf1[1];
	int gatherTraceCount;

	int ffid_n = 0;
	int ffid_p = 0;
	int numGather;

	int byteTraceData = numSample*byteSample;
	char bufData[byteTraceData];

	long long fileSize;
	fseeko(streamIn, 0, SEEK_END);
	fileSize = ftello64(streamIn);
	fseeko(streamIn, 0, SEEK_SET);

	long long pos = 0;
	pos += 3600;	// skip volumn header
	long long startPos;


	int i, k;

	numGather = -1;
	tracesCount = 0;

	while( !feof(streamIn) && (pos < fileSize) ) // may add some other condition
	{
		gatherTraceCount = 0;
		startPos = pos;
		
		for(k = 0; k < maxTraces; k++)
		{
			
			fread(buf1, sizeof(char), 1, streamIn);
			if (feof(streamIn))
				break;
			else
				fseeko(streamIn, -1, SEEK_CUR);
			
			ffid_p = ffid_n;
			fseeko(streamIn, pos, SEEK_SET);
			fread(bufHeader, sizeof(char), 240, streamIn); // read trace header
			pos += 240;

			// extract ffid 9-12 byte, byte_shift=8; SP 17-21 bytes, byte_shfit=16
			for(i = 0; i < 4; i++)
				buf4[i] = bufHeader[byte_shift + i]; 
			stack_mov_4(buf4);
			ffid_n = *((int *) buf4);

			if(k != 0 && ffid_p != ffid_n){
				fseeko(streamIn, -240, SEEK_CUR);
				pos -= 240;
				break;
			}
			// pass over shot data
			fseeko(streamIn, pos, SEEK_SET);
			fread(bufData, sizeof(char), byteTraceData, streamIn);
			pos += byteTraceData;
			gatherTraceCount++;
		}

		numGather++;
		tracesCount += gatherTraceCount;

		preParam[numGather][0] = (long long)numGather;
		preParam[numGather][1] = startPos;
		preParam[numGather][2] = pos;

		//fread(buf1, sizeof(char), 1, streamIn);
	}
	gatherCount = numGather + 1;
	fseeko(streamIn, 3600, SEEK_SET);
}
