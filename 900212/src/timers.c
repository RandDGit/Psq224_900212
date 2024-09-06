/* Centralise all timer functions (as timer functions tend to be critical) */

#include <iom16.h>		 	// Atmel up definitions
#include <inavr.h>			// AVR intrinsic functions
#include "VSMain.h"			// VisualState definitions (further includes)

#include "defs.h"			// Application (GENERAL) defs
#include "globals.h"		// Application global data/variables


/* Timer routines */
void wal_tc_tick( void);
void wal_tc_stall( void);
void wal_timer_counter( void);

/* use structure wdef_ctimer to provide timer-counter functions */
/* ws_ctimer[] is the available counter timer array */
/* maximum allowable functions according to WDEF_MAX_TIMERS */

void wal_tc_tick( void)
{
// generate VSS SEQ tick function (periodic function)
}

void wal_tc_stall( void)
{
// generate VSS SEQ stall function
}

void wal_timer_counter( void)
{
// basic application counter timer using struct wdef_ctimer
// unsigned char used;
// unsigned int count;
// void (*exp_routine)();

	wsp_tc = ws_ctimer;		// &ws_ctimer[0];

	if( wsp_tc->exp_routine != NULL)
	{
		if( wsp_tc->used == 1)
		{
			// tc active
			if( wsp_tc->count != 0)
			{
				wsp_tc->count--;
				if( wsp_tc->count == 0)
				{
					// call expiry routine
					(*wsp_tc->exp_routine)();
				}
			}
		}
		wsp_tc++;
	}
}


