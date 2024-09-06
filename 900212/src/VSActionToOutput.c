/*****************************************************************************
* IAR visualSTATE action functions and error handler.
* i.e. flesh out functions corresponding to System1action.h (VisualSTATE)
*****************************************************************************/

#include <iom16.h>		 	// Atmel up definitions
#include <inavr.h>			// AVR intrinsic functions
#include "VSMain.h"			// VisualState definitions (further includes)
#include "system1Action.h"              // VisualState generated 'action' functions:

#include "usart2.h"			// Application (UART) defs
#include "defs.h"			// Application (GENERAL) defs
#include "globals.h"		// Application global data/variables


unsigned char wal_pos_similar( void);
unsigned char wal_stop_pos_similar( void);
void wal_eetbl_write( unsigned char s_address, unsigned char s_value);

void wal_motor_start( void);
void wal_motor_stop( void);
void wal_position_store( void);

void InitData( void);
void LoadNVData( void);	        // read eeprom (nv data)
void wal_do_command( void);
void wal_picnet_motor( void);

void wal_clear_position( void);

void wal_setup_vss_timer( void);
void wal_vss_timer_function( void);
void vss_seq_error( void);

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
	wvt_period.event = E_Periodic;		        // results in A_Periodic
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

// could use timer ->
// if timer->active == 1 .. if timer->ticks ... timer->event

	if( wvt_period.active == 1)
	{
		if( wvt_period.ticks != 0)
		{
			wvt_period.ticks--;
			if( wvt_period.ticks == 0)
			{
				if( SEQ_AddEvent( wvt_period.event) != UCC_OK )
				{
					vss_seq_error();
				}
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
				if( SEQ_AddEvent( wvt_stall.event) != UCC_OK )
				{
					vss_seq_error();
				}
				wvt_stall.active = 0;
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
				if( SEQ_AddEvent( wvt_txwait.event) != UCC_OK )
				{
					vss_seq_error();
				}
			}
		}
	}
}


/* REFER TO VS generated "System1action.h" */
/* Last updated 08DEC04 */

