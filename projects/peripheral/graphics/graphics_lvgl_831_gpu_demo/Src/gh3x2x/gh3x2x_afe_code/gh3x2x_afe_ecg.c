
#include "gh3x2x_afe_ecg.h"
#include "gh3x2x_afe_user.h"

#if USE_GH3X2X_AFE_ECG_LEAD_LIB

#define   GH3X2X_ECG_FS                             (500)
#define   GH3X2X_ECG_SLOT_NUM                        (0)
#define   GH3X2X_ADT_SLOT_NUM                        (1)


GU8 g_uchAdtSlotNum = 0;
GU32 g_unFuncMode = GH3X2X_NO_FUNCTION;
void GH3X2X_AFE_StartFunction(GU32 unFuncMode)
{
    if(unFuncMode & (GH3X2X_FUNCTION_ADT))
    {
        GH3X2X_AFE_WriteRegBitField(GH3X2X_AFE_REG_SLOT_EN_ADDR,GH3X2X_ADT_SLOT_NUM,GH3X2X_ADT_SLOT_NUM,1);
    }
    if(unFuncMode & (GH3X2X_FUNCTION_ECG))
    {
        GH3X2X_LeadOffDetectInit();
        GH3X2X_ECGResampleConfig(GH3X2X_ECG_FS);        
        GH3X2X_AFE_WriteRegBitField(GH3X2X_AFE_REG_SLOT_EN_ADDR,GH3X2X_ECG_SLOT_NUM,GH3X2X_ECG_SLOT_NUM,1);
        ECG_FAST_RECV_EN_CONFIG(1);
    }
    if(g_unFuncMode == GH3X2X_NO_FUNCTION)
    {
        GH3X2X_AFE_WriteRegBitField(GH3X2X_AFE_REG_START_ADDR,GH3X2X_AFE_REG_START_LSB,GH3X2X_AFE_REG_START_MSB,1);
    }
    g_unFuncMode |= unFuncMode;

}


void GH3X2X_AFE_StopFunction(GU32 unFuncMode)
{
    if(unFuncMode & (GH3X2X_FUNCTION_ADT))
    {
        GH3X2X_AFE_WriteRegBitField(GH3X2X_AFE_REG_SLOT_EN_ADDR,GH3X2X_ADT_SLOT_NUM,GH3X2X_ADT_SLOT_NUM,0);
    }
    if(unFuncMode & (GH3X2X_FUNCTION_ECG))
    {
        GH3X2X_AFE_WriteRegBitField(GH3X2X_AFE_REG_SLOT_EN_ADDR,GH3X2X_ECG_SLOT_NUM,GH3X2X_ECG_SLOT_NUM,0);
        ECG_FAST_RECV_EN_CONFIG(0);

    }
    g_unFuncMode &= (~unFuncMode);
    if(g_unFuncMode == GH3X2X_NO_FUNCTION)
    {
        GH3X2X_AFE_WriteRegBitField(GH3X2X_AFE_REG_START_ADDR,GH3X2X_AFE_REG_START_LSB,GH3X2X_AFE_REG_START_MSB,0);
    }
    
}


void GH3X2X_AFE_Leadon_hook(void)
{
    GH3X2X_AFE_StartFunction(GH3X2X_FUNCTION_ECG);
	ECG_FAST_RECV_EN_CONFIG(0);
	ECG_FAST_RECV_EN_CONFIG(1);
}

void GH3X2X_AFE_Leadoff_hook(void)
{
    GH3X2X_AFE_StopFunction(GH3X2X_FUNCTION_ECG);
    ECG_LEAD_DET_EN_CONFIG(0);// enable lead detect
    ECG_LEAD_DET_EN_CONFIG(1);// enable lead detect
    
}
#else
void GH3X2X_AFE_Leadon_hook(void)
{

}

void GH3X2X_AFE_Leadoff_hook(void)
{

    
}
#endif








