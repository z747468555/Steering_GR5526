/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_soft_led_agc.c
 *
 * @brief   gh3x2x led agc process functions
 *
 * @version see gh3x2x_drv_version.h
 *
 */
 
#include <stdio.h>
#include "gh3x2x_drv_common.h"
#include "gh3x2x_drv_soft_led_agc.h"
#include "gh3x2x_drv_dump.h"
#include "gh3x2x_drv_soft_led_agc_cfg.h"

#define GH3X2X_250MS_TIMER_FOR_2S     (0x8)
#define GH3X2X_250MS_TIMER_FOR_10S    (GH3X2X_250MS_TIMER_FOR_2S * 5)

#define SUB_CHN_PRO_EN     1

// soft agc parameter
static STSoftAgcCfg g_stSoftAgcCfg;

STGh3x2xNewAgcMainChnlInfo  g_pstGh3x2xNewAgcMainChnlInfoEntity[GH3X2X_NEW_AGC_SLOT_NUM_LIMIT];
STGh3x2xNewAgcMainChnlInfo * const g_pstGh3x2xNewAgcMainChnlInfo = g_pstGh3x2xNewAgcMainChnlInfoEntity;
const GU8 g_uchNewAgcSlotNumLimit = GH3X2X_NEW_AGC_SLOT_NUM_LIMIT;
STGh3x2xNewAgcMainChnlSat g_pstGh3x2xNewAgcMainChnlSat;
STGh3x2xNewAgcMainChnlMeanInfo g_pstGh3x2xNewAgcMainChnlMeanInfoEntity[GH3X2X_NEW_AGC_SLOT_NUM_LIMIT];
STGh3x2xNewAgcMainChnlMeanInfo * const g_pstGh3x2xNewAgcMainChnlMeanInfo = g_pstGh3x2xNewAgcMainChnlMeanInfoEntity;

STGh3x2xNewAgcMainChnlIdaelAdjInfo g_pstGh3x2xNewAgcMainChnlIdaelAdjInfoEntity[GH3X2X_NEW_AGC_SLOT_NUM_LIMIT];
STGh3x2xNewAgcMainChnlIdaelAdjInfo * const g_pstGh3x2xNewAgcMainChnlIdaelAdjInfo = g_pstGh3x2xNewAgcMainChnlIdaelAdjInfoEntity;

const GU32 g_unNewAgcSpo2Trig_H_Thd = __GH3X2X_AGC_SPO2_TRIG_H_THD__;
const GU32 g_unNewAgcSpo2Trig_L_Thd = __GH3X2X_AGC_SPO2_TRIG_L_THD__;
const GU32 g_unNewAgcSpo2Ideal_value = __GH3X2X_AGC_SPO2_IDEAL_VALUE__;
const GU32 g_unNewAgcGeneTrig_H_Thd = __GH3X2X_AGC_GENERAL_TRIG_H_THD__;
const GU32 g_unNewAgcGeneTrig_L_Thd = __GH3X2X_AGC_GENERAL_TRIG_L_THD__;
const GU32 g_unNewAgcGeneIdeal_value = __GH3X2X_AGC_GENERAL_IDEAL_VALUE__;
//sub chnl
#if GH3X2X_NEW_AGC_SUB_CHNL_NUM_LIMIT > 0
STGh3x2xNewAgcSubChnlSlotInfo  g_pstGh3x2xNewAgcSubChnlSlotInfoEntity[GH3X2X_NEW_AGC_SLOT_NUM_LIMIT];
STGh3x2xNewAgcSubChnlRxInfo    g_pstGh3x2xNewAgcSubChnlRxInfoEntity[GH3X2X_NEW_AGC_SUB_CHNL_NUM_LIMIT];
STGh3x2xNewAgcSubChnlSlotInfo * const g_pstGh3x2xNewAgcSubChnlSlotInfo = g_pstGh3x2xNewAgcSubChnlSlotInfoEntity;
STGh3x2xNewAgcSubChnlRxInfo  * const g_pstGh3x2xNewAgcSubChnlRxInfo = g_pstGh3x2xNewAgcSubChnlRxInfoEntity;
#else
STGh3x2xNewAgcSubChnlSlotInfo  * const g_pstGh3x2xNewAgcSubChnlSlotInfo = 0;
STGh3x2xNewAgcSubChnlRxInfo  * const g_pstGh3x2xNewAgcSubChnlRxInfo = 0;
#endif
const GU8 g_uchNewAgcSubChnlNumLimit = GH3X2X_NEW_AGC_SUB_CHNL_NUM_LIMIT;

extern const GU16 g_usGH3x2xSlotRegBase[8];


#define GH3X2X_AGC_REG_AGC_EN                     0
#define GH3X2X_AGC_REG_LED_CURRENT_UP_LIMIT       1
#define GH3X2X_AGC_REG_LED_CURRENT_DOWN_LIMIT     2
#define GH3X2X_AGC_REG_LED_CURRENT_DRV0           3
#define GH3X2X_AGC_REG_LED_CURRENT_DRV1           4
#define GH3X2X_AGC_REG_TIA_GAIN_RX0               5
#define GH3X2X_AGC_REG_TIA_GAIN_RX1               6
#define GH3X2X_AGC_REG_TIA_GAIN_RX2               7
#define GH3X2X_AGC_REG_TIA_GAIN_RX3               8
#define GH3X2X_AGC_REG_AGC_SRC                    9
#define GH3X2X_AGC_REG_BG_CANCEL_RX0              10
#define GH3X2X_AGC_REG_BG_CANCEL_RX1              11
#define GH3X2X_AGC_REG_BG_CANCEL_RX2              12
#define GH3X2X_AGC_REG_BG_CANCEL_RX3              13
#define GH3X2X_AGC_REG_SR_MULIPLIPER              14
#define GH3X2X_AGC_ECG_PPG_RX_EN                  15



//sub channel
#define GH3X2X_NEW_AGC_SUB_CHNNL_DISCAED_TIME_SEC 1
#define GH3X2X_NEW_AGC_SUB_CHNNL_ANAYLSIS_TIME_SEC 1



extern const STGh3x2xAgcReg g_pstGH3x2xAgcReg[];

extern const GU16 g_usGH3x2xTiaGainR[];

GU8 g_Gh3x2xNewAgcInitFlag = 0;

void GH3X2X_StoreRXxTiaGainAfterAgc(GU8 uchGain, GU8 uchSlotNo, GU8 uchAdcNo, GU8* puchGainRecord)
{
    puchGainRecord[uchSlotNo*4 + uchAdcNo] = uchGain;
}




void GH3x2xNewAgcModuleReset(void)
{
    g_Gh3x2xNewAgcInitFlag = 0;
    GH3X2X_Memset((GU8*)g_pstGh3x2xNewAgcMainChnlInfo,0,(GU16)g_uchNewAgcSlotNumLimit*sizeof(STGh3x2xNewAgcMainChnlInfo));
    GH3X2X_Memset( (GU8*)(&g_pstGh3x2xNewAgcMainChnlSat),0,sizeof(STGh3x2xNewAgcMainChnlSat)); // init  variable
    GH3X2X_Memset((GU8*)g_pstGh3x2xNewAgcMainChnlMeanInfo,0,(GU16)g_uchNewAgcSlotNumLimit*sizeof(STGh3x2xNewAgcMainChnlMeanInfo)); 
    GH3X2X_Memset((GU8*)g_pstGh3x2xNewAgcMainChnlIdaelAdjInfo,0,(GU16)g_uchNewAgcSlotNumLimit*sizeof(STGh3x2xNewAgcMainChnlIdaelAdjInfo));                           

    Gh3x2xNewAgcSubChnlModuleReset();
    
}
void GH3X2X_NewAgcRecoveryAgcRegToMainChnl(GU8 uchSlotIdx, STGh3x2xNewAgcMainChnlInfo *pstAgcMainChnlInfo)
{
    GH3x2xSetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_UP_LIMIT, uchSlotIdx, pstAgcMainChnlInfo ->uchBakRegLedCurrentMax);
    GH3x2xSetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DOWN_LIMIT, uchSlotIdx, pstAgcMainChnlInfo ->uchBakRegLedCurrentMin);
    GH3x2xSetAgcReg(GH3X2X_AGC_REG_AGC_SRC, uchSlotIdx, pstAgcMainChnlInfo ->uchBakRegAgcSrc);
    GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] recover agc src:agc_src = %d, SlotNo = %d\r\n",pstAgcMainChnlInfo ->uchBakRegAgcSrc,uchSlotIdx);            
}

