// AVR306: Using the AVR UART in C
// Routines for interrupt controlled USART

#include <iom16.h>			// Atmel definitions Mega16
#include <inavr.h>			// Atmel register definitions
#include "VSMain.h"			// Visual State definitions

#include "defs.h"			// Application (GENERAL) defs
#include "globals.h"		// Application global data/variables

void wal_program_ee( char s_address, char s_value);
unsigned char wal_bsf_eeget( unsigned char s_address);

void USART_Init_High( unsigned int baudrate )
{
// Initialize USART High speed comms 9 bit 2 stop

	/* Set the baud rate */
	UBRRH = (unsigned char) (baudrate>>8);                    
	UBRRL = (unsigned char) baudrate;
	
	/* Enable UART RX interupt and enable RX and TX and 9 bit data */
	//UCSRB = ( ( 1 << RXCIE ) | ( 1 << RXEN ) | ( 1 << TXEN ) | ( 1 << UCSZ2 ) ); 
	UCSRB = 0xDC; 

	// Hi-speed frame format: 9 data 2stop no parity
	// For devices with Extended IO
	UCSRC = 0x86;              
}

void USART_Init_Low( unsigned int baudrate )
{
// Initialize USART low speed comms 8bit

	/* Set the baud rate */
	UBRRH = (unsigned char) (baudrate>>8);                  
	UBRRL = (unsigned char) baudrate;
	
	/* Enable UART RX interupt and enable RX and TX and 9 bit data */
	UCSRB = ( ( 1 << RXCIE ) | ( 1 << RXEN ) | ( 1 << TXEN ) | (1 << TXCIE) ); 

	// Lo-speed frame format: 8 data 1stop ( UCSRC defaults OK)
	// UCSRC = (1<<UCSZ1)|(1<<UCSZ0);
}

void wal_start_tx( void)
{
// disable rx interrupt & setup TX_En 
    
    UCSRB &= ~(1<<RXCIE);			// disable rx interrupt
    PORTB = (PINB | 0x10);  		// Switch line driver on
	// ? set address bit ?
    uc_comm_flag |= WAL_COMM_SMSG;	// Transmission in progress

	// if hi-speed then set 9bit (= start of message)
	if( uc_comm_hilo == 0)
	{
    	UCSRB |= (1<<TXB8);
	}
}
    

void wal_end_tx(void)
{
    PORTB = (PINB & ~0x10);			// Switch off line driver
    UCSRB |= (1<<RXCIE);    		// enable RX interrupt

    uc_comm_flag &= ~WAL_COMM_SMSG;	// Flag data sent (msgtx complete)
}


void wal_send485_char( void)
{
	UDR = uc_tx_chout;
	UCSRB &= ~(1<<TXB8);	// reset 9th bit
}


void wal_tx_nrq(void )
{
// Fixed attention request message format CMD:21, Data[4]:14
	uca_msgtx[0] = 0;
	uca_msgtx[1] = uc_comm_adid;
	uca_msgtx[2] = 21;
	uca_msgtx[3] = 0;
	uca_msgtx[4] = 14;
}


void wal_do_command( void)
{
//rx3rx4 = (unsigned int) ((rx[3] << 8) + rx[4]);

uca_msgtx[0] = uca_msgrx[1];    	// back to originator (0)
uca_msgtx[1] = uc_comm_adid; 		// from me...
uca_msgtx[2] = uca_msgrx[2];    	// cmd always echo'd
uca_msgtx[3] = uca_msgrx[3];    	// if in doubt echo data..
uca_msgtx[4] = uca_msgrx[4];    	// ..

switch( uca_msgrx[2])
    {
    case 0:    // get mode
        uca_msgtx[4] = 0x04;
        break;
    case 1: 
    	// set variable (directly)
        switch( uca_msgrx[3])
        {
		case 0:
			// direct set of PORTA
			PORTA = uca_msgrx[4];
			break;
		case 1:
			// direct set of PORTB
			PORTB = uca_msgrx[4];
			break;
		case 2:
			// direct set of PORTC
			PORTC = uca_msgrx[4];
			break;
		case 3:
			// direct set of PORTD
			PORTD = uca_msgrx[4];
			break;
        }
        break;
    case 2: 
    	// or register
        switch( uca_msgrx[3])
        {
		case 0:
			// direct OR of PORTA
			PORTA = PINA | uca_msgrx[4];
			break;
		case 1:
			// direct OR of PORTB
			PORTB = PINB | uca_msgrx[4];
			break;
		case 2:
			// direct OR of PORTC
			PORTC = PINC | uca_msgrx[4];
			break;
		case 3:
			// direct OR of PORTD
			PORTD = PIND | uca_msgrx[4];
			break;
        }
		break;
    case 3: 
    	// and register
        switch( uca_msgrx[3])
        {
		case 0:
			// direct AND of PORTA
			PORTA = PINA & uca_msgrx[4];
			break;
		case 1:
			// direct AND of PORTB
			PORTB = PINB & uca_msgrx[4];
			break;
		case 2:
			// direct AND of PORTC
			PORTC = PINC & uca_msgrx[4];
			break;
		case 3:
			// direct AND of PORTD
			PORTD = PIND & uca_msgrx[4];
			break;
        }
        break;
    case 5: // get register..
        // 01NOV00 the only register directly accessed:
        // should be $A3 (detector ref value)
        switch( uca_msgrx[3])
        {
		case 0:
			// direct read of PORTA
			uca_msgtx[4] = PINA;
			break;
		case 1:
			// direct read of PORTB
			uca_msgtx[4] = PINB;
			break;
		case 2:
			// direct read of PORTC
			uca_msgtx[4] = PINC;
			break;
		case 3:
			// direct read of PORTD
			uca_msgtx[4] = PIND;
			break;
        case 0x80:
            //read from eeprom address rx4
			__disable_interrupt();
            uca_msgtx[3] = wal_bsf_eeget(uca_msgrx[4]);
			__enable_interrupt();
            break;
        }
        break;
    case 15:// VERSION No REQUEST 
        uca_msgtx[3] = def_major_ver;            // get_ee[x]...
        uca_msgtx[4] = def_minor_ver;
        break; 
   	case 20:
   		// clear errors:
   		break;       
	case 100:
		// write to eeprom:
		__disable_interrupt();
        wal_program_ee( uca_msgrx[3], uca_msgrx[4]);
		__enable_interrupt();
		break;
    default:
        // given current tacho by default:
        uca_msgtx[3] = uca_msgtx[4] = 0;
    }  
} 





