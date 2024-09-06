/*
 * Type:        Source Code File
 *
 * Id:          SEMLibB.c
 *
 * Function:    Contains the functions for the SEM Library Basic version 4,
 *              which is described in SEMLibB.h.
 *
 * Portability: ANSI-C Compiler.
 *
 * Copyright (c) 1999 IAR Systems. All rights reserved.
 */

/*
 * Version   ! Date   ! Status ! Changes
 *----------------------------------------------------------------------------
 * 4.0.0.13  ! 240299 ! Closed ! -
 * 4.0.4     ! 010799 ! Closed ! Minor update, there might be no changes to this particular file.
 */

#include "SEMLibB.h"

#if (SEM_SPEED)
#define MODE_SEM_EVENT_ONLY     0x01
#define MODE_SEM_EVENT          0x02
#define MODE_SEM_STATE          0x03
#endif

#if (SEM_SIGNAL_DB)
extern void SEM_SignalDB (SEM_EVENT_TYPE SignalNo);
#endif

#if (VS_REALLINKMODE == 1)
#include "RealLink.h"
VS_RLTASK VS_RL_TQ_DATA vsRLTask;
#endif

#ifdef VS_RUNTIME_INFO
VSRunTimeInfo volatile const VS_RUNTIME_INFO_EXTKW vsRunTimeInfo = 
{
  VS_SIGNATURE_VERSION,
  VS_SIGNATURE_CONTENT
};
#endif



#if (SEM_SIGNAL)
void SEM_InitSignalQueue (void)
{
  SEM.SPut = 0;
  SEM.SGet = 0;
  SEM.SUsed = 0;
}


static unsigned char SEM_SignalQueuePut (SEM_EVENT_TYPE SignalNo)
{
  if (SEM.SUsed == VS_SIGNAL_QUEUE_SIZE)
    return (SES_SIGNAL_QUEUE_FULL);
  SEM.SUsed++;
  SEM.SQueue[SEM.SPut] = SignalNo;
  if (++SEM.SPut == VS_SIGNAL_QUEUE_SIZE)
    SEM.SPut = 0;
  return (SES_OKAY);
}


static SEM_EVENT_TYPE SEM_SignalQueueGet (void)
{
  SEM_EVENT_TYPE SignalNo;

  if (!SEM.SUsed)
    return (EVENT_UNDEFINED);
  SEM.SUsed--;
  SignalNo = SEM.SQueue[SEM.SGet];
  if (++SEM.SGet == VS_SIGNAL_QUEUE_SIZE)
    SEM.SGet = 0;
  return (SignalNo);
}


#if (SEM_SIGNAL_QUEUE_INFO == 1)
void SEM_SignalQueueInfo (SEM_SIGNAL_QUEUE_TYPE *NofSignals)
{
  *NofSignals = SEM.SUsed;
}
#endif
#endif



#if (SEM_INIT_ALL)
void SEM_InitAll (void)
{
  SEM_Init();
#if (SEM_INIT_EXTERNAL_VARIABLES)
  SEM_InitExternalVariables();
#endif
#if (SEM_INIT_INTERNAL_VARIABLES)
  SEM_InitInternalVariables();
#endif
#if (SEM_SIGNAL)
  SEM_InitSignalQueue();
#endif
#if (VS_NOF_INSTANCES > 1)
  SEM_InitInstances();
#if (VS_REALLINKMODE == 1)
  SEM_SetInstance(0);
#endif
#endif
}
#endif



void SEM_Init (void)
{
#ifdef VS_RUNTIME_INFO
  *vsRunTimeInfo.pSignatureVersion;
#endif
#if (VS_NOF_STATE_MACHINES != 0)
  for (SEM.i = 0; SEM.i < VS_NOF_STATE_MACHINES; SEM.i++)
  {
    SEM.WSV[SEM.i] = STATE_UNDEFINED;
#if (VS_NOF_INSTANCES == 1)
    SEM.CSV[SEM.i] = STATE_UNDEFINED;
#endif
  }
#if (SEM_NEXT_STATE_CHG == 1)
  SEM.Chg = 0;
#endif
#endif
  SEM.State = STATE_SEM_INITIALIZE;
#if (SEM_INQUIRY == 1)
  SEM.InqAct = VS_FALSE;
#endif

#if (VS_REALLINKMODE == 1)
  SEM.pTask = &vsRLTask;
#endif
}


#if (VS_NOF_INSTANCES > 1)
unsigned char SEM_InitInstances (void)
{
#if (VS_NOF_STATE_MACHINES != 0)
  for (SEM.i = 0; SEM.i < (VS_NOF_STATE_MACHINES * VS_NOF_INSTANCES); SEM.i++)
  {
    SEM.Ins[SEM.i] = STATE_UNDEFINED;
  }
#endif
  SEM.CSV = SEM.Ins;
  SEM.ActIns = 0;

  return(SES_OKAY);
}


unsigned char SEM_SetInstance (SEM_INSTANCE_TYPE Instance)
{
  if (Instance >= VS_NOF_INSTANCES)
    return (SES_RANGE_ERR);
  if ((SEM.State != STATE_SEM_INITIALIZE)
#if (SEM_INQUIRY == 1)
       || (SEM.InqAct)
#endif
     )
    return (SES_ACTIVE);
#if (VS_NOF_STATE_MACHINES != 0)
  SEM.CSV = (SEM.Ins + VS_NOF_STATE_MACHINES * Instance);
  SEM.ActIns = Instance;
#endif
#if (VS_REALLINKMODE == 1)
  VS_WAIT_ELM(VS_ENABLE_INSTANCE, Ind_INSTANCE_SET, &Instance);
#endif
  return (SES_OKAY);
}
#endif


