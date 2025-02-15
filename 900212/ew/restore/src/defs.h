/******************************** #defines section ****************************/

#define def_major_ver    0
#define def_minor_ver    0
#define def_mini_change  0

#define WAL_UNIQUE_COMMID	30	/* Multi-bus comms ID */
#define WAL_COMMS_GLOBAL	255	/* Global address */

#define WAL_DBNC_IN_ENC 2
#define WAL_DBNC_IN_TRG	10
#define WAL_DBNC_IN_HOM	10
#define WAL_DBNC_IN_LIM	10
#define WAL_DBNC_IN_SW1	60

#define WAL_MAX_AXIS_TACHO 0xFFFF

#define WAL_LED_SCALE	20		/* scale periodic timer to provide longer period */
#define WAL_LED_DEFAULT	5		/* period (scaled) for status LED */

#define WAL_MAX_RXBUFF	32		/* size of rx buffer, must be 2^n */

// uc_comm_flag (comms status & control)
#define WAL_COMM_DRDY 	0x01		// cflags.bit0 'Data-received' flag 
#define WAL_COMM_DERR  	0x02		// cflags.bit1 Software Rx error
#define WAL_COMM_IDLE  	0x04		// cflags.bit2 'Comms line idle' flag
#define WAL_COMM_SMSG  	0x08		// cflags.bit3 Message out in progress
#define WAL_COMM_MSGI	0x10		// Message 'in' in-progress
#define WAL_COMM_MISM	0x20		// ch in != ch out (Tx integrity verification)

#define WAL_OP_LED	(PORTB_Bit0)
#define WAL_OP_TRO	(PORTB_Bit3)
#define WAL_OP_DIR	(PORTD_Bit4)
#define WAL_OP_BRK	(PORTD_Bit5)
#define WAL_OP_PWM	(OCR2)

#define WAL_PWM_TEST	191		/* 75% output on test */

#define WAL_PWM_RAMPUP	128		/* PWM ramp up output (50%) */
#define WAL_PWM_HOME	89		/* PWM during home sequence (35%) */

/* Motor start-stop ramp-up ramp-down */
/* Motor approx. 3000 rpm = 50 rps = 400 pps */
/* 2 sec up-down --> ~800 pulses */

#define WAL_PWM_STEPA	128		/* 50% PWM output within zone A */
#define WAL_PWM_ZONEA	400		/* zone (pulses) within set point */
#define WAL_PWM_STEPB	153		/* 60% PWM output within zone B */
#define WAL_PWM_ZONEB	200		/* zone (pulses) within set point */
#define WAL_PWM_STEPC	178		/* 70% PWM output within zone C */
#define WAL_PWM_ZONEC	100		/* zone (pulses) within set point */
#define WAL_PWM_STEPD	204		/* 80% PWM output within zone D */
#define WAL_PWM_ZONED	50		/* zone (pulses) within set point */
#define WAL_PWM_STEPE	229		/* 90% PWM output within zone E */
#define WAL_PWM_ZONEE	25		/* zone (pulses) within set point */

#define WDEF_MAX_TIMERS	8

typedef struct wdef_rxbuff{
  unsigned char charin;
  unsigned char status;
};

typedef struct wdef_rxmsg{
  unsigned char dest;
  unsigned char from;
  unsigned char proc;
  unsigned char dahi;
  unsigned char dalo;
  unsigned char chks;
};

typedef struct wdef_ctimer{
  unsigned char used;
  unsigned int count;
  void (*exp_routine)();	/* function called */
};


