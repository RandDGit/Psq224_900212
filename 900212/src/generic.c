/*
*
* The following function routines are 'common' to various application
* i.e. not application specific
*
*/

#include <iom16.h>		 	// Atmel up definitions
#include <inavr.h>			// AVR intrinsic functions
#include "VSMain.h"			// VisualState definitions (further includes)


// NB. pre-defined macros 'inavr.h'
// #define __EEPUT(ADR,VAL)  {while (EECR & 0x02); EEAR = (ADR); EEDR = (VAL); EECR = 0x04; EECR = 0x02;}
// #define __EEGET(VAR, ADR) {while (EECR & 0x02); EEAR = (ADR); EECR = 0x01; (VAR) = EEDR;}

// NB. these functions only access the lowest 256 bytes of EEPROM

// NB. wal_program_ee superceeded by wal_eetbl_write(...)
void wal_program_ee( char s_address, char s_value)
{
	while (EECR & 0x02); 
	EEARH = 0;
	EEARL = s_address; 
	EEDR = s_value;
	EECR = 0x04; 
	EECR = 0x02;
}

unsigned char wal_bsf_eeget( unsigned char s_address)
{
while (EECR & 0x02);    // 1<<EEWE
                        // NB: EEARH always 0 !
EEARH = 0;
EEARL = (s_address); 
EECR = 0x01;            // 1<<EERE
                        // ? dummy op required ?
return EEDR;
}

