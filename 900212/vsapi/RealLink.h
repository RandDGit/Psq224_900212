/*
 * Type:        Header File
 *
 * Id:          RealLink.h
 *
 * Function:    Contains the functions for the RealLink SEM API version 4
 *
 * Portability: ANSI-C Compiler.
 *
 * Copyright (c) 2001 IAR Systems. All rights reserved.
 */

/*
 * TODO: By default visualSTATE Basic API is selected.
 *       If using the Expert API please define VS_IS_EXPERT to 1
 */
#ifndef VS_IS_EXPERT
#define VS_IS_EXPERT 0 /* Basic API by default */
#else
#if (VS_IS_EXPERT != 0) 
#if (VS_IS_EXPERT != 1)
#error VS_IS_EXPERT has to be defined to either 0 or 1
#endif
#endif
#endif

/*
 * Include API files
 */
#if (VS_IS_EXPERT)
#include "SEMEDef.h"
#include "SEMLibE.h"
#else
#include "SEMLibB.h"
#endif


#if (VS_REALLINKMODE)

#ifndef __REALLINK_H
#define __REALLINK_H

/*
 * Memory copy function, similar to ansi-c memcpy() but with the correct extended keywords
 */
void MemCopy(unsigned char VS_RL_TQ_DATA * pDest, const unsigned char * pSrc, unsigned char size);

/*
 * If not defined elsewhere define the user-functions with default names.
 * The user must implement these three functions.
 */
#ifndef VS_RL_TRANSMIT
#define VS_RL_TRANSMIT RealLinkTransmit
#endif
extern VS_VOID VS_RL_TRANSMIT(VS_UINT8 ch);

/*
 * If a flush function is needed, e.g. when using a transmit buffer, implement this function
 */
#ifndef VS_RL_TRANSMIT_FLUSH
#define VS_RL_TRANSMIT_FLUSH() ((void)0)
#else
extern VS_VOID VS_RL_TRANSMIT_FLUSH(VS_VOID);
#endif

#ifndef VS_RL_RESET
#define VS_RL_RESET RealLinkReset
#endif
extern VS_VOID VS_RL_RESET(VS_VOID);

/* 
 * VS_VOIDPTR_TYPE must be defined to hold a void* variable.
 * On some micro processors an 'unsigned int' is sufficient whereas 
 * others require an 'unsigned long int' (Default).
 */

#ifdef VS_VOIDPTR_SMALL
#define VS_VOIDPTR_TYPE        unsigned int
#else
#define VS_VOIDPTR_TYPE        unsigned long int
#endif


/*
 * If running in Expert mode and using a preemptive RTOS these
 * #defines must point to the relevant functions.
 */
#if (VS_IS_EXPERT)
#define VS_RL_ENTER_CRITICALSECTION /* Insert function name */ ;
#define VS_RL_LEAVE_CRITICALSECTION /* Insert function name */ ;
#else /* BASIC API */
#define VS_RL_ENTER_CRITICALSECTION ; /* Please leave empty */
#define VS_RL_LEAVE_CRITICALSECTION ; /* Please leave empty */
#endif

#ifndef VS_RL_TQ_PROTOCOL_DATA
#define VS_RL_TQ_PROTOCOL_DATA
#endif

#ifndef VS_RL_TQ_CONST_DATA
#define VS_RL_TQ_CONST_DATA
#endif

#ifndef VS_RL_TQ_DATA
#define VS_RL_TQ_DATA
#endif

/*
 * RealLink protocol defines
 * Do not edit these defines!
 */