/* 

extern VS_VOID A_AxisCalcProfile (VS_VOID);
extern VS_VOID A_AxisPosUpdate (VS_VOID);
extern VS_VOID A_ForceStop (VS_VOID);
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

// use difference to determine motor direction (uc_axis_dir)
// prepare ramp-up & ramp-down profile

// negative difference so move in reverse
// decrease 'set point' for uni-directional motion,
// and set unidirection flag:

  // V1.7 16JAN06: always set 'uni' == 'set'
  ui_axis_uni = ui_axis_set;
  // clear 'uni-directional flag' (set later if required)
  uc_unitravel = 0;

  // if( ui_axis_set == ui_axis_pos)
  if( wal_pos_similar() )
  {
    if( SEQ_AddEvent( E_AxisSetPoint) != UCC_OK )
    {
      vss_seq_error();
    }
    return;
  }

  if( ui_axis_set > ui_axis_pos)
  {
    uc_axis_dir = 1;
    ui_axis_diff = ui_axis_set - ui_axis_pos;
    // calculate ramp profile; pwm output value (STEPA) set in A_MotorOn
    return;
  }
  
  // V1.6 12JAN06: implement UNI-DIRECTIONAL travel!
  // Just force position less than intended, let 'top' control re-send move
  // (A_MotorMove)
  // if( ui_axis_set > WAL_UNI_TRAVEL)
  // {
  //   ui_axis_set -= WAL_UNI_TRAVEL;
  // }
  // else force re-reference.... (maybe set ui_axis_set == 0)
  
  // V1.7 16JAN06: re-implement UNI-DIRECTIONAL travel:
  // but include auto-repositioning based upon internal flag...
  uc_unitravel = 1;     // force 're-position' at end of travel
  // V1.7.. use A_MotorIdle to force move (E_AxisMove) if uc_unitravel
  
  ui_axis_set = 0;
  if( ui_axis_uni > WAL_UNI_TRAVEL)
  {
    ui_axis_set = (ui_axis_uni - WAL_UNI_TRAVEL);
  }
  
  
  uc_axis_dir = 0;
  ui_axis_diff = ui_axis_pos - ui_axis_set;
  // calculate ramp profile

}

VS_VOID A_AxisPosUpdate (VS_VOID)
{
// action due to E_Tacho (update counter position)
// NB. uc_axis_dir fwd == 1, rev == 0

  // uc_stall_tacin++; updated within tacho input routine
  
  // V1.6 13JAN06: rationalise 'tacho' position update within input
  // only handle 'motion' (vss==2) logic within this function...

  if( wal_stop_pos_similar() )
  {
    wal_motor_stop();
    wal_position_store();
    if( SEQ_AddEvent( E_AxisSetPoint) != UCC_OK )
    {
      vss_seq_error();
    }
    return;
  }

  if( ui_axis_pos > WAL_MAX_AXIS_TACHO )
  {
    wal_motor_stop();
    uc_axis_err = 7;
    // V2.4: store in table:
    wal_record_axis_err();
    if( SEQ_AddEvent( E_AxisError) != UCC_OK )
    {
            vss_seq_error();
    }
  }
  
  // check ramp-up, ramp-down conditions:
  // NB. wal_pos_similar calls wal_calc_setpos_diff()
  // therefore ui_axis_diff already determined...
  
  ui_axis_rup++;
  
  /*
  if( ui_axis_diff < WAL_PWM_ZONEE)
  {
    WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPA;
    return;
  }
  
  if( ui_axis_diff < WAL_PWM_ZONED)
  {
    if( ui_axis_rup < WAL_PWM_ZONEE) 
      WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPA;
    else
      WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPB;
    return;
  }

  if( ui_axis_diff < WAL_PWM_ZONEC)
  {
    if( ui_axis_rup < WAL_PWM_ZONEE) 
    {
      WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPA;
    }
    else
    {
      if( ui_axis_rup < WAL_PWM_ZONED)
        WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPB;
      else
        WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPC;
    }
    return;
  }

  if( ui_axis_diff < WAL_PWM_ZONEB)
  {
    if( ui_axis_rup < WAL_PWM_ZONEE) 
    {
      WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPA;
    }
    else
    {
      if( ui_axis_rup < WAL_PWM_ZONED)
      {
        WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPB;
      }
      else
      {
        if( ui_axis_rup < WAL_PWM_ZONEC)
          WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPC;
        else
          WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPD;
      }
    }
    return;
  }

  if( ui_axis_diff < WAL_PWM_ZONEA)
  {
    if( ui_axis_rup < WAL_PWM_ZONEE) 
    {
      WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPA;
    }
    else
    {
      if( ui_axis_rup < WAL_PWM_ZONED)
      {
        WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPB;
      }
      else
      {
        if( ui_axis_rup < WAL_PWM_ZONEC)
        {
          WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPC;
        }
        else
        {
          if( ui_axis_rup < WAL_PWM_ZONEB)
            WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPD;
          else
            WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPE;
        }
      }
    }
    return;
  }
  */

  // no ramp down in effect; therefore check if ramp up only:
  // V1.5 15DEC05: 100% PWM on ramp up:
  /*
  if( ui_axis_rup < WAL_PWM_ZONEA)              // ZONEA is largest
  {
    WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPE;   // STEPE is below max
    if( ui_axis_rup < WAL_PWM_ZONEB)
      WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPD;
    if( ui_axis_rup < WAL_PWM_ZONEC)
      WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPC;
    if( ui_axis_rup < WAL_PWM_ZONED)
      WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPB;
    if( ui_axis_rup < WAL_PWM_ZONEE)
      WAL_OP_PWM = uc_axis_pwm = WAL_PWM_STEPA;   // STEP A is slowest
  }
  else
  {
    WAL_OP_PWM = uc_axis_pwm = WAL_PWM_MAXSP;     // Max. travel velocity
  }
  */
  
  // V1.6 13JAN06: Ramp-up/down PWM disabled, set PWM dependent on function 
  // WAL_OP_PWM = uc_axis_pwm = WAL_PWM_MAXSP;     // Max. travel velocity
    
}

VS_VOID A_ForceStop (VS_VOID)
{
  if( SEQ_AddEvent( E_AxisStop) != UCC_OK )
  {
    vss_seq_error();
  }
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

  // WAL_DEBUG_MISO = 0;

  if( WAL_IN_SW1 == 0)
  {
    // SW1 button depressed at power up
    // start timer... if still depressed after <n> seconds
    // reset comms address?
  }
  // SEQ_AddEvent(E_AxisInvalid);
}
VS_VOID A_MotorError (VS_VOID)
{
  // response to stall, limit detect E_AxisError
  if( SEQ_AddEvent( E_AxisError) != UCC_OK )
  {
          vss_seq_error();
  }
}

