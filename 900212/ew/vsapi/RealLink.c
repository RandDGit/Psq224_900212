/*
 * Type:        Source Code File
 *
 * Id:          RealLink.c
 *
 * Function:    Contains the functions for the RealLink SEM API version 4
 *
 * Portability: ANSI-C Compiler.
 *
 * Copyright (c) 2000 IAR Systems. All rights reserved.
 */

#include "RealLink.h"

#if (VS_REALLINKMODE)

#if (VS_IS_EXPERT)
extern SEM_CONTEXT * vsrl_aVspContextPointer[VS_NOF_SYSTEMS];
extern VS_VOID volatile const VS_RL_TQ_CONST_DATA * const VS_RL_TQ_CONST_DATA vsrl_aVspInfo[SIZEOF_VSPINFO];
#else /* BASIC API */
extern VS_VOID volatile const VS_RL_TQ_CONST_DATA * const VS_RL_TQ_CONST_DATA vsrl_aVspInfo[SIZEOF_VSPINFO];
extern VS_VOID const VS_RL_TQ_CONST_DATA * const VS_RL_TQ_CONST_DATA vsrl_aVssInfo_0[SIZEOF_VSSINFO];
#endif


static VS_UINT8                 GetNextByte(void);
#if (VS_RL_USE_EXTKWS)
static VS_VOIDPTR_TYPE volatile VS_RL_TQ_PROTOCOL_DATA memoryAddressConst;
#endif
static VS_VOIDPTR_TYPE volatile VS_RL_TQ_PROTOCOL_DATA memoryAddress;    
static VS_VOIDPTR_TYPE volatile VS_RL_TQ_PROTOCOL_DATA parameter[4];
static VS_VOIDPTR_TYPE volatile VS_RL_TQ_PROTOCOL_DATA receiveAccumulator;
static VS_UINT16 volatile       VS_RL_TQ_PROTOCOL_DATA byteCount;
static VS_UINT8 volatile        VS_RL_TQ_PROTOCOL_DATA flagRegister; 
static VS_UINT8 volatile        VS_RL_TQ_PROTOCOL_DATA checksum;
static SEM_EVENT_TYPE volatile  VS_RL_TQ_PROTOCOL_DATA storedEvent;
volatile VS_UINT8               VS_RL_TQ_PROTOCOL_DATA latestTransmission; 
volatile VS_UINT8               VS_RL_TQ_PROTOCOL_DATA modeRegister; 
#if (VS_IS_EXPERT)
VS_UINT8                        VS_RL_TQ_PROTOCOL_DATA storedSystem;
#endif

/*
 *
 */
static void PrepareChecksum( void )
{
  VS_UINT8 temp1, temp;
  temp1 = checksum + 4;
  temp = (temp1 & 0x80);
  temp1 <<= 1;
  checksum = temp1 + (temp ? 1 : 0);
  return;
}

/*
 *
 */
void VS_RealLinkTransmit(VS_INT16 value) 
{
#if (VS_RL_USE_EXTKWS)
  if((modeRegister & Mode_MEMORY_READ) != Mode_MEMORY_READ && (modeRegister & Mode_MEMORY_READ_CONST) != Mode_MEMORY_READ_CONST)
#else
  if((modeRegister & Mode_MEMORY_READ) != Mode_MEMORY_READ)
#endif
  {
    if((VS_UINT8)value > Resp_END)
    {
      checksum = 0;
      receiveAccumulator = 0;
      modeRegister |= Mode_SENT_INDICATION;
    }
  }

#if (VS_IS_EXPERT)
  if((value & Flag_REPEAT_SYSTEM) != Flag_REPEAT_SYSTEM)
#endif
    latestTransmission = (VS_UINT8)value;
  (VS_RL_TRANSMIT)((VS_UINT8)value);

#if (VS_IS_EXPERT)
  if((value & Flag_REPEAT_SYSTEM) == Flag_REPEAT_SYSTEM)
    (VS_RL_TRANSMIT)(systemNo);
#endif

#if (VS_RL_USE_EXTKWS)
  if((modeRegister & Mode_MEMORY_READ) == Mode_MEMORY_READ || (modeRegister & Mode_MEMORY_READ_CONST) == Mode_MEMORY_READ_CONST)
#else
  if((modeRegister & Mode_MEMORY_READ) == Mode_MEMORY_READ)
#endif
  {
    unsigned char nRes;
    while(( nRes = GetNextByte() ) != Resp_END)
      (VS_RL_TRANSMIT)(nRes);

#if (VS_RL_USE_EXTKWS)
    (modeRegister & Mode_MEMORY_READ) == Mode_MEMORY_READ ? (modeRegister &= ~Mode_MEMORY_READ) : (modeRegister &= ~Mode_MEMORY_READ_CONST);
#else
    modeRegister &= ~Mode_MEMORY_READ;
#endif
    latestTransmission = nRes;
    (VS_RL_TRANSMIT)(nRes);
  }

  VS_RL_TRANSMIT_FLUSH();
}

