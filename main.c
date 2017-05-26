#define FCY 20000000UL 		//10MHz*8PLL/4
#include <p30f3014.h>
#include <dsp.h>
#include <adc12.h>
#include <delay.h>
#include <libpic30.h>
#include "fft.h"
#include <string.h>

//INDICATORS
#define LED1 LATDbits.LATD1
#define LED9 LATDbits.LATD9
#define LED11 LATAbits.LATA11

//EXTERNS
extern volatile fractional samples[256];
extern volatile int wantADC;
extern volatile int ADCFinish;
extern volatile int ind;
extern volatile int migaj;
extern volatile int DarkReturn;
extern volatile int TimerCount;				
extern volatile int SegmentSequence;
extern volatile int parity;
//^^^^^^^^^^^^^^^^^^^FFT^^^^^^^^^^^^^^^^^^^^^^^^^
volatile fractcomplex sigCmpx[FFT_BLOCK_LENGTH]
    __attribute__((space(ymemory),far,aligned(FFT_BLOCK_LENGTH*2*2)))={0};

#ifndef FFTTWIDCOEFFS_IN_PROGMEM
   fractcomplex twiddleFactors[FFT_BLOCK_LENGTH/2]
   __attribute__ ((section (".xbss, bss, xmemory"), aligned (FFT_BLOCK_LENGTH*2)));
#else
   extern const fractcomplex twiddleFactors[FFT_BLOCK_LENGTH/2]
   __attribute__ ((space(auto_psv), aligned (FFT_BLOCK_LENGTH*2)));
#endif

int	peakFrequencyBin = 0;
unsigned long peakFrequency = 0;
//^^^^^^^^^^^^^^^^^^^FFT^^^^^^^^^^^^^^^^^^^^^^^^^

volatile char *step[16*4+1] = {
"0000000100100011010001010110011110001001101010111100110111101111"}; //4-BIT MOTOR DRIVER ADDRESSING WORD
volatile int TimerHold_A=1;
volatile int TimerHold_B=1;
volatile int TimerHold_Atmp=1;
volatile int TimerHold_Btmp=1;
volatile int FFTgo=0;
int licz=0;
volatile unsigned long average[8];

