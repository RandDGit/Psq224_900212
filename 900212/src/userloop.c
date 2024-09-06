/* UserLoop = Application, excluding State Machine */

#include <iom16.h>		 		// Atmel up definitions
#include <inavr.h>				// AVR intrinsic functions
#include "VSMain.h"				// VisualState definitions (further includes)

#include "defs.h"				// Application (GENERAL) defs

// following (global) application data variables declared in "globals.h"

//#define MOTR_2P5R 1

const char wstr_build_date[] = __DATE__;
const char wstr_build_time[] = __TIME__;

unsigned int ui_axis_set;	// Axis Set Point
unsigned int ui_axis_pos;	// Axis Position
unsigned int ui_axis_setrx;     // Axis set pt. rx'd (0.1mm units)
unsigned int ui_axis_postx;     // Axis pos tx'd (normalised 0.1mm)
unsigned int ui_axis_conv;      // Axis ui value for conversion
unsigned int ui_axis_mult;      // 4/5 -> 5/4 conversion
unsigned int ui_axis_tacho;     // NB. tacho unit 0.125mm
unsigned int ui_axis_diff;      // Axis set pt - actual pos difference
unsigned int ui_axis_rup;	// Axis Ramp-up end point
unsigned int ui_axis_rdn;	// Axis Ramp-dn end point
unsigned int ui_axis_tmm;	// Axis Set Value in 1/10th mm units
unsigned int ui_axis_maxallow;  // Axis Max. Allowable travel (soft)

unsigned int ui_axis_uni;       // Axis unidirectional temp. pos

int si_axis_dif;		// Axis Set vs. Actual difference

unsigned char uc_readdr_timer;

unsigned char uc_carrybit;      // used for 0.1mm <--> 0.125mm conversions
unsigned char uc_tprim;	        // Primary timer base (TMR0 ovf)
unsigned char uc_tprev;	        // Prev. value
unsigned char uc_tm0preset;	// Timer 0 preset (controls base period)

unsigned char uc_num_moveslo;   // Record number of axis moves
unsigned char uc_num_moveshi;   // Record number of axis moves
unsigned char uc_num_moves24;   // Record number of axis moves
unsigned char uc_done_move;     // Record move
unsigned char uc_powerup_once;  // Action only once after power-on
unsigned char uc_settlepower;   // Prevent i-o until power-up

unsigned char uc_axis_vss;	// Motor State
unsigned char uc_axis_dir;	// Motor Direction
unsigned char uc_axis_pwm;	// Motor PWM output
unsigned char uc_axis_rud;	// Motor Ramp Up,Down state

unsigned char uc_axis_chi;	// Axis power on count (hi)
unsigned char uc_axis_clo;	// Axis power on count (lo) = ui_axis_pos
unsigned char uc_axis_err;	// Axis error state

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

unsigned char uc_comm_noecho;	// do not respond to comms

unsigned char uca_msgrx[6];     // rx buffer dst:src:cmd:dhi:dlo:cks
unsigned char uca_msgtx[6];     // tx buffer
unsigned char uc_msgrxptr;	// index to rx message = 6 if hi-speed = 5 if lo-speed
unsigned char uc_msgtxprt;	// index to tx message

unsigned char dblead_ip_enc;	// Debounce Lead edge encoder
unsigned char dbtail_ip_enc;
unsigned char val_ip_enc;

unsigned char dblead_ip_trg;	// Debounce Lead edge trigger input
unsigned char dbtail_ip_trg;
unsigned char val_ip_trg;

unsigned char dblead_ip_hom;	// Debounce Lead edge home input
unsigned char dbtail_ip_hom;

unsigned char dblead_ip_lim;	// Debounce Lead edge limit input
unsigned char dbtail_ip_lim;

unsigned char dblead_ip_sw1;	// Debounce Lead edge button input
unsigned char dbtail_ip_sw1;
unsigned char val_ip_sw1;
unsigned char pin_ip_sw1;

unsigned char uc_braketimer;
unsigned char uc_motoron;       // use motor-on status for home switch 'zero'
unsigned char uc_unitravel;     // flag overshoot
unsigned char por_reset;         // capture POR

