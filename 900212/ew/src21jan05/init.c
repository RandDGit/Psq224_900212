
#define ENABLE_BIT_DEFINITIONS 1


#include "iom16.h"			// Atmel definitions Mega16
#include "inavr.h"			// Atmel register definitions
#include "VSMain.h"			// Visual State definitions

extern unsigned char uc_tprim;		// Primary timer base (TMR0 ovf)
extern unsigned char uc_tprev;		// Prev. value
extern unsigned char uc_tm0preset;	// Timer 0 preset (controls base period)

extern unsigned char USART_RxHead;
extern unsigned char USART_RxTail;
extern unsigned char USART_TxHead;
extern unsigned char USART_TxTail;


void InitOutputs( void)
{
// Once device had been correctly setup (Port I/O, and peripheral usage)
// ensure all ports are correctly configured as per idle state:

	WAL_OP_HEART = 0;		// PWM OC2 output (=Byte)
	WAL_OP_TRO = 1;		// Trigger default = Hi
	WAL_OP_LED = 0;		// LED1 (Lo = ON)
	WAL_OP_DIR = 0;		// Direction towards Home
	WAL_OP_BRK = 0;		// Brake (Motor Enable)

}*/

void InitData( void)
{

	uc_tprim = 0;
	uc_tprev = 0;

	USART_RxHead = USART_RxTail = 0;
	USART_TxHead = USART_TxTail = 0;
}



void InitDevice(void)
{
// __disable_interrupt();

  MCUCR |= 0X0A;    // Falling edge into and int1 and int2
  MCUCSR |= 0x80;
  /* Configure PortB as inputs. */
  // port A 0-4 as inputs 5-7 as outputs
  DDRA = 0xC0;
  DDRB = 0x00;
  DDRC = 0xFF;
  PORTC = ~0;
  PORTA = 0xDF;       //no pullup on PA5
  //InitializeInput();
  PORTB = ~0;
  PORTD = 0x0C;   // pullup on DTMF int input

  //WDTCR = 0x0F;

  // Configure Timer 0 to expire for each ~1 ms.
  // TCCR0 = 0x03;   		// Start the timer by prescale CK/64
  // mTCNT = 193;    		// Load timer to expire for each ~1ms
  TCCR0 = 0x02;   			// Start the timer by prescale CK/8 (2us/pulse)
  // uc_tm0preset = 206;    // Load timer to expire for each ~0.1ms (400 instr/int)
  uc_tm0preset = 8;    		// Load timer to expire for each ~0.5ms (2000 instr/int)
							// 256 - 250 = 6, but allow for latency (16 instr)

  TCNT0 = uc_tm0preset;  	// Load the timer counter register
  //TIMSK |= 0x01;  		// Enable Timer0 overflow interrupt
  TIMSK |= (1<<TOIE0);  	// Enable Timer0 overflow interrupt

  MCUCR |= 0X0A;    		// Falling edge int0, int1 and int2
				// However, INT0, INT1, INT2 not used

  TCCR1B |= 0x86; // noise cancelling on external clk on T1 falling edge
  GICR |= 0xC0;//(1<<INT1);
  //GICR |= (1<<INT0); 
  //GICR |= (1<<INT2);
  
  UCSRB |= (1<<UDRIE);                    /* Enable UDRE interrupt */  
  // __enable_interrupt();
  
}