VS_VOID A_MotorFwd (VS_VOID)
{
// called upon Axis Home Event
  wal_motor_stop();
  uc_axis_dir = 1;
  // V1.6 13JAN06: Ramp-up/down PWM disabled, set PWM dependent on function 
  // WAL_OP_PWM = uc_axis_pwm = WAL_PWM_MAXSP;     // Max. travel velocity
  uc_axis_pwm = WAL_PWM_HOME;
  wal_motor_start();
}
VS_VOID A_MotorIdle (VS_VOID)
{
  // set internal axis state for status
  uc_axis_vss = 0;

  uc_done_move = 1;
  if( uc_powerup_once )
  {
    uc_powerup_once = 0;
    uc_done_move = 0;
  }
  
  // V1.7 16JAN06: self-contained 'uni-directional travel'
  // change in direction delayed using 'uc_delay_uni'

  if( uc_unitravel)
  {
    uc_delay_uni = 1;
    uc_mstimer_lo = uc_mstimer_hi = 0;
    uc_done_move = 0;
  }
}

VS_VOID A_MotorMove (VS_VOID)
{
  // set internal axis state for status
  uc_axis_vss = 2;
  // clear stored position for recall:
  wal_clear_position();
}

VS_VOID A_MotorOn (VS_VOID)
{
  // uc_axis_dir = DIR;
  // uc_axis_pwm = PWM;
  
  // Brake Off, Direction Set, Enable = PWM Output
  uc_axis_rud = 1;	          // ramp up
  ui_axis_rup = 0;                // reset 'ramp-up' counter
  // uc_axis_pwm = WAL_PWM_STEPA;
  // V1.6 13JAN06: Ramp-up/down PWM disabled, set PWM dependent on function 
  WAL_OP_PWM = uc_axis_pwm = WAL_PWM_MAXSP;     // Max. travel velocity
  
  wal_motor_start();
}

VS_VOID A_MotorReRef (VS_VOID)
{
  // set internal axis state for status
  uc_axis_vss = 1;
  
  wal_clear_position();
  
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
  
  // prevent 
  wal_motor_start();
}

VS_VOID A_MotorStop (VS_VOID)
{
  uc_axis_vss = 0;
  wal_motor_stop();
  wvt_stall.active = 0;
}

VS_VOID A_MotorTest (VS_VOID)
{
  // V1.6 13JAN06: if in 'motion' state stop & set error (A_Handle)
  if( uc_axis_vss == 2)
  {
    wal_motor_stop();
    uc_axis_err = 2;
    // V2.4: store in table:
    wal_record_axis_err();
    // Rationalise E_AxisError --> E_AxisStop...
    if( SEQ_AddEvent( E_AxisError) != UCC_OK )
    {
      vss_seq_error();
    }
    return;
  }
  
  // primary action of state reached from idle due to E_Button
  // set internal axis state for status
  uc_axis_vss = 3;
  
  // V1.5 15DEC05: use 100% PWM button:
  // uc_axis_pwm = WAL_PWM_TEST;
  uc_axis_pwm = WAL_PWM_MAXSP;
  
  vsstick_stall = 4000;
  
  if( uc_axis_dir)
  {
    if( SEQ_AddEvent( E_AxisRev) != UCC_OK )
    {
      vss_seq_error();
    }
  }
  else
  {
    if( SEQ_AddEvent( E_AxisFwd) != UCC_OK )
    {
      vss_seq_error();
    }
  }
  
  // force test transmission: E_SendRequest
  uca_msgtx[0] = 0x27;    	        // send to dummy bin
  uca_msgtx[1] = uc_comm_adid; 		// from me...
  uca_msgtx[2] = WAL_OP_PWM;    	// current PWM value
  uca_msgtx[3] = PINB;    	        // 
  uca_msgtx[4] = PIND;    	        // ..
  if( SEQ_AddEvent( E_SendRequest) != UCC_OK )
  {
    vss_seq_error();
  }
}