/*
 *
 */
#if (VS_IS_EXPERT)
typedef unsigned char (* DEDUCT_FUNC_TYPE)(SEM_CONTEXT*, SEM_EVENT_TYPE);
#else
typedef unsigned char (* DEDUCT_FUNC_TYPE)(SEM_EVENT_TYPE);
typedef void (* ACTION_FUNC_TYPE)(void);
#endif

void VS_RealLinkDeductEvent(void)
{
  if(storedEvent == EVENT_TERMINATION_ID)
    return;
  else
  {
    SEM_ACTION_EXPRESSION_TYPE ActionExpress;
    VS_UINT8 localFlag = Resp_DEDUCT_ACK;
    unsigned char CC;
#if (VS_IS_EXPERT)
    SEM_CONTEXT* pContext = vsrl_aVspContextPointer[storedSystem];
    DEDUCT_FUNC_TYPE pDeduct;
    pDeduct = **(DEDUCT_FUNC_TYPE VS_RL_TQ_CONST_DATA * VS_RL_TQ_CONST_DATA *)((VS_VOID * VS_RL_TQ_CONST_DATA * VS_RL_TQ_CONST_DATA *)vsrl_aVspInfo[VS_SYSTEM_INDEX + storedSystem] + VS_DEDUCT_INDEX);
#else
    DEDUCT_FUNC_TYPE pDeduct = *((DEDUCT_FUNC_TYPE VS_RL_TQ_CONST_DATA *) vsrl_aVssInfo_0[VS_DEDUCT_INDEX]);
#endif
    
#if (VS_IS_EXPERT)
    if((CC=pDeduct(pContext, storedEvent)) != SES_OKAY)
      localFlag = Ind_COMMAND_FAILED;

    while ((CC = SMP_GetOutput (pContext, &ActionExpress)) == SES_FOUND)
      SMP_TableAction (pContext, ((VS_VOID (* VS_RL_TQ_CONST_DATA *)(SEM_CONTEXT VS_RL_TQ_DATA *))*((VS_VOID VS_RL_TQ_CONST_DATA * VS_RL_TQ_CONST_DATA *)vsrl_aVspInfo[VS_SYSTEM_INDEX + storedSystem] + VS_ACTION_INDEX)), ActionExpress);
    
    if(CC != SES_OKAY)
      localFlag = Ind_COMMAND_FAILED;
    
    if((CC = SMP_NextState(pContext)) != SES_OKAY)
      localFlag = Ind_COMMAND_FAILED;
#else /* BASIC API */
    if((CC=pDeduct(storedEvent)) != SES_OKAY)
      localFlag = Ind_COMMAND_FAILED;
    
    while ((CC = SEM_GetOutput (&ActionExpress)) == SES_FOUND)
      SEM_TableAction((ACTION_FUNC_TYPE VS_RL_TQ_CONST_DATA *)vsrl_aVssInfo_0[VS_ACTION_INDEX], ActionExpress);
      
    if(CC != SES_OKAY)
      localFlag = Ind_COMMAND_FAILED;
    
    if((CC = SEM_NextState()) != SES_OKAY)
      localFlag = Ind_COMMAND_FAILED;
#endif /* VS_EXPERT_MODE */
    
    VS_RealLinkTransmit(localFlag);
    
#if (VS_IS_EXPERT)
    (VS_RL_TRANSMIT)(storedSystem);
    pContext->pTask->RLConfig |= VS_ENABLE_BREAKED;
#else
    SEM.pTask->RLConfig |= VS_ENABLE_BREAKED;
#endif
    
    storedEvent = EVENT_TERMINATION_ID;
  }
}

/*
 *
 */
