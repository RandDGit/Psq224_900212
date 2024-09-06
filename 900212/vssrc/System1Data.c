/*
 * Id:        System1Data.c
 *
 * Function:  VS System Data Source File.
 *
 * Generated: Fri Jan 13 13:08:29 2006
 *
 * Coder 5.1.0.215 
 * 
 * This is an automatically generated file. It will be overwritten by the Coder. 
 * 
 * DO NOT EDIT THE FILE! 
 */


/*
 * Include SEM Library Header File.
 */
#include "SEMLibB.h"


/*
 * Include VS System Data Header File.
 */
#include "System1Data.h"


/*
 * Include VS System Action Expression Pointer Table File.
 */
#include "System1Action.h"


/*
 * Include Compiler Standard Library Header Files.
 */
#if (SEM_DEDUCT_WITH_VA_LIST)
#include <stdarg.h>
#endif


/*
 * VS System External Variable Declarations.
 */
VS_UCHAR val_ip_hom;

VS_UCHAR val_ip_lim;

VS_UINT vsstick_stall;

VS_UINT vsstick_txwait;

typedef struct
{
  VS_UCHAR         VS_UCHARVar[0X00013];
  VS_UINT16        VS_UINT16Var[0X00002];
  VS_INT           VS_INTVar[0X00002];
} VSINTVARDATA;



/*
 * VS System Internal Variable Declaration.
 */
static VSINTVARDATA VSIntVar;




/*
 * Guard Expression Functions.
 */
static VS_BOOL VSGuard_0 (VS_VOID)
{
  return SEM_GUARD_TYPE_CAST ((val_ip_hom == 0) && (val_ip_lim == 0));
}


/*
 * Guard Expression Pointer Table.
 */
VS_GUARDEXPR_TYPE const VSGuard[1] = 
{
  VSGuard_0
};



/*
 * Action Expression Functions.
 */
VS_VOID System1VSAction_21 (VS_VOID)
{
  TM_Status (14 /* E_Periodic */, 10);
}
VS_VOID System1VSAction_22 (VS_VOID)
{
  TM_Stall (9 /* E_AxisStall */, vsstick_stall);
}
VS_VOID System1VSAction_23 (VS_VOID)
{
  TM_TimerTxWait (19 /* E_TimerTxWait */, vsstick_txwait);
}


/*
 * Action Expression Pointer Table.
 */
VS_ACTIONEXPR_TYPE const System1VSAction[24] = 
{
  A_AxisCalcProfile,
  A_AxisPosUpdate,
  A_ForceStop,
  A_Initialise,
  A_MotorError,
  A_MotorFwd,
  A_MotorIdle,
  A_MotorMove,
  A_MotorOn,
  A_MotorReRef,
  A_MotorRev,
  A_MotorStop,
  A_MotorTest,
  A_Periodic,
  A_RxDoComms,
  A_RxProcess,
  A_TxChar,
  A_TxEnableOff,
  A_TxEnableOn,
  A_TxProcessMsg,
  A_TxTestBusIdle,
  System1VSAction_21,
  System1VSAction_22,
  System1VSAction_23
};

