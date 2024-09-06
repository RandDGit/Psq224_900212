/*****************************************************************************
* IAR visualSTATE Event Handler Header File
*
* The file contains completion codes to be returned by event handling 
* functions. See the individual header files for the various event handlers
* for further information.
*****************************************************************************/



#ifndef _EVENTHANDLER_H
#define _EVENTHANDLER_H



/* *** include directives *** */

/* Defines the undefined event EVENT_UNDEFINED and the types VS_UINT8,
   VS_UINT16, VS_UINT32 and SEM_EVENT_TYPE. If the expert API is used,
   replace SEMLibB.h with SEMLibE.h. */
#include "SEMLibB.h"



/* *** macro definitions *** */

/* User completion codes. */

/** UCC_OK. Returned if the function completed successfully. */
#define UCC_OK                SES_OKAY

/** UCC_QUEUE_EMPTY. Returned on retrieval of an event from an empty queue. */
#define UCC_QUEUE_EMPTY       0x0fe

/** UCC_QUEUE_FULL. Returned on adding of an event to a full queue. */
#define UCC_QUEUE_FULL        0x0fd

/** UCC_MEMORY_ERROR. Returned if memory allocation or reallocation failed. */
#define UCC_MEMORY_ERROR      0x0fc

/** UCC_INVALID_PARAMETER. Returned if an invalid parameter is supplied to a function. */
#define UCC_INVALID_PARAMETER 0x0fb



/* *** type definitions *** */

/** User completion code type. Defines completion codes to be returned by the queue handling
    functions. */
typedef unsigned char UCC_TYPE;



#endif
