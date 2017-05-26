#define FCY 20000000UL
#include <p30fxxxx.h>
#define port    LATBbits.LATB3
#define zegar   LATBbits.LATB1
#define strobe  LATBbits.LATB9
#define OEnable LATBbits.LATB5
#define LASen	LATBbits.LATB8

extern volatile int FFTgo;
extern volatile int TimerHold_A;
extern volatile int TimerHold_B;
extern volatile int TimerHold_Atmp;
extern volatile int TimerHold_Btmp;
extern volatile char step[16*4+1];
extern volatile int ADCFinish;
volatile int SegmentSequence=0;
volatile int TimerCount=0;
volatile int migaj=0;
volatile int DarkReturn=0;

void __attribute__((__interrupt__,auto_psv)) _T1Interrupt( void )
{
	/* reset Timer 1 interrupt flag */
 	IFS0bits.T1IF = 0;
	

//	if(TimerCount==127){TimerCount=0;}
	switch(TimerCount){
		case 127: TimerCount=0; break;
		case 128: TimerCount=0; break;
		case 129: TimerCount=0; break; //na wypadek przeoczenia
		}


//	if(TimerCount==0)
//		{TimerHold_A^=1;}

	switch(TimerCount){


		case 106:DarkReturn=0; migaj=1; LASen=1;	break;						//1 segment
		case 115:migaj=0; LASen=0;	break;

		case 116:migaj=1; LASen=1;	break;//on					//2 segment
		case 122:migaj=0; LASen=0;	break;//off			
		
		case 123:migaj=1; LASen=1;	break;						//3 segment
		case 2:migaj=0;  LASen=0;	break;

		case 3:migaj=1;  LASen=1; 	break;						//4 segment	
		case 9:migaj=0;  LASen=0;	break;

		case 10:migaj=1;  LASen=1;	break;						//5 segment
		case 14:migaj=0;  LASen=0;	break;
		
		case 15:migaj=1; LASen=1;	break;						//6 segment
		case 19:migaj=0; LASen=0;	break;

		case 20:migaj=1; LASen=1;	break;						//7 segment
		case 26:migaj=0; LASen=0;	break;
		
		case 27:migaj=1; LASen=1;	break;						//8 segment on	
		case 37:migaj=0; LASen=0;	DarkReturn=1;				//8 segment off /// TUTAJ wszystkie ADC powinny byc juz skonczone, mozna liczyc FFT
				SegmentSequence++;								//SegSeq zmienia sie na kazdym rozpoczeciu ciemnego powrotu
				if(SegmentSequence>=8)							//stad wynik FFT trzeba wrzucic do worka (SegSeq-1)
					{SegmentSequence=0;}
		break;						


		case 0:
			TimerHold_A^=1;		//wskazniki sterowania motorem
		break;
		case 63:
			TimerHold_B^=1;		//wskazniki sterowania motorem
		break;
	}

		 	

		//uruchomienie ADC na poczatku kazdego segmentu
		switch(SegmentSequence){
		case 0: switch(TimerCount){case 106: ADCFinish=0;  FFTgo=1; ADCON1bits.ASAM=1;  break;} break;
		case 1: switch(TimerCount){case 116: ADCFinish=0;  FFTgo=1; ADCON1bits.ASAM=1;  break;} break;
		case 2: switch(TimerCount){case 123: ADCFinish=0;  FFTgo=1; ADCON1bits.ASAM=1;  break;} break;
		case 3: switch(TimerCount){case 4  : ADCFinish=0;  FFTgo=1; ADCON1bits.ASAM=1;  break;} break;
		case 4: switch(TimerCount){case 9 : ADCFinish=0;  FFTgo=1; ADCON1bits.ASAM=1;  break;} break;
		case 5: switch(TimerCount){case 15: ADCFinish=0;  FFTgo=1; ADCON1bits.ASAM=1;  break;} break;
		case 6: switch(TimerCount){case 20 : ADCFinish=0;  FFTgo=1; ADCON1bits.ASAM=1;  break;} break;
		case 7: switch(TimerCount){case 27 : ADCFinish=0;  FFTgo=1; ADCON1bits.ASAM=1;  break;} break;
		}


TimerCount++;

//	if(TimerCount>=63)
//		{TimerHold_B^=1;}

}