void motor(int,int,const char*);
void ADC_config_mk2(void);
void T1_config(void);
void T2_config(void);
//************************************************************************************
//************************************************************************************
//************************************************************************************
int main(void)
{
memset(average,0,sizeof(unsigned long)*8);

//||||||||||||||||||||||||||||||FFT vars. init|||
	int i = 0;
	fractional *p_real = &sigCmpx[0].real ;
	fractcomplex *p_cmpx = &sigCmpx[0] ;
	#ifndef FFTTWIDCOEFFS_IN_PROGMEM				
		TwidFactorInit (LOG2_BLOCK_LENGTH, &twiddleFactors[0], 0);
	#endif
//||||||||||||||||||||||||||||||FFT vars. init|||


//********************************INITIAL VALUES *************************************
		ADPCFG = 0xFFFF;
		TRISF  = 0x0000;
        TRISD  = 0x0000;
        TRISC  = 0x0000;
		TRISB  = 0x0000;
		TRISA  = 0x0000;
	int FFTfresh=0;
	unsigned long av=0;

//*******************************END OF INITIAL VALUES *******************************
//*******************************ADC CONFIGURATION************************************
		ADC_config_mk2();
		T1_config();
		T2_config();
//*******************************END OF ADC CONFIGURATION*****************************
while(1){

//*******************************MOTOR CONTROL****************************************
if(TimerHold_A != TimerHold_Atmp)
	{
	TimerHold_Atmp=TimerHold_A; 
	motor(15,0,step[0]);
	}
if(TimerHold_B != TimerHold_Btmp)
	{
	TimerHold_Btmp=TimerHold_B; 
	motor(0,15,step[0]);
	}
//*******************************MOTOR CONTROL****************************************

//******* MONITOR ********************************************************************
/*
int count;
for(count=0;count<256;count++)
{
(samples[count])=sigCmpx[count].real;
}
*/
//ustaw breakepoint i obserwuj samples[]
//******* MONITOR ********************************************************************

//*************************************FFT********************************************
														//obliczenia FFT po ADC i w ciemnym powrocie
														//FFTgo wyznacza poczatek zbierania probek na poczatku kazdego segmentu
														//dodatkowo probkowanie musi sie skonczyc
int count;												// a TimerCount doliczyc do ciemnego powrotu
switch(FFTgo*ADCFinish*DarkReturn)						//if statement zabiera bardzo duzo czasu - rozsynchronizowuje skaner jesli jest zamiast switch
	{		
	case 0: break;
	case 1:

//******* MONITOR ********************************************************************
for(count=0;count<256;count++)
{
(samples[count])=sigCmpx[count].real;
}
//******* MONITOR ********************************************************************



		for ( i = 0; i < FFT_BLOCK_LENGTH; i++ )
		{
			*p_real = *p_real >>1 ;		
			*p_real++;	
		}					
		p_real = &sigCmpx[(FFT_BLOCK_LENGTH/2)-1].real ;
		p_cmpx = &sigCmpx[FFT_BLOCK_LENGTH-1] ;
	
		for ( i = FFT_BLOCK_LENGTH; i > 0; i-- )
		{			
			(*p_cmpx).real = (*p_real--);
			(*p_cmpx--).imag = 0x0000;
		}
	#ifndef FFTTWIDCOEFFS_IN_PROGMEM
		FFTComplexIP (LOG2_BLOCK_LENGTH, &sigCmpx[0], &twiddleFactors[0], COEFFS_IN_DATA);
	#else
		FFTComplexIP (LOG2_BLOCK_LENGTH, &sigCmpx[0], (fractcomplex *) __builtin_psvoffset(&twiddleFactors[0]), (int) __builtin_psvpage(&twiddleFactors[0]));
	#endif
		BitReverseComplex (LOG2_BLOCK_LENGTH, &sigCmpx[0]);

		sigCmpx[0].real = 0;	//FFT result detrend
		sigCmpx[0].imag = 0;

		SquareMagnitudeCplx(FFT_BLOCK_LENGTH, &sigCmpx[0], &sigCmpx[0].real);
		VectorMax(FFT_BLOCK_LENGTH/2, &sigCmpx[0].real, &peakFrequencyBin);
		peakFrequency = peakFrequencyBin*(SAMPLING_RATE/FFT_BLOCK_LENGTH);
//******* MONITOR ********************************************************************
/*
	int ra;						//get FFT result to samples[]
	for(ra=0;ra<256;ra++){
	samples[ra]=sigCmpx[ra].real;
	}
*/
//******* MONITOR ********************************************************************
	FFTgo=0;
	FFTfresh=1;
//*************************************END OF TRANSFOTM*******************************

	break;
	}//eof switch


//*******AVERAGE RESULT **************************************************************
if(FFTfresh)	
	{
	average[SegmentSequence]=peakFrequency;
	FFTfresh=0;
	licz++;
	}
if(licz==16)	{
	average[SegmentSequence]=average[SegmentSequence]/2;
	licz=0;
	av=0;
LATDbits.LATD1=0;
	}

//*******AVERAGE RESULT **************************************************************
int dol=100;
int gora=5000;
int rq;
for(rq=0;rq<8;rq++){
	switch(rq){
	case 0: if(average[rq]>dol && average[rq]<gora){
		LATDbits.LATD9 = 1;

	case 1: if(average[rq]>dol && average[rq]<gora){
		LATAbits.LATA11 = 1;
		} 	break;
	case 2:	if(average[rq]>dol && average[rq]<gora){
		LATDbits.LATD3 = 1;
		}	break;
	case 3:	if(average[rq]>dol && average[rq]<gora){
		LATDbits.LATD2 = 1;
		}	break;
	case 4:	if(average[rq]>dol && average[rq]<gora){
		LATFbits.LATF6 = 1;
		}	break;
	case 5:	if(average[rq]>dol && average[rq]<gora){
		LATCbits.LATC14 = 1;
		}	break;
	case 6:	if(average[rq]>dol && average[rq]<gora){
		LATFbits.LATF2 = 1;
		}	break;
	case 7:	if(average[rq]>dol && average[rq]<gora){
		LATFbits.LATF1 = 1;
		}	break;
}
}
	//INDICATORS
	LATDbits.LATD9 = 0;
	LATAbits.LATA11 = 0;
	LATDbits.LATD3 = 0;
	LATDbits.LATD2 = 0;
	LATFbits.LATF6 = 0;
	LATCbits.LATC14 = 0;
	LATFbits.LATF2 = 0;


}//eof while(1)
}//eof main
//^^^^^^^^^^ FUNCTIONS ^^^^^^^^^^^^^^^^^^^^^^^^^^