#if (SEM_DEDUCT_FUNC == 0)
unsigned char SEM_Deduct (SEM_EVENT_TYPE EventNo)
{
  if (VS_NOF_EVENTS <= EventNo)
    return (SES_RANGE_ERR);
  SEM.EventNo = EventNo;
  SEM.State = STATE_SEM_PREPARE;
#if (VS_NOF_EVENT_GROUPS != 0)
  SEM.DIt = 2;
#endif
#if (SEM_INQUIRY == 1)
  if (SEM.InqAct)
    return (SES_ACTIVE);
#endif
#if (VS_REALLINKMODE == 1)
  VS_WAIT_ELM(VS_ENABLE_EVENT, Ind_EVENT, &EventNo);
#endif
  return (SES_OKAY);
}
#endif


unsigned char SEM_GetOutput (SEM_ACTION_EXPRESSION_TYPE *ActionNo)
{
  for(;;)
  {
    switch (SEM.State)
    {
#if (SEM_SIGNAL)
      case STATE_SEM_SIGNAL :
Signal :
        SEM.EventNo = SEM_SignalQueueGet ();
        if (SEM.EventNo == EVENT_UNDEFINED)
        {
          SEM.State = STATE_SEM_OKAY;
          return (SES_OKAY);
        }
#if (VS_NOF_STATE_MACHINES != 0)
        for (SEM.i = 0; SEM.i < VS_NOF_STATE_MACHINES; SEM.i++)
        {
          if (SEM.WSV[SEM.i] != STATE_UNDEFINED)
          {
#if (SEM_NEXT_STATE_CHG == 1)
            if (SEM.CSV[SEM.i] != SEM.WSV[SEM.i])
              SEM.Chg = 1;
#endif
            SEM.CSV[SEM.i] = SEM.WSV[SEM.i];
            SEM.WSV[SEM.i] = STATE_UNDEFINED;
          }
        }
#endif
#if (SEM_SIGNAL_DB)
        SEM_SignalDB (SEM.EventNo);
#endif
#if (VS_REALLINKMODE == 1)
        VS_WAIT_ELM(VS_ENABLE_SIGNAL, Ind_SIGNAL, &SEM.EventNo);
#endif 
        goto FirstRule;
#endif
      case STATE_SEM_PREPARE :
#if (SEM_SPEED)
        SEM.Mode = MODE_SEM_EVENT_ONLY;
#endif
#if (VS_NOF_EVENT_GROUPS != 0)
#if (SEM_EVENT_GROUP_INDEX)
        if (SEM.DIt == 1)
        {
          SEM.EventNo = VS.EGI[SEM.EventNo];
          if (SEM.EventNo == EVENT_GROUP_UNDEFINED)
          {
#if (SEM_SIGNAL)
            SEM.DIt = 0;
            goto Signal;
#else
            SEM.State = STATE_SEM_OKAY;
            return (SES_OKAY);
#endif
          }
          SEM.EventNo += VS_NOF_EVENTS;
        }
        else if (SEM.DIt == 0)
        {
#if (SEM_SIGNAL)
          goto Signal;
#else
          SEM.State = STATE_SEM_OKAY;
          return (SES_OKAY);
#endif
        }
        SEM.DIt--;
#endif
#if (SEM_EVENT_GROUP_TABLE_INDEX)
        if (SEM.DIt == 0)
        {
          if (++SEM.iFirstEgi >= SEM.iLastEgi)
          {
#if (SEM_SIGNAL)
            goto Signal;
#else
            SEM.State = STATE_SEM_OKAY;
            return (SES_OKAY);
#endif
          }
          SEM.EventNo = VS.EGT[SEM.iFirstEgi];
          SEM.EventNo += VS_NOF_EVENTS;
        }
        else if (SEM.DIt == 1)
        {
          SEM.iFirstEgi = VS.EGTI[SEM.EventNo];
          SEM.iLastEgi = VS.EGTI[SEM.EventNo + 1];
          if (SEM.iFirstEgi == SEM.iLastEgi)
          {
#if (SEM_SIGNAL)
            SEM.DIt = 0;
            goto Signal;
#else
            SEM.State = STATE_SEM_OKAY;
            return (SES_OKAY);
#endif
          }
          SEM.EventNo = VS.EGT[SEM.iFirstEgi];
          SEM.EventNo += VS_NOF_EVENTS;
          SEM.DIt--;
        }
        else if (SEM.DIt == 2)
        {
          SEM.DIt--;
        }
#endif
#endif
#if (SEM_SIGNAL)
FirstRule :
#endif
        SEM.iFirstR = VS.RTI[SEM.EventNo];
        SEM.iLastR = VS.RTI[SEM.EventNo + 1];
        SEM.State = STATE_SEM_CONSULT;
      case STATE_SEM_CONSULT :
        while (SEM.iFirstR < SEM.iLastR)
        {
#if (SEM_SPEED)
          if (SEM.Mode == MODE_SEM_EVENT)
          {
            SEM.iRI = VS.RIS[SEM.iFirstR++];
          }
          else
          {
            if (SEM.Mode == MODE_SEM_EVENT_ONLY)
            {
              SEM.iRI = VS.RI[SEM.iFirstR++];
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_16_BIT)
              SEM.i = VS.RD[SEM.iRI];
              SEM.iRI += 2;
              SEM.nNxt = (unsigned char)(SEM.i & 0x0F);
              SEM.nAction = (unsigned char)(SEM.i >> 4);
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_2 && SEM_RDHW_WIDTH_24_BIT)
              SEM.iRI++;
              SEM.nNxt = (unsigned char)(VS.RD[SEM.iRI++] >> 4);
              SEM.i = VS.RD[SEM.iRI++];
              SEM.nAction = (unsigned char)(SEM.i & 0x0F);
              SEM.nSignal = (unsigned char)(SEM.i >> 4);
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_32_BIT)
              SEM.nAction = (unsigned char)VS.RD[SEM.iRI++];
              SEM.nNxt = (unsigned char)(VS.RD[SEM.iRI++];
              SEM.iRI += 2;
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_2 && SEM_RDHW_WIDTH_48_BIT)
              SEM.iRI += 2;
              SEM.nNxt = (unsigned char)VS.RD[SEM.iRI];
              SEM.iRI += 2;
              SEM.nSignal = (unsigned char)VS.RD[SEM.iRI++];
              SEM.nAction = (unsigned char)VS.RD[SEM.iRI++];
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_16_BIT)
              SEM.i = VS.RD[SEM.iRI++];
              SEM.nNxt = (unsigned char)((SEM.i >> 8) & 0x0F);
              SEM.nAction = (unsigned char)(SEM.i >> 12);
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_3 && SEM_RDHW_WIDTH_32_BIT)
              SEM.nNxt = (unsigned char)(VS.RD[SEM.iRI++] >> 12);
              SEM.i = VS.RD[SEM.iRI++];
              SEM.nAction = (unsigned char)(SEM.i & 0x0F);
              SEM.nSignal = (unsigned char)((SEM.i >> 4) & 0x0F);
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_32_BIT)
              SEM.i = VS.RD[SEM.iRI];
              SEM.iRI += 2;
              SEM.nNxt = (unsigned char)(SEM.i & 0x0FF);
              SEM.nAction = (unsigned char)(SEM.i >> 8);
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_2 && SEM_RDHW_WIDTH_48_BIT)
              SEM.iRI++;
              SEM.nNxt = (unsigned char)(VS.RD[SEM.iRI++] >> 8);
              SEM.i = VS.RD[SEM.iRI++];
              SEM.nAction = (unsigned char)(SEM.i & 0x0FF);
              SEM.nSignal = (unsigned char)(SEM.i >> 8);