unsigned char uc_read_portd;    // PIND
unsigned char uc_read_portb;    // PINB

unsigned char uc_stall_active;  // i.e. motor moving
unsigned char uc_stall_error;   // stall condition detected
unsigned char uc_stall_tacin;   // num. of tachos received
unsigned char uc_stall_mslo;    // msec
unsigned char uc_stall_mshi;    // hi msec

unsigned char uc_mstimer_lo;    // lo-count 0.5msec timer
unsigned char uc_mstimer_hi;    // hi-count = 100msec count
unsigned char uc_delay_uni;     // flag to delay direction change

unsigned char uc_tenmstimer;    // counter for 10msec
unsigned char uc_joggle_fsm;
unsigned char uc_joggle_tachs;
unsigned char uc_joggle_dist;
unsigned char uc_joggle_on;
unsigned char uc_joggle_dwell;
unsigned char uc_joggle_wait;
unsigned char uc_joggle_pause;
unsigned char uc_joggle_trap;

unsigned char uc_atbl_err;    // WAL_EE_ERRTBLA
unsigned char uc_atbl_phi;
unsigned char uc_atbl_plo;

unsigned char uc_btbl_err;    // WAL_EE_ERRTBLB
unsigned char uc_btbl_phi;
unsigned char uc_btbl_plo;

unsigned char uc_ctbl_err;    // WAL_EE_ERRTBLC
unsigned char uc_ctbl_phi;
unsigned char uc_ctbl_plo;

unsigned char uc_por_fd;
unsigned char uc_por_lo;
unsigned char uc_por_hi;

unsigned char uc_wr_errtbl;
unsigned char uc_eewr_adr;
unsigned char uc_eewr_val;

unsigned char uc_ee_cpuser_hi;
unsigned char uc_ee_cpuser_lo;

unsigned char uc_ee_calib_hi;
unsigned char uc_ee_calib_lo;

unsigned char uc_eetbl_fsm;
unsigned char uc_eetbl_wrp;
unsigned char uc_eetbl_rdp;


unsigned char uc_eetbl_adr[16];
unsigned char uc_eetbl_val[16];

struct wdef_ctimer ws_ctimer[WDEF_MAX_TIMERS];	// application timer function(s)
struct wdef_ctimer *wsp_tc;			// timer counter implementation

// external variables used by VSS model (critical)
// defined & declared within VSS <System1>Data generated files
// VS_UCHAR val_ip_hom;
// VS_UCHAR val_ip_lim;
// VS_UINT vsstick_stall;
// VS_UINT vsstick_txwait;

#ifdef MOTR_2P5R
unsigned int uc_run_counter = 0;
unsigned int uc_stop_counter = 0;
unsigned int uc_state = 0;
#endif


void wal_calc_setpos_diff( void);
unsigned char wal_pos_similar( void);
void wal_motor_goto( void);
void wal_motor_start( void);
void wal_motor_stop( void);
void wal_timer_counter( void);

void UserLoop( void);


// void wal_vss_timer_function( void);		// VSS Timer (tick) function
void wal_dbnc_inputs( void);                    // input handler level & debounce
void wal_eetbl_fsm( void);
void wal_eetbl_write( unsigned char s_address, unsigned char s_value);


void vss_seq_error( void)
{
// UCC_QUEUE_FULL condition has occurred:
  uc_led_value = WAL_LED_QERROR;	// Status LED (reload value = frequency)
  uc_led_count = 0;			// Status LED (counter)
}

void wal_eetbl_write( unsigned char s_address, unsigned char s_value)
{
// V1.6 Write to EE directly to avoid processor lock-up during write
  uc_eetbl_adr[uc_eetbl_wrp] = s_address;
  uc_eetbl_val[uc_eetbl_wrp] = s_value;
  uc_eetbl_wrp++;
  uc_eetbl_wrp &= 0x0F;
  return;  
}

