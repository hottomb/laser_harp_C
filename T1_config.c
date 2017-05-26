#include<p30fxxxx.h>

void T1_config(void){

	/* ensure Timer 1 is in reset state */
	T1CON = 0;

	/* reset Timer 1 interrupt flag */
 	IFS0bits.T1IF = 0;
 	
 	/* set Timer1 interrupt priority level to 4 */
	IPC0bits.T1IP = 1;

	/* enable Timer 1 interrupt */
 	IEC0bits.T1IE = 1;
 	  	
	/* set Timer 1 period register */
	PR1 = 6250; //5000 * 50ns

 	/* select external timer clock/ internal =1/ external =0 *///
	T1CONbits.TCS = 0;
 	
 	T1CONbits.TCKPS = 0;//preskaler 1
	/* enable Timer 1 and start the count */ 
	T1CONbits.TON = 1;
}
