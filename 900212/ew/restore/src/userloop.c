/* UserLoop = Application, excluding State Machine */

#include <iom16.h>		 		// Atmel up definitions
#include <inavr.h>				// AVR intrinsic functions
#include "VSMain.h"				// VisualState definitions (further includes)

#include "defs.h"				// Application (GENERAL) defs

// following (global) application data variables declared in "globals.h"

unsigned int ui_axis_set;		// Axis Set Point
unsigned int ui_axis_pos;		// Axis Position
unsigned int ui_axis_rup;		// Axis Ramp-up end point
unsigned int ui_axis_rdn;		// Axis Ramp-dn end point
unsigned int ui_axis_tmm;		// Axis Set Value in 1/10th mm units

unsigned char uc_tprim;		// Primary timer base (TMR0 ovf)
unsigned char uc_tprev;		// Prev. value
unsigned char uc_tm0preset;	// Timer 0 preset (controls base period)

unsigned char uc_axis_vss;	// Motor State
unsigned char uc_axis_dir;	// Motor Direction
unsigned char uc_axis_pwm;	// Motor PWM output
unsigned char uc_axis_rud;	// Motor Ramp Up,Down state

unsigned char uc_led_value;	// Status LED (reload value = frequency)
unsigned char uc_led_count;	// Status LED (counter)
unsigned char uc_led_scale;	// Scale factor to LED timebase

unsigned char uc_rx_stata;	// comms rx status (a) used by USART_RXC_interrupt
unsigned char uc_rx_statb;	// comms rx status (b) used by USART_RXC_interrupt
unsigned char uc_rx_chbyte;	// comms rx byte       used by USART_RXC_interrupt


struct wdef_rxbuff uc_rxbuff[WAL_MAX_RXBUFF];
unsigned char uc_rxrdptr;	// read pointer (rxbuff)
unsigned char uc_rxwrptr;	// write pointer (rxbuff)
unsigned char uc_rx_chrecv;	// comms rx received, read from buffer to function
unsigned char uc_rx_chksum;	// comms rx check sum
unsigned char uc_rx_process;	// comms rx byte processed

unsigned char uc_tx_chout;	// character to be TX'd out
unsigned char uc_tx_chksum;	// comms tx check sum
unsigned char uc_tx_chsent;	// TX char out complete

unsigned char uc_comm_adid;	// comm address id
unsigned char uc_comm_idle;	// comm RS485 bus idle test
unsigned char uc_comm_flag;	// comms bit flags

unsigned char uc_comm_9set;	// 9th bit set (derived from statb)
unsigned char uc_comm_hilo;	// Comms hi-speed (9bit) or lo-speed (8bit)
unsigned char uc_comm_comp;	// Comms compare Tx out with Rx in (TX integrity)

unsigned char uca_msgrx[6];      	// rx buffer dst:src:cmd:dhi:dlo:cks
unsigned char uca_msgtx[6];      	// tx buffer
unsigned char uc_msgrxptr;			// index to rx message = 6 if hi-speed = 5 if lo-speed
unsigned char uc_msgtxprt;			// index to tx message

unsigned char dblead_ip_enc;		// Debounce Lead edge encoder
unsigned char dbtail_ip_enc;
unsigned char val_ip_enc;

unsigned char dblead_ip_trg;		// Debounce Lead edge trigger input
unsigned char dbtail_ip_trg;
unsigned char val_ip_trg;

unsigned char dblead_ip_hom;		// Debounce Lead edge home input
unsigned char dbtail_ip_hom;

unsigned char dblead_ip_lim;		// Debounce Lead edge limit input
unsigned char dbtail_ip_lim;

unsigned char dblead_ip_sw1;		// Debounce Lead edge button input
unsigned char dbtail_ip_sw1;
unsigned char val_ip_sw1;
unsigned char pin_ip_sw1;

unsigned char uc_read_portd;            // PIND
unsigned char uc_read_portb;            // PINB

struct wdef_ctimer ws_ctimer[WDEF_MAX_TIMERS];	// application timer function(s)
struct wdef_ctimer *wsp_tc;			// timer counter implementation

// external variables used by VSS model (critical)
// defined & declared within VSS <System1>Data generated files
// VS_UCHAR val_ip_hom;
// VS_UCHAR val_ip_lim;
// VS_UINT vsstick_stall;
// VS_UINT vsstick_txwait;

void wal_vss_timer_function( void);		// VSS Timer (tick) function

void wal_dbnc_inputs( void);                    // input handler level & debounce

unsigned char wal_similar( unsigned int suia, unsigned int suib)
{
// compare two unsigned int values, return 1 if difference < 5
	if( suib <= suia)
	{
		if( (suia - suib) < 5)
		{
			return 1;
		}
	}
	else
	{
		if( (suib - suia) < 5)
		{
			return 1;
		}
	}
	return 0;
}

void wal_motor_start( void)
{
	WAL_OP_DIR = (uc_axis_dir == 1) ? 1: 0;
	WAL_OP_PWM = uc_axis_pwm;
	WAL_OP_BRK = 1;
}

void wal_motor_stop( void)
{
	WAL_OP_BRK = 0;
	WAL_OP_PWM = 0;			// NB. Output inversion
}

void UserLoop( void)
{

	// Timer0 overflow should occur every 0.5msec
	if( uc_tprim != uc_tprev)
	{
		uc_tprev = uc_tprim;
		// Seed VisualSTATE tick:
		wal_vss_timer_function();
		// Also 'fast' debounce & switching on inputs:
    		wal_dbnc_inputs();
	}

	// If Rx char received, check if OK & set RX_Event
	// (unless doing TX)
	// ensure only single event until 'action' achieved:
	if( (uc_rxrdptr != uc_rxwrptr) && ( uc_rx_process == 0) )
	{
                uc_rx_process = 1;
                // valid character stored in uc_rx_chbyte
		uc_rx_chrecv = uc_rxbuff[uc_rxrdptr].charin;
		uc_comm_9set = (uc_rxbuff[uc_rxrdptr].status & 0x02) ? 1 : 0;
		// event E_RxChar causes action A_RxProcess
		SEQ_AddEvent( E_RxChar);
		uc_rxrdptr++;
		uc_rxrdptr &= (WAL_MAX_RXBUFF-1);
	}

	// If TXC interrupt flagged:
	// uc_tx_chsent used rather than Add_Event directly to minimize overhead
	if( uc_tx_chsent != 0)
	{
		uc_tx_chsent = 0;
		SEQ_AddEvent(E_TxComplete);
	}

	// NB. Most 'housekeeping' should be within 'state' periodic function.

}