void wal_joggle_fsm( void)
{
  switch( uc_joggle_fsm)
  {
  case 0:
    if( uc_joggle_on == 1)
      uc_joggle_fsm++;
    break;
  case 1: // move out (wal_motor_start)
    uc_joggle_tachs = uc_joggle_dist;
    WAL_OP_DIR = 0;
    WAL_OP_PWM = WAL_PWM_MAXSP;
    WAL_OP_BRK = 1;
    uc_motoron = 1;
    uc_joggle_fsm++;
    break;
  case 2: // wait move out complete..
    if( uc_joggle_tachs == 0)
    {
      WAL_OP_PWM = 0;
      WAL_OP_BRK = 0;
			uc_braketimer = 200; // 100ms
      uc_motoron = 0;
      uc_joggle_fsm++;
    }
    break;
  case 3: // set-up dwell
    uc_joggle_dwell = WAL_JOG_DWELL;
    uc_joggle_fsm++;
    break;
  case 4: // await dwell
    uc_joggle_dwell--;
    if( !uc_joggle_dwell)
    {
      uc_joggle_fsm++;
    }
    break;
  case 5: // move in
    WAL_OP_DIR = 1;
    WAL_OP_PWM = WAL_PWM_MAXSP;
    WAL_OP_BRK = 1;
    uc_motoron = 1;
    uc_joggle_fsm++;
    break;
  case 6: // await move end:
    uc_joggle_trap = 0;
    uc_joggle_fsm++;
    break;
  case 7: // await move start:
    if( wal_pos_similar() || (uc_joggle_trap > (uc_joggle_dist + 16) ) )
    {
      WAL_OP_BRK = 0;
			uc_braketimer = 200; // 100ms
      WAL_OP_PWM = 0;			// NB. Output inversion
      uc_motoron = 0;
      uc_joggle_fsm++;
    }
    break;
  case 8: // set-up pause
    uc_joggle_wait = uc_joggle_pause;
    uc_joggle_fsm++;
    break;
  case 9: // await pause
    if( uc_joggle_wait == 0)
    {
      uc_joggle_fsm++;
    }
    break;
  case 10: // end (ok)
    uc_joggle_fsm = 0;
    break;
  default:
    uc_joggle_fsm = 0;
  }
}

void wal_eetbl_fsm( void)
{
// V1.6 13JAN06: provide FSM to prevent processor lock-up EE writes

  if( uc_eetbl_wrp != uc_eetbl_rdp)
  {
    // outstand EE write..
    switch( uc_eetbl_fsm)
    {
    case 0:
      // only progress if (EECR & 0x02) not active...
      if( !(EECR & 0x02))
      {
        uc_eetbl_fsm = 1;
      }
      break;
    case 1:
      EEARH = 0;
      EEARL = uc_eetbl_adr[uc_eetbl_rdp];
      EEDR = uc_eetbl_val[uc_eetbl_rdp];
      EECR = 0x04;
      EECR = 0x02;
      uc_eetbl_fsm = 2;
      break;
    case 2:
      // only progress if (EECR & 0x02) not active...
      if( !(EECR & 0x02))
      {
        uc_eetbl_fsm = 3;
      }
      break;
    case 3:
      // end of sequence, increment 'read' pointer and exit
      uc_eetbl_rdp++;
      uc_eetbl_rdp &= 0x0F;
      uc_eetbl_fsm = 0;
      break;
    default: 
      uc_eetbl_fsm = 0;
    }
  }
}

void wal_clear_position( void)
{
// provide function to invalidate position stored value (until at pos)

  wal_eetbl_write( WAL_EE_COUNTHI, 0x32);
  wal_eetbl_write( WAL_EE_COUNTLO, 0x32);
  wal_eetbl_write( WAL_EE_ERROR, 1);
}

void wal_position_store( void)
{
  uc_axis_chi = (unsigned char) (ui_axis_pos >> 8);
  uc_axis_clo = (unsigned char) (ui_axis_pos & 0x00FF);
  uc_axis_err = 0;
  
  wal_eetbl_write( WAL_EE_COUNTHI, uc_axis_chi);
  wal_eetbl_write( WAL_EE_COUNTLO, uc_axis_clo);
  wal_eetbl_write( WAL_EE_ERROR, uc_axis_err);
}

