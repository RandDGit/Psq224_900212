/*****************************************************************************
* IAR visualSTATE action functions and error handler.
* i.e. flesh out functions corresponding to System1action.h (VisualSTATE)
*****************************************************************************/

#include <iom16.h>		 	// Atmel up definitions
#include <inavr.h>			// AVR intrinsic functions
#include "VSMain.h"			// VisualState definitions (further includes)

#include "usart2.h"			// Application (UART) defs
#include "defs.h"			// Application (GENERAL) defs
#include "globals.h"		// Application global data/variables


unsigned char wal_similar( unsigned int, unsigned int);
void wal_motor_start( void);
void wal_motor_stop( void);

void InitData( void);
void LoadNVData( void);	        // read eeprom (nv data)
void wal_do_command( void);

/* Implement timer counter structure */
/* Relies upon hardware::application tick generation */


typedef struct wdef_vss_timer{
  VS_UINT event;
  VS_UINT ticks;
  unsigned char active;
  unsigned char other;
};

struct wdef_vss_timer wvt_period;
struct wdef_vss_timer wvt_stall;
struct wdef_vss_timer wvt_txwait;

void wal_setup_vss_timer( void)
{
	wvt_period.event = E_Periodic;		// results in A_Periodic
	wvt_period.ticks = 0;
	wvt_period.active = 0;
	wvt_period.other = 0;
	wvt_stall.event = E_AxisStall;			// used within 'MoveTo' state
	wvt_stall.ticks = 0;
	wvt_stall.active = 0;
	wvt_stall.other = 0;
	wvt_txwait.event = E_TimerTxWait;		// used within 'Sending' state
	wvt_txwait.ticks = 0;
	wvt_txwait.active = 0;
	wvt_txwait.other = 0;
}

void wal_vss_timer_function( void)
{
// Function for VSS Timer counter activity
// Must be called every <x> msec, where <x> = VSS timebase
// i.e. this application tick = 0.5 msec

	if( wvt_period.active == 1)
	{
		if( wvt_period.ticks != 0)
		{
			wvt_period.ticks--;
			if( wvt_period.ticks == 0)
			{
				SEQ_AddEvent( wvt_period.event);
			}
		}
	}
	if( wvt_stall.active == 1)
	{
		if( wvt_stall.ticks != 0)
		{
			wvt_stall.ticks--;
			if( wvt_stall.ticks == 0)
			{
				SEQ_AddEvent( wvt_stall.event);
			}
		}
	}
	if( wvt_txwait.active == 1)
	{
		if( wvt_txwait.ticks != 0)
		{
			wvt_txwait.ticks--;
			if( wvt_txwait.ticks == 0)
			{
				SEQ_AddEvent( wvt_txwait.event);
			}
		}
	}
}

/* REFER TO VS generated "System1action.h" */
/* Last updated 08DEC04 */

/* 
extern VS_VOID A_AxisCalcProfile (VS_VOID);
extern VS_VOID A_AxisPosUpdate (VS_VOID);
extern VS_VOID A_HandleButton (VS_VOID);
extern VS_VOID A_Initialise (VS_VOID);
extern VS_VOID A_MotorError (VS_VOID);
extern VS_VOID A_MotorFwd (VS_VOID);
extern VS_VOID A_MotorIdle (VS_VOID);
extern VS_VOID A_MotorMove (VS_VOID);
extern VS_VOID A_MotorOn (VS_VOID);
extern VS_VOID A_MotorReRef (VS_VOID);
extern VS_VOID A_MotorRev (VS_VOID);
extern VS_VOID A_MotorStop (VS_VOID);
extern VS_VOID A_MotorTest (VS_VOID);
extern VS_VOID A_Periodic (VS_VOID);
extern VS_VOID A_ResetTmStall (VS_VOID);
extern VS_VOID A_RxDoComms (VS_VOID);
extern VS_VOID A_RxProcess (VS_VOID);
extern VS_VOID A_TxChar (VS_VOID);
extern VS_VOID A_TxEnableOff (VS_VOID);
extern VS_VOID A_TxEnableOn (VS_VOID);
extern VS_VOID A_TxProcessMsg (VS_VOID);
extern VS_VOID A_TxTestBusIdle (VS_VOID);
extern VS_VOID TM_Stall (VS_UINT event, VS_UINT ticks);
extern VS_VOID TM_Status (VS_UINT event, VS_UINT ticks);
extern VS_VOID TM_TimerTxWait (VS_UINT event, VS_UINT ticks);
*/

