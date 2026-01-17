#ifndef __GH3X2X_AFE_ECG_H__
#define __GH3X2X_AFE_ECG_H__
#include "gh3x2x_afe_drv.h"
#include "gh3x2x_drv.h"

//#define   GH3X2X_NO_FUNCTION                        (0)                                             /**<0x00000000 NO function */
//#define   GH3X2X_FUNCTION_ADT                       ((0x1) << (0))             /**<0x00000001 ADT function */ 
//#define   GH3X2X_FUNCTION_ECG                       ((0x1) << (7))             /**<0x00000080 ECG function */
typedef struct
{
    GS32 snRsXbuf[250/5 + 1];
    GS32 snBufHead;
    GS32 snRsFirState;
    GU32 nRsIcounter;
    GU32 nRrsDcounter;
    GU8 uchFlagFrPrev;
} STGh3x2xRsInfo;


typedef struct
{
    GS32 snCounter;
    GS32 sniqCounter;
    long long snniVal;
    long long snnqVal;
    GU8 uchFlagFrPrev;
    GU8 uchFastRecoryHistory;
    long long snniqAmp[5];
} STGh3x2xSoftLeadOffDetInfo;

  
/* ecg sample event type */
#define  ECG_SAMPLE_EVENT_TYPE_SAMPLE                     (0x01)     /**< sample evt type sample config */
#define  ECG_SAMPLE_EVENT_TYPE_SLOT                       (0x02)     /**< sample evt type slot config */
#define  ECG_SAMPLE_EVENT_TYPE_LEAD                       (0x04)     /**< sample evt type lead irq */
#define  ECG_SAMPLE_EVENT_ALL                             (ECG_SAMPLE_EVENT_TYPE_SAMPLE | ECG_SAMPLE_EVENT_TYPE_SLOT | \
                                                            ECG_SAMPLE_EVENT_TYPE_LEAD)     /**< sample evt all */

#define   GH3X2X_ECG_CTRL_REG_ADDR                      (0x01EA)
#define   GH3X2X_ECG_FAST_RECV_EN_LSB               (10)        /**< ecg fast recv en lsb @GH3X2X_ECG_CTRL_REG */
#define   GH3X2X_ECG_FAST_RECV_EN_MSB               (10)        /**< ecg fast recv en msb @GH3X2X_ECG_CTRL_REG */
/// ecg fast recover enable config
#define  ECG_FAST_RECV_EN_CONFIG(en)        do {\
                                                GH3X2X_AFE_WriteRegBitField(GH3X2X_ECG_CTRL_REG_ADDR, \
                                                    GH3X2X_ECG_FAST_RECV_EN_LSB, GH3X2X_ECG_FAST_RECV_EN_MSB, (en)); \
                                            } while (0)

#define   GH3X2X_ADT_LEADON_CR_REG_ADDR                 (0x0400)
#define   GH3X2X_ECG_LEAD_DET_EN_LSB                (0)         /**< ecg lead det en lsb @GH3X2X_ADT_LEADON_CR_REG */
#define   GH3X2X_ECG_LEAD_DET_EN_MSB                (0)         /**< ecg lead det en msb @GH3X2X_ADT_LEADON_CR_REG */
                                            
  /// ecg lead detect enable config
#define  ECG_LEAD_DET_EN_CONFIG(en)         do {\
                                                GH3X2X_AFE_WriteRegBitField(GH3X2X_ADT_LEADON_CR_REG_ADDR, \
                                                    GH3X2X_ECG_LEAD_DET_EN_LSB, GH3X2X_ECG_LEAD_DET_EN_MSB, (en)); \
                                            } while (0)                                          
                                            
                                            
#define GET_ECG_LEAD_DET_EN_VAL()              (GH3X2X_AFE_ReadRegBitField(GH3X2X_ADT_LEADON_CR_REG_ADDR, \
                                                    GH3X2X_ECG_LEAD_DET_EN_LSB, GH3X2X_ECG_LEAD_DET_EN_MSB))
 
 

GU8 GH3X2X_AFE_EcgRsHandle(GU8* puchFifoBuffer, GU16* pusFifoBufferLen, GU8 uchEcgSlotNum, GU16 usEcgSampleRate);
GS32 GH3X2X_LeadOffDetectInit(void);
void GH3X2X_ECGResampleConfig(GU16 usEcgSampleRate);
GU8 GH3X2X_LeadOffDetect(GU8* puchFifoBuffer, GU16* pusFifoBufferLen, GU8 uchEcgSlotNum);
void GH3X2X_AFE_Leadon_hook(void);
void GH3X2X_AFE_Leadoff_hook(void);


#endif
