/* UserLoop = Application, excluding State Machine */

#include "iom16.h"		 	// Atmel up definitions
#include "inavr.h"			// AVR intrinsic functions
#include "VSMain.h"			// VisualState definitions (further includes)

#include "defs.h"			// Application (GENERAL) defs
#include "globals.h"		        // Application global data/variables

void vss_seq_error( void);
void wal_dbnc_inputs( void);
void wal_eetbl_write( unsigned char s_address, unsigned char s_value);

unsigned char wal_tacho_change( void);

unsigned char wal_tacho_change( void)
{
// return 1 (true) if tacho polarity changes
// use WAL_IN_ENC as macro form of input pin

  if( WAL_IN_ENC == 1)
  {
    if( dblead_ip_enc != 0)
    {
      dblead_ip_enc--;
      if( dblead_ip_enc == 0)
      {
        val_ip_enc = 1;
        dbtail_ip_enc = WAL_DBNC_IN_ENC;		// re-load trailing edge dbnc
        return 1;
      }
    }
    if( dblead_ip_enc == 0)
    {
      dbtail_ip_enc = WAL_DBNC_IN_ENC;		// re-load trailing edge dbnc
    }
    return 0;
  }

  // else WAL_IN_ENC == 0 

  if( dbtail_ip_enc != 0)
  {
    dbtail_ip_enc--;
    if( !dbtail_ip_enc)
    {
      // IN_ENC has gone low (trailing edge)
      val_ip_enc = 0;
      dblead_ip_enc = WAL_DBNC_IN_ENC;		// re-load leading edge dbnc
      return 1;
    }
  }
  
  if( dbtail_ip_enc == 0)
  {
    dblead_ip_enc = WAL_DBNC_IN_ENC;		// re-load leading edge dbnc
  }
  return 0;
      
}