GU8 GH3X2X_NewAgcFindSubChnlSlotInfo(GU8 uchSlot)
{
    for(GU8 uchSlotCnt = 0; uchSlotCnt < g_uchNewAgcSlotNumLimit; uchSlotCnt ++)
    {
        STGh3x2xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xNewAgcMainChnlInfo[uchSlotCnt];
        if(pstAgcMainChnlInfo ->uchEnable)
        {
            if(uchSlot == (pstAgcMainChnlInfo ->uchSlotAdc >> 2))
            {
                return uchSlotCnt;
            }
        }
    }
    return 0;
}

GU16 GH3x2xCalTrigerUpLimitForBiggerGain(GU8 uchCurrentGain ,GU8 uchSlotNo)
{
    GU32 unTempRawdata;
    GU32 unTempTrigThd_H = g_stSoftAgcCfg.unAgcTrigThd_H;

    GU8 uchSlotCnt = GH3X2X_NewAgcFindSubChnlSlotInfo(uchSlotNo);
    STGh3x2xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xNewAgcMainChnlInfo[uchSlotCnt];
    if(pstAgcMainChnlInfo->uchSpo2ChnlIndex)
    {
        unTempTrigThd_H = g_unNewAgcSpo2Trig_H_Thd;
    }else
    {
        unTempTrigThd_H = g_unNewAgcGeneTrig_H_Thd;
    }
        
    if(12 <= uchCurrentGain)
    {
        return 0xFFFF;
    }
    else
    {
        unTempRawdata = unTempTrigThd_H - 8388608;
        unTempRawdata = 8388608 + (GU32)(((float)unTempRawdata)/(float)1.3*(float)g_usGH3x2xTiaGainR[uchCurrentGain]/(float)g_usGH3x2xTiaGainR[uchCurrentGain + 1]);
        return unTempRawdata>>8;
    }
}


#if SUB_CHN_PRO_EN
__weak void GH3x2xNewAgcSubChnlClearAnalysisCntAndSetGain(GU8 uchSlotNo, GU8 uchRxEn ,GU8 uchNextGain)
{
    for(GU8 uchRxCnt = 0; uchRxCnt < 4; uchRxCnt ++)
    {
        if(uchRxEn&(1 << uchRxCnt))
        {
            for(GU8 uchChnlCnt = 0; uchChnlCnt < g_uchNewAgcSubChnlNumLimit; uchChnlCnt ++)
            {
                STGh3x2xNewAgcSubChnlRxInfo *pstSTGh3x2xNewAgcSubChnlRxInfo = &(g_pstGh3x2xNewAgcSubChnlRxInfo[uchChnlCnt]);
                if(pstSTGh3x2xNewAgcSubChnlRxInfo->uchEn)
                {
                    if(((uchSlotNo << 2) + uchRxCnt) == pstSTGh3x2xNewAgcSubChnlRxInfo ->uchSlotAdcNo)
                    {
                        pstSTGh3x2xNewAgcSubChnlRxInfo ->usAnalysisCnt = 0;
                        if(0xFF != uchNextGain)
                        {
                            GH3x2xSetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + uchRxCnt, uchSlotNo, 0);
                            pstSTGh3x2xNewAgcSubChnlRxInfo ->uchCurrentGain = uchNextGain;
                            pstSTGh3x2xNewAgcSubChnlRxInfo ->usTrigeUpLimitForBiggerGain = GH3x2xCalTrigerUpLimitForBiggerGain(uchNextGain,pstSTGh3x2xNewAgcSubChnlRxInfo->uchSlotAdcNo >> 2);
                        }
                    }    
                }
            }
        }
    }
}
#endif



__weak void GH3x2xNewAgcSetSubChnlSlotInfo(GU8 uchSlotCnt, GU16 usSampleRate)
{
    STGh3x2xNewAgcSubChnlSlotInfo *pstAgcSubChnlSlotInfo = &g_pstGh3x2xNewAgcSubChnlSlotInfo[uchSlotCnt];
    pstAgcSubChnlSlotInfo ->usAnalysisDiscardLen = usSampleRate * GH3X2X_NEW_AGC_SUB_CHNNL_DISCAED_TIME_SEC;
    pstAgcSubChnlSlotInfo ->usAnalysisEndLen = pstAgcSubChnlSlotInfo ->usAnalysisDiscardLen + usSampleRate * GH3X2X_NEW_AGC_SUB_CHNNL_ANAYLSIS_TIME_SEC;
}

GU8 GH3x2xGetDrvCurrent(GU8 uchSlotIdx ,GU8 uchAgcEn)
{
    GU8 uchTempDrv;
    if(2 == uchAgcEn)
    {    
        uchTempDrv = GH3x2xGetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DRV1, uchSlotIdx);
    }
    else
    {
        uchTempDrv = GH3x2xGetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DRV0, uchSlotIdx);
    }
    return uchTempDrv;
}


void GH3x2xNewAgcChnlInfoInit(void)
{
    if(0 == g_uchNewAgcSlotNumLimit)
    {
        return;
    }
    if(0 == g_Gh3x2xNewAgcInitFlag)
    {
        GU8 uchNewAgcSlotCnt = 0;
        for(GU8 uchSlotCnt = 0; uchSlotCnt < 8; uchSlotCnt ++)
        {
            if((g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn & (1 << uchSlotCnt)))
            {
                if(uchNewAgcSlotCnt < g_uchNewAgcSlotNumLimit)
                {
                    g_pstGh3x2xNewAgcMainChnlInfo[uchNewAgcSlotCnt].uchEnable = 1;
                    g_pstGh3x2xNewAgcMainChnlInfo[uchNewAgcSlotCnt].uchSlotAdc = (uchSlotCnt << 2); 
                                    
                    /******* slot function Distinguish   START******************************************************/          
                    if((g_stSoftAgcCfg.uchSpo2RedSlotCfgEn & (1 << uchSlotCnt))||(g_stSoftAgcCfg.uchSpo2IrSlotCfgEn & (1 << uchSlotCnt)))
                    {
                        g_pstGh3x2xNewAgcMainChnlInfo[uchNewAgcSlotCnt].uchSpo2ChnlIndex =  1;
                        GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] uchNewAgcSpo2SlotCnt = %d\r\n",uchNewAgcSlotCnt);   
                    }
                    /******* slot function Distinguish   END******************************************************/
                    GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] uchNewAgcSlotCnt = %d\r\n",uchNewAgcSlotCnt);             
                    uchNewAgcSlotCnt ++;
                }
                else
                {
                    GH3X2X_DEBUG_LOG_PARAM("[NewAgc] error !!! main chnl info is no enough! \r\n");    
                    while(1);
                }
            }
        }
        g_Gh3x2xNewAgcInitFlag = 1;
    }

    for(GU8 uchSlotCnt = 0; uchSlotCnt < g_uchNewAgcSlotNumLimit; uchSlotCnt ++)
    {    
        STGh3x2xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xNewAgcMainChnlInfo[uchSlotCnt];
            
        if(pstAgcMainChnlInfo ->uchEnable)
        {
            GU8 uchSlotIdx;
            uchSlotIdx = pstAgcMainChnlInfo ->uchSlotAdc >> 2;
            if(0 == pstAgcMainChnlInfo ->uchHaveBakRegFlag)
            {
                GU8 uchBgCancel;
                GU8 uchGainLimit;

                GU16 usSampleRate;
                GU16 usEcgPpgRxEn;

                //bak reg
                pstAgcMainChnlInfo ->uchBakRegAgcEn = GH3x2xGetAgcReg(GH3X2X_AGC_REG_AGC_EN, uchSlotIdx);
                pstAgcMainChnlInfo ->uchBakRegAgcSrc = GH3x2xGetAgcReg(GH3X2X_AGC_REG_AGC_SRC, uchSlotIdx);
                pstAgcMainChnlInfo ->uchSlotAdc = (uchSlotIdx << 2) + pstAgcMainChnlInfo ->uchBakRegAgcSrc;
                pstAgcMainChnlInfo ->uchBakRegLedCurrentMax = GH3x2xGetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_UP_LIMIT, uchSlotIdx);                
                pstAgcMainChnlInfo ->uchBakRegLedCurrentMin = GH3x2xGetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DOWN_LIMIT, uchSlotIdx);    
                pstAgcMainChnlInfo ->uchBakTiaGain = GH3x2xGetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + pstAgcMainChnlInfo ->uchBakRegAgcSrc, uchSlotIdx);
                pstAgcMainChnlInfo ->uchCurrentTiaGain = pstAgcMainChnlInfo ->uchBakTiaGain;
                pstAgcMainChnlInfo ->uchCurrentDrv =  GH3x2xGetDrvCurrent(uchSlotIdx, pstAgcMainChnlInfo->uchBakRegAgcEn);    
                uchBgCancel = GH3x2xGetAgcReg(GH3X2X_AGC_REG_BG_CANCEL_RX0 + pstAgcMainChnlInfo ->uchBakRegAgcSrc, uchSlotIdx);
                GH3X2X_StoreRXxTiaGainAfterAgc(pstAgcMainChnlInfo ->uchBakTiaGain, uchSlotIdx, pstAgcMainChnlInfo ->uchBakRegAgcSrc, g_puchTiaGainAfterSoftAgc);
                if(0 == uchBgCancel)
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg32uA;
                }
                else if(1 == uchBgCancel)
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg64uA;
                }
                else if(2 == uchBgCancel)
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg128uA;
                }
                else
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg256uA;
                }
                GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] uchGainLimit = %d, uchSlotIdx = %d\r\n",uchGainLimit, uchSlotIdx);    
                pstAgcMainChnlInfo ->uchTiaGainUpLimit = uchGainLimit;
                usSampleRate = 1000/(GH3x2xGetAgcReg(GH3X2X_AGC_REG_SR_MULIPLIPER, uchSlotIdx) + 1);
                usEcgPpgRxEn = GH3x2xGetAgcReg(GH3X2X_AGC_ECG_PPG_RX_EN, uchSlotIdx);
                GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] usEcgPpgRxEn = 0x%X\r\n",usEcgPpgRxEn);        
                if(usEcgPpgRxEn&0x0001)  // ecg is enable
                {
                    usEcgPpgRxEn = ((usEcgPpgRxEn>>1) & (~0x0008));  //clear RX3 enable bit
                }
                else
                {
                    usEcgPpgRxEn = (usEcgPpgRxEn>>1);
                }
                usEcgPpgRxEn &= (~(1 << pstAgcMainChnlInfo ->uchBakRegAgcSrc));   //clear main channel RX enable bit
                
                g_pstGh3x2xNewAgcMainChnlSat.Sat_Check_Num_Max[uchSlotIdx] = 5*usSampleRate; //init
                g_pstGh3x2xNewAgcMainChnlMeanInfo[uchSlotIdx].unSampleRate = usSampleRate;  //init

                //sub channel slot
