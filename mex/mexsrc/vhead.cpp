#include "vhead.hpp"
#include "io.hpp"
#include <iostream>
using namespace std;

/*void stack_mov_2(char * buf) {
	//stack_mov_2(char *buf):
	// 2 bytes of char buf[2], change the stack memory sequence;
	// used for reading binary file.
	// 

	char buf_temp;

	buf_temp = buf[0];
	buf[0] = buf[1];
	buf[1] = buf_temp;
}*/

int vheadChecker(char* buf3600)
{
	char buf2[2];
	char buf4[4];	
	int statusError = 0;

	// sample rate
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3216 + i];
	stack_mov_2(buf2);
	short sample = *(short *) (buf2);
	if (sample < 0 || sample > 10000)
	{
		statusError = 1;
		cout << "The sample interval is " << sample << endl;
		return statusError;
	}

	// number of samples per trace
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3220 + i];
	stack_mov_2(buf2);
	int sample_num = (int) (*((short *) (buf2)));
	if (sample_num < 0 || sample_num > 10000)
	{
		statusError = 1;
		cout << "The sample number is " << sample_num << endl;
		return statusError;
	}	
	
	// sample byte
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3224 + i];
	stack_mov_2(buf2);
	int format = (int) (*((short *) (buf2)));
	if (format < 0 || format > 10)
	{
		statusError = 1;
		cout << "The format code is " << format << endl;
		return statusError;
	}

	return statusError;	
}


int vheadReader(char* buf3600, int &sample_byte, int &sample_num, double &sample_rate)
{
	char buf2[2];
	char buf4[4];
	int headerChar = 240;
	int statusError = 0;

	// sample rate
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3216 + i];
	stack_mov_2(buf2);
	short sample = *(short *) (buf2);

	sample_rate = (double)sample * 0.000001;

	// number of samples per trace
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3220 + i];
	stack_mov_2(buf2);
	sample_num = (int) (*((short *) (buf2)));

	// sample byte
	for(int i = 0; i < 2; i++)
		buf2[i] = buf3600[3224 + i];
	stack_mov_2(buf2);
	int format = (int) (*((short *) (buf2)));
	if (format == 1 || format == 2 || format == 4 || format == 5)
		// 4-byte IBM floating-point, 4-byte two's complement integer, 
		// 4-byte fixed point with gain (obsolete), % 4-byte IEEE floating-point
		sample_byte = 4;
	else if (format == 3) 
		// 2-byte two's complement integer 
		sample_byte = 2;
	else if (format == 8)
		// 1-byte two's complement integer
		sample_byte = 1;
	else 
	{
		cout << "Unrecognized data sample format code " << format << endl;	
		statusError = 1;
	}

	return statusError;
}