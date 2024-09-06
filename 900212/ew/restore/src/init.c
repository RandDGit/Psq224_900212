#include "iom16.h"			// Atmel definitions Mega16
#include "inavr.h"			// Atmel register definitions
#include "VSMain.h"			// Visual State definitions

#include "defs.h"			// Application (GENERAL) defs
#include "globals.h"		// Application global data/variables


// REFERENCE TO HARDWARE 900-152C

void LoadNVData( void)
{
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

}

void InitData( void)
{

	vsstick_stall = 40;
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
	dbtail_ip_hom = WAL_DBNC_IN_HOM;
	val_ip_hom = 0;			// assume (set) home inactive at POR

	dblead_ip_lim = WAL_DBNC_IN_LIM;
	dbtail_ip_lim = WAL_DBNC_IN_LIM;
	val_ip_lim = 0;			// assume (set) limit inactive at POR

	dblead_ip_sw1 = WAL_DBNC_IN_SW1;
	dbtail_ip_sw1 = WAL_DBNC_IN_SW1;
	val_ip_sw1 = 0;			// assume (set) button inactive at POR

	// set comm_id must be unique for this hardware
	uc_comm_adid = WAL_UNIQUE_COMMID;

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

  MCUCSR |= 0x80;
  /* Configure PortA..  +28V */
  DDRA = 0x00;
  /* Port B..7..0 , rxtx_en, op_tr0, in_trg, in_enc, heart*/
  DDRB = 0x19;
  /* Port C.. n/c */
  DDRC = 0x00;
  /* Port D.7..0. OP_PWM, IN_SW1, OP_BRK, OP_DIR, IN_LIM, IN_HOM, TX, RX */
  DDRD = 0xB0;

  WDTCR = 0x0F;

  // Configure Timer 0 to expire for each ~1 ms.
  // TCCR0 = 0x03;   		// Start the timer by prescale CK/64
  // mTCNT = 193;    		// Load timer to expire for each ~1ms
  TCCR0 = 0x02;   			// Start the timer by prescale CK/8 (2us/pulse)
  // uc_tm0preset = 206;    // Load timer to expire for each ~0.1ms (400 instr/int)
  uc_tm0preset = 8;    		// Load timer to expire for each ~0.5ms (2000 instr/int)
							// 256 - 250 = 6, but allow for latency (16 instr)

  TCNT0 = uc_tm0preset;  	// Load the timer counter register
  TIMSK |= 0x01;  			// Enable Timer0 overflow interrupt

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