#if SUB_CHN_PRO_EN
                //pstAgcSubChnlSlotInfo ->usAnalysisDiscardLen = usSampleRate * GH3X2X_NEW_AGC_SUB_CHNNL_DISCAED_TIME_SEC;
                //pstAgcSubChnlSlotInfo ->usAnalysisEndLen = pstAgcSubChnlSlotInfo ->usAnalysisDiscardLen + usSampleRate * GH3X2X_NEW_AGC_SUB_CHNNL_ANAYLSIS_TIME_SEC;
                GH3x2xNewAgcSetSubChnlSlotInfo(uchSlotCnt, usSampleRate);
                GH3x2xNewAgcSetNewSubChnl(uchSlotIdx,usEcgPpgRxEn);
#endif
                pstAgcMainChnlInfo ->uchHaveBakRegFlag = 1;
                                
                GH3x2xSetAgcReg(GH3X2X_AGC_REG_AGC_EN, uchSlotIdx, 0);   // disable hardware agc for all slot 
            }
            else
            {
              pstAgcMainChnlInfo ->uchCurrentTiaGain = GH3x2xGetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + pstAgcMainChnlInfo ->uchBakRegAgcSrc, uchSlotIdx);
              pstAgcMainChnlInfo ->uchCurrentDrv =  GH3x2xGetDrvCurrent(uchSlotIdx, pstAgcMainChnlInfo->uchBakRegAgcEn);
              ////get initial DRV and current in case them change before StartSampling()
              GH3X2X_NewAgcRecoveryAgcRegToMainChnl(uchSlotIdx,pstAgcMainChnlInfo);
            }
        }
    }
}


GU32 GH32x2xMedSel(GU32 unData0, GU32 unData1, GU32 unData2)
{
    if((unData0 >= unData1)&&(unData0 <= unData2))
    {
        return unData0;
    }
    if((unData0 >= unData2)&&(unData0 <= unData1))
    {
        return unData0;
    }
    if((unData1 >= unData0)&&(unData1 <= unData2))
    {
        return unData1;
    }
    if((unData1 >= unData2)&&(unData1 <= unData0))
    {
        return unData1;
    }
    return unData2;
}
float GH3X2X_CalcfRelativeVpp(GU32 unRawdata, GU8 uchDrv, GU8 uchGain)
{
    return ((float)unRawdata/(float)uchDrv/(float)g_usGH3x2xTiaGainR[uchGain]);
}

/**
 * @fn  void GH3X2X_LedAgcReset(void)
 *
 * @brief   reset agc parameter
 *
 * @attention    None
 *
 * @param[in]    None
 * @param[out]   None
 *
 * @return  None
 */
__weak void GH3X2X_LedAgcReset(void)
{
    GH3X2X_Memset(&g_stSoftAgcCfg, 0, sizeof(STSoftAgcCfg));
    GH3x2xNewAgcModuleReset();
}

/**
 * @fn     void GH3X2X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi)
 *
 * @brief  write AGC parameter
 *
 * @attention   None
 *
 * @param[in]   usRegVal         reg value
 * @param[in]   uchRegPosi         reg posi
 * @param[out]  None
 *
 * @return  None
 */
__weak void GH3X2X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi)
{

    GU16 usByteOffset;
    GU8 uchByteVal;
    GU8 uchByteCnt;
    for(uchByteCnt = 0; uchByteCnt < 2; uchByteCnt ++)
    {
        uchByteVal = ((usRegVal >> (8*uchByteCnt))&0x00FF);
        usByteOffset = (usRegPosi * 2) + uchByteCnt;
        if(usByteOffset < sizeof(g_stSoftAgcCfg))
        {
            GH3X2X_Memcpy(((GU8 *)(&g_stSoftAgcCfg)) + usByteOffset, (GU8*)(&uchByteVal), 1);
        }
        //uchSlotxSoftAgcAdjEn  and uchSlotxSoftBgAdjEn is loaded

        if((1 == uchByteCnt)&&(usRegPosi ==((GH3X2X_AGC_RESTRAIN_THD_L_MSB_16_ADDR - GH3X2X_AGC_BG_CANCEL_ADJUST_SLOT_EN_ADDR) / GH3X2X_REG_ADDR_SIZE)))
        {
            GH3X2X_DEBUG_LOG_PARAM_AGC("uchSlotxSoftAgcAdjEn = 0x%X, uchSlotxSoftBgAdjEn = 0x%X\r\n",(int)g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn,(int)g_stSoftAgcCfg.uchSlotxSoftBgAdjEn);
            GH3X2X_DEBUG_LOG_PARAM_AGC("uchAmbSampleEn = 0x%X, uchAmbSlot = 0x%X\r\n",(int)g_stSoftAgcCfg.stAmbSlotCtrl.uchAmbSampleEn,(int)g_stSoftAgcCfg.stAmbSlotCtrl.uchAmbSlot);
            GH3X2X_DEBUG_LOG_PARAM_AGC("stSoftAgcGainLimit = 0x%X,0x%X,0x%X,0x%X\r\n",g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg32uA,g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg64uA,g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg128uA,g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg256uA);
            GH3X2X_DEBUG_LOG_PARAM_AGC("unAgcTrigThd_H = %d,unAgcTrigThd_L = %d\r\n",(int)g_stSoftAgcCfg.unAgcTrigThd_H,(int)g_stSoftAgcCfg.unAgcTrigThd_L);
            GH3X2X_DEBUG_LOG_PARAM_AGC("unAgcRestrainThd_H = %d,unAgcRestrainThd_L = %d\r\n",(int)g_stSoftAgcCfg.unAgcRestrainThd_H,(int)g_stSoftAgcCfg.unAgcRestrainThd_L);
        }
        
    }
}