#endif
#if (SEM_RD_WIDTH_32_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_32_BIT)
              SEM.i = VS.RD[SEM.iRI++];
              SEM.nNxt = (unsigned char)((SEM.i >> 16) & 0X0FF);
              SEM.nAction = (unsigned char)(SEM.i >> 24);
#endif
#if (SEM_RD_WIDTH_32_BIT && SEM_RDHW_TYPE_3 && SEM_RDHW_WIDTH_64_BIT)
              SEM.nNxt = (unsigned char)(VS.RD[SEM.iRI++] >> 24);
              SEM.i = VS.RD[SEM.iRI++];
              SEM.nAction = (unsigned char)(SEM.i & 0x0FF);
              SEM.nSignal = (unsigned char)((SEM.i >> 8) & 0x0FF);
#endif
              goto ded_match_found;
            }
            else
              SEM.iRI = VS.RIS[SEM.ruleq[SEM.iFirstR++]];
          }
#else
          SEM.iRI = VS.RI[SEM.iFirstR++];
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_16_BIT)
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nNxt = (unsigned char)(SEM.i & 0x0F);
          SEM.nAction = (unsigned char)(SEM.i >> 4);
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nPos = (unsigned char)(SEM.i & 0x0F);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)(SEM.i >> 4);
#endif
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_2 && SEM_RDHW_WIDTH_24_BIT)
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nPos = (unsigned char)(SEM.i & 0x0F);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)(SEM.i >> 4);
#endif
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nGuard = (unsigned char)(SEM.i & 0x0F);
          SEM.nNxt = (unsigned char)(SEM.i >> 4);
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nAction = (unsigned char)(SEM.i & 0x0F);
          SEM.nSignal = (unsigned char)(SEM.i >> 4);
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_32_BIT)
          SEM.nAction = (unsigned char)VS.RD[SEM.iRI++];
          SEM.nNxt = (unsigned char)VS.RD[SEM.iRI++];
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)VS.RD[SEM.iRI++];
#else
          SEM.iRI++;
#endif
          SEM.nPos = (unsigned char)VS.RD[SEM.iRI++];
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_2 && SEM_RDHW_WIDTH_48_BIT)
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)VS.RD[SEM.iRI++];
#else
          SEM.iRI++;
