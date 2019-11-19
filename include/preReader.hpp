#ifndef _PREREADER_H_
#define _PREREADER_H_

#include <cstdio>
#include <iostream>
#include <fstream>

void preReader2D(FILE* streamIn, long long **preParam, int maxTraces,
  		int numSample, int byteSample, int& shotCount, int& tracesCount);
/*This file pre-reads the trace headers of segy files to establish an index
	mapping between FFIDs and data locations in segy file
*/

#endif