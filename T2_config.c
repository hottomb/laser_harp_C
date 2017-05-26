#include<p30fxxxx.h>

void T2_config(void){

	/* ensure Timer 2 is in reset state */
	T2CON = 0;

	/* reset Timer 2 interrupt flag */
 	IFS0bits.T2IF = 0;
 	
 	/* set Timer2 interrupt priority level to 4 */
	IPC1bits.T2IP = 4;

	/* enable Timer 2 interrupt */
	IEC0bits.T2IE = 1;
 	  	
	/* set Timer 2 period register */
	PR2 = 500; //500-on, 500-off -> T=1000(*50ns)

 	/* select external timer clock/ internal =1/ external =0 */
	T2CONbits.TCS = 0;

 	T2CONbits.TCKPS = 0;		//preskaler=1

/* enable Timer 1 and start the count */ 
	T2CONbits.TON = 1;
}
