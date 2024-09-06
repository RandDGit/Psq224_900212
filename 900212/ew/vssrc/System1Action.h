/*
 * Id:        System1Action.h
 *
 * Function:  VS System Action Expression Pointer Table Header File.
 *
 * Generated: Fri Jan 13 13:08:29 2006
 *
 * Coder 5.1.0.215 
 * 
 * This is an automatically generated file. It will be overwritten by the Coder. 
 * 
 * DO NOT EDIT THE FILE! 
 */


#ifndef __SYSTEM1ACTION_H
#define __SYSTEM1ACTION_H


/*
 * Include SEM Defines Header File.
 */
#include "SEMBDef.h"


/*
 * Action Function Prototypes.
 */
extern VS_VOID A_AxisCalcProfile (VS_VOID);
extern VS_VOID A_AxisPosUpdate (VS_VOID);
extern VS_VOID A_ForceStop (VS_VOID);
extern VS_VOID A_Initialise (VS_VOID);
extern VS_VOID A_MotorError (VS_VOID);
extern VS_VOID A_MotorFwd (VS_VOID);
extern VS_VOID A_MotorIdle (VS_VOID);
extern VS_VOID A_MotorMove (VS_VOID);
extern VS_VOID A_MotorOn (VS_VOID);
extern VS_VOID A_MotorReRef (VS_VOID);
extern VS_VOID A_MotorRev (VS_VOID);
extern VS_VOID A_MotorStop (VS_VOID);
extern VS_VOID A_MotorTest (VS_VOID);
extern VS_VOID A_Periodic (VS_VOID);
extern VS_VOID A_RxDoComms (VS_VOID);
extern VS_VOID A_RxProcess (VS_VOID);
extern VS_VOID A_TxChar (VS_VOID);
extern VS_VOID A_TxEnableOff (VS_VOID);
extern VS_VOID A_TxEnableOn (VS_VOID);
extern VS_VOID A_TxProcessMsg (VS_VOID);
extern VS_VOID A_TxTestBusIdle (VS_VOID);
extern VS_VOID TM_Stall (VS_UINT event, VS_UINT ticks);
extern VS_VOID TM_Status (VS_UINT event, VS_UINT ticks);
extern VS_VOID TM_TimerTxWait (VS_UINT event, VS_UINT ticks);


/*
 * Action Expression Function Prototypes.
 */
extern VS_VOID System1VSAction_21 (VS_VOID);

extern VS_VOID System1VSAction_22 (VS_VOID);

extern VS_VOID System1VSAction_23 (VS_VOID);



/*
 * Action Expression Pointer Table.
 */
extern VS_ACTIONEXPR_TYPE const System1VSAction[24];



#endif