#endif
          SEM.nPos = (unsigned char)VS.RD[SEM.iRI++];
          SEM.nNxt = (unsigned char)VS.RD[SEM.iRI++];
          SEM.nGuard = (unsigned char)VS.RD[SEM.iRI++];
          SEM.nSignal = (unsigned char)VS.RD[SEM.iRI++];
          SEM.nAction = (unsigned char)VS.RD[SEM.iRI++];
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_16_BIT)
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nPos = (unsigned char)(SEM.i & 0x0F);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)((SEM.i >> 4) & 0x0F);
#endif
          SEM.nNxt = (unsigned char)((SEM.i >> 8) & 0x0F);
          SEM.nAction = (unsigned char)((SEM.i >> 12) & 0x0F);
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_3 && SEM_RDHW_WIDTH_32_BIT)
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nPos = (unsigned char)(SEM.i & 0x0F);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)((SEM.i >> 4) & 0x0F);
#endif
          SEM.nGuard = (unsigned char)((SEM.i >> 8) & 0x0F);
          SEM.nNxt = (unsigned char)(SEM.i >> 12);
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nAction = (unsigned char)(SEM.i & 0x0F);
          SEM.nSignal = (unsigned char)((SEM.i >> 4) & 0x0F);
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_32_BIT)
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nNxt = (unsigned char)(SEM.i & 0x0FF);
          SEM.nAction = (unsigned char)(SEM.i >> 8);
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nPos = (unsigned char)(SEM.i & 0x0FF);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)(SEM.i >> 8);
#endif
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_2 && SEM_RDHW_WIDTH_48_BIT)
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nPos = (unsigned char)(SEM.i & 0x0FF);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)(SEM.i >> 8);
#endif
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nGuard = (unsigned char)(SEM.i & 0x0FF);
          SEM.nNxt = (unsigned char)(SEM.i >> 8);
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nAction = (unsigned char)(SEM.i & 0x0FF);
          SEM.nSignal = (unsigned char)(SEM.i >> 8);
#endif
#if (SEM_RD_WIDTH_32_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_32_BIT)
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nPos = (unsigned char)(SEM.i & 0X0FF);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)((SEM.i >> 8) & 0X0FF);
#endif
          SEM.nNxt = (unsigned char)((SEM.i >> 16) & 0x0FF);
          SEM.nAction = (unsigned char)((SEM.i >> 24) & 0x0FF);
#endif
#if (SEM_RD_WIDTH_32_BIT && SEM_RDHW_TYPE_3 && SEM_RDHW_WIDTH_64_BIT)
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nPos = (unsigned char)(SEM.i & 0x0FF);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          SEM.nNeg = (unsigned char)((SEM.i >> 8) & 0x0FF);
#endif
          SEM.nGuard = (unsigned char)((SEM.i >> 16) & 0x0FF);
          SEM.nNxt = (unsigned char)(SEM.i >> 24);
          SEM.i = VS.RD[SEM.iRI++];
          SEM.nAction = (unsigned char)(SEM.i & 0x0FF);
          SEM.nSignal = (unsigned char)((SEM.i >> 8) & 0x0FF);
#endif
#if (VS_NOF_STATE_MACHINES != 0)
          for (SEM.nNo = 0 ; SEM.nNo < SEM.nPos; SEM.nNo++)
          {
            SEM.sa = (SEM_STATE_TYPE) VS.RD[SEM.iRI++];
            if (SEM.sa != SEM.CSV[VS.SMI[SEM.sa]])
              goto NextRule;
          }
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
          for (SEM.nNo = 0; SEM.nNo < SEM.nNeg; SEM.nNo++)
          {
            SEM.i = VS.RD[SEM.iRI++];
            SEM.sa = SEM.CSV[VS.SMI[SEM.i]];
            if ((SEM.sa == STATE_UNDEFINED) || (SEM.sa == (SEM_STATE_TYPE) SEM.i))
              goto NextRule;
          }
#endif
#endif
#if ((SEM_RDHW_TYPE_2 || SEM_RDHW_TYPE_3) && (SEM_GUARD_EXPRESSION))
          if (SEM.nGuard)
          {
            for (SEM.nNo = 0; SEM.nNo < SEM.nGuard; SEM.nNo++)
            {
#if (SEM_FUNCEXPHANDLING == 0)
              if ((*VSGuard[VS.RD[SEM.iRI++]])() == VS_FALSE)              
#else
                if (VSGuard(VS.RD[SEM.iRI++]) == VS_FALSE)              
#endif
                goto NextRule;
            }
          }
#endif

#if (VS_REALLINKMODE == 1)
          {
          SEM_RULE_TABLE_INDEX_TYPE ri = SEM.iFirstR - 1;
          VS_WAIT_ELM(VS_ENABLE_TRANS, Ind_TRANS, &ri);
          }
#endif

#if (SEM_SPEED)
ded_match_found:
#endif
#if (VS_NOF_STATE_MACHINES != 0)
          for (SEM.nNo = 0; SEM.nNo < SEM.nNxt; SEM.nNo++)
          {
            SEM.sa = (SEM_STATE_TYPE) VS.RD[SEM.iRI++];
            SEM.i = VS.SMI[SEM.sa];
            if (SEM.WSV[SEM.i] == STATE_UNDEFINED)
              SEM.WSV[SEM.i] = SEM.sa;
            else if (SEM.WSV[SEM.i] != SEM.sa)
              return (SES_CONTRADICTION);
          }
#endif
#if (SEM_SIGNAL)
          if (SEM.nSignal)
          {
            for (SEM.nNo = 0; SEM.nNo < SEM.nSignal; SEM.nNo++)
            {
              SEM.i = VS.RD[SEM.iRI++];
#if (SEM_SIGNAL_QUEUE_ERROR_IF_FULL)
              if (SEM_SignalQueuePut ((SEM_EVENT_TYPE)SEM.i) == SES_SIGNAL_QUEUE_FULL)
                return (SES_SIGNAL_QUEUE_FULL);
#endif
#if (SEM_SIGNAL_QUEUE_NO_ERROR_IF_FULL)
              SEM_SignalQueuePut ((SEM_EVENT_TYPE)SEM.i);
#endif
            }
          }
