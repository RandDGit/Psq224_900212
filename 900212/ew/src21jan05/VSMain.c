/*****************************************************************************
* IAR visualSTATE Main Loop Source File
*
* The file contains an implementation for a main loop using the visualSTATE
* basic API.
*
* The code uses a simple queue for storing events. The functions for inter-
* facing to the queue are described in the sample code file
* simpleEventHandler.h.
*****************************************************************************/



#include <iom16.h>		 	// Atmel up definitions
#include <inavr.h>			// AVR intrinsic functions
#include "VSMain.h"			// VisualState definitions (further includes)

void InitDevice(void);		// Critical device I/O setup
void InitOutputs(void);		// Critical PORT setup (i.e. devices off)
void UserLoop(void);		// User (application) loop

							// NB. Setup of interrupts = DeviceSetup.c


void RealLinkCommInit(void);


void HandleError(unsigned char cc) 
{
  /* Critical error: Stop State Machine: */
  while(1)
  {
  }
}

void main(void) 
{
/* Define completion code variable. */
unsigned char cc;

/* Define action expression variable. */
SEM_ACTION_EXPRESSION_TYPE actionExpressNo;
  
/* Define and initialize. */
SEM_EVENT_TYPE eventNo;

	__disable_interrupt();

	InitDevice();

	InitOutputs();
    
	/* Initialize the VS System. */
	SEM_InitAll();
    
#if (VS_REALLINKMODE == 1)    
	/* Initialize RealLink communication. */
	RealLinkCommInit();
	/* Initializing RealLink API. */
	VS_RealLinkInit();    
#endif  
  
	/* Initialize the VS Event queue. */
	SEQ_Initialize();
  
	/* Add SE_RESET to initialize system. */
	SEQ_AddEvent(SE_RESET);

	__enable_interrupt();
  
	/* Do forever: */
	for(;;) 
  	{    
#if (VS_REALLINKMODE == 1)
    		VS_WAIT();
#endif        

		if(SEQ_RetrieveEvent(&eventNo) != UCC_QUEUE_EMPTY)
		{     
      		/* Deduct the event. */
      		if ((cc = SEM_Deduct(eventNo)) != SES_OKAY) 
	        		HandleError(eventNo);
      
      		/* Get resulting action expressions and execute them. */
      		while ((cc = SEM_GetOutput(&actionExpressNo)) == SES_FOUND) 
        			SEM_Action(actionExpressNo);
            
      		/* Check for error. */
      		if (cc != SES_OKAY) 
        			HandleError(cc);
      
      		/* Change the next state vector. */
      		if ((cc = SEM_NextState()) != SES_OKAY) 
        			HandleError(cc);          
    		}  

		/* Provide function to do any other user (application) processing */
		UserLoop();
	}
}
