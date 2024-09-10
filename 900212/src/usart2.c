// AVR306: Using the AVR UART in C
// Routines for interrupt controlled USART

#include <iom16.h>			// Atmel definitions Mega16
#include <inavr.h>			// Atmel register definitions
#include "VSMain.h"			// Visual State definitions

#include "defs.h"			// Application (GENERAL) defs
#include "globals.h"		// Application global data/variables

#include "usart2.h"			// Function declarations for this file

void vss_seq_error( void);
void wal_eetbl_write( unsigned char s_address, unsigned char s_value);
unsigned char wal_bsf_eeget( unsigned char s_address);
void wal_motor_start( void);
void wal_motor_stop( void);
unsigned char wal_pos_similar( void);

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
	// UCSRC == 0x86 or 0x8E if 2 stop bits
	UCSRC = 0x8E;              
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


unsigned char wal_picnet_checkdata( void)
{
  if( (uca_msgrx[2] != 0x26) || ( uca_msgrx[3] != 0x26))
  {
    uca_msgtx[1] = '*';
    uca_msgtx[2] = 'I';
    return 1;
  }
  return 0;
}



void wal_picnet_motor( void)
{
uca_msgtx[0] = (uc_comm_adid + 0x30);	// from me...
uca_msgtx[1] = (uca_msgrx[1] & 0xDF);   // echo,but CAPS masking 0x20
uca_msgtx[2] = uca_msgrx[2];    		// if in doubt echo data..
uca_msgtx[3] = uca_msgrx[3];    		// ..

// response == ID, *, I, <x> if invalid
// NB. confirm RXDHI == uca_msgtx[3]

	switch( uca_msgrx[1])
	{
	case 'A':
		// assign address: NB. only reply if trigger in
		if( val_ip_trg == 0)
		{
			if( uca_msgrx[3] == 0x1f)
			{
				uc_comm_adid = uca_msgrx[2] & 0x0F;
				wal_eetbl_write( WAL_EE_COMMID, uc_comm_adid);
				uca_msgtx[0] = (uc_comm_adid + 0x30);	// from me...
			}
			else
			{
				uca_msgtx[1] = '*';
				uca_msgtx[2] = 'I';
			}
		}
		else
		{
			// ignore, no reply
			uc_comm_noecho = 1;
		}
		break;
	case 'B':
		// backward move (datahi-lo = 0x26)
		if( wal_picnet_checkdata() == 0)
		{
			// set motor
			uc_axis_dir = 0;
			uc_axis_pwm = WAL_PWM_TEST;
			wal_motor_start();
		}
		break;
	case 'D':
		// define trigger in-out
		// NB. has to force pattern as per porta into RXDLO
		// assign address: NB. only reply if trigger in
		if( val_ip_trg == 0)
		{
			if( uca_msgrx[3] == 0x1f)
			{
				if( uca_msgrx[2] == 1)
				{
					// set trigger out (active low)
					WAL_OP_TRO = 0;
				}
				else
				{
					WAL_OP_TRO = 1;
				}
			}
			else
			{
				uca_msgtx[1] = '*';
				uca_msgtx[2] = 'I';
			}
		}
		else
		{
			// ignore, no reply
			uc_comm_noecho = 1;
		}
		break;
	case 'F':
		// forward move (datahi-lo = 0x26)
		if( wal_picnet_checkdata() == 0)
		{
			// set motor
			uc_axis_dir = 1;
			uc_axis_pwm = WAL_PWM_TEST;
			wal_motor_start();
		}
		break;
	case 'G':
		// goto position
		// respond with '*','U' if error condition:
		// NB. error condition should include limit or home switch
		if( wal_bsf_eeget( WAL_EE_ERROR) != 0)
		{
			uca_msgtx[1] = '*';
			uca_msgtx[2] = 'U';
			uca_msgtx[3] = wal_bsf_eeget( WAL_EE_ERROR);
		}
		else
		{
			// return '*','Q' if within +/- 3 units
			// NB. convert from comms units (1/10th mm)	Lo-value + Hi-value
			// to 'GEARR' units pulses WAL_GEAR_RATIO
			// comms (Hi+Lo) * WAL_GEAR_RATIO == SET POINT
			// difference == COUNTER (Hi+Lo) - SET POINT (Hi+Lo)
			ui_axis_tmm = (uca_msgtx[3] << 8) + uca_msgtx[2];
			ui_axis_set = ui_axis_tmm * 3;	// << 1 + 1
			// set point (unsigned) if > 420mm then ignore (*U) 
			if( ui_axis_set > WAL_MAX_SET_VALUE)
			{
				uca_msgtx[1] = '*';
				uca_msgtx[2] = 'U';
				uca_msgtx[3] = 0xFE;
			}
			else
			{
				if( ui_axis_pos > ui_axis_set)
				{
					ui_axis_diff = ui_axis_pos - ui_axis_set;
					// +'ve difference if < 4 then ignore
					// also *U if difference too large (or causing -'ve value)
					if( ui_axis_diff < 4)
					{
						// ignore small difference
						uca_msgtx[1] = '*';
						uca_msgtx[2] = 'Q';
					}
					else
					{
						// move forward (use sequence E_AxisMove)
						// reply with difference in data lo-hi

						if( SEQ_AddEvent( E_AxisMove) != UCC_OK )
						{
							vss_seq_error();
						}

						// uc_axis_dir = 1;
						// uc_axis_pwm = WAL_PWM_STEPA;	// begin ramp up
						// wal_motor_start();
						uca_msgtx[2] = (unsigned char) (ui_axis_diff & 0x00FF);
						uca_msgtx[3] = (unsigned char) (ui_axis_diff >> 8);
					}
				}
				else
				{
					ui_axis_diff = ui_axis_set - ui_axis_pos;
					// also *U if difference too large (or causing -'ve value)
					if( ui_axis_diff < 4)
					{
						// negative difference so move in reverse
						// decrease 'set point' for uni-directional motion,
						// and set unidirection flag:

						if( SEQ_AddEvent( E_AxisMove) != UCC_OK )
						{
							vss_seq_error();
						}

						// uc_axis_dir = 0;
						// uc_axis_pwm = WAL_PWM_STEPA;	// begin ramp up
						// wal_motor_start();
						uca_msgtx[2] = (unsigned char) (ui_axis_diff & 0x00FF);
						uca_msgtx[3] = (unsigned char) (ui_axis_diff >> 8);
					}
					else
					{
						// ignore small difference
						uca_msgtx[1] = '*';
						uca_msgtx[2] = 'Q';
					}
				}
			}
		}
		break;
	case 'P':
		// position: reply 'C'+ count if moving; 'P' + position if stationery
		// NB. check order of data-hi, data-lo
		if( uc_axis_vss == 0)
		{
			// idle, reply 'P' (set point ui_axis_set)
			uca_msgtx[1] = 'P';
			uca_msgtx[2] = (unsigned char) (ui_axis_set & 0x00FF);
			uca_msgtx[3] = (unsigned char) (ui_axis_set >> 8);
		}
		else
		{
			// axis active, reply 'C' (current position ui_axis_pos)
			uca_msgtx[1] = 'C';
			uca_msgtx[2] = (unsigned char) (ui_axis_pos & 0x00FF);
			uca_msgtx[3] = (unsigned char) (ui_axis_pos >> 8);
		}
		break;
	case 'S':
		// stop
		wal_motor_stop();
		break;
	case 'V':
		// version (datahi-lo = 0x26)
		if( wal_picnet_checkdata() == 0)
		{
			// respond with version	WAL_PIC_VERSION
			uca_msgtx[1] = 'I';
			uca_msgtx[2] = 'V';
			uca_msgtx[3] = WAL_PIC_VERSION;
		}
		break;
	default:
		// send '*','I' response + 'command' in data[3]
		uca_msgtx[1] = '*';
		uca_msgtx[2] = 'I';
		uca_msgtx[3] = uca_msgrx[1];
	}

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
        case 12:
            // manipulate MCUCSR
            MCUCSR = uca_msgrx[4];
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
        case 12:
            // manipulate MCUCSR
            MCUCSR |= uca_msgrx[4];
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
        case 12:
            // manipulate MCUCSR
            MCUCSR &= uca_msgrx[4];
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
        case 12:
            // manipulate MCUCSR
            uca_msgrx[4] = MCUCSR;
            break;
        case 0x80:
            //read from eeprom address rx4
			__disable_interrupt();
            uca_msgtx[3] = wal_bsf_eeget(uca_msgrx[4]);
			__enable_interrupt();
            break;
        } 
        break;
				
    case 12:
        // por_reset = MCUCSR;
        uca_msgtx[3] = WDTCR;
        uca_msgtx[4] = por_reset;         // capture POR
        break;
				
    case 13:
        // error information ?
        switch( uca_msgrx[3])
        {
        case 0:
          uca_msgtx[3] = uc_axis_err;
          uca_msgtx[4] = por_reset;       // capture POR
          break;
        case 1: // give relevant 'ERR' data history a->b->c
          switch( uca_msgrx[4])
          {
          case 1: // erra
            uca_msgtx[3] = 0;
            uca_msgtx[4] = uc_atbl_err;
            break;
          case 2: // errb
            uca_msgtx[3] = 0;
            uca_msgtx[4] = uc_btbl_err;
            break;
          case 3: // errc
            uca_msgtx[3] = 0;
            uca_msgtx[4] = uc_ctbl_err;
            break;
          }
          break;
        case 2: // give relevant 'POR' data assoc. with error
          switch( uca_msgrx[4])
          {
          case 1: // erra
            uca_msgtx[3] = uc_atbl_phi;
            uca_msgtx[4] = uc_atbl_plo;
            break;
          case 2: // errb
            uca_msgtx[3] = uc_btbl_phi;
            uca_msgtx[4] = uc_btbl_plo;
            break;
          case 3: // errc
            uca_msgtx[3] = uc_ctbl_phi;
            uca_msgtx[4] = uc_ctbl_plo;
            break;
          }
          break;
        case 3: // current 'POR' counter: uc_por_hi,uc_por_lo
          uca_msgtx[3] = uc_por_hi;
          uca_msgtx[4] = uc_por_lo;
          break;
        }
        break;
				
    case 14: // Status request:
        uca_msgtx[3] = uc_axis_err;
        // uc_motoron
        uca_msgtx[4] = uc_axis_vss;
        uca_msgtx[4] |= uc_motoron ? 0x80 : 0;
        uca_msgtx[4] |= (WAL_IN_HOM == 1) ? 0x40 : 0;
        uca_msgtx[4] |= (WAL_IN_LIM == 1) ? 0x20 : 0;
        break; 
				
    case 15: // VERSION No REQUEST 
        uca_msgtx[3] = def_major_ver;            // get_ee[x]...
        uca_msgtx[4] = def_minor_ver;
        break; 
				
    case 20:
        // clear errors:
        break;
				
    case 68:
        // joggle_on
        uc_joggle_pause = uca_msgtx[3];
        uc_joggle_dist = uca_msgtx[4];
        uc_joggle_on = 1;
        break;
				
    case 69:
        // joggle_off
        uc_joggle_on = 0;
        break;
				
    case 88:
        // Get Max. allowable tacho limit: ui_axis_maxallow
        //CXP 100924
        uca_msgtx[3] = (unsigned char) (NEW2OLD(ui_axis_maxallow) >> 8);
        uca_msgtx[4] = (unsigned char) (NEW2OLD(ui_axis_maxallow) & 0x00FF);
        break;
				
    case 89:
        // Set Max. allowable tacho limit: ui_axis_maxallow WAL_EE_MAXTHI, WAL_EE_MAXTLO
        //CXP 100924
        ui_axis_maxallow = (unsigned int) (uca_msgrx[3] << 8) + uca_msgrx[4];
        ui_axis_maxallow = OLD2NEW(ui_axis_maxallow);
        uc_axis_chi = OLD2NEW(uca_msgrx[3]);
        uc_axis_clo = OLD2NEW(uca_msgrx[4]);
        wal_eetbl_write( WAL_EE_MAXTHI, uc_axis_chi);
        wal_eetbl_write( WAL_EE_MAXTLO, uc_axis_clo);
        break;
				
    case 90: // stop
        // clear 'uni-directional flag' (set later if required)
        uc_unitravel = 0;
        uc_joggle_on = 0;
        uc_done_move = 1;
        // don't stop if we're rereferencing via either the RS485 comms or the switch
        if(uc_axis_vss != 1 && uc_axis_vss != 4)
        {
          if(uc_motoron)
          {
            // if we're currently moving stop
            uc_axis_vss = 0;
            A_MotorStop();
            if( SEQ_AddEvent( E_AxisSetPoint) != UCC_OK )
            {
              vss_seq_error();
            }
          }
        }
	break;	
    case 91: // re-reference AddEvent
        // V1.3 27JUN05: allow 'home' sequence if home switch active:
        // if( val_ip_hom == 0){}
        // joggle_off
        uc_joggle_on = 0;
        
        uc_axis_vss = 4;
        
        if( SEQ_AddEvent( E_AxisInvalid) != UCC_OK )
        {
                vss_seq_error();
        }
        break;
				
    case 92: // forwards or backward depending upon rx[3],rx[4]
        if( val_ip_lim == 0) 
        {
          uc_axis_dir = (uca_msgtx[3] == 1) ? 1: 0;
          if( uca_msgtx[4] == 0)
              uc_axis_pwm = WAL_PWM_TEST;
          else
              uc_axis_pwm = uca_msgtx[4];
          wal_motor_start();
        }
        break;
				
    case 93: // position
        // position: reply 'C'+ count if moving; 'P' + position if stationery
        // command must match in high speed comms (NB. data hi-lo swapped )
        // therefore no differentiation of ui_axis_set or ui_axis_pos
        // if( uc_axis_vss == 0) 
        // need to convert ui_axis_pos -> 0.1mm units using conversion *5 /4
        // ui_axis_postx = ui_axis_pos * 5/4 (ui_axis_conv, ui_axis_tacho)
        
        // ui_axis_tacho = ui_axis_pos >> 1;      // divide by 2
        // store 'carry' flag
        // uc_carrybit = WAL_CARRY_BIT ? 3: 0;
        // ui_axis_mult = ui_axis_tacho * 5;     // mult. by 5
        // if( uc_carrybit)
        // {
        //  ui_axis_mult += 3;
        // }
        // ui_axis_conv = ui_axis_mult >> 1;
        // uca_msgtx[3] = (unsigned char) (ui_axis_conv >> 8);
        // uca_msgtx[4] = (unsigned char) (ui_axis_conv & 0x00FF);
        
        // V1.8 27JAN06: give set point if moving...
        //CXP 100924
         if( uc_motoron)
        {
          uca_msgtx[3] = (unsigned char) (NEW2OLD(ui_axis_pos) >> 8);
          uca_msgtx[4] = (unsigned char) (NEW2OLD(ui_axis_pos) & 0x00FF);
        }
        else
        {
        
          // V1.3 work in tacho units only
          uca_msgtx[3] = (unsigned char) (NEW2OLD(ui_axis_pos) >> 8);
          uca_msgtx[4] = (unsigned char) (NEW2OLD(ui_axis_pos) & 0x00FF);
          
          // V1.7 16JAN06: if uni-directional movement in progress
          // report 'set-point' to prevent 'goto'
          if( uc_unitravel)
          {
            uca_msgtx[3] = (unsigned char) (NEW2OLD(ui_axis_uni) >> 8);
            uca_msgtx[4] = (unsigned char) (NEW2OLD(ui_axis_uni) & 0x00FF);
          }
        }       
        break;
				
    case 94: // goto
        // i.e. set position, ui_axis_set = rx[3][4]
        // value set in 0.1mm units: conversion to tachos (0.125mm) = 4/5
        // method *2; +3; divide by 5; *2 = 0.125 (i.e. tacho units)
        // use WAL_MAX_SET_HIBYTE to protect max. parameter
        
        // V1.3 27JUN05: ignore 'goto' if axis error flag set: (must reref)
        // uc_axis_err = 0;
        // joggle_off
        uc_joggle_on = 0;
                
        // V1.8 27JAN06: ignore 'goto' if moving...
        if( (uca_msgrx[3] <= WAL_MAX_SET_HIBYTE) && (uc_axis_err == 0))
        {
          if (uc_comm_adid != 28) // not centre axis
          {
            if ( uc_comm_adid != 36) //not rotate
            {
              if (uc_motoron == 1) 
              {
                 break;  // reject cmd 94 if not those axis & moving
              } 
            }
          }
          // i.e. position < MAX
          if( (uca_msgrx[3] == 0) && (uca_msgrx[4] < WAL_MIN_SET_LOBYTE) )
          {
            // i.e. position < 0.5mm therefore invalid
          }
          else
          {
            // V1.3 work in tacho units only
            ui_axis_setrx = (unsigned int) (uca_msgrx[3] << 8) + uca_msgrx[4];
            
            //CXP 100924
            ui_axis_setrx = OLD2NEW(ui_axis_setrx);
            
            if( ui_axis_setrx < ui_axis_maxallow)
            {
              ui_axis_set = ui_axis_setrx;
            }
            
            // new (valid) set point
            // ui_axis_setrx = (unsigned int) (uca_msgrx[3] << 8) + uca_msgrx[4];
            // need to convert to tacho units :: 4/5
            // ui_axis_tacho = ui_axis_setrx << 1;
            // ui_axis_tacho += 3;
            // ui_axis_mult = ui_axis_tacho / 5;     // divide by 5
            // ui_axis_conv = ui_axis_mult << 1;
            // ui_axis_set = ui_axis_conv;
            
            // E_AxisMove (MoveToPosition); Entry = A_AxisCalcProfile, A_MotorMove
            // if( ui_axis_set != ui_axis_pos)
            
            
            if( !wal_pos_similar())
            {
              if( SEQ_AddEvent( E_AxisMove) != UCC_OK )
              {
                vss_seq_error();
              }
            }     // set != pos
          }       // else  set pt > LOBYTE
        }         // rx[3] < HIBYTE
        break;
				
    case 95: // address
				uc_readdr_timer = 255;
				
        if( val_ip_sw1 == 1)
        {
            uc_comm_adid = uca_msgrx[4] & 0x3F;
            wal_eetbl_write( WAL_EE_COMMID, uc_comm_adid);
            uca_msgtx[1] = uc_comm_adid;
            uc_axis_err = 5;
            // wal_eetbl_write( WAL_EE_ERROR, uc_axis_err);
            // store in table..
            // wal_record_axis_err();
        }
        else
        {
            // ignore, no reply
            uc_comm_noecho = 1;
        }
        break;
				
    case 96: // trigger (respond input) + button status
        break;
				
    case 97: // trigger (set output)
        break;
				
    case 98: // status (moving, error) - Change to:
		         // Clear & reset error history
				uc_axis_err = 0;
				
				wal_clear_axis_err();
        break;
				
    case 99: // speed
        uc_axis_pwm = uca_msgrx[4];
        break;
				
    case 100:
        // write to eeprom:
        __disable_interrupt();
        wal_eetbl_write( uca_msgrx[3], uca_msgrx[4]);
        __enable_interrupt();
        break;
				
    case 110:
        // read 'cpu serial number'
        uca_msgtx[3] = wal_bsf_eeget(WAL_EE_CPUSERHI);
        uca_msgtx[4] = wal_bsf_eeget(WAL_EE_CPUSERLO);
        break;
				
    case 111:
        // write 'cpu serial number'
        wal_eetbl_write( WAL_EE_CPUSERHI, uca_msgrx[3]);
        wal_eetbl_write( WAL_EE_CPUSERLO, uca_msgrx[4]);
        break;
				
    case 112:
        // read axis calibration value
        //CXP 100924
        uca_msgtx[3] = NEW2OLD(wal_bsf_eeget(WAL_EE_CALIBHI));
        uca_msgtx[4] = NEW2OLD(wal_bsf_eeget(WAL_EE_CALIBLO));
        break;
				
    case 113:
        // write axis calibration value
        //CXP 100924
        wal_eetbl_write( WAL_EE_CALIBHI, OLD2NEW(uca_msgrx[3]));
        wal_eetbl_write( WAL_EE_CALIBLO, OLD2NEW(uca_msgrx[4]));
        break;
				
    default:
        uca_msgtx[3] = uca_msgtx[4] = 0;
    }  
} 