void wal_clear_axis_err( void)
{
  // use EEPROM to store last 3 axis error (& when using POR counter)
  // stored in first to last order a,b,c
  // therefore must push b->c, & a->b
  uc_wr_errtbl = 0;
  
  uc_ctbl_err = uc_btbl_err = 0;
  uc_ctbl_phi = uc_btbl_phi = 0;
  uc_ctbl_plo = uc_btbl_plo = 0;

  uc_btbl_err = uc_atbl_err = 0;
  uc_btbl_phi = uc_atbl_phi = 0;
  uc_btbl_plo = uc_atbl_plo = 0;

  // uc_axis_err..
  uc_atbl_err = uc_axis_err = 0;
  uc_atbl_phi = uc_por_hi = 0;
  uc_atbl_plo = uc_por_lo = 0;

  wal_eetbl_write( WAL_EE_ERRTBLA, uc_atbl_err);
  wal_eetbl_write( WAL_EE_ERRACHI, uc_atbl_phi);
  wal_eetbl_write( WAL_EE_ERRACLO, uc_atbl_plo);

  wal_eetbl_write( WAL_EE_ERRTBLB, uc_btbl_err);
  wal_eetbl_write( WAL_EE_ERRBCHI, uc_btbl_phi);
  wal_eetbl_write( WAL_EE_ERRBCLO, uc_btbl_plo);

  wal_eetbl_write( WAL_EE_ERRTBLC, uc_ctbl_err);
  wal_eetbl_write( WAL_EE_ERRCCHI, uc_ctbl_phi);
  wal_eetbl_write( WAL_EE_ERRCCLO, uc_ctbl_plo);
	
		// POR counter
	wal_eetbl_write( WAL_EE_PORCHI, uc_por_hi);
	wal_eetbl_write( WAL_EE_PORCLO, uc_por_lo);

}

void wal_record_axis_err( void)
{
  // use EEPROM to store last 3 axis error (& when using POR counter)
  // stored in first to last order a,b,c
  // therefore must push b->c, & a->b
  uc_wr_errtbl = 0;
  
  uc_ctbl_err = uc_btbl_err;
  uc_ctbl_phi = uc_btbl_phi;
  uc_ctbl_plo = uc_btbl_plo;

  uc_btbl_err = uc_atbl_err;
  uc_btbl_phi = uc_atbl_phi;
  uc_btbl_plo = uc_atbl_plo;

  // uc_axis_err..
  uc_atbl_err = uc_axis_err;
  uc_atbl_phi = uc_por_hi;
  uc_atbl_plo = uc_por_lo;

  wal_eetbl_write( WAL_EE_ERRTBLA, uc_atbl_err);
  wal_eetbl_write( WAL_EE_ERRACHI, uc_atbl_phi);
  wal_eetbl_write( WAL_EE_ERRACLO, uc_atbl_plo);

  wal_eetbl_write( WAL_EE_ERRTBLB, uc_btbl_err);
  wal_eetbl_write( WAL_EE_ERRBCHI, uc_btbl_phi);
  wal_eetbl_write( WAL_EE_ERRBCLO, uc_btbl_plo);

  wal_eetbl_write( WAL_EE_ERRTBLC, uc_ctbl_err);
  wal_eetbl_write( WAL_EE_ERRCCHI, uc_ctbl_phi);
  wal_eetbl_write( WAL_EE_ERRCCLO, uc_ctbl_plo);

}

void wal_calc_setpos_diff( void)
{
  if( ui_axis_set > ui_axis_pos)
  {
    ui_axis_diff = ui_axis_set - ui_axis_pos;
  }
  else
  {
    ui_axis_diff = ui_axis_pos - ui_axis_set;
  }
}  

unsigned char wal_pos_similar( void)
{
// compare ui_axis_set with ui_axis_pos; return (TRUE) if difference zero or +/- 1
// use wal_calc_setpos_diff() to determine ui_axis_diff

  wal_calc_setpos_diff();
  if( ui_axis_diff < WDEF_TACHO_DEADBAND)
    return 1;
  return 0;
}