void GH3X2X_StoreTiaGainAfterAgc(GU16 usGainRegValue, GU8 uchSlotNo, GU8* puchGainRecord)
{
    for(GU8 uchAdcCnt = 0; uchAdcCnt < 4; uchAdcCnt ++)
    {
        puchGainRecord[uchSlotNo*4 + uchAdcCnt] = (usGainRegValue>>(uchAdcCnt*4))&0x000F;
        GH3X2X_DEBUG_LOG_PARAM_AGC("StoreTiaGainAfterAgc slot no = %d, adc no = %d, usGainRegValue = %d\r\n", uchSlotNo,uchAdcCnt,usGainRegValue);
    }
}

/**
 * @fn     static void GH3X2X_LedAgcInit()
 * 
 * @brief  init led agc
 *
 * @attention   just use to show function support
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
__weak void GH3X2X_LedAgcInit(void)
{
    GH3x2xNewAgcChnlInfoInit();
}


__weak void Gh3x2xNewAgcSubChnlModuleReset(void)
{
    #if SUB_CHN_PRO_EN
    GH3X2X_Memset((GU8*)g_pstGh3x2xNewAgcSubChnlSlotInfo,0,(GU16)g_uchNewAgcSlotNumLimit*sizeof(STGh3x2xNewAgcSubChnlSlotInfo));
    GH3X2X_Memset((GU8*)g_pstGh3x2xNewAgcSubChnlRxInfo,0,(GU16)g_uchNewAgcSubChnlNumLimit*sizeof(STGh3x2xNewAgcSubChnlRxInfo));
    #endif
}
__weak GU8 GH3x2xNewAgcSetNewSubChnl(GU8 uchSlotNo, GU8 uchRxEn)
{
    GU8 uchSetSuccess = 1;
    for(GU8 uchRxCnt = 0; uchRxCnt < 4; uchRxCnt ++)
    {
        if(uchRxEn&(1 << uchRxCnt))
        {
            uchSetSuccess = 0;
            for(GU8 uchChnlCnt = 0; uchChnlCnt < g_uchNewAgcSubChnlNumLimit; uchChnlCnt ++)
            {
                STGh3x2xNewAgcSubChnlRxInfo *pstSTGh3x2xNewAgcSubChnlRxInfo = &(g_pstGh3x2xNewAgcSubChnlRxInfo[uchChnlCnt]);
                if(0 ==  pstSTGh3x2xNewAgcSubChnlRxInfo->uchEn)
                {
                    pstSTGh3x2xNewAgcSubChnlRxInfo->uchEn = 1;
                    pstSTGh3x2xNewAgcSubChnlRxInfo->uchSlotAdcNo = (uchSlotNo << 2) + uchRxCnt;
                    //read current gain
                    pstSTGh3x2xNewAgcSubChnlRxInfo ->uchCurrentGain = GH3x2xGetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + uchRxCnt, uchSlotNo);                    
                    GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] Sub chnl current Gain = %d, Slot = %d, Rx = %d\r\n",pstSTGh3x2xNewAgcSubChnlRxInfo ->uchCurrentGain,uchSlotNo,uchRxCnt);         
                    pstSTGh3x2xNewAgcSubChnlRxInfo ->usTrigeUpLimitForBiggerGain = GH3x2xCalTrigerUpLimitForBiggerGain(pstSTGh3x2xNewAgcSubChnlRxInfo ->uchCurrentGain, uchSlotNo);    
                    GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] usTrigeUpLimitForBiggerGain = %d, Slot = %d, Rx = %d\r\n",((GU32)pstSTGh3x2xNewAgcSubChnlRxInfo ->usTrigeUpLimitForBiggerGain)<<8,uchSlotNo,uchRxCnt);         
                    GH3X2X_StoreRXxTiaGainAfterAgc(pstSTGh3x2xNewAgcSubChnlRxInfo ->uchCurrentGain, uchSlotNo, uchRxCnt, g_puchTiaGainAfterSoftAgc);
                    uchSetSuccess = 1;
                    break;
                }
            }
            if(0 == uchSetSuccess)
            {
                GH3X2X_DEBUG_LOG_PARAM("[NewAgc] Set new sub chnl fail !!! Slot = %d, Rx = %d\r\n",uchSlotNo,uchRxCnt);
                while(1);
            }
        }
    }
    return uchSetSuccess;
}

#if SUB_CHN_PRO_EN
__weak void GH3X2X_NewAgcSubChnlGainAdj(GU8* puchReadFifoBuffer, GU16 usFifoLen)
{
    for(GU8 uchChnlCnt = 0; uchChnlCnt < g_uchNewAgcSubChnlNumLimit; uchChnlCnt ++)
    {    
        GU16 usAnalysisDiscardLen;
        GU16 usAnalysisEndLen;
        GU8  uchSlotCnt;
        GU8  uchSlotIdx;
        GU8  uchAdcNo;
        GU32 unTempTrigThd_L = g_stSoftAgcCfg.unAgcTrigThd_L;
        GU32 unTempTrigThd_H = g_stSoftAgcCfg.unAgcTrigThd_H;
        STGh3x2xNewAgcSubChnlRxInfo *pstSTGh3x2xNewAgcSubChnlRxInfo = &(g_pstGh3x2xNewAgcSubChnlRxInfo[uchChnlCnt]);
        uchSlotIdx = pstSTGh3x2xNewAgcSubChnlRxInfo ->uchSlotAdcNo >> 2;
        uchAdcNo = pstSTGh3x2xNewAgcSubChnlRxInfo ->uchSlotAdcNo&0x03;
        uchSlotCnt = GH3X2X_NewAgcFindSubChnlSlotInfo(uchSlotIdx);
        STGh3x2xNewAgcSubChnlSlotInfo *pstGh3x2xNewAgcSubChnlSlotInfo = &(g_pstGh3x2xNewAgcSubChnlSlotInfo[uchSlotCnt]);
        GU8 uchTiaGainLimit = g_pstGh3x2xNewAgcMainChnlInfo[uchSlotCnt].uchTiaGainUpLimit;
        usAnalysisDiscardLen = pstGh3x2xNewAgcSubChnlSlotInfo ->usAnalysisDiscardLen;
        usAnalysisEndLen = pstGh3x2xNewAgcSubChnlSlotInfo ->usAnalysisEndLen;
            
        if(g_pstGh3x2xNewAgcMainChnlInfo[uchSlotCnt].uchSpo2ChnlIndex)
        {
           unTempTrigThd_L = g_unNewAgcSpo2Trig_L_Thd;
           unTempTrigThd_H = g_unNewAgcSpo2Trig_H_Thd; 
        }else
        {
           unTempTrigThd_L = g_unNewAgcGeneTrig_L_Thd;
           unTempTrigThd_H = g_unNewAgcGeneTrig_H_Thd;  
        }
                    
        if(pstSTGh3x2xNewAgcSubChnlRxInfo ->uchEn)
        {
           // prevent unRawdataSum(GU32) overflow
           if( (usAnalysisEndLen - usAnalysisDiscardLen) > 200 )  
           {
              usAnalysisDiscardLen = usAnalysisEndLen - 200;   // maximum number of accumulated rawdata is 200
           }
   
            for (GU16 usIndex = 0; usIndex < usFifoLen; usIndex += GH3X2X_FIFO_RAWDATA_SIZE) 
            {
                GU32 unFifoVal;
                GU8  uchSlotAdcNo;
                GU32 unDataRaw; 
                unFifoVal = GH3X2X_MAKEUP_DWORD(puchReadFifoBuffer[usIndex], puchReadFifoBuffer[usIndex + 1], 
                                                puchReadFifoBuffer[usIndex + 2], puchReadFifoBuffer[usIndex + 3]);
                uchSlotAdcNo = ((unFifoVal >> 27)&0x0000001F);
                unDataRaw = GH3X2X_RAWDATA_CLEAR_ALL_FLAG(unFifoVal);
                if(uchSlotAdcNo == pstSTGh3x2xNewAgcSubChnlRxInfo ->uchSlotAdcNo)
                {
                    if(usAnalysisDiscardLen == pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt) //init
                    {
                        pstSTGh3x2xNewAgcSubChnlRxInfo ->unRawdataSum = 0;
                        pstSTGh3x2xNewAgcSubChnlRxInfo ->unLastLastRawdata = unDataRaw;
                        pstSTGh3x2xNewAgcSubChnlRxInfo ->unLastRawdata = unDataRaw;
                        pstSTGh3x2xNewAgcSubChnlRxInfo ->usMoreThanUpLimitPointCnt = 0;
                        pstSTGh3x2xNewAgcSubChnlRxInfo ->usMoreThanUpLimitPointCntForBiggerGain = 0;
                    }
                    if(pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt < usAnalysisEndLen)  //analysising
                    {                
                        GU32 unTempRawdata = GH32x2xMedSel(pstSTGh3x2xNewAgcSubChnlRxInfo ->unLastLastRawdata, pstSTGh3x2xNewAgcSubChnlRxInfo ->unLastRawdata, unDataRaw);
                        if(usAnalysisDiscardLen == pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt)
                        {
                            GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] sub chnl unTempRawdata  = %d, Slot = %d, rx = %d\r\n",unTempRawdata, uchSlotIdx, uchAdcNo);                                
                        }
                        pstSTGh3x2xNewAgcSubChnlRxInfo ->unRawdataSum += unTempRawdata;
                        if(unTempRawdata > unTempTrigThd_H)
                        {
                            pstSTGh3x2xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCnt ++;
                        }
                        if((unTempRawdata >> 8) > pstSTGh3x2xNewAgcSubChnlRxInfo->usTrigeUpLimitForBiggerGain)
                        {
                            pstSTGh3x2xNewAgcSubChnlRxInfo->usMoreThanUpLimitPointCntForBiggerGain ++;
                        }
                        pstSTGh3x2xNewAgcSubChnlRxInfo ->unLastLastRawdata = pstSTGh3x2xNewAgcSubChnlRxInfo ->unLastRawdata;
                        pstSTGh3x2xNewAgcSubChnlRxInfo ->unLastRawdata = unDataRaw;
                        pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt ++;                    
                    }
                    else //anlysis finished
                    {
                        GU32 unAvaRawdata;
                        GU16 unRealAnaylsisNum = usAnalysisEndLen - usAnalysisDiscardLen;
                        GU8  uchNextGain = 0xFF;
                        GU8  uchCurrentGain = pstSTGh3x2xNewAgcSubChnlRxInfo ->uchCurrentGain;
                        unAvaRawdata = pstSTGh3x2xNewAgcSubChnlRxInfo ->unRawdataSum/unRealAnaylsisNum;
                        GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] sub chnl rawdata ava  = %d,DownLimit = %d, Slot = %d, rx = %d\r\n",unAvaRawdata, unTempTrigThd_L,uchSlotIdx, uchAdcNo);                                
                        GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] usMoreThanUpLimitPointCnt  = %d,UpLimit = %d, Slot = %d, rx = %d\r\n",pstSTGh3x2xNewAgcSubChnlRxInfo ->usMoreThanUpLimitPointCnt, unTempTrigThd_H,uchSlotIdx, uchAdcNo);                        
                        GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] usMoreThanUpLimitPointCntForBiggerGain = %d,UpLimit = %d, Slot = %d, rx = %d\r\n",pstSTGh3x2xNewAgcSubChnlRxInfo ->usMoreThanUpLimitPointCntForBiggerGain, ((GU32)pstSTGh3x2xNewAgcSubChnlRxInfo->usTrigeUpLimitForBiggerGain)<<8, uchSlotIdx, uchAdcNo);            
                        if(pstSTGh3x2xNewAgcSubChnlRxInfo ->usMoreThanUpLimitPointCnt > (unRealAnaylsisNum >> 3))  // 1/8 points is bigger than trige up limit
                        {
                            if(uchCurrentGain > 0)
                            {
                                uchNextGain = uchCurrentGain - 1;
                            }
                        }
                        if((pstSTGh3x2xNewAgcSubChnlRxInfo ->usMoreThanUpLimitPointCntForBiggerGain < (unRealAnaylsisNum >> 3))&&(unAvaRawdata < unTempTrigThd_L))
                        {
                            if(uchCurrentGain < uchTiaGainLimit)
                            {
                                uchNextGain = uchCurrentGain + 1;
                            }
                        }
                        if(0xFF != uchNextGain)
                        {                            
                            GH3X2X_DEBUG_LOG_PARAM_AGC("[NewAgc] sub chnl gain change: NewGain = %d, OldGain = %d, Slot = %d, rx = %d\r\n",uchNextGain,uchCurrentGain, uchSlotIdx, uchAdcNo);            
                            GH3x2xSetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + uchAdcNo, uchSlotIdx, uchNextGain);
                            pstSTGh3x2xNewAgcSubChnlRxInfo ->uchCurrentGain = uchNextGain;
                            pstSTGh3x2xNewAgcSubChnlRxInfo ->usTrigeUpLimitForBiggerGain = GH3x2xCalTrigerUpLimitForBiggerGain(uchNextGain, uchSlotIdx);                                    
                            GH3X2X_StoreRXxTiaGainAfterAgc(uchNextGain, uchSlotIdx, uchAdcNo, g_puchTiaGainAfterSoftAgc);
                        }
                        pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt = 0;    
                    }
                }
            }
        }
    }
}
#endif

/**
 * @fn     GU8 GH3X2X_GetLedAgcState(void)
 * 
 * @brief  get agc state
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  GH3X2X_LED_AGC_ENABLE or GH3X2X_LED_AGC_DISABLE
 */ 
