/******************************** #defines section ****************************/

#define def_major_ver    0
#define def_minor_ver    0
#define def_mini_change  0

#define MAX_NUMBERS 30
#define NUMBER_LEN 14
#define NUMBER_INDEX = 0   //eeprom location

#define WAL_LED_SCALE	20		/* scale periodic timer to provide longer period */
#define WAL_LED_DEFAULT	5		/* period (scaled) for status LED */


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