unsigned char wal_stop_pos_similar( void)
{
// compare ui_axis_set with ui_axis_pos; return (TRUE) if difference zero or +/- 1
// use wal_calc_setpos_diff() to determine ui_axis_diff

	if( ui_axis_set > ui_axis_pos)
  {
    ui_axis_diff = ui_axis_set - ui_axis_pos;
		if( ui_axis_diff < (WDEF_TACHO_DEADBAND + 1))
			return 1;
  }
  else
  {
    ui_axis_diff = ui_axis_pos - ui_axis_set;
		if( ui_axis_diff < WDEF_TACHO_DEADBAND)
			return 1;
  }
  
  return 0;
}

void wal_motor_goto( void)
{

// mgoto
/*
				; appears to be out of range...
				; a set point of 500mm only has
				; a high byte value of 19!
				; & exclude negative numbers.
	movlw	0xF0		; -'ve & greater than 409.6mm
	andwf	RXDHI,0		; hi byte AND with 0xF0 
	btfss	_zero		; should be zero, if not
	return 			; ignore
	call	readcf		; read confidence byte
	iorlw	.0		; OR with 0
	btfss	_zero		; is zero?
	goto	mgopos		; seems ok
	movlw	.42		; *
	movwf	RXCOM		;
	movlw	.85		; U
	movwf	RXDLO		; 
	return			; do not move if no confidence
mgopos	clrf	SETLO		;
	clrf	SETHI		;
	movfw	GEARR		;
	movwf	COUNT		;
mgoadd	movfw	RXDLO		; load Data lo
	addwf	SETLO,1		; store as low set point
	btfsc	_carry		; > 256
	incf	SETHI,1		; 
	movfw	RXDHI		; load Data hi
	addwf	SETHI,1		; store as high set point
	decfsz	COUNT,1		; multiply by gear ratio
	goto	mgoadd		; keep adding
	call	subcnt		; subtract Set point - Counter = Diff

	movfw	DIFFLO		; for debug return calculated difference
	movwf	RXDLO		; 
	movfw	DIFFHI		; 
	movwf	RXDHI		; 

	btfss	_differr	; negative if bit 7 of DIFFHI set
	goto	gofwd		; not negative so go forward
gorev	comf	DIFFHI,0	; negate DIFFHI -> W
	btfss	_zero		; if zero test DIFFLO
	goto	mgorev		; Difference large so do reverse..
	comf	DIFFLO,0	; negate DIFFLO -> W
	andlw	0xFC		; was difference < -4?
	btfss	_zero		; if zero then do not go backward..
	goto	mgorev		; not zero so reverse
mnogo	movlw	.81		; Q
	movwf	RXCOM		; i.e. error *Q
	return			; return without starting reverse..
mgorev	movlw	OverRun		; alter SET point
	subwf	SETLO,1		; v2.6 unidirection to position
	btfss	_carry		; SETLO - OverRun -> SETLO
	decf	SETHI,1		; OverRun > SETLO, SETHI--
	bsf	_unidir		; make sure unidirection flag indicator set
	bsf	_fwdrev		; set reverse indicator
	call	mrev		; start motor backwards (goto)
	return
gofwd	nop			; Disable unidirectional control 7.11.96
;	movlw	OverRun		; overshoot value
;	addwf	SETLO,1		; add to target value
;	btfsc	_carry		; was addition > 255
;	incf	SETHI,1		; yes, so add to hi-value
	bcf	_unidir		; reset unidirection flag indicator..
	movf	DIFFHI,1	; 1st check hi-byte
	btfss	_zero		; if zero test DIFFLO
	goto	mgofwd		; Difference large so fwd ok
	movfw	DIFFLO		; test DIFFLO
	andlw	0xFC		; is diff > 3
	btfss	_zero		; if zero then do not go forward..
	goto	mgofwd		; not zero so fwd ok
	goto	mnogo		; do not move
mgofwd	bcf	_fwdrev		; reset forward indicator
	call	mfwd		; start motor forwards (GOTO)
	return

*/
}