__weak GU8 GH3X2X_GetLedAgcState(void)
{
    return GH3X2X_LED_AGC_ENABLE;
}


/*********************************************self-test function start*****************************************/
#define GH3X2X_SPO2_SELF_TEST 0


#if GH3X2X_SPO2_SELF_TEST
const GU32 g_uchSpo2AgcSelftestSlotAdc = 0x01;  //slot0, adc1
const GU32 g_usSpo2AgcFifoLen = 25*4;
const GU32 g_unSpo2AgcRawdata[] = 
{
    10111744,9818116,10074798,9832931,10038819,9812989,10067119,9791632,10220862,9843610,9888564,9974512,10041146,10117796,9858828,10260863,9900963,10159857,10259720,10054459,10247477,9957985,10078474,9916146,9821879
};

GU8 g_uchSpo2AgcFifoBuffer[4*100];

void GH3X2X_Spo2AgcFifoDataGen(void)
{
    for (GU16 usIndex = 0; usIndex < g_usSpo2AgcFifoLen; usIndex += GH3X2X_FIFO_RAWDATA_SIZE) 
    {
        if(usIndex >= sizeof(g_uchSpo2AgcFifoBuffer))
        {
            break;
        }
        g_uchSpo2AgcFifoBuffer[usIndex + 0] = (g_uchSpo2AgcSelftestSlotAdc<<3);
        g_uchSpo2AgcFifoBuffer[usIndex + 1] = (0x000000FF&(g_unSpo2AgcRawdata[usIndex/4] >> 16));
        g_uchSpo2AgcFifoBuffer[usIndex + 2] = (0x000000FF&(g_unSpo2AgcRawdata[usIndex/4] >> 8));
        g_uchSpo2AgcFifoBuffer[usIndex + 3] = (0x000000FF&(g_unSpo2AgcRawdata[usIndex/4] >> 0));
    }
}
#endif
/*********************************************self-test function end*****************************************/


