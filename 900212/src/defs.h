/******************************** #defines section ****************************/

/*
 * 2.8   JATF  18-jun-07	Stop upto WDEF_TACHO_DEADBAND + 1 tachos before tacho
 *                        count reached!
 * 2.9   JATF  10-oct-07 Motor ramp up PWM!! 
 *                      (Change for V2.9 Now removed again)
 * 3.0   JATF  04-feb-08	Add cmd: 98 to clear error history/state
 *                        PSQ #219 on re-addr do-not move axis
 */
#define def_major_ver    3
#define def_minor_ver    5  /* Change release output to match ! */

// V2.4 04AUG06: change default 'address-> 39'
// Notify 'top' (status indication?) if BORF detected
// Store circular buffer (NV EEPROM) of error(s) ?time-stamp?


/* __DATE__ __TIME__ */

#define WAL_PIC_VERSION	        15

#define WAL_UNIQUE_COMMID	30	/* Multi-bus comms ID */
#define WAL_UNDEFINED_ID    99
#define WAL_COMMS_GLOBAL	255	/* Global address */

#define WAL_CARRY_BIT (SREG_Bit0)

/* define EEPROM locations for nv ram */
#define WAL_EE_COMMID	1
#define WAL_EE_COMMHILO	2
#define WAL_EE_COUNTHI	3
#define WAL_EE_COUNTLO	4
#define WAL_EE_ERROR	5
#define WAL_EE_MAXTHI   6
#define WAL_EE_MAXTLO   7
#define WAL_EE_MOVELO   8
#define WAL_EE_MOVEHI   9
#define WAL_EE_MOVE24   10

// record number of POR: 16bit 65k on-off
#define WAL_EE_MCUCSR   16
#define WAL_EE_PORCHI   17
#define WAL_EE_PORCLO   18
// record (push down) last three uc_axis_err: 
#define WAL_EE_ERRTBLA  19
#define WAL_EE_ERRACHI  20
#define WAL_EE_ERRACLO  21
#define WAL_EE_ERRTBLB  22
#define WAL_EE_ERRBCHI  23
#define WAL_EE_ERRBCLO  24
#define WAL_EE_ERRTBLC  25
#define WAL_EE_ERRCCHI  26
#define WAL_EE_ERRCCLO  27

// store CPU Serial No.
#define WAL_EE_CPUSERHI 28
#define WAL_EE_CPUSERLO 29
// store (this) AXIS calibration
#define WAL_EE_CALIBHI  30
#define WAL_EE_CALIBLO  31

#define WAL_DBNC_IN_ENC 2
#define WAL_DBNC_IN_TRG	10
#define WAL_DBNC_IN_HOM	10
#define WAL_DBNC_IN_LIM	10
#define WAL_DBNC_IN_SW1	60

//CXP 100924
//V3.4 and back will support old motor NIDEC
//V3.5 and forward will support new motor EISSES
// where old is 288 tachos per rev and new is 310 tachos per rev
#define OLD2NEW(tac) (unsigned int)((float)tac * (float)1.076)   
#define NEW2OLD(tac) (unsigned int)((float)tac / (float)1.076)

// Valeo Motor Pitch = 1 pulse = 0.125mm limit travel to 2022mm (0x4EFF) i.e. 16176 0x3F30 ("?+")
// also manually determine MAX hi-byte for comms parsing, & MIN lo-byte 0.5mm
#define WAL_MAX_SET_VALUE   20220
#define WAL_MAX_SET_HIBYTE  0x4E
#define WAL_MAX_AXIS_TACHO  0x3FFF
#define WAL_MAX_AXIS_HIBYTE 0x3F
#define WAL_MIN_SET_LOBYTE  0x04

#define WAL_JOG_DWELL   60

#define WAL_LED_SCALE	20		/* scale periodic timer to provide longer period */
#define WAL_LED_DEFAULT	5		/* period (scaled) for status LED */
#define WAL_LED_QERROR	1		/* fast flash error */

#define WAL_GEAR_RATIO	3		/* conversion from comms units to motor pulses */
#define WAL_UNI_TRAVEL  80              /* #tachos for uni-directional overshoot */
#define WAL_MAX_RXBUFF	32		/* size of rx buffer, must be 2^n */