void wal_motor_start( void)
{
  WAL_OP_DIR = (uc_axis_dir == 1) ? 1: 0;
  // V1.5 15DEC05: always 100% PWM
  // WAL_OP_PWM = uc_axis_pwm;
  WAL_OP_PWM = WAL_PWM_MAXSP;
  WAL_OP_BRK = 1;
  
  uc_motoron = 1;
  
  uc_stall_active = 1;
  uc_stall_tacin = 0;
  uc_stall_mshi = uc_stall_mslo = 0;
}

void wal_motor_stop( void)
{
  /*
  bcf	_brake		; stop motor .. brake active low
  bsf	_enable		; active low
  bcf	_moving		; clear _moving flag
  bsf	_lastdir	; reset _lastdir flag to reverse
  btfss	_direct		; last known direction (reset is fwd)
  bcf	_lastdir	; (0 = CW at hall, 1=CCW)
  */

  WAL_OP_BRK = 0;
	uc_braketimer = 200; // 100ms
  WAL_OP_PWM = 0;			// NB. Output inversion
  
  uc_motoron = 0;
  
  uc_stall_active = 0;
}

void wal_stall_manage( void)
{


  // unsigned char uc_stall_active;  // i.e. motor moving
  // unsigned char uc_stall_error;   // stall condition detected
  // unsigned char uc_stall_tacin;   // num. of tachos received
  // unsigned char uc_stall_mslo;    // msec
  // unsigned char uc_stall_mshi;    // hi msec

  uc_stall_mslo++;
  if( uc_stall_mslo == 200)
  {
    // ~100 msec
    uc_stall_mslo = 0;
    uc_stall_mshi++;
    
    if( uc_stall_mshi > 2)
    {
      // V1.7 reduce 'stall detect period' to avoid 'wrap' of stall counter.
      // stall if less than 5 tachos in 300msec
      if( uc_stall_tacin < 5)
      {
        // error condition
        wal_motor_stop();
        uc_axis_err = 3;
        // wal_eetbl_write( WAL_EE_ERROR, uc_axis_err);
        // V2.4: store in table:
        wal_record_axis_err();
        // V1.6 13JAN06: Force E_AxisError to quit motion state
        // NB. Rationalise to use E_AxisStop (E_AxisStall)
        if( SEQ_AddEvent( E_AxisStall) != UCC_OK )
        {
          vss_seq_error();
        }
      }
      uc_stall_mshi = 0;
      uc_stall_tacin = 0;
    }
  }
}