VS_VOID A_AxisCalcProfile (VS_VOID)
{
// action upon entry to motor profile
// associated with A_MotorMove()
}
VS_VOID A_AxisPosUpdate (VS_VOID)
{
// action due to E_Tacho (update counter position)

        // if in Test State then ignore:
        if( uc_axis_vss == 3)
        {
          if( uc_axis_dir == 0)
          {
	    if( ui_axis_pos > 0)
	    {
		ui_axis_pos = ui_axis_pos - 1;
	    }
	   }
	   else
	   {
	    ui_axis_pos = ui_axis_pos + 1;
	   }
          return;
        }

	if( uc_axis_dir == 0)
	{
		if( ui_axis_pos > 0)
		{
			ui_axis_pos = ui_axis_pos - 1;
			if( wal_similar( ui_axis_pos, ui_axis_set) )
			{
				SEQ_AddEvent(E_AxisSetPoint);
			}
		}
		else
		{
			// position error
			SEQ_AddEvent(E_AxisError);
		}
	}
	else
	{
		// 65535UL
		if( ui_axis_pos < WAL_MAX_AXIS_TACHO )
		{
			ui_axis_pos = ui_axis_pos + 1;
			if( wal_similar( ui_axis_pos, ui_axis_set) )
			{
				SEQ_AddEvent(E_AxisSetPoint);
			}
		}
		else
		{
			// position error
			SEQ_AddEvent(E_AxisError);
		}
	}
}
VS_VOID A_HandleButton (VS_VOID)
{
// action due to E_Button during 'MoveTo' state
// stop motor and force E_AxisSetPoint to exit state
	SEQ_AddEvent(E_AxisSetPoint);
}
VS_VOID A_Initialise (VS_VOID)
{
	// Primary initialisation done prior to application start
	// setup ports and interupts
	// InitDevice();  
	// once ports setup, set outputs per application
	// InitOutputs();

	// reset variables:
	InitData();
	// read eeprom (nv data)
	LoadNVData();

	// setup baud rate dependent upon option
	if( uc_comm_hilo == 0)
  	{
    	// (max) 115200 ~= 125k baud (vario)
		USART_Init_High(1);
	}
	else
	{
		// 9600 baud (pic) = 25 with 4MHz osc and U2X = 0
  		USART_Init_Low( 25);
	}

	// SEQ_AddEvent(E_AxisInvalid);
}
VS_VOID A_MotorError (VS_VOID)
{
	// response to stall, limit detect E_AxisError
	SEQ_AddEvent(E_AxisError);
}
VS_VOID A_MotorFwd (VS_VOID)
{
// called upon Axis Home Event
	wal_motor_stop();
	uc_axis_dir = 1;
	uc_axis_pwm = WAL_PWM_HOME;

	wal_motor_start();
}
VS_VOID A_MotorIdle (VS_VOID)
{
	// set internal axis state for status
	uc_axis_vss = 0;
}
VS_VOID A_MotorMove (VS_VOID)
{
	// set internal axis state for status
	uc_axis_vss = 2;
}
VS_VOID A_MotorOn (VS_VOID)
{
	// uc_axis_dir = DIR;
	// uc_axis_pwm = PWM;

	// Brake Off, Direction Set, Enable = PWM Output
	uc_axis_rud = 1;	// ramp up
	uc_axis_pwm = WAL_PWM_RAMPUP;

	wal_motor_start();
}
VS_VOID A_MotorReRef (VS_VOID)
{
	// set internal axis state for status
	uc_axis_vss = 1;

	// if home switch start moving backwards (rev)
	if( val_ip_hom == 0)
	{
		A_MotorRev();
	}
	else
	{
		// else move forwards (if not at limit)
		if( val_ip_lim == 0)
		{
			A_MotorFwd();
		}
	}
}
VS_VOID A_MotorRev (VS_VOID) 
{
	uc_axis_dir = 0;
	uc_axis_pwm = WAL_PWM_RAMPUP;

	wal_motor_start();
}
VS_VOID A_MotorStop (VS_VOID)
{
	wal_motor_stop();
}
VS_VOID A_MotorTest (VS_VOID)
{
	// primary action of state reached from idle due to E_Button
	// set internal axis state for status
	uc_axis_vss = 3;
	uc_axis_dir = uc_axis_dir ? 0: 1;
	uc_axis_pwm = WAL_PWM_TEST;

	vsstick_stall = 4000;

	wal_motor_start();
}
VS_VOID A_Periodic (VS_VOID)
{
// This function is indicative of concurrent state activity.
// A_Periodic is called upon E_Periodic due to timer TM_Status 
// tick value = 10, i.e. every 5msec

	__watchdog_reset();

	// Indicate current status using LED1
	if( uc_led_scale == 0)
	{
		uc_led_scale = WAL_LED_SCALE;		// Scale factor to LED timebase
		uc_led_count++;						// use uc_led_count wrt value to set flash rate

		// LED1 'status' usage:
		// Solid on = moving
		// Solid off = Home or Limit active
		// Normal flash = idle, Fast flash = error, Slow flash = comms

		if( (val_ip_hom == 1) || (val_ip_lim == 1) )
		{
			WAL_OP_LED = 1;
		}
		else
		{
			if( uc_axis_vss != 0)
			{
				WAL_OP_LED = 0;
			}
			else
			{
				if( uc_led_count >= uc_led_value)
				{
					// Toggle LED1 (PB.0)
					PORTB = PINB ^ 0x01;
					uc_led_count = 0;
				}
			}
		}
	}
	else
	{
		uc_led_scale--;
	}


}
VS_VOID A_ResetTmStall (VS_VOID)
{
// reset 'stall' counter timer use global vsstick_stall:
	wvt_stall.ticks = vsstick_stall;		// NB. now two instances of write to wvt_stall.ticks
	wvt_stall.active = 1;
}

