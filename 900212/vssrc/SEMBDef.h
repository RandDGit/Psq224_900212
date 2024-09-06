/*
 * Id:        SEMBDef.h
 *
 * Function:  SEM Defines Header File.
 *
 * Generated: Fri Jan 13 13:08:29 2006
 *
 * Coder 5.1.0.215 
 * 
 * This is an automatically generated file. It will be overwritten by the Coder. 
 * 
 * DO NOT EDIT THE FILE! 
 */


#ifndef __SEMBDEF_H
#define __SEMBDEF_H


/*
 * visualSTATE CoderDll Version No
 */
#define VS_CODER_VERSION 50000


/*
 * Include SEM Types Header File.
 */
#include "SEMTypes.h"


/*
 * Number of Identifiers.
 */
#define VS_NOF_EVENTS                    0X019  /*  25 */
#define VS_NOF_EVENT_GROUPS              0X000  /*   0 */
#define VS_NOF_SIGNALS                   0X000  /*   0 */
#define VS_NOF_STATES                    0X00C  /*  12 */
#define VS_NOF_STATE_MACHINES            0X004  /*   4 */
#define VS_NOF_ACTION_FUNCTIONS          0X015  /*  21 */
#define VS_NOF_ACTION_EXPRESSIONS        0X018  /*  24 */
#define VS_NOF_GUARD_EXPRESSIONS         0X001  /*   1 */
#define VS_NOF_INSTANCES                 0X001  /*   1 */
#define VS_NOF_INTERNAL_VARIABLES        0X017  /*  23 */
#define VS_NOF_EXTERNAL_VARIABLES        0X004  /*   4 */


/*
 * Signal Queue Size.
 */
#define VS_SIGNAL_QUEUE_SIZE             0X001  /*   1 */


/*
 * Conditional Compilation Definitions.
 */
#define SEM_RDHW_TYPE_1                  0
#define SEM_RDHW_TYPE_2                  1
#define SEM_RDHW_TYPE_3                  0
#define SEM_RDHW_WIDTH_16_BIT            0
#define SEM_RDHW_WIDTH_24_BIT            1
#define SEM_RDHW_WIDTH_32_BIT            0
#define SEM_RDHW_WIDTH_48_BIT            0
#define SEM_RDHW_WIDTH_64_BIT            0
#define SEM_RD_WIDTH_8_BIT               1
#define SEM_RD_WIDTH_16_BIT              0
#define SEM_RD_WIDTH_32_BIT              0
#define SEM_RDFM_NUMBER                  1
#define SEM_EVENT_GROUP_INDEX            0
#define SEM_EVENT_GROUP_TABLE_INDEX      0
#define SEM_FUNC_OLD_NAMING_STYLE        0
#define SEM_SIGNAL_QUEUE_ERROR_IF_FULL   1
#define SEM_SIGNAL_QUEUE_NO_ERROR_IF_FULL 0
#define SEM_FUNCEXPHANDLING              0
#define SEM_GUARD_EXPRESSION             1
#define SEM_SIGNAL                       0
#define SEM_SIGNAL_DB                    0
#define SEM_RMN_POSITIVE_STATE_SYNCS     3
#define SEM_RMN_NEGATIVE_STATE_SYNCS     0
#define SEM_RMN_GUARDS                   1
#define SEM_RMN_NEXT_STATES              4
#define SEM_RMN_ACTIONS                  5
#define SEM_RMN_SIGNALS                  0
#define SEM_USE_STATUS_FLAG              0
#define VS_NOF_SYSTEMS                   1
#define VS_REALLINKMODE                  0
#define VS_RL_USE_EXTKWS                 0
#define VS_USE_HEAP                      0
#define SEM_INIT_ALL                     1


/*
 * Conditional Compilation Definitions.
 */
#define SEM_INIT_EXTERNAL_VARIABLES      0
#define SEM_INIT_INTERNAL_VARIABLES      0
#define SEM_DEDUCT_FUNC                  0
#define SEM_DEDUCT_WITH_VA_LIST          0
#define SEM_SPEED                        0
#define VS_CPP                           0


/*
 * Conditional Compilation Definitions.
 */
