/*
 * Id:        System1Data.h
 *
 * Function:  VS System Data Header File.
 *
 * Generated: Fri Jan 13 13:08:29 2006
 *
 * Coder 5.1.0.215 
 * 
 * This is an automatically generated file. It will be overwritten by the Coder. 
 * 
 * DO NOT EDIT THE FILE! 
 */


#ifndef __SYSTEM1DATA_H
#define __SYSTEM1DATA_H


/*
 * Include SEM Defines Header File.
 */
#include "SEMBDef.h"


/*
 * Event Identifier Definitions.
 */
#define SE_RESET                         0X000  /*   0 */
#define E_AxisError                      0X001  /*   1 */
#define E_AxisFwd                        0X002  /*   2 */
#define E_AxisHome                       0X003  /*   3 */
#define E_AxisInvalid                    0X004  /*   4 */
#define E_AxisLimit                      0X005  /*   5 */
#define E_AxisMove                       0X006  /*   6 */
#define E_AxisRev                        0X007  /*   7 */
#define E_AxisSetPoint                   0X008  /*   8 */
#define E_AxisStall                      0X009  /*   9 */
#define E_AxisStop                       0X00A  /*  10 */
#define E_AxisTacho                      0X00B  /*  11 */
#define E_AxisZeroed                     0X00C  /*  12 */
#define E_Button                         0X00D  /*  13 */
#define E_Periodic                       0X00E  /*  14 */
#define E_RxChar                         0X00F  /*  15 */
#define E_RxMsg                          0X010  /*  16 */
#define E_SendComplete                   0X011  /*  17 */
#define E_SendRequest                    0X012  /*  18 */
#define E_TimerTxWait                    0X013  /*  19 */
#define E_TxBusIdle                      0X014  /*  20 */
#define E_TxComplete                     0X015  /*  21 */
#define E_TxError                        0X016  /*  22 */
#define E_TxHiSpeed                      0X017  /*  23 */
#define E_TxMoreMsg                      0X018  /*  24 */


/*
 * There is no VS System constants, so this section is intentionally empty!
 */


/*
 * VS System External Variable Declaration Prototypes.
 */
extern VS_UCHAR val_ip_hom;

extern VS_UCHAR val_ip_lim;

extern VS_UINT vsstick_stall;

extern VS_UINT vsstick_txwait;




#endif
