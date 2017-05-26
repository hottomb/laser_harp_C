#include <p30fxxxx.h>

void ADC_config_mk2(void){

ADCON1bits.FORM=3;		//data output form: 0-int| 1-sig int| 2-fract| 3-sig fract|
ADCON1bits.SSRC=7;		//convertion trigger source (111-auto convert by Tad)

ADCON2bits.VCFG=0;		//Vref configuration (0)AVdd/ss
ADCON2bits.SMPI=15;		//interrupt per conversion (0)-1int/1con| (1111)-1int/16con

ADCON3bits.SAMC=1;		//Auto sample time bits (0)-0Tad| (11111)-31Tad
ADCON3bits.ADRC=0;		//conv clock src bit (1)-internal RC| (0)-sysclk derived 
ADCON3bits.ADCS=60;		//A/D conversion clock sel bits
//(1+14) *(Tad.min=667ns) -> 100kHz
//Tad=61*50/2ns; 15*Tad -> 43,71kHz

ADCHSbits.CH0NA=0;		//select Vref- for CH0- input
ADCHSbits.CH0SA=10;		//select AN5 for CH0+ input

TRISBbits.TRISB10=1;	//set AN5 as input
ADPCFGbits.PCFG10=0; 	//AN5 in analog mode

IEC0bits.ADIE = 1;		//A/D interrupt on
ADCON1bits.ADON=1;		//enable ADC module

}