VS_VOID A_Periodic (VS_VOID)
{
// This function is indicative of concurrent state activity.
// A_Periodic is called upon E_Periodic due to timer TM_Status 
// tick value = 10, i.e. every 5msec

  __watchdog_reset();
  
  if( uc_settlepower)
  {
    uc_settlepower--;
  }
  
  // V1.7 16JAN06: add 'move-count' feature:
  if( uc_done_move)
  {
    uc_done_move = 0;
    uc_num_moveslo++;
    if( uc_num_moveslo == 0)
    {
      uc_num_moveshi++;
      if( uc_num_moveshi == 0)
      {
        uc_num_moves24++;
      }
    }
    wal_eetbl_write( WAL_EE_MOVELO, uc_num_moveslo);
    wal_eetbl_write( WAL_EE_MOVEHI, uc_num_moveshi);
    wal_eetbl_write( WAL_EE_MOVE24, uc_num_moves24);
  }

  // Indicate current status using LED1
  if( uc_led_scale == 0)
  {
    uc_led_scale = WAL_LED_SCALE;		// Scale factor to LED timebase
    uc_led_count++;				// use uc_led_count wrt value to set flash rate

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
    // V1.7 16JAN06: ensure LED flashes even if limits active
    if( uc_led_scale == 0)
    {
      if( (val_ip_hom == 1) || (val_ip_lim == 1))
      {
        WAL_OP_LED = 0;
      }
    }
  }
}


VS_VOID A_RxDoComms (VS_VOID)
{
	// E_RxMsg, valid message received:
	// if valid address (i.e. match or global process message)
	// process according to hi or lo-speed comms:
    if( (uca_msgrx[0] == WAL_COMMS_GLOBAL) || (uca_msgrx[0] == uc_comm_adid) )
    {
        // assume response (echo) always (unless no-echo specifically set)
        uc_comm_noecho = 0;

        // response to message (transition to Sending state)
        wal_do_command();

        // NB. reply may be suppressed:
        if( uc_comm_noecho == 0)
        {
            if( SEQ_AddEvent( E_SendRequest) != UCC_OK )
            {
                vss_seq_error();
            }
        }
    }

    // usually response required: E_SendRequest
    // in any case, reset receive buffer & flags:
    uc_comm_flag &= ~WAL_COMM_DERR;
    uc_comm_flag &= ~WAL_COMM_DRDY;
    uc_msgrxptr = 0;
    uc_rx_chksum = 0;
}

VS_VOID A_RxProcess (VS_VOID)
{
// structure uc_rxbuff( charin & status) loaded into uc_rx_chrecv & uc_comm_9set

  uc_rx_process = 0;				// reset rx processed flag (semaphore)

  // if error detected then reset any message (in) in-progress
  if( (uc_comm_flag & WAL_COMM_DERR) == WAL_COMM_DERR)
  {
    // reset error, but clear any existing in-progress:
    uc_comm_flag &= ~WAL_COMM_DERR;
    uc_comm_flag &= ~WAL_COMM_DRDY;
    uc_msgrxptr = 0;
    uc_rx_chksum = 0;

    // uc_rx_process = 0;	// reset rx processed flag (semaphore)
    return;
  }

  // process according to hi or lo-speed comms:
  // Hi-speed 9-bit comms
  // Check if 9th bit set: NB UCSRB bit.1 has RXB8 if so = start of message
  if( uc_comm_9set)
  {
    uca_msgrx[0] = uc_rx_chksum = uc_rx_chrecv;
    uc_msgrxptr = 1;
    uc_comm_flag |= WAL_COMM_DRDY;
    return;
  }
  
  // check that address byte has been received:
  if( (uc_comm_flag & WAL_COMM_DRDY) != WAL_COMM_DRDY)
  {
    // ignore: byte received but not valid 'start of message'.
    return;
  }
  
  // at this point buffer (uc_rxrdptr != uc_rxwrptr)
  // may contain complete message received
  if( uc_msgrxptr < 5)
  {
    uc_rx_chksum += uc_rx_chrecv;
    uca_msgrx[uc_msgrxptr] = uc_rx_chrecv;
    uc_msgrxptr++;

    // maybe use conditional flag to break while loop:
    while( uc_rxrdptr != uc_rxwrptr )
    {
      uc_rx_chrecv = uc_rxbuff[uc_rxrdptr].charin;
      uc_comm_9set = (uc_rxbuff[uc_rxrdptr].status & 0x02) ? 1 : 0;
      uc_rxrdptr++;
      uc_rxrdptr &= (WAL_MAX_RXBUFF-1);
      if( uc_comm_9set)
      {
        uca_msgrx[0] = uc_rx_chksum = uc_rx_chrecv;
        uc_msgrxptr = 1;
        uc_comm_flag |= WAL_COMM_DRDY;
        // break while loop
        break;
      }
      else
      {
        if( uc_msgrxptr < 5)
        {
          uc_rx_chksum += uc_rx_chrecv;
          uca_msgrx[uc_msgrxptr] = uc_rx_chrecv;
          uc_msgrxptr++;
        }
        else
        {
          // break while loop
          uc_comm_flag &= ~WAL_COMM_DRDY;
          // if checksum matches then this is a valid message:
          if( uc_rx_chksum == uc_rx_chrecv)
          {
            if( SEQ_AddEvent( E_RxMsg) != UCC_OK )
            {
              vss_seq_error();
            }
          }
          break;
        }
      }
    }		// while RX buffer loaded
  }
  else
  {
    // should have received last byte of message, so clear DRDY
    uc_comm_flag &= ~WAL_COMM_DRDY;
    // if checksum matches then this is a valid message:
    if( uc_rx_chksum == uc_rx_chrecv)
    {
      if( SEQ_AddEvent( E_RxMsg) != UCC_OK )
      {
        vss_seq_error();
      }
    }
  }

  // uc_rx_process = 0;				// reset rx processed flag (semaphore)
}