#endif
          if (SEM.nAction)
          {
            *ActionNo = (SEM_ACTION_EXPRESSION_TYPE)VS.RD[SEM.iRI];
            if (SEM.nAction > 1)
            {
              SEM.iRI++;
              SEM.nAction--;
              SEM.State = STATE_SEM_OUTPUT;
            }
#if (VS_REALLINKMODE == 1)
            VS_WAIT_ELM(VS_ENABLE_ACTION, Ind_ACTION, ActionNo);
#endif
            return (SES_FOUND);
          }
NextRule :
          ;
        }
#if (SEM_SPEED)
        if (SEM.Mode == MODE_SEM_EVENT_ONLY)
        {
          SEM.iFirstR = VS.RTIS[SEM.EventNo];
          SEM.iLastR = VS.RTIS[SEM.EventNo + 1];
          if ((SEM.iLastR - SEM.iFirstR) > ((SE_MAXQUEUE + 1) / 2))
          {
            SEM.iLastR = 0;
            SEM.fm_i = VS.sti[SEM.EventNo];
            SEM.lm_i = VS.sti[SEM.EventNo + 1];
            while (SEM.fm_i < SEM.lm_i)
            {
              SEM.sa = SEM.CSV[VS.smn[SEM.fm_i]];
              SEM.fs_i = VS.sri[SEM.sa];
              SEM.ls_i = VS.sri[SEM.sa + 1];
              while (SEM.fs_i < SEM.ls_i)
              {
                if (VS.sit[SEM.fs_i] == SEM.EventNo)
                {
                  SEM.qe = VS.srt[SEM.fs_i];
                  if (VS.cmc[SEM.qe] > 1)
                  {
                    for (SEM.iFirstR = 0; SEM.iFirstR < SEM.iLastR; SEM.iFirstR++)
                    {
                      if (SEM.ruleq[SEM.iFirstR] == SEM.qe)
                        break;
                    }
                    if (SEM.iFirstR == SEM.iLastR)
                      SEM.ruleq [SEM.iLastR++] = SEM.qe;
                  }
                  else
                    SEM.ruleq [SEM.iLastR++] = SEM.qe;
                }
                SEM.fs_i++;
              }
              SEM.fm_i++;
            }
            SEM.iFirstR = 0;
            SEM.Mode = MODE_SEM_STATE;
          }
          else
            SEM.Mode = MODE_SEM_EVENT;
          break;
        }
        else
#endif
#if (VS_NOF_EVENT_GROUPS != 0)
        {
          SEM.State = STATE_SEM_PREPARE;
          break;
        }
#else
        {
#if (SEM_SIGNAL)
          goto Signal;
#else
          SEM.State = STATE_SEM_OKAY;
          return (SES_OKAY);
#endif
        }
#endif
      case STATE_SEM_OUTPUT :
        if (SEM.nAction)
        {
          *ActionNo = (SEM_ACTION_EXPRESSION_TYPE) VS.RD[SEM.iRI++];
          SEM.nAction--;
#if (VS_REALLINKMODE == 1)
            VS_WAIT_ELM(VS_ENABLE_ACTION, Ind_ACTION, ActionNo);
#endif
          return (SES_FOUND);
        }
        SEM.State = STATE_SEM_CONSULT;
        break;
      case STATE_SEM_OKAY :
        return (SES_OKAY);
      default :
        return (SES_EMPTY);
    }
  }
}


#if (SEM_GET_OUTPUT_ALL == 1)
#if (SEM_SIGNAL)
#error SEM_GetOutputAll cannot be used when the Project contains signals.
#endif
unsigned char SEM_GetOutputAll (SEM_ACTION_EXPRESSION_TYPE *ActionVector,
  SEM_ACTION_EXPRESSION_TYPE MaxSize)
{
  SEM_ACTION_EXPRESSION_TYPE i;
  unsigned char CC;

  for (i = 0; ((CC = SEM_GetOutput(&ActionVector[i])) == SES_FOUND) && i < MaxSize - 1; i++);

  if (CC == SES_OKAY)
  {
    ActionVector[i] = ACTION_EXPRESSION_TERMINATION_ID;
    return (SES_OKAY);
  }
  if (CC == SES_FOUND)
    return (SES_BUFFER_OVERFLOW);
  return (CC);
}
#endif


#if (SEM_NEXT_STATE_CHG == 1)
unsigned char SEM_NextStateChg (void)
#else
unsigned char SEM_NextState (void)
#endif
{
  unsigned char CC;
  SEM_ACTION_EXPRESSION_TYPE nAction;

  if (SEM.State != STATE_SEM_OKAY)
  {
    while ((CC = SEM_GetOutput (&nAction)) == SES_FOUND);
    if (CC != SES_OKAY)
      return(CC);
  }
#if (VS_NOF_STATE_MACHINES != 0)
  for (SEM.i = 0; SEM.i < VS_NOF_STATE_MACHINES; SEM.i++)
  {
    if (SEM.WSV[SEM.i] != STATE_UNDEFINED)
    {
#if (SEM_NEXT_STATE_CHG == 1)
      if (SEM.CSV[SEM.i] != SEM.WSV[SEM.i])
        SEM.Chg = 1;
#endif
      SEM.CSV[SEM.i] = SEM.WSV[SEM.i];
      SEM.WSV[SEM.i] = STATE_UNDEFINED;
    }
  }
#endif
  SEM.State = STATE_SEM_INITIALIZE;

#if (SEM_NEXT_STATE_CHG == 1)
  if (SEM.Chg)
  {
    SEM.Chg = 0;
    return (SES_FOUND);
  }
  return (SES_OKAY);
#else
  return (SES_OKAY);
#endif
}


