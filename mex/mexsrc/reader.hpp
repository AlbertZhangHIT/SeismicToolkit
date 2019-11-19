#ifndef _READER_H_
#define _READER_H_

#include <iostream>
#include <fstream>
using namespace std;

int readCertainTraces(double* Data, FILE* streamIn, int numTraces, int numSamples, int offset, int bit);

#endif