VS_VOID A_TxChar (VS_VOID)
{
// TxChar state (initial entry & TxMoreMsg)

  uc_tx_chout = uca_msgtx[uc_msgtxprt];

  if( uc_comm_hilo == 0)
  {
    if( uc_msgtxprt < 5)
    {
      // checksum generation:
      uca_msgtx[5] += uc_tx_chout;
      uc_msgtxprt++;
    }
  }
  else
  {
    if( uc_msgtxprt < 4)
    {
      // checksum generation:
      uca_msgtx[4] ^= uc_tx_chout;
      uc_msgtxprt++;
    }
  }

  uc_comm_flag &= ~WAL_COMM_MISM;

  UDR = uc_tx_chout;
  // reset 9th bit once character TXC
}

VS_VOID A_TxEnableOff (VS_VOID)
{
  // end of Tx Message
  PORTB = (PINB & ~0x10);		// Switch off line driver
  UCSRB |= (1<<RXCIE);    	        // enable RX interrupt

  uc_comm_flag &= ~WAL_COMM_SMSG;	// Flag data sent (msgtx complete)
  
  // force re-do of RX char event if uc_rx_process set
  if( uc_rx_process != 0)
  {
    if( SEQ_AddEvent( E_RxChar) != UCC_OK )
    {
      vss_seq_error();
    }
  }
}

VS_VOID A_TxEnableOn (VS_VOID)
{

    // uc_tx_chout = uca_msgtx[0];

    // disable rx interrupt & setup TX_En 
    
    UCSRB &= ~(1<<RXCIE);		// disable rx interrupt
    PORTB = (PINB | 0x10);  		// Switch line driver on
    // ? set address bit ?
    uc_comm_flag |= WAL_COMM_SMSG;	// Transmission in progress

    // start of Tx Message
    uc_msgtxprt = 0;

    if( uc_comm_hilo == 0)
    {
      uca_msgtx[5]= 0;	// clear checksum
      // if hi-speed then set 9bit (= start of message)
      UCSRB |= (1<<TXB8);
    }
    else
    {
      uca_msgtx[4]= 0;	// clear checksum
    }

}

VS_VOID A_TxProcessMsg (VS_VOID)
{
// response to E_TxComplete i.e. TXC event

	// test if error condition exists, if so then return to wait state (retry send)
	if( (uc_comm_flag & WAL_COMM_MISM) == WAL_COMM_MISM)
	{
		// i.e. abort send message
		// but must also end transmission:
		// so either quit 'sending' state completely
		// (let master resend original message...)
		// using E_SendComplete
		
		// or remain in 'sending' state, & retry
		// using E_TxBusIdle ??
		
		if( SEQ_AddEvent( E_SendComplete) != UCC_OK )
		{
			vss_seq_error();
		}
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
			if( SEQ_AddEvent( E_TxMoreMsg) != UCC_OK )
			{
				vss_seq_error();
			}
		}
	}
	else
	{
		if( uc_msgtxprt >= 4)
		{
			if( SEQ_AddEvent( E_SendComplete) != UCC_OK )
			{
				vss_seq_error();
			}
		}
		else
		{
			if( SEQ_AddEvent( E_TxMoreMsg) != UCC_OK )
			{
				vss_seq_error();
			}
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

		if( uc_comm_hilo == 0)
		{
			// different transition required if hi-speed comms:
			if( SEQ_AddEvent( E_TxHiSpeed) != UCC_OK )
			{
				vss_seq_error();
			}
		}
		else
		{
			if( SEQ_AddEvent( E_TxBusIdle) != UCC_OK )
			{
				vss_seq_error();
			}
		}
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
	// V1.6 13JAN06: disable this method of stall detection
	wvt_stall.active = 0; // 1->0
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