#define Mode_ESC_RECEIVED             0x02
#define Mode_EXT_RECEIVED             0x04
#define Mode_SENT_INDICATION          0x08
#define Mode_MEMORY_READ_CONST        0x10
#define Mode_MEMORY_READ              0x20
#define Mode_MEMORY_WRITE             0x40
#define Mode_MEMORY_WRITE_PROGRESS    0x80
#define Flag_SENT_ESC_CODE            0x01
#define Flag_SEND_DATA_END            0x02
#define Host_ESCAPE_CODE              0xE0
#define Cmnd_EXTENDED_CMND            0xE1
#define Cmnd_EXECUTE                  0xE2
#define Cmnd_READ_CONST               0xE3
#define Cmnd_READ                     0xE4
#define Cmnd_WRITE                    0xE5
#define Cmnd_WRITE_END                0xE6
#define Cmnd_SET_PARAM1               0xE8
#define Cmnd_SET_PARAM2               0xE9
#define Cmnd_SET_PARAM3               0xEA
#define Cmnd_SET_PARAM4               0xEB
#define Cmnd_WRITE_STOP               0xEC
#define Cmnd_IND_ACK                  0xED
#define Cmnd_INIT                     0xEF
#define Host_ESC_BYTE_OK              0xC0
#define Host_COMMAND_NIBBLE           0xE0
#define Flag_GETINPUT                 0x0100
#define Flag_DEDUCT_EVENT             0x0200
#define Flag_GETARRAY_ADDR            0x0400
#define Flag_RESET                    0x0800
#define Flag_GETENDIAN_CONFIG         0x1000
#define Flag_REPEAT_SYSTEM            0x0200
#define Common_ESC_OFFSET             0x20
#define Target_ESCAPE_CODE            0xD0
#define Resp_ACK                      0xE1
#define Resp_END                      0xE2
#define Resp_DEDUCT_ACK               0xE5
#define Ind_HARDWARE_RESET            0xFF
#define Ind_CHECKSUM_ERROR            0xFE
#define Ind_COMMAND_FAILED            0xFD
#define Ind_MACROSTEP                 0xFC
#define Ind_ACTION                    0xFB
#define Ind_EVENT                     0xFA
#define Ind_INSTANCE_SET              0xF9
#define Ind_SIGNAL                    0xF8
#define Ind_SYSTEM_CONNECT            0xF7  
#define Ind_SYSTEM_FREE               0xF6
#define Ind_EVENT_ACTIVE              0xF4
#define Ind_EVENT_NOT_ACTIVE          0xF3
#define Ind_TRANS                     0xF2
#define VS_ENABLE_BREAKED             0x01
#define VS_ENABLE_MACROSTEP           0x02
#define VS_ENABLE_INSTANCE            0x04 
#define VS_ENABLE_ACTION              0x08
#define VS_ENABLE_EVENT               0x10
#define VS_ENABLE_SIGNAL              0x20
#define VS_ENABLE_SYSTEM              0x40
#define VS_ENABLE_TRANS               0x80
#define VS_SYSTEM_INDEX               0x0a
#define VS_DEDUCT_INDEX               0x05
#define VS_ACTION_INDEX               0x07
#define SIZEOF_VSPINFO                10 + VS_NOF_SYSTEMS
#define SIZEOF_VSSINFO                8

/*
 * Name        :    VS_RealLinkReceive
 *
 * Description :    Decodes the RealLink protocol.
 *                  Each char received from the visualSTATE Validator
 *                  must be processed by RealLink using this function.
 *                  
 *
 * Argument    :    ch:
 *                    char received. 
 *
 * Return      :    None.
 *
 * Portability :    ANSI-C Compiler.
 */
extern void VS_RealLinkReceive(unsigned char ch);


/*
 * Functions used internally by the RealLink protocol
 */
extern void VS_RealLinkTransmit(VS_INT16 value);
extern void VS_RealLinkDeductEvent(void);

/*
 * Variables used internally by the RealLink protocol
 */
extern unsigned char volatile VS_RL_TQ_PROTOCOL_DATA latestTransmission;
extern unsigned char volatile VS_RL_TQ_PROTOCOL_DATA modeRegister;

#if (VS_IS_EXPERT)
static unsigned char VS_RL_TQ_DATA systemNo;

#if (VS_USE_HEAP == 1)
/*
 * Name        :    VS_RealLinkInit
 *
 * Description :    Initialize the RealLink protocol.
 *                  Must be called after SMP_Init() but before any
 *                  other SMP_xxx() library function.
 *                  Use this version if heap memory is available   
 *                  
 *
 * Argument    :    ppTask:
 *                    VS_RLTASK  
 *
 * Return      :    Completion code:
 *
 *                    SES_MEM_ERR:
 *                      There is not enough heap memory.
 *
 *                    SES_OKAY:
 *                      Success.
 *
 * Portability :    ANSI-C Compiler.
 */

extern unsigned char VS_RealLinkInit(VS_RLTASK** ppTask);

/*
 * Name        :    VS_RealLinkFree
 *
 * Description :    Free the memory allocated by VS_RealLinkInit()
 *
 * Argument    :    pTask:
 *                    VS_RLTASK pointer 
 *
 * Return      :    None.
 *
 * Portability :    ANSI-C Compiler.
 */

extern void VS_RealLinkFree(VS_RLTASK* pTask);

#else /* VS_USE_HEAP == 0 */
/*
 * Name        :    VS_RealLinkInit
 *
 * Description :    Initialize the RealLink protocol.
 *                  Must be called after SMP_Init() but before any
 *                  other SMP_xxx() library function.
 *                  Use this version if no heap memory is available
 *                  
 *
 * Argument    :    pTask:
 *                    VS_RLTASK  
 *
 * Return      :    Completion code:
 *
 *                    SES_OKAY:
 *                      Success.
 *
 * Portability :    ANSI-C Compiler.
 */

extern unsigned char VS_RealLinkInit(VS_RLTASK* pTask);

#endif /* VS_USE_HEAP == 1 */