#if (SEM_INQUIRY == 1)
unsigned char SEM_Inquiry(void)
{
  SEM.InqAct = VS_FALSE;
  if (SEM.State != STATE_SEM_INITIALIZE)
    return (SES_ACTIVE);
  return (SES_OKAY);
}


unsigned char SEM_GetInput (SEM_EVENT_TYPE *EventNo,
  SEM_EVENT_TYPE *EventList)
{
  if (!SEM.InqAct)
    SEM.InqEventNo = 0;
  for(;;)
  {
    SEM.InqAct = VS_FALSE;
    if (SEM.InqEventNo >= VS_NOF_EVENTS)
      break;
    if (EventList != NULL)
    {
      SEM.InqFoundEventNo = EventList[SEM.InqEventNo];
      if (SEM.InqFoundEventNo == EVENT_TERMINATION_ID)
        break;
      if (VS_NOF_EVENTS <= SEM.InqFoundEventNo)
        return (SES_RANGE_ERR);
    }
    else
      SEM.InqFoundEventNo = SEM.InqEventNo;
    SEM.InqEventNo++;
    SEM.InqCurrentEventNo = SEM.InqFoundEventNo;
    for (SEM.IIt = 2; SEM.IIt; SEM.IIt--)
    {
#if (VS_NOF_EVENT_GROUPS > 0)
#if (SEM_EVENT_GROUP_TABLE_INDEX)
      if (SEM.IIt == 2)
      {
        SEM.iInqFirstEgi = VS.EGTI[SEM.InqCurrentEventNo];
        SEM.iInqLastEgi = VS.EGTI[SEM.InqCurrentEventNo + 1];
      }
#endif
#endif
      if (SEM.IIt == 1)
      {
#if (VS_NOF_EVENT_GROUPS > 0)
#if (SEM_EVENT_GROUP_INDEX)
        SEM.InqCurrentEventNo = VS.EGI[SEM.InqCurrentEventNo];
        if (SEM.InqCurrentEventNo == EVENT_GROUP_UNDEFINED)
          break;
        SEM.InqCurrentEventNo += VS_NOF_EVENTS;
#endif
#if (SEM_EVENT_GROUP_TABLE_INDEX)
        if (SEM.iInqFirstEgi == SEM.iInqLastEgi)
          break;
        SEM.InqCurrentEventNo = VS.EGT[SEM.iInqFirstEgi++];
        SEM.InqCurrentEventNo += VS_NOF_EVENTS;
        SEM.IIt++;
#endif
#else
        break;
#endif
      }
      SEM.iInqFirstR = VS.RTI[SEM.InqCurrentEventNo];
      SEM.iInqLastR = VS.RTI[SEM.InqCurrentEventNo + 1];
#if (SEM_SPEED)
      if (SEM.iInqFirstR >= SEM.iInqLastR)
      {
        SEM.iInqFirstR = VS.RTIS[SEM.InqCurrentEventNo];
        SEM.iInqLastR = VS.RTIS[SEM.InqCurrentEventNo+1];
      }
      else
      {
        SEM.InqAct = VS_TRUE;
        *EventNo = SEM.InqFoundEventNo;
        return (SES_FOUND);
      }
#endif
      while (SEM.iInqFirstR < SEM.iInqLastR)
      {
#if (SEM_SPEED)
        SEM.iInqRI = VS.RIS[SEM.iInqFirstR++];
#else
        SEM.iInqRI = VS.RI[SEM.iInqFirstR++];
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_16_BIT)
       	SEM.iInqRI++;
 		    SEM.j = VS.RD[SEM.iInqRI++];
        SEM.nInqPos = (unsigned char)(SEM.j & 0x0F);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)(SEM.j >> 4);
#endif
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_2 && SEM_RDHW_WIDTH_24_BIT)
 		    SEM.j = VS.RD[SEM.iInqRI];
        SEM.iInqRI += 3;
        SEM.nInqPos = (unsigned char)(SEM.j & 0x0F);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)(SEM.j >> 4);
#endif
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_32_BIT)
        SEM.iInqRI += 2;
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)VS.RD[SEM.iInqRI++];
#else
        SEM.iInqRI++;
#endif
        SEM.nInqPos = (unsigned char)VS.RD[SEM.iInqRI++];
#endif
#if (SEM_RD_WIDTH_8_BIT && SEM_RDHW_TYPE_2 && SEM_RDHW_WIDTH_48_BIT)
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)VS.RD[SEM.iInqRI++];
#else
        SEM.iInqRI++;
#endif
        SEM.nInqPos = (unsigned char)VS.RD[SEM.iInqRI];
        SEM.iInqRI += 5;
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_16_BIT)
 		    SEM.j = VS.RD[SEM.iInqRI++];
        SEM.nInqPos = (unsigned char)(SEM.j & 0x0F);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)((SEM.j >> 4) & 0x0F);
#endif
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_3 && SEM_RDHW_WIDTH_32_BIT)
        SEM.j = VS.RD[SEM.iInqRI];
        SEM.iInqRI += 2;
        SEM.nInqPos = (unsigned char)(SEM.j & 0x0F);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)((SEM.j >> 4) & 0x0F);
#endif
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_32_BIT)
        SEM.iInqRI++;
        SEM.j = VS.RD[SEM.iInqRI++];
        SEM.nInqPos = (unsigned char)(SEM.j & 0x0FF);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)(SEM.j >> 8);
#endif
#endif
#if (SEM_RD_WIDTH_16_BIT && SEM_RDHW_TYPE_2 && SEM_RDHW_WIDTH_48_BIT)
 		    SEM.j = VS.RD[SEM.iInqRI];
        SEM.iInqRI += 3;
        SEM.nInqPos = (unsigned char)(SEM.j & 0x0FF);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)(SEM.j >> 8);
