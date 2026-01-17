#include <stdio.h>
#include "gh3x2x_demo_common.h"
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algo_config.h"
#include "gh3x2x_demo_algo_hook.h"
#include "SEGGER_RTT.h"
#include "gh_demo_config.h"
#include "gus.h"
#include "service_hr_bo.h"

#if (__GOODIX_ALGO_CALL_MODE__)
extern unsigned char GH3220_HR, GH3220_SPO2;

/**
 * @fn     void GH3X2X_AlgoLog(char *log_string)
 * 
 * @brief  for debug version, log
 *
 * @attention   this function must define that use debug version lib
 *
 * @param[in]   log_string      pointer to log string
 * @param[out]  None
 *
 * @return  None
 */

void GH3X2X_AlgoLog(GCHAR *log_string)
{
	SEGGER_RTT_printf(0, "%s", log_string);
}

/**
 * @fn     void GH3X2X_AdtAlgorithmResultReport(STHbAlgoResult stHbAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hb algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHbAlgoRes             hb algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AdtAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_ADT_ALGORITHM__)
    if (pstAlgoResult->uchUpdateFlag)
    {
        if (pstAlgoResult->snResult[0] == 1)     //wear on
        {
            #if __GH_MSG_WTIH_DRV_LAYER_EN__
            GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_ON_ALGO_ADT);
            #else
            GH3X2X_ClearSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF_ALGO_ADT);
            GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_ON_ALGO_ADT);
            #endif
            /* code implement by user */
			
        }
        else if (pstAlgoResult->snResult[0] == 2)   //wear off
        {
            #if __GH_MSG_WTIH_DRV_LAYER_EN__
            GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_OFF_ALGO_ADT);
            #else
            GH3X2X_ClearSoftEvent(GH3X2X_SOFT_EVENT_WEAR_ON_ALGO_ADT);
            GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF_ALGO_ADT);
            #endif
            /* code implement by user */
        }
    }
#endif
}

/**
 * @fn     void GH3X2X_HrAlgorithmResultReport(STHbAlgoResult stHbAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hb algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHbAlgoRes             hb algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_HrAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_HR_ALGORITHM__)
    GH3X2X_INFO_LOG("[%s]:%dbpm CL=%d SNR=%d\r\n", 
                      __FUNCTION__, pstAlgoResult->snResult[0],
                                    pstAlgoResult->snResult[1],
                                    pstAlgoResult->snResult[2]);
    GH3220_HR = pstAlgoResult->snResult[0];
#endif
}

/**
 * @fn     void GH3X2X_Spo2AlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult)
 *
 *
 * @brief  This function will be called after calculate spo2 algorithm.
 *
 * @attention   None
 *
 * @param[in]   stSpo2AlgoRes           spo2 algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_Spo2AlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
	char BLE_TX_BUFF[20];
#if (__USE_GOODIX_SPO2_ALGORITHM__)
    GH3X2X_INFO_LOG("[%s]:%d%% R=%d CL=%d %d %d %d\r\n", 
                      __FUNCTION__, pstAlgoResult->snResult[0],
                                    pstAlgoResult->snResult[1],
                                    pstAlgoResult->snResult[2],
                                    pstAlgoResult->snResult[3],
                                    pstAlgoResult->snResult[4],
                                    pstAlgoResult->snResult[5]);
    GH3220_SPO2 = pstAlgoResult->snResult[0];
	// sprintf(BLE_TX_BUFF, "SPO:%d%% ", pstAlgoResult->snResult[0]);
	// gus_tx_data_send(0, BLE_TX_BUFF, sizeof(BLE_TX_BUFF));
#endif
}

/**
 * @fn     void GH3X2X_HrvAlgorithmResultReport(STHrvAlgoResult stHrvAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate hrv algorithm.
 *
 * @attention   None
 *
 * @param[in]   stHrvAlgoRes            hrv algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_HrvAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_HRV_ALGORITHM__)
    /* code implement by user */
//    GH3X2X_INFO_ALGO_LOG("%d: hrv[%d] = %dms, %dms, %dms, %dms\r\n",lubFrameId, 
//                                                pstAlgoResult->snResult[5], 
//                                                pstAlgoResult->snResult[0],
//                                                pstAlgoResult->snResult[1], 
//                                                pstAlgoResult->snResult[2],
//                                                pstAlgoResult->snResult[3]);
	app_hrv_update( pstAlgoResult->snResult[5], pstAlgoResult->snResult);
#endif
}

#include "app_3220_task.h"
#include "service_hr_bo.h"
extern int16_t GH3220_ECG[];
extern uint8_t ECG_Index;