void VS_RealLinkReceive(VS_UINT8 value)
{
  VS_UINT16 localFlag  = 0;

  VS_RL_ENTER_CRITICALSECTION;

  if( Host_ESCAPE_CODE == value )
  {
    if( Mode_ESC_RECEIVED & modeRegister )
      localFlag = Ind_COMMAND_FAILED;
    else
      modeRegister |= Mode_ESC_RECEIVED;

    if((VS_UINT8)localFlag != 0)
      VS_RealLinkTransmit( (VS_UINT8) localFlag );

    VS_RL_LEAVE_CRITICALSECTION;
    return;
  }

  if( Mode_ESC_RECEIVED & modeRegister )
  {
    modeRegister &= ~Mode_ESC_RECEIVED;

    if( Host_ESC_BYTE_OK != (value & 0xF0))
    {
      VS_RealLinkTransmit( Ind_COMMAND_FAILED );
      VS_RL_LEAVE_CRITICALSECTION;
      return;
    }
    else
    {
      value += Common_ESC_OFFSET;
      localFlag |= 0x0100;
    }
  }

  PrepareChecksum();
  checksum ^= value;

  if( (Host_COMMAND_NIBBLE == (value & 0xF0) ) && !(localFlag & 0x0100) )
  {
    if( Cmnd_EXTENDED_CMND == value )
    {
      if( Mode_EXT_RECEIVED & modeRegister )
        localFlag |=  Ind_COMMAND_FAILED;
      else
        modeRegister |= Mode_EXT_RECEIVED;
    }
    
    else if ( Cmnd_INIT == value )
    {
      flagRegister          = 0; 
      checksum              = 0; 
      receiveAccumulator    = 0; 
      modeRegister          = 0; 
      memoryAddress         = 0;    
      byteCount             = 0;
      latestTransmission    = 0;
      storedEvent           = EVENT_TERMINATION_ID;    
#if (VS_IS_EXPERT)
      storedSystem          = 0;
#endif      
      localFlag             = Resp_ACK;
    }
    
    else
    {
      VS_UINT8 parameterNo = 0;

      if( Mode_SENT_INDICATION & modeRegister )
      {
        if( Cmnd_IND_ACK == value ) 
        {
          if( (VS_UINT8) receiveAccumulator == (VS_UINT8) ~latestTransmission )
          {            
            modeRegister      &= ~Mode_SENT_INDICATION;
            checksum           = 0;
            receiveAccumulator = 0;
            localFlag         |= Resp_ACK;
            latestTransmission = Resp_ACK;
          }
          else
          {
            localFlag |= latestTransmission;
#if (VS_IS_EXPERT)
            localFlag |= Flag_REPEAT_SYSTEM;
#endif
          }
        }
        else
        {
          localFlag |= latestTransmission;
#if (VS_IS_EXPERT)
          localFlag |= Flag_REPEAT_SYSTEM;
#endif
        }
      }
      
      else if( (modeRegister & Mode_MEMORY_WRITE ) && (Cmnd_WRITE_STOP != value) )
      {
        localFlag |= Ind_COMMAND_FAILED;
      }
      else
      {
        if( !(0x08 & value) && checksum )
          localFlag |= Ind_CHECKSUM_ERROR;
        else
        {
          switch( value )
          {
          case Cmnd_EXECUTE:
            localFlag    |= Resp_ACK;
            
            if(Flag_DEDUCT_EVENT & receiveAccumulator)
            {
#if (VS_IS_EXPERT)
              SEM_CONTEXT* pContext = vsrl_aVspContextPointer[(VS_UINT8)parameter[0]];
              storedEvent  = *((SEM_EVENT_TYPE*)pContext->pTask->RLBuffer);
              storedSystem = (VS_UINT8)parameter[0];
#else /* BASIC API */
              storedEvent = *((SEM_EVENT_TYPE*)SEM.pTask->RLBuffer);
#endif
            }

            if(Flag_GETINPUT & receiveAccumulator)
            {
#if (VS_IS_EXPERT)
              SEM_EVENT_TYPE EventNo = EVENT_UNDEFINED;
              SEM_EVENT_TYPE KeyTable[2];
              SEM_CONTEXT* pContext = vsrl_aVspContextPointer[(VS_UINT8)parameter[1]];
              KeyTable[0] = (SEM_EVENT_TYPE)parameter[0];
              KeyTable[1] = EVENT_TERMINATION_ID;
              localFlag = Ind_EVENT_NOT_ACTIVE;
              if(SMP_Inquiry(pContext) == SES_OKAY)
              {
                if(SMP_GetInput(pContext, &EventNo, KeyTable) == SES_FOUND)
                  localFlag = Ind_EVENT_ACTIVE;
                pContext->InqAct = VS_FALSE;
              }
#else /* BASIC API */
#if (SEM_INQUIRY == 1)
              SEM_EVENT_TYPE EventNo = EVENT_UNDEFINED;
              SEM_EVENT_TYPE KeyTable[2];
              KeyTable[0] = (SEM_EVENT_TYPE)parameter[0];
              KeyTable[1] = EVENT_TERMINATION_ID;
              
              localFlag = Ind_EVENT_NOT_ACTIVE; 
              if(SEM_Inquiry() == SES_OKAY)
              {
                if(SEM_GetInput(&EventNo, KeyTable) == SES_FOUND)
                  localFlag = Ind_EVENT_ACTIVE;
                SEM.InqAct = VS_FALSE;
              }
#else
              localFlag = Ind_EVENT_ACTIVE; 
#endif /* SEM_INQUIRY */
#endif /* VS_EXPERT_MODE */
            }

            if(Flag_GETENDIAN_CONFIG & receiveAccumulator )
            {
              static VS_RL_TQ_DATA VS_INT16 nIndian = 0x01;
              memoryAddress = (VS_VOIDPTR_TYPE)&nIndian;
              byteCount = (VS_UINT16)sizeof(VS_INT16);              
              modeRegister |= Mode_MEMORY_READ;
              receiveAccumulator = 0;
              localFlag |= Resp_ACK;
            }
            
            if(Flag_GETARRAY_ADDR & receiveAccumulator )
            {
              static VS_RL_TQ_DATA VS_VOID const VS_RL_TQ_CONST_DATA * pTbl = &vsrl_aVspInfo;
              memoryAddress = (VS_VOIDPTR_TYPE)&pTbl;
              byteCount = (VS_UINT16)sizeof(void VS_RL_TQ_CONST_DATA *);
              modeRegister |= Mode_MEMORY_READ;
              receiveAccumulator = 0;
              localFlag |= Resp_ACK;
              
            }
            
            if( Flag_RESET & receiveAccumulator )
            {
              (VS_RL_RESET)();
            }
            
            receiveAccumulator = 0;
            break;
            

          case Cmnd_READ_CONST:
#if (VS_RL_USE_EXTKWS)
            memoryAddressConst = (VS_VOIDPTR_TYPE)(parameter[0] + ((signed short) ((signed char)((receiveAccumulator >> 8) & 0x00FF))));
            byteCount = (VS_UINT16)parameter[2];
            parameter[0] += parameter[2];
            modeRegister |= Mode_MEMORY_READ_CONST;
            receiveAccumulator = 0;
            localFlag |= Resp_ACK;
            break;
#endif
          case Cmnd_READ:
            memoryAddress = (VS_VOIDPTR_TYPE)(parameter[0] + ((signed short) ((signed char)((receiveAccumulator >> 8) & 0x00FF))));
            byteCount = (VS_UINT16)parameter[2];
            parameter[0] += parameter[2];
            modeRegister |= Mode_MEMORY_READ;
            receiveAccumulator = 0;
            localFlag |= Resp_ACK;
            break;
            
          case Cmnd_WRITE:
            if( modeRegister & Mode_MEMORY_WRITE )
              localFlag |= Ind_COMMAND_FAILED; 
            else
            {
              memoryAddress =  (VS_VOIDPTR_TYPE)(parameter[0] + ((signed short) ((signed char)((receiveAccumulator >> 8) & 0x00FF))));
              modeRegister |= (Mode_MEMORY_WRITE | Mode_MEMORY_WRITE_PROGRESS);
              receiveAccumulator = 0;
            }
            break;
            
          case Cmnd_WRITE_END:
            modeRegister &= ~Mode_MEMORY_WRITE_PROGRESS; 
            localFlag |= Resp_ACK;
            receiveAccumulator = 0;
            break;
          
          case Cmnd_SET_PARAM4:
            ++parameterNo;
          case Cmnd_SET_PARAM3:
            ++parameterNo;
          case Cmnd_SET_PARAM2:
            ++parameterNo;
          case Cmnd_SET_PARAM1:
            parameter[parameterNo] = receiveAccumulator;
            receiveAccumulator = 0;
            break;
          
          case Cmnd_WRITE_STOP:
            if( modeRegister & Mode_MEMORY_WRITE )
            {
              modeRegister &= ~Mode_MEMORY_WRITE;
              parameter[0] = memoryAddress; 
            }
            else
              localFlag |= Ind_COMMAND_FAILED; 
            break;
          
          case Cmnd_IND_ACK:
            receiveAccumulator = 0;
            checksum           = 0;
            localFlag         |= Resp_ACK; 
            break;
          
          default:
            break;
          }
        }
      }
    }
  }  
  
  else
  {
    if( Mode_MEMORY_WRITE & modeRegister )
    {
      *(volatile unsigned char VS_RL_TQ_DATA *) memoryAddress = value;
      ++memoryAddress;
    }
    
    else
    {
      receiveAccumulator <<= 8;
      receiveAccumulator += value;
    }
  }

  if(((VS_UINT8)localFlag != 0) && ((localFlag & Flag_REPEAT_SYSTEM) != Flag_REPEAT_SYSTEM))
    VS_RealLinkTransmit(localFlag);

  VS_RL_LEAVE_CRITICALSECTION;
}