#endif
#endif
#if (SEM_RD_WIDTH_32_BIT && SEM_RDHW_TYPE_1 && SEM_RDHW_WIDTH_32_BIT)
 		    SEM.j = VS.RD[SEM.iInqRI++];
        SEM.nInqPos = (unsigned char)(SEM.j & 0X0FF);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)((SEM.j >> 8) & 0X0FF);
#endif
#endif
#if (SEM_RD_WIDTH_32_BIT && SEM_RDHW_TYPE_3 && SEM_RDHW_WIDTH_64_BIT)
        SEM.j = VS.RD[SEM.iInqRI++];
        SEM.nInqPos = (unsigned char)(SEM.j & 0x0FF);
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        SEM.nInqNeg = (unsigned char)((SEM.j >> 8) & 0x0FF);
#endif
        SEM.iInqRI++;
#endif
#if (VS_NOF_STATE_MACHINES != 0)
        for (SEM.nInqNo = 0; SEM.nInqNo < SEM.nInqPos; SEM.nInqNo++)
        {
          SEM.sb = (SEM_STATE_TYPE) VS.RD[SEM.iInqRI++];
          if (SEM.sb != SEM.CSV[VS.SMI[SEM.sb]])
            goto NextRuleInq;
        }
#if (SEM_RMN_NEGATIVE_STATE_SYNCS)
        for (SEM.nInqNo = 0; SEM.nInqNo < SEM.nInqNeg; SEM.nInqNo++)
        {
          SEM.j = VS.RD[SEM.iInqRI++];
          SEM.sb = SEM.CSV[VS.SMI[SEM.j]];
          if ((SEM.sb == STATE_UNDEFINED) || (SEM.sb == (SEM_STATE_TYPE) SEM.j))
            goto NextRuleInq;
        }
#endif
#endif
        SEM.InqAct = VS_TRUE;
        *EventNo = SEM.InqFoundEventNo;
        return (SES_FOUND);
NextRuleInq:
        ;
      }
    }
  }
  return (SES_OKAY);
}


#if (SEM_GET_INPUT_ALL == 1)
unsigned char SEM_GetInputAll (SEM_EVENT_TYPE *EventVector,
  SEM_EVENT_TYPE *EventList, SEM_EVENT_TYPE MaxSize)
{
  SEM_EVENT_TYPE  i;
  unsigned char CC;

  for (i = 0; ((CC = SEM_GetInput (&EventVector[i], EventList)) == SES_FOUND)
        && i < MaxSize - 1; i++);
  if (CC == SES_OKAY)
  {
    EventVector[i] = EVENT_TERMINATION_ID;
    return (SES_OKAY);
  }
  if (CC == SES_FOUND)
    return (SES_BUFFER_OVERFLOW);
  return (CC);
}
#endif
#endif


#if (SEM_NAME == 1)
unsigned char SEM_Name (unsigned char IdentType, 
  SEM_EXPLANATION_TYPE IdentNo, char *Text, unsigned short MaxSize)
{
  char  c, *s;
  unsigned short i;

  if (!MaxSize)
    return (SES_TEXT_TOO_LONG);
  switch (IdentType)
  {
#if (VS_EVENT_NAMES != 0)
    case EVENT_TYPE :
      if (VS_NOF_EVENTS <= IdentNo)
        return (SES_RANGE_ERR);
      s = (char *)VS.ENames + VS.ENI[IdentNo];
      break;
#endif
#if ((VS_STATE_NAMES != 0) && (VS_NOF_STATES != 0))
    case STATE_TYPE :
      if (VS_NOF_STATES <= IdentNo)
        return (SES_RANGE_ERR);
      s = (char *)VS.SNames + VS.SNI[IdentNo];
      break;
#endif
#if ((VS_ACTION_FUNCTION_NAMES != 0) && (VS_NOF_ACTION_FUNCTIONS != 0))
    case ACTION_TYPE :
      if (VS_NOF_ACTION_FUNCTIONS <= IdentNo)
        return (SES_RANGE_ERR);
      s = (char *)VS.ANames + VS.ANI[IdentNo];
      break;
#endif
    default :
      return (SES_TYPE_ERR);
  }
  for (i = 0; i < MaxSize; i++)
  {
    c = *s++;
    *Text++ = c;
    if (c == '\0')
      return (SES_OKAY);
  }
  Text--;
  *Text = '\0';
  return (SES_TEXT_TOO_LONG);
}
#endif


#if (SEM_NAME_ABS == 1)
unsigned char SEM_NameAbs (unsigned char IdentType, 
  SEM_EXPLANATION_TYPE IdentNo, char VS_TQ_RULEBASE **Text)
{
  switch (IdentType)
  {
#if (VS_EVENT_NAMES != 0)
    case EVENT_TYPE :
      if (VS_NOF_EVENTS <= IdentNo)
        return (SES_RANGE_ERR);
      *Text = (char const *)&VS.ENames + VS.ENI[IdentNo];
      break;
#endif
#if ((VS_STATE_NAMES != 0) && (VS_NOF_STATES != 0))
    case STATE_TYPE :
      if (VS_NOF_STATES <= IdentNo)
        return (SES_RANGE_ERR);
      *Text = (char const *)&VS.SNames + VS.SNI[IdentNo];
      break;
#endif
#if ((VS_ACTION_FUNCTION_NAMES != 0) && (VS_NOF_ACTION_FUNCTIONS != 0))
    case ACTION_TYPE :
      if (VS_NOF_ACTION_FUNCTIONS <= IdentNo)
        return (SES_RANGE_ERR);
      *Text = (char const *)&VS.ANames + VS.ANI[IdentNo];
      break;
#endif
    default :
      return (SES_TYPE_ERR);
  }
  return (SES_OKAY);
}
#endif