/****************************************************************
* Description: calculate mean value of main channel  
* Input:  rawdata  
* Return: none
******************************************************************/
static void GH3X2X_NewAgcMainChnlMeanValueCal(GU8* puchReadFifoBuffer, GU16 usFifoLen, GU32 *punRawdataMean, GU8 *pAdjFlag)
{
    for(GU8 uchSlotCnt = 0; uchSlotCnt < g_uchNewAgcSlotNumLimit; uchSlotCnt ++)
    {
        STGh3x2xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xNewAgcMainChnlInfo[uchSlotCnt];
        GU8 uchMainChnlSlotAdcNo = pstAgcMainChnlInfo ->uchSlotAdc;
        
        if(pstAgcMainChnlInfo ->uchEnable)   // is  enabled  slot
        {        
            GU32 unTemp_Trig_H_Thd = 0;    
            GU32 unTemp_Trig_L_Thd = 0;    
            GU32 unTemp_Ideal_value = 0;
            GU16 usAnalysisEndLen = 0; 
            GU16 usAnalysisDiscardLenAdjust = 0;
            GU16 usAnalysisDiscardLenTrige = 0;
            GU32 TrigeMean  = 0; 

            if(0 != pstAgcMainChnlInfo->uchSpo2ChnlIndex) // is  spo2 slot
            {
                unTemp_Trig_H_Thd = g_unNewAgcSpo2Trig_H_Thd;
                unTemp_Trig_L_Thd = g_unNewAgcSpo2Trig_L_Thd;
                unTemp_Ideal_value= g_unNewAgcSpo2Ideal_value;
            }else
            {
                unTemp_Trig_H_Thd = g_unNewAgcGeneTrig_H_Thd;
                unTemp_Trig_L_Thd = g_unNewAgcGeneTrig_L_Thd;                            
                unTemp_Ideal_value= g_unNewAgcGeneIdeal_value;
            }
                      
            STGh3x2xNewAgcMainChnlMeanInfo *pstNewAgcMainChnlMeanInfo = &g_pstGh3x2xNewAgcMainChnlMeanInfo[uchSlotCnt];
            STGh3x2xNewAgcMainChnlIdaelAdjInfo *pstNewAgcMainChnlIdaelAdjInfo = &g_pstGh3x2xNewAgcMainChnlIdaelAdjInfo[uchSlotCnt];
            usAnalysisEndLen = pstNewAgcMainChnlMeanInfo->unSampleRate ;
            usAnalysisDiscardLenTrige =  3*usAnalysisEndLen/5;  // drop the first 3/5 data    
            //have agc motion at last fifo read period or first init                        
            if(pstNewAgcMainChnlMeanInfo->uchHaveAgcAtLastReadPrd)   
            {
                usAnalysisDiscardLenAdjust =  2*usAnalysisEndLen/5;  // drop the first 2/5 data
                //// After AGC, the previous data is drop to calcluate precise mean
                if(!pstNewAgcMainChnlMeanInfo->uchDropFlag)
                {  // The frame count is only cleared for the first time to prevent data num cannot to be SampleRate
                    pstNewAgcMainChnlMeanInfo->usAnalysisCnt = 0;  
                    pstNewAgcMainChnlMeanInfo->uchDropFlag = 1;
                }         
            }
            //The data fluctuates when initializing the early stage  
            if(!pstNewAgcMainChnlIdaelAdjInfo->uchIniteFirst)  
            {
                usAnalysisDiscardLenAdjust =  2*usAnalysisEndLen/5;  // drop the first 2/5 data
            }

           // prevent unSumAdjust(GU32) overflow
           if( (usAnalysisEndLen - usAnalysisDiscardLenAdjust) > 200 )
           {
              usAnalysisDiscardLenAdjust = usAnalysisEndLen - 200; // maximum number of accumulated rawdata is 200
           }
           
           if( (usAnalysisEndLen - usAnalysisDiscardLenTrige) > 200 )
           {
              usAnalysisDiscardLenTrige = usAnalysisEndLen - 200;
           }

            for (GU16 usIndex = 0; usIndex < usFifoLen; usIndex += GH3X2X_FIFO_RAWDATA_SIZE) 
            {
                GU32 unFifoVal;
                GU8  uchSlotAdcNo;
                GU32 unDataRaw; 
                unFifoVal = GH3X2X_MAKEUP_DWORD(puchReadFifoBuffer[usIndex], puchReadFifoBuffer[usIndex + 1], 
                                                puchReadFifoBuffer[usIndex + 2], puchReadFifoBuffer[usIndex + 3]);
                uchSlotAdcNo = ((unFifoVal >> 27)&0x0000001F);
                unDataRaw = GH3X2X_RAWDATA_CLEAR_ALL_FLAG(unFifoVal); 
                //GH3X2X_DEBUG_LOG_PARAM_AGC_SPO2("uchSlotAdcNo = %d\r\n",uchSlotAdcNo);
                //GH3X2X_DEBUG_LOG_PARAM_AGC_SPO2("uchMainChnlSlotAdcNo = %d\r\n",uchMainChnlSlotAdcNo);    
                if(uchSlotAdcNo == uchMainChnlSlotAdcNo)
                {
                    // begin Saturation judge   
                    {  
                        g_pstGh3x2xNewAgcMainChnlSat.Sat_CheckCnt[uchSlotCnt]++;
                        if( unDataRaw > unTemp_Trig_H_Thd)
                        {
                            g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt]++;
                        }
                        //// as soon as posible set the Saturation flag 
                        if( g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt] >=  (GU16)(g_pstGh3x2xNewAgcMainChnlSat.Sat_Check_Num_Max[uchSlotCnt]>>4)) // about 8% of all checked data
                        {
                            g_pstGh3x2xNewAgcMainChnlSat.Sat_flag |= (1<<uchSlotCnt);  
                            g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt] = 0; 
                        }     

                        if(g_pstGh3x2xNewAgcMainChnlSat.Sat_CheckCnt[uchSlotCnt] >= g_pstGh3x2xNewAgcMainChnlSat.Sat_Check_Num_Max[uchSlotCnt]) 
                        {                                      
                            g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt] = 0;
                            g_pstGh3x2xNewAgcMainChnlSat.Sat_CheckCnt[uchSlotCnt] = 0;  
                        }                                 
                    } 
                    // end Saturation judge   
                    
                    //// begin mean calculate    
                    //GH3X2X_SAMPLE_LOG_PARAM("slot = %d, uchSlotAdcNo = %d\r\n",uchSlotCnt,uchSlotAdcNo);
//                  if(uchSlotAdcNo == 7) 
//                  {
//                      GH3X2X_SAMPLE_LOG_PARAM("Mean cal: slot = %d, uchSlotAdcNo = %d, usAnalysisDiscardLenAdjust = %d\r\n",uchSlotCnt, uchSlotAdcNo,usAnalysisDiscardLenAdjust);
//                      GH3X2X_SAMPLE_LOG_PARAM("Mean cal: FifoVal = %d, Raw = %d, CalCnt = %d, AdjSum = %d\r\n", unFifoVal,unDataRaw, pstNewAgcMainChnlMeanInfo->usAnalysisCnt, pstNewAgcMainChnlMeanInfo->unSumAdjust);
//                  }    
                                     
                   if(usAnalysisDiscardLenAdjust == pstNewAgcMainChnlMeanInfo->usAnalysisCnt) //init
                    {
                        pstNewAgcMainChnlMeanInfo->unSumAdjust =  0;
                        pstNewAgcMainChnlMeanInfo->unMaxRawdata =  0;      
                        pstNewAgcMainChnlMeanInfo->unMinRawdata =  16777216;
                    }
                                        
                    if(usAnalysisDiscardLenTrige == pstNewAgcMainChnlMeanInfo->usAnalysisCnt) //init
                    {
                        pstNewAgcMainChnlMeanInfo->unSumTrige  =  0;
                    }    
                                        
                    if(pstNewAgcMainChnlMeanInfo->usAnalysisCnt  < usAnalysisEndLen)   //analysising
                    {
                    pstNewAgcMainChnlMeanInfo->unSumAdjust +=  unDataRaw;
                    pstNewAgcMainChnlMeanInfo->unSumTrige +=  unDataRaw;
                    pstNewAgcMainChnlMeanInfo->usAnalysisCnt++;   
                    pstNewAgcMainChnlMeanInfo->unMaxRawdata = ((unDataRaw > pstNewAgcMainChnlMeanInfo->unMaxRawdata)? unDataRaw: pstNewAgcMainChnlMeanInfo->unMaxRawdata ); // Max of  rawdata
                    pstNewAgcMainChnlMeanInfo->unMinRawdata = ((unDataRaw < pstNewAgcMainChnlMeanInfo->unMinRawdata)? unDataRaw: pstNewAgcMainChnlMeanInfo->unMinRawdata ); // Min of  rawdata
                    }
                    else //anlysis finished   calculate mean and judge whether AGC
                    {    
                        // mean value to calculate gain and current
                        if(usAnalysisEndLen > usAnalysisDiscardLenAdjust)
                        {
                            punRawdataMean[uchSlotCnt] = pstNewAgcMainChnlMeanInfo->unSumAdjust/(usAnalysisEndLen - usAnalysisDiscardLenAdjust);    
                            TrigeMean = pstNewAgcMainChnlMeanInfo->unSumTrige / (usAnalysisEndLen - usAnalysisDiscardLenTrige);
                        }else //  The normal case will not be executed, in case the division by 0 in abnormal case
                        {
                            punRawdataMean[uchSlotCnt] =   unDataRaw; 
                            TrigeMean =   unDataRaw; 
                        }
                        pstNewAgcMainChnlMeanInfo ->unRawdataMean = punRawdataMean[uchSlotCnt];
//                         TrigeMean = pstNewAgcMainChnlMeanInfo->unSumTrige / (usAnalysisEndLen - usAnalysisDiscardLenTrige);
                        pstNewAgcMainChnlMeanInfo->usAnalysisCnt=0;
                        // The mean calculation key variable is cleared    (Prevent mean calculation from being too small:Accumulation lost some data, but numble of raw is all)    
                        usAnalysisDiscardLenAdjust = 0; 
                        pstNewAgcMainChnlMeanInfo->uchHaveAgcAtLastReadPrd = 0; //  clear  after caculate mean!!!       
                        //// end mean calculate

                        ///////////////////// whether AGC  and adjust the mean value to calculate gain and current
                        if( (TrigeMean < unTemp_Trig_L_Thd) || (TrigeMean > unTemp_Trig_H_Thd) )
                        {
                            *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                        }
                        
                        if( g_pstGh3x2xNewAgcMainChnlSat.Sat_flag & (1<<uchSlotCnt)) //if the channel is saturation
                        {
                            GU32 unTempMean = 0;         //Pay attention to initialization of local variables
                            if( pstNewAgcMainChnlMeanInfo->unMaxRawdata > unTemp_Trig_H_Thd)
                            {
                                /////////// Pay attention to data truncation, use 64-bi
                                unTempMean =   (GS64)unTemp_Ideal_value*(GS64)(pstNewAgcMainChnlMeanInfo->unMaxRawdata)/15099494;  // to adjust the max data to (0.8*2^23 + 2^23)
                                *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                            }
                            
                            if( punRawdataMean[uchSlotCnt] > unTemp_Ideal_value)
                            {
                                // adjust the mean value to calculate gain and current (reduce the DC amount)
                                punRawdataMean[uchSlotCnt] = ( (punRawdataMean[uchSlotCnt]>unTempMean)? punRawdataMean[uchSlotCnt]:unTempMean );
                                *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                            } 
                            else if( pstNewAgcMainChnlMeanInfo->unMaxRawdata > unTemp_Trig_H_Thd)
                            {
                                punRawdataMean[uchSlotCnt] = unTempMean; //  Increasing the mean value to reduce the DC amount
                            } 
                        }
                        ////////////////////////////////////////////////////
                        //////////////begin ideal value adjust 
                        if( (!((*pAdjFlag) & (1<<uchSlotCnt))) && (!( g_pstGh3x2xNewAgcMainChnlSat.Sat_flag & (1<<uchSlotCnt))))//if the channel not adjust and satuartion
                        {
                            //// The deviation of the mean and the ideal target value is above 1/8  and   Data fluctuation is less than a certain value
                            if(((pstNewAgcMainChnlMeanInfo->unRawdataMean< ((unTemp_Ideal_value*7)>>3) ) || (pstNewAgcMainChnlMeanInfo->unRawdataMean > ((unTemp_Ideal_value*9)>>3))) \
                                &&( 10*pstNewAgcMainChnlMeanInfo->unMaxRawdata/pstNewAgcMainChnlMeanInfo->unMinRawdata < 12) ) 
                            {
                                if( !pstNewAgcMainChnlIdaelAdjInfo->uchIniteFirst ) //// Inite First time 
                                {
                                    pstNewAgcMainChnlIdaelAdjInfo->uchIniteFirst = 1;
                                    *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                                }
                                else
                                {
                                    pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt++;
                                    if(pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt >= 15)
                                    {
                                        *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                                        pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt  = 0; 
                                    }    
                                }
                                            
                            }
                            else
                            {
                                 pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt  = 0;
                            }                    
                        }
                        else
                        {
                             pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt  = 0;
                        }
                        pstNewAgcMainChnlIdaelAdjInfo->uchIniteFirst = 1;  //// Must be set to 1 after the first second!!!
                        //////////////end ideal value adjust 
//                        GH3X2X_SAMPLE_LOG_PARAM("Mean cal: Rawmean = %d, Adjmean = %d, Slot_num = %d\r\n",pstNewAgcMainChnlMeanInfo->unRawdataMean, punRawdataMean[uchSlotCnt], uchSlotCnt);
//                        GH3X2X_SAMPLE_LOG_PARAM("Sat flag: SatCnt = %d, SatCheck = %d, CheckMaxNum = %d, Satflag = %d\r\n", g_pstGh3x2xNewAgcMainChnlSat.Sat_cnt[uchSlotCnt], g_pstGh3x2xNewAgcMainChnlSat.Sat_CheckCnt[uchSlotCnt], g_pstGh3x2xNewAgcMainChnlSat.Sat_Check_Num_Max[uchSlotCnt], g_pstGh3x2xNewAgcMainChnlSat.Sat_flag);
//                        GH3X2X_SAMPLE_LOG_PARAM("Ideal adjust: SeconsCnt = %d\r\n", pstNewAgcMainChnlIdaelAdjInfo->uchSeconsCnt);
//                        GH3X2X_SAMPLE_LOG_PARAM("Adj flag: Trigmean = %d, L_Thd = %d, H_Thd = %d, Adjflag = %d\r\n", TrigeMean, unTemp_Trig_L_Thd, unTemp_Trig_H_Thd, *pAdjFlag);                                     
                        g_pstGh3x2xNewAgcMainChnlSat.Sat_flag &= (~(1<<uchSlotCnt)) ; ////clear the Sat flag  
												
                    }
                }  
             }
        }
        
    }
}


