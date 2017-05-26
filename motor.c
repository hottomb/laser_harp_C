#include <p30fxxxx.h>

#define port    LATBbits.LATB3
#define zegar   LATBbits.LATB1
#define strobe  LATBbits.LATB9
#define OEnable LATBbits.LATB5
#define LASen	LATBbits.LATB8

void motor(int lewo, int prawo,const char *step)
{
OEnable=0;
strobe=1;
	int a=3;
	while(a>-1) {zegar=0; port=step[a+(lewo<<2)];a--; zegar=1;}  //4 wpisy do rejestru na narastajacym zboczu
	a=3;
	while(a>-1) {zegar=0; port=step[a+(prawo<<2)];a--; zegar=1;} //4 kolejne wpisy do rejestru na narastajacym zboczu
strobe=0;
OEnable=1;
}
