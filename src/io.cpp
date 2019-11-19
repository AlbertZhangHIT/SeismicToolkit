#include <cmath>
#include "io.hpp"


void stack_mov_4(char * buf) {
	/*stack_mov_4(char *buf):
	 4 bytes of char buf[4], change the stack memory sequence;
	 used for reading binary file.*/

	char buf_temp;

	buf_temp = buf[0];
	buf[0] = buf[3];
	buf[3] = buf_temp;

	buf_temp = buf[1];
	buf[1] = buf[2];
	buf[2] = buf_temp;
}

void stack_mov_2(char * buf) {
	/*stack_mov_2(char *buf):
	 2 bytes of char buf[2], change the stack memory sequence;
	 used for reading binary file.*/

	char buf_temp;

	buf_temp = buf[0];
	buf[0] = buf[1];
	buf[1] = buf_temp;
}

float singleIBM2float(char *buf) {
	/*Transform 32-bits single precision IBM to float;

	 For 32-bit IBM,
	 V=pow(-1.0,S) * M * pow(16.0,E-64);
	 M= C+F = 0+F = F;
	 Important:
	 # include <math.h>
	 */

	unsigned char b1 = buf[0];
	unsigned char b2 = buf[1];
	unsigned char b3 = buf[2];
	unsigned char b4 = buf[3];

	double s = (b1 & 0x80) >> 7;
	double e = b1 & 0x7f;
	double f = (b2 << 16) + (b3 << 8) + b4;
	double m = f / 16777216.0; /* 2^24 */
//      cout<<" s "<<s<<" e "<<e<<" f "<<f<<" m "<<m<<" ";

	int int_s = (int)s;
	int int_e = (int)e;

	if (s == 0 && e == 0 && f == 0) {
		return 0.0;
	} else {
		return (float) pow(-1.0, int_s) * m * pow(16.0, int_e - 64);
	}
}

int  float2singleIBM(char * buf, float value) {

	//	char * buf = new char[4];//MEMORY LEAKAGE
	/*NOTE: Here the pointer buf pointed memory won't be freed after this function are executed, 
	 unless we use a free synstax. AND it's won't be freed in the main function unless we do 
	 free or the entire program has been executed done.*/
	long sign;
	long exp;
	double mantissa;
	long fraction;

	int int_sign;
	int int_exp;

	sign = (value < 0 ? 1 : 0);
	int_sign = sign;
	value = 1.0 * value * pow(-1.0, int_sign);

	if (value > 0) {

		exp = (long) (log(value) / (4.0 * log(2.0)) + 65);
		int_exp = exp;
		mantissa = value * 1.0 / pow(16.0, int_exp - 64);
		fraction = (long) (mantissa * 16777216.0);

	} else {
		exp = 0;
		fraction = 0;
	}

	buf[0] = (sign << 7) & 0x80 | exp & 0x7f; //NOTE: | stands for "OR";
	buf[1] = (fraction >> 16) & 0xff;
	buf[2] = (fraction >> 8) & 0xff;
	buf[3] = fraction & 0xff;

	return 0;
}