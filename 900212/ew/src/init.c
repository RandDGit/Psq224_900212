#include "iom16.h"			// Atmel definitions Mega16
#include "inavr.h"			// Atmel register definitions
#include "VSMain.h"			// Visual State definitions

#include "defs.h"			// Application (GENERAL) defs
#include "globals.h"		// Application global data/variables

//#define MOTR_2P5R 1

void wal_eetbl_write( unsigned char s_address, unsigned char s_value);
unsigned char wal_bsf_eeget( unsigned char s_address);

// REFERENCE TO HARDWARE 900-152C

void LoadNVData( void)
{
	// read predefined EE locations: 

	// comms ID, vital for comms, DEST & FROM
	// comm ID limited to 0..31
	uc_comm_adid = wal_bsf_eeget( WAL_EE_COMMID);
	if( uc_comm_adid > 37 || uc_comm_adid == 0xFF || uc_comm_adid == 0)
	{
		uc_comm_adid = WAL_UNDEFINED_ID;  // WAL_UNIQUE_COMMID;
	}

	// communications mode baud: 0 = Hi (125k) or 1 = Lo (9600):
	// NB. consider baud rate mode as pre-programmed (pragma def)
	uc_comm_hilo = wal_bsf_eeget( WAL_EE_COMMHILO);
	if( (uc_comm_hilo > 1) || (uc_comm_hilo == 0xFF))
	{
		uc_comm_hilo = 0;     // 0 is hi-speed comms
	}
	// V1.7 16JAN06: Hi-speed comms only:
	uc_comm_hilo = 0;     // 0 is hi-speed comms

	uc_axis_chi = wal_bsf_eeget( WAL_EE_MAXTHI);
	uc_axis_clo = wal_bsf_eeget( WAL_EE_MAXTLO);
        ui_axis_maxallow = (unsigned int) (uc_axis_chi << 8) + uc_axis_clo;

	uc_axis_chi = wal_bsf_eeget( WAL_EE_COUNTHI);
	uc_axis_clo = wal_bsf_eeget( WAL_EE_COUNTLO);
	
	uc_axis_err = wal_bsf_eeget( WAL_EE_ERROR);

    // block re-load if at home or limit position:
    if( uc_axis_err == 0 && ( WAL_IN_HOM == 0) && ( WAL_IN_LIM == 0) )
    {	
	  ui_axis_pos = (unsigned int) (uc_axis_chi << 8);
	  ui_axis_pos += (unsigned int) (uc_axis_clo);
	}
	else
	{
	  ui_axis_pos = 0;
	  uc_axis_err = 6;
	  // axis error 'not' recorded at power on
	}
	
	uc_num_moveslo = wal_bsf_eeget( WAL_EE_MOVELO);
	uc_num_moveshi = wal_bsf_eeget( WAL_EE_MOVEHI);
	uc_num_moves24 = wal_bsf_eeget( WAL_EE_MOVE24);
	if( uc_num_moves24 & 0xC0 )
	{
	  uc_num_moveslo = uc_num_moveshi = uc_num_moves24 = 0;
	}

	
	uc_atbl_err = wal_bsf_eeget( WAL_EE_ERRTBLA);
	uc_atbl_phi = wal_bsf_eeget( WAL_EE_ERRACHI);
	uc_atbl_plo = wal_bsf_eeget( WAL_EE_ERRACLO);
	if( uc_atbl_phi & 0x80)
	{
	  uc_atbl_plo = uc_atbl_phi = uc_atbl_err = 0;
	  uc_wr_errtbl = 1;
	}

	uc_btbl_err = wal_bsf_eeget( WAL_EE_ERRTBLB);
	uc_btbl_phi = wal_bsf_eeget( WAL_EE_ERRBCHI);
	uc_btbl_plo = wal_bsf_eeget( WAL_EE_ERRBCLO);
	if( uc_btbl_phi & 0x80)
	{
	  uc_btbl_plo = uc_btbl_phi = uc_btbl_err = 0;
	}

	uc_ctbl_err = wal_bsf_eeget( WAL_EE_ERRTBLC);
	uc_ctbl_phi = wal_bsf_eeget( WAL_EE_ERRCCHI);
	uc_ctbl_plo = wal_bsf_eeget( WAL_EE_ERRCCLO);
	if( uc_ctbl_phi & 0x80)
	{
	  uc_ctbl_plo = uc_ctbl_phi = uc_ctbl_err = 0;
	}

    // record each power on: (use as reference for errors)
    // WAL_EE_MCUCSR
    uc_por_hi = wal_bsf_eeget( WAL_EE_PORCHI);
    uc_por_lo = wal_bsf_eeget( WAL_EE_PORCLO);
    uc_por_fd = uc_por_lo;
    
    if( uc_por_hi & 0x80)
    {
      uc_por_lo = uc_por_hi = 0;
      wal_eetbl_write( WAL_EE_PORCHI, uc_por_hi);
    }
    
    uc_por_fd = uc_por_lo;

    // NB. compiler may skip next line!?
    uc_por_lo++;
    if( uc_por_fd == uc_por_lo)
      uc_por_lo++;

    // and the next ?    
    wal_eetbl_write( WAL_EE_PORCLO, uc_por_lo);
    // ?
    if( uc_por_lo == 0)
    {
      uc_por_hi++;
      wal_eetbl_write( WAL_EE_PORCHI, uc_por_hi);
    }

    uc_ee_cpuser_hi = wal_bsf_eeget( WAL_EE_CPUSERHI);
    uc_ee_cpuser_lo = wal_bsf_eeget( WAL_EE_CPUSERLO);

    uc_ee_calib_hi = wal_bsf_eeget( WAL_EE_CALIBHI);
    uc_ee_calib_lo = wal_bsf_eeget( WAL_EE_CALIBLO);
    //if( uc_ee_calib_hi & 0x80)
    // {
    //  uc_ee_calib_lo = uc_ee_calib_hi = 0;
    //  wal_eetbl_write( WAL_EE_CALIBHI, uc_ee_calib_hi);
    //  wal_eetbl_write( WAL_EE_CALIBLO, uc_ee_calib_lo);
    // }
}