#define VS_EVENT_NAMES                   0
#define VS_STATE_NAMES                   0
#define VS_ACTION_FUNCTION_NAMES         0
#define VS_EVENT_EXPLS                   0
#define VS_STATE_EXPLS                   0
#define VS_ACTION_EXPLS                  0
#define SEM_OPTIMIZED_GUARDS             0
#define SEM_OPTIMIZED_ACTIONS            0
#define SEM_INQUIRY                      0
#define SEM_NAME                         0
#define SEM_EXPL                         0
#define SEM_STATE                        0
#define SEM_MACHINE                      0
#define SEM_FORCE_STATE                  0
#define SEM_STATE_ALL                    0
#define SEM_NEXT_STATE_CHG               0
#define SEM_EXPL_ABS                     0
#define SEM_NAME_ABS                     0
#define SEM_GET_OUTPUT_ALL               0
#define SEM_GET_INPUT_ALL                0
#define SEM_SIGNAL_QUEUE_INFO            0


/*
 * Undefined State.
 */
#define STATE_UNDEFINED                  0X0FF  /* 255 */


/*
 * Undefined Event.
 */
#define EVENT_UNDEFINED                  0X0FF  /* 255 */


/*
 * Undefined Event Group.
 */
#define EVENT_GROUP_UNDEFINED            0X0FF  /* 255 */


/*
 * Event Termination ID.
 */
#define EVENT_TERMINATION_ID             0X0FF  /* 255 */


/*
 * Action Expression Termination ID.
 */
#define ACTION_EXPRESSION_TERMINATION_ID 0X0FF  /* 255 */
#define ACTION_FPT_NAME System1VSAction


/*
 * Action Expression Pointer Table.
 */


/*
 * Type qualifier macros
 */
#define VS_TQ_RULEBASE                 const 
#define VS_TQ_GUARDEXPRCOL             const 
#define VS_TQ_ACTIONEXPRCOL            const 
#define VS_TQ_CONTEXT                  
#define VS_RL_TQ_DATA                  
#define VS_RL_TQ_PROTOCOL_DATA         
#define VS_RL_TQ_CONST_DATA            


/*
 * Functional expression type definitions
 */
typedef VS_BOOL (* VS_GUARDEXPR_TYPE) (VS_VOID);
typedef VS_VOID (* VS_ACTIONEXPR_TYPE) (VS_VOID);


/*
 * SEM Library Datatype Definition.
 */
typedef struct SEMDATA
{
  VS_UINT8                                      Status;
  VS_UINT8                                      State;
  VS_UINT8                                      DIt;
  VS_UINT8                                      nNo;
  VS_UINT8                                      nNeg;
  VS_UINT8                                      nPos;
  VS_UINT8                                      nNxt;
  VS_UINT8                                      nAction;
  SEM_EVENT_TYPE                                EventNo;
  SEM_RULE_INDEX_TYPE                           iRI;
  SEM_RULE_TABLE_INDEX_TYPE                     iFirstR;
  SEM_RULE_TABLE_INDEX_TYPE                     iLastR;
  SEM_INTERNAL_TYPE                             i;
  SEM_STATE_TYPE                                sa;
  VS_UINT8                                      nGuard;
  VS_UINT8                                      nSignal;
  SEM_STATE_TYPE                                CSV[VS_NOF_STATE_MACHINES];
  SEM_STATE_TYPE                                WSV[VS_NOF_STATE_MACHINES];
  SEM_EVENT_TYPE                                SQueue[VS_SIGNAL_QUEUE_SIZE];
  SEM_SIGNAL_QUEUE_TYPE                         SPut;
  SEM_SIGNAL_QUEUE_TYPE                         SGet;
  SEM_SIGNAL_QUEUE_TYPE                         SUsed;
} SEMDATA;


/*
 * VS System Datatype Definition.
 */
typedef struct 
{
  VS_UINT8       SMI[0X00C];
  VS_UINT8       RD[0X0FA];
  VS_UINT8       RI[0X023];
  VS_UINT8       RTI[0X01A];
} VSDATA;



/*
 * Data External Declaration.
 */
extern VSDATA const VS;

extern SEMDATA SEM;



/*
 * External Declarations for Guard Expressions Function Pointer Table.
 */
extern VS_GUARDEXPR_TYPE const VSGuard[1];



/*
 * Action Functions Header File Name.
 */
#define SEM_FN_ACTION_HEADER           "System1Action.h"


/*
 * Action Functions Identifier Name.
 */
#define SEM_IN_ACTION                  System1VSAction


/*
 * Action Expression Collection Macro.
 */
#define VSAction                       System1VSAction


#endif
