/* UserLoop = Application, excluding State Machine */

#include <iom16.h>		 		// Atmel up definitions
#include <inavr.h>				// AVR intrinsic functions
#include "VSMain.h"				// VisualState definitions (further includes)

#define WDEF_MAX_TIMERS	8

//struct wdef_ctimer ws_ctimer[WDEF_MAX_TIMERS];	// application timer function(s)
//struct wdef_ctimer *wsp_tc;			// timer counter implementation

extern unsigned char uc_tprim;		// Primary timer base (TMR0 ovf)
extern unsigned char uc_tprev;		// Prev. value
extern unsigned char uc_tm0preset;	// Timer 0 preset (controls base period)

// external variables used by VSS model (critical)
// defined & declared within VSS <System1>Data generated files
// VS_UCHAR val_ip_hom;
// VS_UCHAR val_ip_lim;
// VS_UINT vsstick_stall;
// VS_UINT vsstick_txwait;

void wal_vss_timer_function( void);		// VSS Timer (tick) function


void UserLoop( void)
{

	// Timer0 overflow should occur every 0.5msec
	if( uc_tprim != uc_tprev)
	{
		uc_tprev = uc_tprim;
		// Seed VisualSTATE tick:
		wal_vss_timer_function();
		// Also 'fast' debounce & switching on inputs:
    		//wal_dbnc_inputs();
	}

	// NB. Most 'housekeeping' should be within 'state' periodic function.
}

