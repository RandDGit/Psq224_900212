/*****************************************************************************
* IAR visualSTATE Device Setup File. Specific to Atmel ATmega16 processor
* Provide handle for each of the processor interrupt function;
* with corresponding event trigger as appropriate.
*****************************************************************************/

/* *** include directives *** */

#include <iom16.h>
#include <inavr.h>
#include "VSMain.h"

#include "defs.h"			// Application (GENERAL) defs
#include "globals.h"		// Application global data/variables


/* ATMEGA16 Interrupts */
/*
#define    INT0_vect            (0x04)
#define    INT1_vect            (0x08)
#define    TIMER2_COMP_vect     (0x0C)
#define    TIMER2_OVF_vect      (0x10)
#define    TIMER1_CAPT_vect     (0x14)
#define    TIMER1_COMPA_vect    (0x18)
#define    TIMER1_COMPB_vect    (0x1C)
#define    TIMER1_OVF_vect      (0x20)
#define    TIMER0_OVF_vect      (0x24)
#define    SPI_STC_vect         (0x28)
#define    USART_RXC_vect       (0x2C)
#define    USART_UDRE_vect      (0x30)
#define    USART_TXC_vect       (0x34)
#define    ADC_vect             (0x38)
#define    EE_RDY_vect          (0x3C)
#define    ANA_COMP_vect        (0x40)
#define    TWI_vect             (0x44)
#define    INT2_vect            (0x48)
#define    TIMER0_COMP_vect     (0x4C)
#define    SPM_RDY_vect         (0x50)
*/

#pragma vector=INT0_vect
__interrupt void INT0_interrupt( void ) 
{
}
#pragma vector=INT1_vect
__interrupt void INT1_interrupt( void ) 
{
}
#pragma vector=TIMER2_COMP_vect
__interrupt void TIMER2_COMP_interrupt( void ) 
{
}
#pragma vector=TIMER2_OVF_vect
__interrupt void TIMER2_OVF_interrupt( void ) 
{
}
#pragma vector=TIMER1_CAPT_vect
__interrupt void TIMER1_CAPT_interrupt( void ) 
{
}
#pragma vector=TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA_interrupt( void ) 
{
}
#pragma vector=TIMER1_COMPB_vect
__interrupt void TIMER1_COMPB_interrupt( void ) 
{
}
#pragma vector=TIMER1_OVF_vect
__interrupt void TIMER1_OVF_interrupt( void ) 
{
}
#pragma vector=TIMER0_OVF_vect
__interrupt void TIMER0_OVF_interrupt( void ) 
{
	// reload overflow preset to determine overflow period: TCNT0 = mTCNT;
	TCNT0 = uc_tm0preset;
	uc_tprim++;
}
#pragma vector=SPI_STC_vect
__interrupt void SPI_STC_interrupt( void ) 
{
}
#pragma vector=USART_RXC_vect
__interrupt void USART_RXC_interrupt( void ) 
{
// following status & character in sequence must be used for UART Rx operation:
    uc_rx_stata = UCSRA;
    uc_rx_statb = UCSRB;
    uc_rx_chbyte = UDR;		// Read UART data register

	// Clear line_idle monitor (i.e. block transmit)
    uc_comm_idle = 0;
	uc_comm_flag &= ~WAL_COMM_IDLE;
    
    // ATMEL Framing error 0x10 or Overrun 0x08
    if( uc_rx_stata & 0x18)
    {
		// ignore character (noise)
		uc_comm_flag |= WAL_COMM_DERR;
    }
    else
    {
		uc_rxbuff[uc_rxwrptr].status = uc_rx_statb;	/* uc_comm_9set = */
		uc_rxbuff[uc_rxwrptr].charin = uc_rx_chbyte;/* uc_rx_chrecv = */
		uc_rxwrptr++;
		uc_rxwrptr &= (WAL_MAX_RXBUFF-1);
	}
}
#pragma vector=USART_UDRE_vect
__interrupt void USART_UDRE_interrupt( void ) 
{
}
#pragma vector=USART_TXC_vect
__interrupt void USART_TXC_interrupt( void ) 
{
	uc_tx_chsent = 1;	// used rather than Add_Event directly to minimize overhead

	// character should now be in UDR as 485 RX always on
	// therefore RXC bit should now be set, so clear by reading UDR
	// and then check character in == character out 
	if( UCSRA & 0x80)
	{
		// character received
		uc_comm_comp = UDR;
		if( uc_tx_chout != uc_comm_comp)
        {
			uc_comm_flag |= WAL_COMM_MISM;
        }
	}
	else
	{
		uc_comm_flag |= WAL_COMM_MISM;
	}

}
#pragma vector=ADC_vect
__interrupt void ADC_interrupt( void ) 
{
}
#pragma vector=EE_RDY_vect
__interrupt void EE_RDY_interrupt( void ) 
{
}
#pragma vector=ANA_COMP_vect
__interrupt void ANA_COMP_interrupt( void ) 
{
}
#pragma vector=TWI_vect
__interrupt void TWI_interrupt( void ) 
{
}
#pragma vector=INT2_vect
__interrupt void INT2_interrupt( void ) 
{
}
#pragma vector=TIMER0_COMP_vect
__interrupt void TIMER0_COMP_interrupt( void ) 
{
}
#pragma vector=SPM_RDY_vect
__interrupt void SPM_RDY_interrupt( void ) 
{
}



#if (VS_REALLINKMODE == 1)
/* Interrupt Service Routine (ISR) for UART Receive. */
#pragma vector=UART_RX_vect
__interrupt void UART_RX_interrupt(void) 
{
  while (USR & (1 << RXC)) 
    VS_RealLinkReceive((unsigned char)UDR);
}

void RealLinkCommInit(void)
{
  /* If using a 4MHz crystal, set UBBR to 12. */
  /* Set the baudrate to 19,200 bps using a 3.69MHz clock. */
  UBRR = 11; 	

  /* Enable UART receiver and transmitter, and receive interrupt */
  UCR = ((1<<RXCIE) | (1<<RXEN) | (1<<TXEN));
}

void RealLinkReset( void ) 
{
}

void RealLinkTransmit(unsigned char data)
{
  /* Start transmittion */
  UDR = data; 			        
  /* Wait for empty transmit buffer */
  while (!(USR & (1<<UDRE)));
}
#endif
