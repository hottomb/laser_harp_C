#define FCY 20000000UL
#include <p30fxxxx.h>
#define LASen	LATBbits.LATB8

extern  volatile int SegmentSequence;
extern volatile int migaj;
volatile int parity=0;

void __attribute__((__interrupt__,auto_psv)) _T2Interrupt( void )
//void _ISR _T2Interrupt( void )
{
 	IFS0bits.T2IF = 0;



	switch(migaj){
		case 0: break;
		case 1:
		LASen^=1;
		break;
	}
//	if(parity%2 != 0){LASen^=1;parity=0;}
}