/*
 *
 */
static VS_UINT8 GetNextByte( void )
{
  VS_UINT8 value;
  if(0 == byteCount)
  {
    if( Flag_SEND_DATA_END & flagRegister )
    {
      flagRegister &= ~Flag_SEND_DATA_END;
      checksum = 0;
      value    = Resp_END;
    }
    else if( Flag_SENT_ESC_CODE & flagRegister )
    {
      flagRegister &= ~Flag_SENT_ESC_CODE;
      flagRegister |= Flag_SEND_DATA_END;
      value = checksum - Common_ESC_OFFSET;
    }
    else
    {
      PrepareChecksum();
      checksum ^= 0x65;

      if((checksum >= Resp_ACK) || (checksum == Target_ESCAPE_CODE))
      {
        flagRegister |= Flag_SENT_ESC_CODE;
        value         = Target_ESCAPE_CODE;
      }
      else
      {
        flagRegister |= Flag_SEND_DATA_END;
        value         = checksum;
      }
    }
  }
  else
  {
#if (VS_RL_USE_EXTKWS)
    if((modeRegister & Mode_MEMORY_READ_CONST) == Mode_MEMORY_READ_CONST)
      value = * (volatile unsigned char VS_RL_TQ_CONST_DATA *) memoryAddressConst;
    else
      value = * (volatile unsigned char VS_RL_TQ_DATA *) memoryAddress;
#else
    value = * (volatile unsigned char VS_RL_TQ_DATA *) memoryAddress;
#endif
    if( flagRegister & Flag_SENT_ESC_CODE )
    {
      flagRegister &= ~Flag_SENT_ESC_CODE;
      value        -= Common_ESC_OFFSET;
    }
    else
    {
      PrepareChecksum();
      checksum ^= value;
      if(value >= Target_ESCAPE_CODE)
      {
        flagRegister |= Flag_SENT_ESC_CODE;
        return Target_ESCAPE_CODE;
      }
    }
#if (VS_RL_USE_EXTKWS)
    if((modeRegister & Mode_MEMORY_READ_CONST) == Mode_MEMORY_READ_CONST)
      ++memoryAddressConst;
    else
      ++memoryAddress;
#else
    ++memoryAddress;
#endif
    --byteCount;
  }

  return value;
}