/**
 * @fn     void GH3X2X_EcgAlgorithmResultReport(STEcgAlgoResult stEcgAlgoRes[], GU16 pusAlgoResIndexArr[], usAlgoResCnt)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_EcgAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
	#define SAMPLE_CNT	4
	static int16_t ecg_data[SAMPLE_CNT];
    static int32_t ecg_avg = 0;
    static uint8_t index = 0;
#if (__USE_GOODIX_ECG_ALGORITHM__)
    /* code implement by user */
    //GH3X2X_INFO_LOG("%d: ecg = %d uV, %d bpm\r\n",lubFrameId, pstAlgoResult->snResult[0] * 10, pstAlgoResult->snResult[1]);
	
	ecg_data[index] = (int16_t)(pstAlgoResult->snResult[0]);
	ecg_avg += ecg_data[index];
	index++;
	
//	{
//		char ble_tx[32];
//		sprintf( ble_tx, "t:%d:%d\r\n", lubFrameId, pstAlgoResult->snResult[0]);
//		gus_tx_data_send(0, (uint8_t*)ble_tx, strlen(ble_tx));
//	}

	if(pstAlgoResult->snResult[1])
	{
		ecg_hr_set(pstAlgoResult->snResult[1]);  
	}
	
	
	if(index == SAMPLE_CNT)
	{		
		GH3220_ECG[ECG_Index] = ecg_avg / SAMPLE_CNT;
		ecg_avg = 0;
		index = 0;
//		{
//			char ble_tx[32];
//			sprintf( ble_tx, "a:%d:%d\r\n", lubFrameId, GH3220_ECG[ECG_Index]);
//			gus_tx_data_send(0, (uint8_t*)ble_tx, strlen(ble_tx));
//		}
		ECG_Index++;
		if(ECG_Index >= MEASURE_POINTS_NUM)
		{
			ECG_Index = 0;
			if(gh3220_ecg_print_get())
			{
				app_cardiogram_updated( GH3220_ECG, MEASURE_POINTS_NUM);
			}
		}
	}
	
#endif
}

/**
 * @fn     void GH3X2X_SoftAdtGreenAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */

GU8 g_SoftAdtWearState = GH3X2X_SOFT_ALGO_ADT_DEFAULT; // 0:default 1: wear on  1: wear off
void GH3X2X_SoftAdtGreenAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
    //live object
    if (pstAlgoResult->snResult[0] == 0x1 && g_SoftAdtWearState != GH3X2X_SOFT_ALGO_ADT_WEAR_ON)
    {
       #if __GH_MSG_WTIH_DRV_LAYER_EN__
       GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_ON);
       #else
       GH3X2X_ClearSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF);
       GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_ON);
       #endif
       g_SoftAdtWearState = GH3X2X_SOFT_ALGO_ADT_WEAR_ON;
        /* code implement by user */
    }
    //non live object
    else if ((pstAlgoResult->snResult[0] & 0x2) && g_SoftAdtWearState != GH3X2X_SOFT_ALGO_ADT_WEAR_OFF)
    {
        #if __GH_MSG_WTIH_DRV_LAYER_EN__
       GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_OFF);
       #else
       GH3X2X_ClearSoftEvent(GH3X2X_SOFT_EVENT_WEAR_ON);
       GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF);
       #endif
       g_SoftAdtWearState = GH3X2X_SOFT_ALGO_ADT_WEAR_OFF;
        /* code implement by user */
    }
    GOODIX_PLATFORM_NADT_RESULT_HANDLE_ENTITY();
#endif
}

/**
 * @fn     void GH3X2X_SoftAdtIrAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
 *
 * @brief  This function will be called after calculate ecg algorithm.
 *
 * @attention   None
 *
 * @param[in]   stEcgAlgoRes            ecg algorithm result array
 * @param[in]   pusAlgoResIndexArr      frame index of every algo result
 * @param[in]   usAlgoResCnt            number of algo result
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SoftAdtIrAlgorithmResultReport(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId)
{
#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
    //live object
    if (pstAlgoResult->snResult[0] == 0x1 && g_SoftAdtWearState != GH3X2X_SOFT_ALGO_ADT_WEAR_ON)
    {
       #if __GH_MSG_WTIH_DRV_LAYER_EN__
       GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_ON);
       #else
       GH3X2X_ClearSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF);
       GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_ON);
       #endif
       g_SoftAdtWearState = GH3X2X_SOFT_ALGO_ADT_WEAR_ON;
        /* code implement by user */
    }
    //non live object
    else if ((pstAlgoResult->snResult[0] & 0x2) && g_SoftAdtWearState != GH3X2X_SOFT_ALGO_ADT_WEAR_OFF)
    {
       #if __GH_MSG_WTIH_DRV_LAYER_EN__
       GH_SEND_MSG_WEAR_EVENT(GH3X2X_SOFT_EVENT_WEAR_OFF);
       #else
       GH3X2X_ClearSoftEvent(GH3X2X_SOFT_EVENT_WEAR_ON);
       GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF);
       #endif
       g_SoftAdtWearState = GH3X2X_SOFT_ALGO_ADT_WEAR_OFF;
        /* code implement by user */
    }
    GOODIX_PLATFORM_NADT_RESULT_HANDLE_ENTITY();
#endif
}

#endif // end else #if (__DRIVER_LIB_MODE__==__DRV_LIB_WITHOUT_ALGO__)