// uc_comm_flag (comms status & control)
#define WAL_COMM_DRDY 	0x01		// cflags.bit0 'Data-received' flag 
#define WAL_COMM_DERR  	0x02		// cflags.bit1 Software Rx error
#define WAL_COMM_IDLE  	0x04		// cflags.bit2 'Comms line idle' flag
#define WAL_COMM_SMSG  	0x08		// cflags.bit3 Message out in progress
#define WAL_COMM_MSGI	0x10		// Message 'in' in-progress
#define WAL_COMM_MISM	0x20		// ch in != ch out (Tx integrity verification)

// PORTA .0 reserved for 28v analogue sense input
// PORTA .1 unused ?
#define WAL_IN_WFEED    (PINA_Bit2)
#define WAL_IN_STAPLE   (PINA_Bit3)
#define WAL_IN_WIRESENS (PINA_Bit4)
// PORTA .5,.6,.7 spare

#define WAL_OP_LED	(PORTB_Bit0)
#define WAL_IN_ENC      (PINB_Bit1)
#define WAL_IN_TRG      (PINB_Bit2)
#define WAL_OP_TRO	(PORTB_Bit3)
#define WAL_RXTX_EN     (PORTB_Bit4)
// PORTB .5,.6,.7 reserved for SPI
#define WAL_DEBUG_MISO  (PORTB_Bit6)

// PORTC .0,.1 reserved for I2C
// PORTC .2,.3,.4,.5 reserved for JTAG
#define WAL_OP_WIRE     (PORTC_Bit6)
#define WAL_OP_STITCH   (PORTC_Bit7)

// PORTD .0,.1 = RxD, TxD comms
#define WAL_IN_HOM      (PIND_Bit2)
#define WAL_IN_LIM      (PIND_Bit3)
#define WAL_OP_DIR	(PORTD_Bit4)
#define WAL_READ_DIR    (PIND_Bit4)
#define WAL_OP_BRK	(PORTD_Bit5)
#define WAL_IN_SW1      (PIND_Bit6)
#define WAL_OP_PWM	(OCR2)

// 'deadband' for +/- tacho counts at rest position
// 2 --> 1/4 turn on motor; +/- 0.125 mm
#define WDEF_TACHO_DEADBAND 3

#define WAL_PWM_TEST	192		/* 75% output on test */
#define WAL_PWM_MAXSP   255             /* 100% output for max_speed */
#define WAL_PWM_RAMPUP	204		/* PWM ramp up output (50%) */
#define WAL_PWM_HOME	204		/* PWM during home sequence (35%) */

/* Motor start-stop ramp-up ramp-down */
/* Motor approx. 3000 rpm = 50 rps = 200 pps */
/* 1 sec up-down --> ~200 pulses */

#define WAL_PWM_WEAKA	76		/* 30% PWM output within zone A */
#define WAL_PWM_WEAKB	102		/* 40% PWM output within zone B */
#define WAL_PWM_WEAKC	128		/* 50% PWM output within zone C */
#define WAL_PWM_WEAKD	178		/* 70% PWM output within zone D */
#define WAL_PWM_WEAKE	229		/* 90% PWM output within zone E */

#define WAL_PWM_STEPA	204		/* 153 60% PWM output within zone A */
#define WAL_PWM_STEPB	204		/* 178 70% PWM output within zone B */
#define WAL_PWM_STEPC	217		/* 204 80% PWM output within zone C */
#define WAL_PWM_STEPD	229		/* 229 90% PWM output within zone D */
#define WAL_PWM_STEPE	242		/* 242 95% PWM output within zone E */

#define WAL_PWM_ZONEA	64		/* zone (pulses) within set point */
#define WAL_PWM_ZONEB	48		/* zone (pulses) within set point */
#define WAL_PWM_ZONEC	32		/* zone (pulses) within set point */
#define WAL_PWM_ZONED	24		/* zone (pulses) within set point */
#define WAL_PWM_ZONEE	12		/* zone (pulses) within set point */

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