void MemCopy(unsigned char VS_RL_TQ_DATA * pDest, const unsigned char * pSrc, unsigned char size) 
{
  unsigned char i = 0;
  for(; i < size; i++)
    *(pDest + i) = *(pSrc + i);
}

/*
 *
 */
#if (VS_IS_EXPERT)

#if (VS_USE_HEAP == 1)
unsigned char VS_RealLinkInit(VS_RLTASK** ppTask)
#else
unsigned char VS_RealLinkInit(VS_RLTASK* pTask)
#endif
{
#if (VS_USE_HEAP == 1)  
  VS_RLTASK* pTask;
  if ((pTask = (VS_RLTASK*)malloc (sizeof (VS_RLTASK))) == NULL)
    return (SES_MEM_ERR);

  *ppTask = pTask;
#endif
  
  pTask->RLConfig = 0;

  flagRegister          = 0; 
  checksum              = 0; 
  receiveAccumulator    = 0; 
  modeRegister          = 0; 
  memoryAddress         = 0;    
  byteCount             = 0;
  latestTransmission    = 0;
  storedSystem          = 0;
  storedEvent           = EVENT_TERMINATION_ID;
  VS_RealLinkTransmit(Ind_HARDWARE_RESET); 

  return (SES_OKAY);
}

#if (VS_USE_HEAP == 1)
void VS_RealLinkFree(VS_RLTASK* pTask)
{
  if(pTask != NULL)
  {
    free(pTask);
    pTask = NULL;
  }
}
#endif

#else /* BASIC API */
void VS_RealLinkInit(void)
{
  flagRegister          = 0; 
  checksum              = 0; 
  receiveAccumulator    = 0; 
  modeRegister          = 0; 
  memoryAddress         = 0;    
  byteCount             = 0;
  latestTransmission    = 0;
  storedEvent           = EVENT_TERMINATION_ID;

  SEM.pTask->RLConfig   = 0;
  VS_RealLinkTransmit(Ind_HARDWARE_RESET); 
}
#endif 

#endif