void InitOutputs( void)
{
// Once device had been correctly setup (Port I/O, and peripheral usage)
// ensure all ports are correctly configured as per idle state:

	WAL_OP_PWM = 0;		// PWM OC2 output (=Byte)
	WAL_OP_TRO = 1;		// Trigger default = Hi
	WAL_OP_LED = 0;		// LED1 (Lo = ON)
	WAL_OP_DIR = 0;		// Direction towards Home
	WAL_OP_BRK = 0;		// Brake (Motor Enable)
	uc_braketimer = 200; // 100ms
	
    // WAL_DEBUG_MISO = 1;
    // WAL_DEBUG_MISO = 0;

}

void InitData( void)
{
  // InitData called before 'LoadNVData'..

	uc_readdr_timer = 0;
	
	uc_braketimer = 0;
	
	vsstick_stall = 240;
	vsstick_txwait = 3;

	uc_tprim = 0;
	uc_tprev = 0;

	uc_rxrdptr = uc_rxwrptr = 0;

	uc_tx_chsent = 0;
	uc_rx_process = 0;

	uc_led_value = WAL_LED_DEFAULT;	// Status LED (reload value = frequency)
	uc_led_count = 0;				// Status LED (counter)

	dblead_ip_enc = WAL_DBNC_IN_ENC;
	dbtail_ip_enc = WAL_DBNC_IN_ENC;
	val_ip_enc = 2;			// set = 2 ==> don't care

	dblead_ip_trg = WAL_DBNC_IN_TRG;
	dbtail_ip_trg = WAL_DBNC_IN_TRG;
	val_ip_trg = 0;			// assume (set) trigger inactive at POR

	dblead_ip_hom = WAL_DBNC_IN_HOM;
	dbtail_ip_hom = 0;
	val_ip_hom = 0;			// assume (set) home inactive at POR

	dblead_ip_lim = WAL_DBNC_IN_LIM;
	dbtail_ip_lim = 0;
	val_ip_lim = 0;			// assume (set) limit inactive at POR

	dblead_ip_sw1 = 0;              // start with IN_SW1 set at hi
	dbtail_ip_sw1 = WAL_DBNC_IN_SW1;
	val_ip_sw1 = 0;			// assume (set) button inactive at POR


        // V1.6 12JAN06: setting uc_axis_dir = 1 forces 'reverse' on first button press
        uc_powerup_once = 1;
        uc_axis_dir = 1;    
        uc_motoron = 0;
        uc_unitravel = 0;

	    uc_wr_errtbl = 0;
        
        uc_settlepower = 200;     // Prevent i-o until power-up

        
	// set comm_id must be unique for this hardware
	// uc_comm_adid = WAL_UNIQUE_COMMID;

	// clear counter timer and setup functions
	ws_ctimer[0].exp_routine = NULL;
	ws_ctimer[1].exp_routine = NULL;
	ws_ctimer[2].exp_routine = NULL;
	ws_ctimer[3].exp_routine = NULL;
	ws_ctimer[4].exp_routine = NULL;
	ws_ctimer[5].exp_routine = NULL;
	ws_ctimer[6].exp_routine = NULL;
	ws_ctimer[7].exp_routine = NULL;
}