VS_VOID A_RxDoComms (VS_VOID)
{
	// E_RxMsg, valid message received:
	// if valid address (i.e. match or global process message)
	// process according to hi or lo-speed comms:
	if( uc_comm_hilo == 0)
	{
		if( (uca_msgrx[0] == WAL_COMMS_GLOBAL) || (uca_msgrx[0] == uc_comm_adid) )
		{
			wal_do_command();
			// response to message (transition to Sending state)
			SEQ_AddEvent(E_SendRequest);
		}
	}
	else
	{
	}

// usually response required: E_SendRequest

}
VS_VOID A_RxProcess (VS_VOID)
{
// structure uc_rxbuff( charin & status) loaded into uc_rx_chrecv & uc_comm_9set

	// if error detected then reset any message (in) in-progress
	if( (uc_comm_flag & WAL_COMM_DERR) == WAL_COMM_DERR)
	{
		// reset error, but clear any existing in-progress:
		uc_comm_flag &= ~WAL_COMM_DERR;
		uc_comm_flag &= ~WAL_COMM_DRDY;
		uc_msgrxptr = 0;
		uc_rx_chksum = 0;
		return;
	}

	// process according to hi or lo-speed comms:
	if( uc_comm_hilo == 0)
	{
		// Hi-speed 9-bit comms
		// Check if 9th bit set: NB UCSRB bit.1 has RXB8 if so = start of message
		if( uc_comm_9set)
		{
			uca_msgrx[0] = uc_rx_chksum = uc_rx_chrecv;
			uc_msgrxptr = 1;
			uc_comm_flag |= WAL_COMM_DRDY;
		}
		else
		{
			// check that address byte has been received:
			if( (uc_comm_flag & WAL_COMM_DRDY) == WAL_COMM_DRDY)
			{
				if( uc_msgrxptr < 5)
				{
					uc_rx_chksum += uc_rx_chrecv;
					uca_msgrx[uc_msgrxptr] = uc_rx_chrecv;
					uc_msgrxptr++;
				}
				else
				{
					// should have received last byte of message, so clear DRDY
					uc_comm_flag &= ~WAL_COMM_DRDY;
					// if checksum matches then this is a valid message:
					if( uc_rx_chksum == uc_rx_chrecv)
					{
						SEQ_AddEvent(E_RxMsg);
					}
				}
			}
			else
			{
				// ignore: byte received but not valid 'start of message'.
			}
		}
	}

	uc_rx_process = 0;				// reset rx processed flag (semaphore)
}
VS_VOID A_TxChar (VS_VOID)
{
// TxChar state (initial entry & TxMoreMsg)
	UDR = uc_tx_chout;
	UCSRB &= ~(1<<TXB8);	// reset 9th bit
}
VS_VOID A_TxEnableOff (VS_VOID)
{
// end of Tx Message
    PORTB = (PINB & ~0x10);		// Switch off line driver
    UCSRB |= (1<<RXCIE);    	// enable RX interrupt

    uc_comm_flag &= ~WAL_COMM_SMSG;	// Flag data sent (msgtx complete)
}
VS_VOID A_TxEnableOn (VS_VOID)
{
// start of Tx Message
	uc_msgtxprt = 0;
	uc_tx_chout = uca_msgtx[0];

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
VS_VOID A_TxProcessMsg (VS_VOID)
{
// response to E_TxComplete i.e. TXC event

	// test if error condition exists, if so then return to wait state (retry send)
	if( (uc_comm_flag & WAL_COMM_MISM) == WAL_COMM_MISM)
	{
		SEQ_AddEvent(E_TxBusIdle);
		return;
	}

	// if last character of message tx'd then end Tx Msg using E_SendComplete
	// otherwise push next character of message to transmit
	// uc_tx_chout = uca_msgtx[uc_msgtxprt]

	if( uc_comm_hilo == 0)
	{
		if( uc_msgtxprt >= 5)
		{
			SEQ_AddEvent(E_SendComplete);
		}
		else
		{
			uc_msgtxprt++;
			SEQ_AddEvent(E_TxMoreMsg);
		}
	}
	else
	{
		if( uc_msgtxprt >= 4)
		{
			SEQ_AddEvent(E_SendComplete);
		}
		else
		{
			uc_msgtxprt++;
			SEQ_AddEvent(E_TxMoreMsg);
		}
	}
}
VS_VOID A_TxTestBusIdle (VS_VOID)
{
// Test if RS485 bus idle using 'uc_line_idle':
// NB. as uc_comm_idle may be written during RX interrupt disable
	__disable_interrupt();
	if( uc_comm_idle >= 3)
	{
		// E_TxBusIdle will cause State 'Transmit' to begin
		uc_comm_flag |= WAL_COMM_IDLE;
		SEQ_AddEvent(E_TxBusIdle);
	}
	else
	{
		uc_comm_idle++;
	}
	__enable_interrupt(); 
}
VS_VOID TM_Stall (VS_UINT event, VS_UINT ticks)
{
// Seed E_AxisStall
// NB. this routine called with VS_UINT vsstick_stall as tick parameter
	wvt_stall.event = event;
	wvt_stall.ticks = ticks;
	wvt_stall.active = 1;
}
VS_VOID TM_Status (VS_UINT event, VS_UINT ticks)
{
// Seed E_Periodic
	wvt_period.event = event;
	wvt_period.ticks = ticks;
	wvt_period.active = 1;
}
VS_VOID TM_TimerTxWait (VS_UINT event, VS_UINT ticks)
{
// Seed E_TimerTxWait
// NB. this routine called with VS_UINT vsstick_txwait as tick parameter
	wvt_txwait.event = event;
	wvt_txwait.ticks = ticks;
	wvt_txwait.active = 1;
}

// VS_VOID TM_Timer0 (VS_UINT event, VS_UINT ticks){  TIMER0_SW_start( event, ticks );}