/**
 * @fn     GU8 GH3X2X_NewAgcCalDrvCurrentAndGain(GU32 CurRawdata, GU32 IdealRawdata, STGh3x2xNewAgcMainChnlInfo *pstAgcMainChnlInfo, GU8* puchIdealDrvCurrent, GU8* puchIdealGain)
 *
 * @brief  mark abnormal chnl
 *
 * @attention   
 *
 * @param[in]   CurRawdata, IdealRawdata, uchMainChnlInfoIndex
 * @param[out]  puchIdealDrvCurrent, puchIdealGain
 *
 * @return  0:DrvCurrent or gain will NOT change  1: DrvCurrent or gain will  change
 */
GU8 GH3X2X_NewAgcCalDrvCurrentAndGain_1(GU32 CurRawdata, GU8 uchMainChnlInfoIndex,  GU8* puchIdealDrvCurrent, GU8* puchIdealGain)
{
    STGh3x2xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xNewAgcMainChnlInfo[uchMainChnlInfoIndex];
    STGh3x2xNewAgcMainChnlMeanInfo *pstMainChnlExtralInfo = &g_pstGh3x2xNewAgcMainChnlMeanInfo[uchMainChnlInfoIndex];

    GU8 uchDefaultGain = pstAgcMainChnlInfo->uchBakTiaGain;
    GU8 uchGainUpLimit = pstAgcMainChnlInfo->uchTiaGainUpLimit;
    GU8 uchLedCurrentMin = pstAgcMainChnlInfo->uchBakRegLedCurrentMin;
    GU8 uchLedCurrentMax = pstAgcMainChnlInfo->uchBakRegLedCurrentMax;
    GU8 uchCurrentDrv = pstAgcMainChnlInfo->uchCurrentDrv;
    GU8 uchCurrentTiaGain = pstAgcMainChnlInfo->uchCurrentTiaGain;
 
    GU8 uchNewGain = uchDefaultGain;
    GU32 unIdealRawdata_no;
    GU32 unTempDrvCurrent;
    GU8 uchChangeFlag = 0;  //bit0: drv current flag  bit1: gain flag
    GU8 uchCalFinish = 0;
    GU32 unTrigUpLimit;
//  GU32 unTrigLowLimit;
    GU8  unCalNum = 12;
  
    
        if(0 != pstAgcMainChnlInfo->uchSpo2ChnlIndex) // is  spo2 slot
        {
            unTrigUpLimit = g_unNewAgcSpo2Trig_H_Thd;
//          unTrigLowLimit = g_unNewAgcSpo2Trig_L_Thd;
            unIdealRawdata_no= g_unNewAgcSpo2Ideal_value - 8388608;
        }else
        {
            unTrigUpLimit = g_unNewAgcGeneTrig_H_Thd;
//          unTrigLowLimit = g_unNewAgcGeneTrig_L_Thd;
            unIdealRawdata_no = g_unNewAgcGeneIdeal_value - 8388608;
        }
    
        
//    GH3X2X_SAMPLE_LOG_PARAM("Drv cal: UpLimit = %d, Mean = %d, LastMean = %d\r\n",unTrigUpLimit, pstMainChnlExtralInfo->unRawdataMean, pstMainChnlExtralInfo->unLastRawdataMean);            
    // reduce rawdata as soon as posible
    if((unTrigUpLimit < pstMainChnlExtralInfo->unRawdataMean)&&(unTrigUpLimit < pstMainChnlExtralInfo->unLastRawdataMean)&&(uchCurrentTiaGain > 0)) 
    {             
       uchNewGain = 0;  
       unTempDrvCurrent =  uchCurrentDrv;
       uchCalFinish = 1;
    }
    pstMainChnlExtralInfo ->unLastRawdataMean = pstMainChnlExtralInfo ->unRawdataMean;
         
    if(0 == uchCalFinish)
    {
        //remove offset
        if(CurRawdata >= (8388608+2048))  //// Can't open DC Cance !!!
        {
            CurRawdata -= 8388608;
        }
        else
        {
            //The value may be less than 2^23, preventing division by 0
            CurRawdata = 2048;  // Take an appropriately large number to prevent calculated DRV current overflow 32bit
        }
        //Rawdata stretch  (The principle of magnification: the mean divided by the ideal target value is the data reduction factor )
        if(CurRawdata < 8000000)
        {
        }
        else if(CurRawdata < 8100000)
        {
            CurRawdata = 8000000 + (CurRawdata - 8000000)*6;
        }
        else if(CurRawdata < 8200000)
        {
            CurRawdata = 8000000 + (CurRawdata - 8000000)*8;
        }
        else
        {
            // The more saturated the data, the bigger the stretch, the key is that the data is really saturated
            CurRawdata = 8000000 + (CurRawdata - 8000000)*14;   //  Blood oxygen drops 1 times, heart rate drops 2.5 times  
        }

        //find new drv current and gain
        do
        {   
            unTempDrvCurrent = (GS64)(((GU32)uchCurrentDrv*(GU32)g_usGH3x2xTiaGainR[uchCurrentTiaGain]))* \
            (GS64)unIdealRawdata_no/g_usGH3x2xTiaGainR[uchNewGain]/CurRawdata;

//          GH3X2X_SAMPLE_LOG_PARAM("Drv cal: uchCurrentDrv = %d, uchCurrentTiaGain = %d, unIdealRawdata_no = %d\r\n",uchCurrentDrv, uchCurrentTiaGain, unIdealRawdata_no);
//          GH3X2X_SAMPLE_LOG_PARAM("Drv cal: unTempDrvCurrent = %d, uchNewGain = %d, Meanraw = %d, CurRawdata = %d\r\n",unTempDrvCurrent, uchNewGain, pstMainChnlExtralInfo->unRawdataMean, CurRawdata);
          
            if(unTempDrvCurrent > uchLedCurrentMax)  //drv is too big, need increase gain
            {
                if(uchNewGain < uchGainUpLimit)
                {
                    uchNewGain ++ ;
                }
                else
                {
                    unTempDrvCurrent = uchLedCurrentMax;
                    break;
                }
            }
            else if(unTempDrvCurrent < uchLedCurrentMin)  //drv is too small, need dncrease gain
            {
                if(uchNewGain > 0)
                {
                    uchNewGain -- ;
                }
                else
                {
                    unTempDrvCurrent = uchLedCurrentMin;
                    break;
                }
            }
            else   //got right drv
            {
                break;
            }
            unCalNum --;
        }
        while (unCalNum);
    }

    puchIdealDrvCurrent[0] = (GU8)unTempDrvCurrent;
    puchIdealGain[0] = (GU8)uchNewGain;

    if(puchIdealDrvCurrent[0] != uchCurrentDrv)
    {
        uchChangeFlag |= 0x01;
    }
    if(puchIdealGain[0] != uchCurrentTiaGain)
    {
        uchChangeFlag |= 0x02;
    }
        
    return uchChangeFlag;
}