void InitDevice(void)
{
// __disable_interrupt();

  por_reset = MCUCSR;
  // MCUCSR = 0;
  MCUCSR &= ~0x01;
  MCUCSR &= ~0x02;
  MCUCSR &= ~0x04;
  MCUCSR &= ~0x08;
  MCUCSR &= ~0x10;

  MCUCSR |= 0x80;
  /* Configure PortA..  +28V */
  DDRA = 0x00;
  /* Port B..7..0 , rxtx_en, op_tr0, in_trg, in_enc, heart*/
  /* V1.6 13JAN06: use PB.6 as debug pin WAL_DEBUG_MISO */
  /* DDRB = 0x19; */
  DDRB = 0x59;
  /* Port C.. n/c */
  DDRC = 0x00;
  /* Port D.7..0. OP_PWM, IN_SW1, OP_BRK, OP_DIR, IN_LIM, IN_HOM, TX, RX */
  DDRD = 0xB0;
	
	#ifdef MOTR_2P5R
  //_WDR();
  WDTCR |= (1<<WDTOE) | (1<<WDE);
  WDTCR = 0x00;	
	#else
  WDTCR = 0x0F;
	#endif

  // Configure Timer 0 to expire for each ~1 ms.
  // TCCR0 = 0x03;   		// Start the timer by prescale CK/64
  // mTCNT = 193;    		// Load timer to expire for each ~1ms
  TCCR0 = 0x02;   			// Start the timer by prescale CK/8 (2us/pulse)
  // uc_tm0preset = 206;    // Load timer to expire for each ~0.1ms (400 instr/int)
  uc_tm0preset = 8;    		// Load timer to expire for each ~0.5ms (2000 instr/int)
							// 256 - 250 = 6, but allow for latency (16 instr)

  TCNT0 = uc_tm0preset;  	// Load the timer counter register
  TIMSK |= 0x01;  		// Enable Timer0 overflow interrupt

  MCUCR |= 0X0A;    		// Falling edge int0, int1 and int2
							// However, INT0, INT1, INT2 not used
  // GICR |= (1<<INT1);  GICR |= (1<<INT0);   GICR |= (1<<INT2); 

  // Setup TIMER2 = PWM output on OC2
  // Phase correct PWM (WGM20), Set OC2 on compare (COM21,20), Clk/1024 = (CS22,21,20) = 0x07
  // Clk/8 | (1<<CS22)  | (1<<CS20)
  TCCR2 = (1<<WGM20) | (1<<COM21) | (1<<COM20) | (1<<CS21);
  OCR2 = 0;

  ASSR = 0;					// non Asynchronous operation
  
  // __enable_interrupt();
  
}