void wal_dbnc_inputs( void)
{
// Debounce all inputs and set event and/or static condition accordingly:
// This routine is called ea. "0.5" milliseconds.
// A debounce value 2-255 = 1-127 milliseconds.
// Each input requires Leading (L) and Trailing (T) debounce value.

// Handle Following Inputs:
// IN_ENC	Tacho (motor) input PB.1 (T1)
// IN_TRG	Trigger input PB.2 (INT2)
// IN_HOM	Home switch PD.2 (INT0)
// IN_LIM	Limit switch PD.3 (INT1)
// IN_SW1	Button PD.6 (ICP)

// IN_ENC	Tacho (motor) input PB.1 (T1)
// NB. Max RPM 3600 = 60 RPS 4 ppr = 8 edges = 480 transitions/sec = 2msec (max dbnc = 3)

uc_read_portb = PINB;
uc_read_portd = PIND;

// WAL_IN_ENC
// if( (uc_read_portb & (1<<PINB1)) == ( 1<<PINB1) )
// approximately 4msec per tacho pulse

  if( wal_tacho_change() )
  {
    uc_stall_tacin++;
    
    // V1.7 16JAN06: Ignore whilst 'power-up'
    if( uc_settlepower == 0)
    {
      if( WAL_READ_DIR == 0)
      {
        if( ui_axis_pos > 0)
        {
          ui_axis_pos--;
        }
      }
      if( WAL_READ_DIR == 1)
      {
        ui_axis_pos++;
      }
      if( SEQ_AddEvent( E_AxisTacho) != UCC_OK )
      {
        vss_seq_error();
      }
      if( uc_joggle_tachs)
      {
        uc_joggle_tachs--;
      }
      if( uc_joggle_on)
      {
        uc_joggle_trap++;
      }
    }
  }
      
    
// IN_TRG	Trigger input PB.2 (INT2) (has no relevance except addressing)
// WAL_IN_TRG
// if( uc_read_portb & (1<<PINB2))

if( WAL_IN_TRG == 1)
{
  if( dblead_ip_trg == 0)
    dbtail_ip_trg = WAL_DBNC_IN_TRG;		// re-load trailing edge dbnc
  
	if( dblead_ip_trg)
	{
		dblead_ip_trg--;
		if( !dblead_ip_trg)
		{
			// IN_TRG has gone high (leading edge)
			val_ip_trg = 1;
			dbtail_ip_trg = WAL_DBNC_IN_TRG;		// re-load trailing edge dbnc
		}
	}
}

if( WAL_IN_TRG == 0)
{
  if( dbtail_ip_trg == 0)
    dblead_ip_trg = WAL_DBNC_IN_TRG;		// re-load leading edge dbnc
    
  if( dbtail_ip_trg)
  {
    dbtail_ip_trg--;
    if( !dbtail_ip_trg)
    {
          // IN_TRG has gone low (trailing edge)
          // ACTIVE CONDITION
          val_ip_trg = 0;
          dblead_ip_trg = WAL_DBNC_IN_TRG;		// re-load leading edge dbnc
    }
  }
}

// IN_HOM, WAL_IN_HOM	Home switch PD.2 (INT0)
// if( uc_read_portd & (1<<PIND2))
if( WAL_IN_HOM == 1)
{
	if( dblead_ip_hom == 0)
		dbtail_ip_hom = WAL_DBNC_IN_HOM;		// re-load trailing edge dbnc
		
	if( dblead_ip_hom)
	{
		dblead_ip_hom--;
		if( !dblead_ip_hom)
		{
			// IN_HOM has gone high (leading edge)
			// ACTIVE CONDITION
			val_ip_hom = 1;
			dbtail_ip_hom = WAL_DBNC_IN_HOM;		// re-load trailing edge dbnc
			if( SEQ_AddEvent( E_AxisHome) != UCC_OK )
			{
				vss_seq_error();
			}
		}
	}
}

if( WAL_IN_HOM == 0)
{
	if( dbtail_ip_hom == 0)
		dblead_ip_hom = WAL_DBNC_IN_HOM;		// re-load leading edge dbnc
		
	if( dbtail_ip_hom)
	{
		dbtail_ip_hom--;
		if( !dbtail_ip_hom)
		{
			// IN_HOM has gone low (trailing edge)
			val_ip_hom = 0;
			dblead_ip_hom = WAL_DBNC_IN_HOM;		// re-load leading edge dbnc
			// When HOME SWITCH becomes inactive = End of Home Sequence
			if( SEQ_AddEvent( E_AxisZeroed) != UCC_OK )
			{
				vss_seq_error();
			}
			
			// V1.6 12JAN06: only allow 'zero' if 'motor-on'
			if( uc_motoron)
			{
                          // Reset axis position value:
                          // NB. Could be done as part of reaction to E_AxisZeroed, but:
                          // i.  further tacho events will be 'lost'
                          // ii. additional 'action' function required
                          ui_axis_pos = (unsigned int) 0;
                          // V1.3 27JUN05: reset error flag:
                          uc_axis_err = 0;
                          wal_eetbl_write( WAL_EE_ERROR, uc_axis_err);
                        }
			
		}
	}
}

// IN_LIM, WAL_IN_LIM	Limit switch PD.3 (INT1) 
// if( uc_read_portd & (1<<PIND3))

if( WAL_IN_LIM == 1)
{
    if( dblead_ip_lim == 0)
      dbtail_ip_lim = WAL_DBNC_IN_LIM;		// re-load trailing edge dbnc
	
	if( dblead_ip_lim)
	{
		dblead_ip_lim--;
		if( !dblead_ip_lim)
		{
			// IN_LIM has gone high (leading edge)
			// ACTIVE CONDITION
			val_ip_lim = 1;
			dbtail_ip_lim = WAL_DBNC_IN_LIM;		// re-load trailing edge dbnc
			if( SEQ_AddEvent( E_AxisLimit) != UCC_OK )
			{
				vss_seq_error();
			}
			// V2.4: record error:
            uc_axis_err = 4;
            // store in table..
            wal_record_axis_err();
		}
	}
}

if( WAL_IN_LIM == 0)
{
	if( dbtail_ip_lim == 0)
		dblead_ip_lim = WAL_DBNC_IN_LIM;		// re-load leading edge dbnc
	if( dbtail_ip_lim)
	{
		dbtail_ip_lim--;
		if( !dbtail_ip_lim)
		{
			// IN_LIM has gone low (trailing edge)
			val_ip_lim = 0;
			dblead_ip_lim = WAL_DBNC_IN_LIM;		// re-load leading edge dbnc
		}
	}
}

  // IN_SW1	Button PD.6 (ICP)
  // pin_ip_sw1 = ( uc_read_portd & (1<<PIND6))? 1 : 0;
  // switch( pin_ip_sw1)
  if( WAL_IN_SW1)
  {
    if( dblead_ip_sw1 == 0)
      dbtail_ip_sw1 = WAL_DBNC_IN_SW1;	// re-load trailing edge dbnc
    else
    {
	dblead_ip_sw1--;
	if( dblead_ip_sw1 == 0)
	{
		// IN_SW1 has gone high (leading edge)
		val_ip_sw1 = 0;
		dbtail_ip_sw1 = WAL_DBNC_IN_SW1;// re-load trailing edge dbnc
	}
    }
  }
  else
  {
    if( dbtail_ip_sw1 == 0)
      dblead_ip_sw1 = WAL_DBNC_IN_SW1;	// re-load leading edge dbnc
    else
    {
      dbtail_ip_sw1--;
      if( dbtail_ip_sw1 == 0)
      {
        // IN_SW1 has gone low (trailing edge)
        val_ip_sw1 = 1;
        dblead_ip_sw1 = WAL_DBNC_IN_SW1;	// re-load leading edge dbnc
        // REVIEW: appears to be problem regarding same event
        // for both entry and exit from single state:
        // use uc_axis_vss (idle == 0)
        if( uc_axis_vss == 0)
        {
					if (uc_readdr_timer == 0)
					{
						if( SEQ_AddEvent( E_Button) != UCC_OK )
						{
							vss_seq_error();
						}
					}
        }
        else
        {
          if( SEQ_AddEvent( E_AxisStop) != UCC_OK )
          {
            vss_seq_error();
          }
        }
      }
    }
  }

}