GU8 NewAgcMainChelFlag = 0;

void GH3X2X_NewAgcMainChnlPro(GU8* puchReadFifoBuffer, GU16 usFifoLen)
{
    GU32 unNewAgcRawdataMeanChnl[8] = {0};
    GU8 uchAdjFlag = 0;     // Local variables used as judgments must be initialized to 0
    GU8 uchNewDrvCurrent = 0;
    GU8 uchNewGain = 0;    
      GU8 uchChangeFlag = 0;  //bit0: drv current flag  bit1: gain flag
    
    if(0 == g_uchNewAgcSlotNumLimit)
    {
        return;
    }

#if GH3X2X_SPO2_SELF_TEST

    GH3X2X_Spo2AgcFifoDataGen();
    puchReadFifoBuffer = g_uchSpo2AgcFifoBuffer;
    usFifoLen = g_usSpo2AgcFifoLen;
#endif
    
            
    GH3X2X_NewAgcMainChnlMeanValueCal( puchReadFifoBuffer,usFifoLen,unNewAgcRawdataMeanChnl,&uchAdjFlag); // caculate mean and whther adjust gain  current 
    
    for(GU8 uchSlotCnt = 0; uchSlotCnt < g_uchNewAgcSlotNumLimit; uchSlotCnt ++)
    {
        if( uchAdjFlag & (1<<uchSlotCnt)) // need to AGC
        {
            STGh3x2xNewAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xNewAgcMainChnlInfo[uchSlotCnt];
            uchChangeFlag = GH3X2X_NewAgcCalDrvCurrentAndGain_1(unNewAgcRawdataMeanChnl[uchSlotCnt], uchSlotCnt, &uchNewDrvCurrent,&uchNewGain);
            GU8  uchSlotIdx = ((pstAgcMainChnlInfo ->uchSlotAdc >> 2) & 0x0f);
            if(uchChangeFlag&0x02)  //write tia gain to chip
            {
                    
                GH3x2xSetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + pstAgcMainChnlInfo ->uchBakRegAgcSrc, uchSlotIdx, uchNewGain);    
                pstAgcMainChnlInfo->uchCurrentTiaGain = uchNewGain;
                GH3X2X_StoreRXxTiaGainAfterAgc(uchNewGain, uchSlotCnt, pstAgcMainChnlInfo ->uchBakRegAgcSrc, g_puchTiaGainAfterSoftAgc);
            }
            if(uchChangeFlag&0x01)  //write drv current to chip
            {
                //change drv0/1
                pstAgcMainChnlInfo ->uchCurrentDrv = uchNewDrvCurrent;
                if((pstAgcMainChnlInfo ->uchBakRegAgcEn)&0x01)
                {
                    GH3x2xSetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DRV0, uchSlotIdx, uchNewDrvCurrent);    
                }
                if((pstAgcMainChnlInfo ->uchBakRegAgcEn)&0x02)
                {
                    GH3x2xSetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DRV1, uchSlotIdx, uchNewDrvCurrent);    
                }
                #if SUB_CHN_PRO_EN
                GH3x2xNewAgcSubChnlClearAnalysisCntAndSetGain(uchSlotCnt, 0x0F, 0xFF); //clear all rx analysis cnt in this slot
                #endif
            }
            if(uchChangeFlag)
            {
                g_pstGh3x2xNewAgcMainChnlMeanInfo[uchSlotCnt].uchHaveAgcAtLastReadPrd = 1; // current or  gain change at last time
                g_pstGh3x2xNewAgcMainChnlMeanInfo[uchSlotCnt].uchDropFlag = 0;
                NewAgcMainChelFlag |= (1<<uchSlotCnt); // ADjFlag write to CSV file
            }                          
        }                   
    }     
}


/**
 * @fn     void GH3X2X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen)
 * 
 * @brief  led agc process function
 *
 * @attention   None
 *
 * @param[in]   puchReadFifoBuffer         pointer to read fifo data buffer
 * @param[in]   usFifoLen           read fifo data buffer length
 * @param[out]  None
 *
 * @return  None
 */
__weak void GH3X2X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen)
{
    if(usFifoLen==0)
    {
        return;
    }
    
    GH3X2X_NewAgcMainChnlPro(puchReadFifoBuffer, usFifoLen);
    
#if SUB_CHN_PRO_EN
    GH3X2X_NewAgcSubChnlGainAdj(puchReadFifoBuffer, usFifoLen);
#endif

}

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