#if (SEM_EXPL == 1)
unsigned char SEM_Expl (unsigned char IdentType, SEM_EXPLANATION_TYPE IdentNo,
  char *Text, unsigned short MaxSize)
{
  char  c, *s;
  unsigned short i;

  if (!MaxSize)
    return (SES_TEXT_TOO_LONG);
  switch (IdentType)
  {
#if (VS_EVENT_EXPLS != 0)
    case EVENT_TYPE :
      if (VS_NOF_EVENTS <= IdentNo)
        return (SES_RANGE_ERR);
      s = (char *)VS.EExpls + VS.EEI[IdentNo];
      break;
#endif
#if ((VS_STATE_EXPLS != 0) && (VS_NOF_STATES != 0))
    case STATE_TYPE :
      if (VS_NOF_STATES <= IdentNo)
        return (SES_RANGE_ERR);
      s = (char *)VS.SExpls + VS.SEI[IdentNo];
      break;
#endif
#if ((VS_ACTION_EXPLS != 0) && (VS_NOF_ACTION_FUNCTIONS != 0))
    case ACTION_TYPE :
      if (VS_NOF_ACTION_FUNCTIONS <= IdentNo)
        return (SES_RANGE_ERR);
      s = (char *)VS.AExpls + VS.AEI[IdentNo];
      break;
#endif
    default :
      return (SES_TYPE_ERR);
  }

  for (i = 0; i < MaxSize; i++)
  {
    c = *s++;
    *Text++ = c;
    if (c == '\0')
      return (SES_OKAY);
  }
  Text--;
  *Text = '\0';
  return (SES_TEXT_TOO_LONG);
}
#endif


#if (SEM_EXPL_ABS == 1)
unsigned char SEM_ExplAbs (unsigned char IdentType,
  SEM_EXPLANATION_TYPE IdentNo, char VS_TQ_RULEBASE **Text)
{
  switch (IdentType)
  {
#if (VS_EVENT_EXPLS != 0)
    case EVENT_TYPE :
      if (VS_NOF_EVENTS <= IdentNo)
        return (SES_RANGE_ERR);
      *Text = (char const *)&VS.EExpls + VS.EEI[IdentNo];
      break;
#endif
#if ((VS_STATE_EXPLS != 0) && (VS_NOF_STATES != 0))
    case STATE_TYPE :
      if (VS_NOF_STATES <= IdentNo)
        return (SES_RANGE_ERR);
      *Text = (char const *)&VS.SExpls + VS.SEI[IdentNo];
      break;
#endif
#if ((VS_ACTION_EXPLS != 0) && (VS_NOF_ACTION_FUNCTIONS != 0))
    case ACTION_TYPE :
      if (VS_NOF_ACTION_FUNCTIONS <= IdentNo)
        return (SES_RANGE_ERR);
      *Text = (char const *)&VS.AExpls + VS.AEI[IdentNo];
      break;
#endif
    default :
      return (SES_TYPE_ERR);
  }
  return (SES_OKAY);
}
#endif


#if (SEM_STATE == 1)
unsigned char SEM_State (SEM_STATE_MACHINE_TYPE StateMachineNo,
  SEM_STATE_TYPE *StateNo)
{
  if (VS_NOF_STATE_MACHINES <= StateMachineNo)
    return (SES_RANGE_ERR);
#if (VS_NOF_STATE_MACHINES != 0)
  *StateNo = SEM.CSV[StateMachineNo];
#else
  *StateNo = 0;
#endif
  return (SES_FOUND);
}
#endif


#if (SEM_STATE_ALL == 1)
unsigned char SEM_StateAll (SEM_STATE_TYPE *StateVector,
  SEM_STATE_MACHINE_TYPE MaxSize)
{
  SEM_STATE_MACHINE_TYPE i;

  if (VS_NOF_STATE_MACHINES > MaxSize)
    return (SES_BUFFER_OVERFLOW);

#if (VS_NOF_STATE_MACHINES != 0)
  for (i = 0; i < VS_NOF_STATE_MACHINES; i++)
    StateVector[i] = SEM.CSV[i];
#else
  for (i = 0; i < VS_NOF_STATE_MACHINES; i++)
    StateVector[i] = 0;
#endif
  return (SES_FOUND);
}
#endif


#if (SEM_MACHINE == 1)
unsigned char SEM_Machine (SEM_STATE_TYPE StateNo,
  SEM_STATE_MACHINE_TYPE *StateMachineNo)
{
  if (VS_NOF_STATES <= StateNo)
    return (SES_RANGE_ERR);
#if (VS_NOF_STATE_MACHINES != 0)
  *StateMachineNo = VS.SMI[StateNo];
#else
  *StateMachineNo = 0;
#endif
  return (SES_FOUND);
}
#endif


#if (SEM_FORCE_STATE == 1)
unsigned char SEM_ForceState (SEM_STATE_TYPE StateNo)
{
  if (VS_NOF_STATES <= StateNo)
    return (SES_RANGE_ERR);
#if (VS_NOF_STATE_MACHINES != 0)
  SEM.CSV[VS.SMI[StateNo]] = StateNo;
#endif
  return (SES_OKAY);
}
#endif