#define VS_WAIT(context)  if((context) != NULL) { \
                            VS_BOOL bStatus; \
                            VS_RL_ENTER_CRITICALSECTION \
                            bStatus = (((context)->pTask->RLConfig & VS_ENABLE_MACROSTEP) == VS_ENABLE_MACROSTEP) && ((modeRegister & Mode_MEMORY_WRITE_PROGRESS) != Mode_MEMORY_WRITE_PROGRESS); \
                            VS_RL_LEAVE_CRITICALSECTION \
                            if(bStatus){ \
                              latestTransmission = Ind_MACROSTEP; \
                              systemNo = (context)->systemNo; \
                              VS_RealLinkTransmit(Ind_MACROSTEP);\
                              (VS_RL_TRANSMIT)((context)->systemNo); \
                              (context)->pTask->RLConfig |= VS_ENABLE_BREAKED; \
                              bStatus = 1; \
                              while(bStatus) { \
                                VS_RL_ENTER_CRITICALSECTION \
                                bStatus = ((((context)->pTask->RLConfig & VS_ENABLE_BREAKED) == VS_ENABLE_BREAKED) || ((modeRegister & Mode_MEMORY_WRITE_PROGRESS) == Mode_MEMORY_WRITE_PROGRESS)); \
                                VS_RL_LEAVE_CRITICALSECTION \
                                VS_RealLinkDeductEvent(); }}} 

#define VS_WAIT_ELM(context, elm, ind, data)  { VS_BOOL bStatus; \
                                                VS_RL_ENTER_CRITICALSECTION \
                                                bStatus = (((context)->pTask->RLConfig & elm) == elm) && ((modeRegister & Mode_MEMORY_WRITE_PROGRESS) != Mode_MEMORY_WRITE_PROGRESS); \
                                                VS_RL_LEAVE_CRITICALSECTION \
                                                if(bStatus){ \
                                                  MemCopy(((context)->pTask->RLBuffer), (const unsigned char *) data, sizeof(*data)); \
                                                  latestTransmission = ind; \
                                                  systemNo = (context)->systemNo; \
                                                  VS_RealLinkTransmit(ind); \
                                                  (VS_RL_TRANSMIT)((context)->systemNo); \
                                                  (context)->pTask->RLConfig |= VS_ENABLE_BREAKED; \
                                                  bStatus = 1; \
                                                  while(bStatus) { \
                                                    VS_RL_ENTER_CRITICALSECTION \
                                                    bStatus = (((context)->pTask->RLConfig & VS_ENABLE_BREAKED) == VS_ENABLE_BREAKED) || ((modeRegister & Mode_MEMORY_WRITE_PROGRESS) == Mode_MEMORY_WRITE_PROGRESS); \
                                                    VS_RL_LEAVE_CRITICALSECTION }}}

#else /* BASIC  API */


/*
 * Name        :    VS_RealLinkInit
 *
 * Description :    Initialize the RealLink protocol.
 *                  Must be called after SEM_Init() but before any
 *                  other SEM_xxx() library function.
 *                  
 *
 * Argument    :    None.
 *
 * Return      :    None.
 *
 * Portability :    ANSI-C Compiler.
 */
extern void VS_RealLinkInit(void);

#define VS_WAIT() if(((SEM.pTask->RLConfig & VS_ENABLE_MACROSTEP) == VS_ENABLE_MACROSTEP)&&((modeRegister & Mode_MEMORY_WRITE_PROGRESS) != Mode_MEMORY_WRITE_PROGRESS)){ \
                    latestTransmission = Ind_MACROSTEP; \
                    VS_RealLinkTransmit(Ind_MACROSTEP);\
                    SEM.pTask->RLConfig |= VS_ENABLE_BREAKED; \
                    while(((SEM.pTask->RLConfig & VS_ENABLE_BREAKED) == VS_ENABLE_BREAKED) || ((modeRegister & Mode_MEMORY_WRITE_PROGRESS) == Mode_MEMORY_WRITE_PROGRESS))\
                      VS_RealLinkDeductEvent();\
                    }

#define VS_WAIT_ELM(elm, ind, data)   if(((SEM.pTask->RLConfig & elm) == elm)&&((modeRegister & Mode_MEMORY_WRITE_PROGRESS) != Mode_MEMORY_WRITE_PROGRESS)){ \
                                        MemCopy(SEM.pTask->RLBuffer, (const unsigned char *) data, sizeof(*data)); \
                                        latestTransmission = ind; \
                                        VS_RealLinkTransmit(ind);\
                                        SEM.pTask->RLConfig |= VS_ENABLE_BREAKED; \
                                        while(((SEM.pTask->RLConfig & VS_ENABLE_BREAKED) == VS_ENABLE_BREAKED) || ((modeRegister & Mode_MEMORY_WRITE_PROGRESS) == Mode_MEMORY_WRITE_PROGRESS));}

#endif /* VS_EXPERT_MODE */
#endif /* __SEMREALLINK_H */
#endif /* VS_REALLINK */





