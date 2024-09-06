/*****************************************************************************
* IAR visualSTATE Main Loop Header File
*
* The file contains an implementation for a main loop using the visualSTATE
* basic API.
*
* The code uses a simple queue for storing events. The functions for inter-
* facing to the queue are described in the sample code file
* simpleEventHandler.h.
*****************************************************************************/



#ifndef _VSMAIN_H
#define _VSMAIN_H

/* *** include directives *** */

#include "simpleEventHandler.h"
#include "SEMLibB.h"
#include "system1.h"
#include "system1Data.h"
#include "system1Action.h"

#if (VS_REALLINKMODE == 1)    
#include "reallink.h"
#endif  

#endif
