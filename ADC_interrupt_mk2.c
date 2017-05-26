#include <p30f3014.h>
#include <dsp.h>
#include <adc12.h>
#include "fft.h"

extern volatile fractcomplex sigCmpx[FFT_BLOCK_LENGTH]
    __attribute__((space(ymemory),far,aligned(FFT_BLOCK_LENGTH*2*2)));

volatile fractional samples[256]={0};
volatile int ADCFinish=0;
volatile int wantADC=0;
volatile int ind=0;

int VOL=FFT_BLOCK_LENGTH>>4;
int i,b;

void __attribute__((__interrupt__,auto_psv)) _ADCInterrupt(void)
{
	
	b=ind<<4;		//multiplication times 16
	for(i=0;i<16;i++)
		{
		sigCmpx[b+i].real=ReadADC12(i);
		}

	ind++;			//counts from 0 to 8 (8 passes!)
	if(ind>=VOL)	//terminating statement
		{
		ind=0;
		ADCON1bits.ASAM=0;
		ADCFinish=1;
		wantADC=0;
		}

	IFS0bits.ADIF = 0;

}