void UserLoop( void)
{

#ifndef MOTR_2P5R

  // Timer0 overflow should occur every 0.5msec
  if( uc_tprim != uc_tprev)
  {
    uc_tprev = uc_tprim;
    // Seed VisualSTATE tick:
    wal_vss_timer_function();   // VSActionToOutput.c
    // wal_timer_counter();
    
    // Also 'fast' debounce & switching on inputs:
    // WAL_DEBUG_MISO = 1;
    // WAL_OP_TRO = 0;
    wal_dbnc_inputs();
    // WAL_DEBUG_MISO = 0;
    // WAL_OP_TRO = 1;
    
    if( uc_stall_active)
    {
      wal_stall_manage();
    }
    
		// Timer for Brake release
		if (uc_braketimer)
		{
					uc_braketimer--;
			if (uc_braketimer == 0)
			{
				WAL_OP_BRK = 1; // release brake
			}
		}
		
    // V2.2 16MAR06: uc_joggle_wait (using 10msec timer)
    uc_tenmstimer--;
    if( !uc_tenmstimer)
    {
      uc_tenmstimer = 20;
			
			// 10 MS timer
			//
      if( uc_joggle_wait)
      {
        uc_joggle_wait--;
      }
			
      if( uc_wr_errtbl)
      {
        wal_record_axis_err();
      }
			
			if (uc_readdr_timer)
			{
				uc_readdr_timer--;
			}
    }
    
    // V1.6 13JAN06: use FSM to structure EE writes
    wal_eetbl_fsm();
    
    // V2.2 16MAR06: add joggle function:
    wal_joggle_fsm();
    
    // V1.7 16JAN06: use timer function to delay uni-reverse:
    if( uc_delay_uni)
    {
      uc_mstimer_lo++;
      if( uc_mstimer_lo > 199)
      {
        uc_mstimer_lo = 0;
        uc_mstimer_hi++;
        if( uc_mstimer_hi > 1)
        {
				          // i.e. approx. 0.2 sec
          ui_axis_set = ui_axis_uni;
          uc_mstimer_hi = 0;
          uc_unitravel = 0;
          uc_delay_uni = 0;
          if( !wal_pos_similar())
          {
            if( SEQ_AddEvent( E_AxisMove) != UCC_OK )
            {
                    vss_seq_error();
            }
          }
        }
      }
    }
    
  }


  // If TXC interrupt flagged:
  // uc_tx_chsent used rather than Add_Event directly to minimize overhead
  if( uc_tx_chsent != 0)
  {
    
    // character should now be in UDR as 485 RX always on
    // therefore RXC bit should now be set, so clear by reading UDR
    // and then check character in == character out 
    uc_rx_stata = UCSRA;
    if( uc_rx_stata & 0x80)
    {
      uc_rx_statb = UCSRB;
      uc_rx_chbyte = UDR;		// Read UART data register

      UCSRB &= ~(1<<TXB8);	// reset 9th bit

      // character received
      uc_comm_comp = uc_rx_chbyte;
      if( uc_tx_chout != uc_comm_comp)
      {
        uc_comm_flag |= WAL_COMM_MISM;
      }
    }
    else
    {
      uc_comm_flag |= WAL_COMM_MISM;
      UCSRB &= ~(1<<TXB8);	// reset 9th bit
    }

    uc_tx_chsent = 0;

    // only use State Transitions if not hi-speed comms:
    if( uc_comm_hilo == 0)
    {
      // continue (E_TxComplete) directly:
      if( (uc_msgtxprt > 5) || (uc_comm_flag & WAL_COMM_MISM) )
      {
        uc_comm_flag &= ~WAL_COMM_MISM;
        if( SEQ_AddEvent( E_SendComplete) != UCC_OK )
        {
          vss_seq_error();
        }
      }
      else
      {
        // uc_msgtxprt ==> 1,2,3,4,5
        uc_tx_chout = uca_msgtx[uc_msgtxprt];

        // checksum generation:
        if( uc_msgtxprt != 5)
        {
          uca_msgtx[5] += uc_tx_chout;
        }
        uc_msgtxprt++;

        UDR = uc_tx_chout;
        // SEQ_AddEvent( E_TxMoreMsg)
      }
    }
    else
    {
      // Low speed comms:
      if( SEQ_AddEvent( E_TxComplete) != UCC_OK )
      {
        vss_seq_error();
      }
    }
  }
  else
  {
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
      if( SEQ_AddEvent( E_RxChar) != UCC_OK )
      {
        vss_seq_error();
      }
      uc_rxrdptr++;
      uc_rxrdptr &= (WAL_MAX_RXBUFF-1);
    }
  }

  // NB. Most 'housekeeping' should be within 'state' periodic function.

#else               // MOTR_2P5R

  
  
  switch(uc_state) {
	  case 0:
			WAL_OP_BRK = 0;
			WAL_OP_PWM = 0;
			
			if (uc_axis_dir) 
			  uc_axis_dir = 0;
			else
			  uc_axis_dir = 1;
			  
			WAL_OP_DIR = uc_axis_dir;
			uc_state++;
			break;
			
		case 1:
		  uc_run_counter = 8200;
			WAL_OP_PWM = WAL_PWM_MAXSP;
			WAL_OP_BRK = 1;
			uc_state++;
			break;
			
		case 2:
		  if( uc_tprim != uc_tprev) {
        
				uc_tprev = uc_tprim;

				if(uc_run_counter) {
					uc_run_counter--;
					
					if(!uc_run_counter) { 
						WAL_OP_BRK = 0;
						WAL_OP_PWM = 0;						
						uc_stop_counter = 4000;
					}
				}
				
				if(uc_stop_counter) {
				  uc_stop_counter--;
					
					if(!uc_stop_counter) 
					  uc_state = 0;					
				  
				}
				
			}
			break;
	}
	
#endif				      // MOTR_2P5R    
}


