#include <stdio.h>
#include <string.h>

const int toneTable[] = {
	// fingering, tone
	96,62,
	128,62,
	240,64,
	252,63,
	352,61,
	384,61,
	384,62,
	444,60,
	448,60,
	480,65,
	496,63,
	496,64,
	511,62,
	1408,69,
	1440,68,
	1472,67,
	1491,66,
	1504,64,
	1520,64,
	1536,61,
	1648,60,
	1663,61,
	1664,60,
	1728,59,
	1760,58,
	1790,62,
	1792,59,
	1840,58,
	1912,57,
	1920,57,
	1976,56,
	1983,55,
	1984,55,
	2015,54,
	2016,53,
	2019,52,
	2032,52,
	2040,51,
	2044,50,
	2045,49,
	2047,48

};

double Avg(double a, double b, double w){
	return b*w + a*(1.0-w);
}

int main(void)
{
	int i, fg;
	int finger = 0;
	for(fg = 0; fg < 2048; fg ++) // for all fingering patterns...
	{
		finger = fg;
		// fix fingering
		if((finger & 0x600) == 0x200){
			finger = finger ^ 0x600;
		}
		if((finger & 0x00C) == 0x004){
			finger = finger ^ 0x00C;
		}
		if((finger & 0x003) == 0x001){
			finger = finger ^ 0x003;
		}
		
		// search first idx
		int firstIdx = 0;
		for(i=0; i<sizeof(toneTable)/sizeof(int)/2; i++){
			firstIdx = i;
			if(toneTable[2*i] > finger) {
				break;
			}
		}
		double curTone = 0;
		if(firstIdx == 0){
			curTone = toneTable[2*0+1];
		}else{
			double w = (finger - toneTable[2*(firstIdx-1)]) / (double)(1e-6 + toneTable[2*firstIdx] - toneTable[2*(firstIdx-1)]);
			curTone = Avg(toneTable[2*(firstIdx-1)+1], toneTable[2*(firstIdx)+1], w);
		}
		// AVR
//		printf("%d, %f, 0x%02X\n", fg, curTone-48,
//			((int)(curTone-48)<<3) | (int)((curTone-(int)curTone)*8)); // upper 5 bits are tone, lower 3 bits are subtone
		// Main Controller
		printf("0x%02X, ", 
			((int)(curTone-48)<<3) | (int)((curTone-(int)curTone)*8)); // upper 5 bits are tone, lower 3 bits are subtone
		if(fg % 16 == 15) printf("\n");
	}
	
	return 0;
}

