/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_afe_drv.h
 *
 * @brief   gh3x2x afe driver functions
 *
 * @version v3.2.4.6
 *
 * @author  Gooidx Iot Team
 *
 */
#ifndef __GH3X2X_AFE_DRV_H__
#define __GH3X2X_AFE_DRV_H__

#include "gh3x2x_afe_user.h"
#include "gh3x2x_afe_version.h"

#ifndef GH3X2X_AFE_UNUSED_VAR
#define GH3X2X_AFE_UNUSED_VAR(X) ((void)(X))
#endif  // GH3X2X_AFE_UNUSED_VAR


#if defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */






/* type redefine */
typedef unsigned char       GU8;    /**< 8bit unsigned integer type */
typedef signed char         GS8;    /**< 8bit signed integer type */
typedef unsigned short      GU16;   /**< 16bit unsigned integer type */
typedef signed short        GS16;   /**< 16bit signed integer type */
typedef int                 GS32;   /**< 32bit signed integer type */
typedef unsigned int        GU32;   /**< 32bit unsigned integer type */
typedef float               GF32;   /**< float type */
typedef double              GD64;   /**< double type */
typedef char                GCHAR;  /**< char type */
typedef unsigned char       GBOOL; 
typedef long long           GS64;   /**< 64bit signed integer type */




#define GH3020_ID                           (0x00)
#define GH3026_ID                           (0x01)

#define GH3X2X_MP_DISABLE                   (0)
#define GH3X2X_MP_ENABLE                    (1)

/********************* communication interface type******************************/
#define GH3X2X_AFE_COM_INTERFACE_SPI  0
#define GH3X2X_AFE_COM_INTERFACE_I2C  1


/*********************  spi type ************************************************/
#define GH3X2X_AFE_SPI_SOFTWARE_CS        0
#define GH3X2X_AFE_SPI_HARDWEAR_CS        1


/*********************  chip process type ***************************************/
#define GH3X2X_AFE_PRO_BY_INT             0
#define GH3X2X_AFE_PRO_BY_POLLING         1

/*********************  chip reset type ***************************************/
#define GH3X2X_AFE_RESET_HARDWARE         0
#define GH3X2X_AFE_RESET_SOFTWARE         1         



#define GH3X2X_AFE_SPI_CS_CTRL_LOW        0
#define GH3X2X_AFE_SPI_CS_CTRL_HIGH       1




#define GH3X2X_AFE_CMD_SLEEP  0xC4
#define GH3X2X_AFE_CMD_WAKEUP 0xC3
#define GH3X2X_AFE_CMD_RESET  0xC2
#define GH3X2X_AFE_SPI_CMD_WRITE              (0xF0)
#define GH3X2X_AFE_SPI_CMD_READ               (0xF1)



#define   GH3X2X_AFE_RET_OK                             (0)             /**< there is no error */
#define   GH3X2X_AFE_RET_GENERIC_ERROR                  (-1)            /**< a generic error happens */

// special reg addr
#define   GH3X2X_AFE_FIFO_REG_ADDR                      (0xAAAA)    /**< fifo reg addr */
#define   GH3X2X_AFE_I2C_CMD_ADDR                       (0xDDDD)    /**< i2c cmd reg addr */


#define GH3X2X_AFE_SPI_END_DELAY_X_US         (3)
#define GH3X2X_AFE_SPI_WAIT_DELAY_X_US        (4)

#define GH3X2X_AFE_WAKE_UP_CMD_DELAY_X_US   (500)
#define GH3X2X_AFE_HARD_RESET_DELAY_X_US    (100)



#define GH3X2X_AFE_CHANGE_BYTE_U16(x)			((((u16)(((u8*)(&x))[0]))<<8) + ((u16)(((u8 *)(&x))[1])))
/// get high byte from word
#define   GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(sValue)    ((GU8)(((sValue) >> 8) & 0xFFU))
/// get low byte from word
#define   GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(sValue)     ((GU8)((sValue) & 0xFFU))
#define   GH3X2X_AFE_MAKEUP_WORD(uchHighByte, uchLowByte)              ((GU16)(((((GU16)(uchHighByte)) << 8)& 0xFF00) |\
                                                                            (((GU16)(uchLowByte))& 0x00FF)))

/// makeup dword from bytes
#define   GH3X2X_AFE_MAKEUP_DWORD(uchByte3, uchByte2, uchByte1, uchByte0)     (((((GU32)(uchByte3)) << 24) & 0xFF000000U)|\
                                                                          ((((GU32)(uchByte2)) << 16) & 0x00FF0000U) |\
                                                                          ((((GU32)(uchByte1)) << 8) & 0x0000FF00U) |\
                                                                          (((GU32)(uchByte0)) & 0x000000FFU))


/// rawdata clear all flag
#define   GH3X2X_RAWDATA_CLEAR_ALL_FLAG(x)              ((x) & 0x00FFFFFF)


/// macro of val get bits
#define   GH3X2X_AFE_VAL_GET_BIT(x, b)          ((x) &= (b))



#define GH3X2X_AFE_EVENT_BIT(x)   ((GU16)1 << x)



#define GH3X2X_AFE_REG_IS_VALID(x)		((x) <= 0x0780)
/// macro of val clear bits
#define   GH3X2X_VAL_CLEAR_BIT(x, b)        ((x) &= (~(b)))
/// macro of val set bits
#define   GH3X2X_VAL_SET_BIT(x, b)          ((x) |= (b))


#define GH3X2X_PIN_STATUS_CS_0_MISO_0   (0)
#define GH3X2X_PIN_STATUS_CS_1_MISO_0   (1)
#define GH3X2X_PIN_STATUS_CS_0_MISO_1   (2)
#define GH3X2X_PIN_STATUS_CS_1_MISO_1   (3)


/// get byte3 from dword
#define   GH3X2X_AFE_GET_BYTE3_FROM_DWORD(unValue)      ((GU8)(((unValue) >> 24) & 0x000000FFU))
/// get byte2 from dword
#define   GH3X2X_AFE_GET_BYTE2_FROM_DWORD(unValue)      ((GU8)(((unValue) >> 16) & 0x000000FFU))
/// get byte1 from dword
#define   GH3X2X_AFE_GET_BYTE1_FROM_DWORD(unValue)      ((GU8)(((unValue) >> 8) & 0x000000FFU))
/// get byte0 from dword
#define   GH3X2X_AFE_GET_BYTE0_FROM_DWORD(unValue)      ((GU8)((unValue) & 0x000000FFU))

/// macro of val set bits
#define   GH3X2X_VAL_SET_BIT(x, b)          ((x) |= (b))

/// macro of clear bits
#define   GH3X2X_CLEAR_BIT(x, b)            ((x) & (~(b)))

/// macro of val clear bits
#define   GH3X2X_VAL_CLEAR_BIT(x, b)        ((x) &= (~(b)))

/// macro of val get bits
#define   GH3X2X_VAL_GET_BIT(x, b)          ((x) &= (b))


#define GH3X2X_AFE_I2C_DEVICE_BASE        (0x28)
extern GU16 gh3x2x_i2c_addr;
#define GH3X2X_AFE_I2C_DEVICE_ADDR        (gh3x2x_i2c_addr)
//#define GH3X2X_AFE_I2C_DEVICE_ADDR        (GH3X2X_AFE_I2C_DEVICE_BASE | (GH3X2X_AFE_I2C_DEVICE_CTRL_PIN_STATUS << 1))


#define   GH3X2X_AFE_FIFO_RAWDATA_SIZE                           (4)
#define   GH3X2X_AFE_SLOT_ADC_NUM_BITS                       (0xF8)
#define   GH3X2X_AFE_CHANNEL_MAP_GET(x)                     ((x) & GH3X2X_AFE_SLOT_ADC_NUM_BITS)

 

/**
 * @brief register struct
 */
typedef struct
{
    GU16 usRegAddr;     /**< register address */
    GU16 usRegData;     /**< register val */
} STGH3X2X_AFEReg;



typedef struct
{
    GU8 uchAddrOffset;
    GU8 uchMsb;
    GU8 uchLsb;
} STGh3x2xPpgCtrlReg;

extern void GH3X2X_AFE_SpiSendCmd(GU8 uchCmd);
extern void GH3X2X_AFE_SpiWriteReg(GU16 usRegAddr, GU16 usRegValue);
extern GU16 GH3X2X_AFE_SpiReadReg(GU16 usRegAddr);
extern void GH3X2X_AFE_SpiReadRegs(GU16 usRegAddr, GU16 *pusRegValueBuffer, GU8 uchLen);

extern void GH3X2X_AFE_SpiWriteRegHardwareCs(GU16 usRegAddr, GU16 usRegValue);
extern void GH3X2X_AFE_SpiReadRegsHardwareCs(GU16 usRegAddr, GU16 *pusRegValueBuffer, GU8 uchLen);
extern GU16 GH3X2X_AFE_SpiReadRegHardwareCs(GU16 usRegAddr);
extern void GH3X2X_AFE_I2cWriteReg(GU16 usRegAddr, GU16 usRegValue);
extern GU16 GH3X2X_AFE_I2cReadReg(GU16 usRegAddr);
extern void GH3X2X_I2cReadRegs(GU16 usRegAddr, GU16 *pusRegValueArr, GU8 uchLen);
extern void GH3X2X_AFE_Reset(void);
extern void GH3X2X_AFE_SpiSendCmdHardwareCs(GU8 uchCmd);
extern void GH3X2X_AFE_I2cSendCmd(GU8 uchCmd);


/****************  cmd/read/write  funciton define **************************/
#if(GH3X2X_AFE_COM_INTERFACE_TYPE == GH3X2X_AFE_COM_INTERFACE_SPI)
#if(GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_SOFTWARE_CS)
#define GH3X2X_AFE_SEND_CMD(cmd)                                               GH3X2X_AFE_SpiSendCmd(cmd)
#define GH3X2X_AFE_WRITE_REG(usRegAddr,  usRegValue)                           GH3X2X_AFE_SpiWriteReg(usRegAddr, usRegValue)
#define GH3X2X_AFE_READ_REG(usRegAddr)                                         GH3X2X_AFE_SpiReadReg(usRegAddr)
#define GH3X2X_AFE_READ_FIFO(buf, len)                                         GH3X2X_AFE_SpiReadFifo(buf, len)
#define GH3X2X_AFE_READ_REGS(usRegAddr,pusRegValueBuffer,uchLen)               GH3X2X_AFE_SpiReadRegs(usRegAddr,pusRegValueBuffer,uchLen)
#else
#define GH3X2X_AFE_SEND_CMD(cmd)                                               GH3X2X_AFE_SpiSendCmdHardwareCs(cmd)
#define GH3X2X_AFE_WRITE_REG(usRegAddr,  usRegValue)                           GH3X2X_AFE_SpiWriteRegHardwareCs(usRegAddr, usRegValue)
#define GH3X2X_AFE_READ_REG(usRegAddr)                                         GH3X2X_AFE_SpiReadRegHardwareCs(usRegAddr)
#define GH3X2X_AFE_READ_FIFO(buf, len)                                         GH3X2X_AFE_SpiReadFifoHardwareCs(buf, len)
#define GH3X2X_AFE_READ_REGS(usRegAddr,pusRegValueBuffer,uchLen)               GH3X2X_AFE_SpiReadRegsHardwareCs(usRegAddr,pusRegValueBuffer,uchLen)

#endif
#else
#define GH3X2X_AFE_SEND_CMD(cmd)                                                GH3X2X_AFE_I2cSendCmd(cmd)
#define GH3X2X_AFE_WRITE_REG(usRegAddr,  usRegValue)                            GH3X2X_AFE_I2cWriteReg(usRegAddr, usRegValue)
#define GH3X2X_AFE_READ_REG(usRegAddr)                                          GH3X2X_AFE_I2cReadReg(usRegAddr)
#define GH3X2X_AFE_READ_FIFO(buf, len)                                          GH3X2X_AFE_I2cReadFifo(buf, len)
#define GH3X2X_AFE_READ_REGS(usRegAddr,pusRegValueBuffer,uchLen)                GH3X2X_I2cReadRegs(usRegAddr,pusRegValueBuffer,uchLen)

#endif


/***************************** GH3X2X event *************************************************/

#define GH3X2X_AFE_EVENT_COM_READY                    (0)
#define GH3X2X_AFE_EVENT_LEAD_ON                      (1)
#define GH3X2X_AFE_EVENT_LEAD_OFF                     (2)
#define GH3X2X_AFE_EVENT_FAST_RECOVER_DONE            (3)
#define GH3X2X_AFE_EVENT_ADC_DONE                     (4)
#define GH3X2X_AFE_EVENT_FIFO_FULL                    (5)
#define GH3X2X_AFE_EVENT_FIFO_OV                      (6)
#define GH3X2X_AFE_EVENT_FIFO_UN                      (7)
#define GH3X2X_AFE_EVENT_LED_TUNE_FAIL     (8)
#define GH3X2X_AFE_EVENT_LED_TUNE_DONE     (9)
#define GH3X2X_AFE_EVENT_WEAR_ON           (10)
#define GH3X2X_AFE_EVENT_WEAR_OFF          (11)
#define GH3X2X_AFE_EVENT_SLOT_TIME_OUT     (12)
#define GH3X2X_AFE_EVENT_SAMPLE_RATE_ERROR (13)
#define GH3X2X_AFE_EVENT_ERSET             (14)






/***************************** GH3X2X registers *********************************************/


#define GH3X2X_AFE_REG_INT_EVENT_ADDR                          0x0508
#define GH3X2X_AFE_REG_FIFO_USE_ADDR                           0x050A
#define GH3X2X_AFE_REG_CHIP_READY_CODE_REG_ADDR                0x0036
#define GH3X2X_AFE_REG_SLOT_TMR0_REG_ADDR                      0x01EC
#define GH3X2X_AFE_REG_INT_EVENT_EN_ADDR                       0x0502

#define GH3X2X_AFE_REG_SLOT_EN_ADDR                            0x0108


#define GH3X2X_AFE_REG_START_ADDR                              0x0000
#define GH3X2X_AFE_REG_START_MSB                               0
#define GH3X2X_AFE_REG_START_LSB                               0


/**************************** GH3X2X ppg control reg index ***********************************************/
#define GH3X2X_AFE_PPG_CTRL_REG_AGC_EN                     0
#define GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_UP_LIMIT       1
#define GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_DOWN_LIMIT     2
#define GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_DRV0           3
#define GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_DRV1           4
#define GH3X2X_AFE_PPG_CTRL_REG_TIA_GAIN_RX0               5
#define GH3X2X_AFE_PPG_CTRL_REG_TIA_GAIN_RX1               6
#define GH3X2X_AFE_PPG_CTRL_REG_TIA_GAIN_RX2               7
#define GH3X2X_AFE_PPG_CTRL_REG_TIA_GAIN_RX3               8
#define GH3X2X_AFE_PPG_CTRL_REG_AGC_SRC                    9
#define GH3X2X_AFE_PPG_CTRL_REG_BG_CANCEL_RX0              10
#define GH3X2X_AFE_PPG_CTRL_REG_BG_CANCEL_RX1              11
#define GH3X2X_AFE_PPG_CTRL_REG_BG_CANCEL_RX2              12
#define GH3X2X_AFE_PPG_CTRL_REG_BG_CANCEL_RX3              13
#define GH3X2X_AFE_PPG_CTRL_REG_SR_MULIPLIPER              14
#define GH3X2X_AFE_PPG_CTRL_ECG_PPG_RX_EN                  15

#define  GH3X2X_DRV0_FULL_SCAL_CURRENT_LSB          (0)
#define  GH3X2X_DRV0_FULL_SCAL_CURRENT_MSB          (1)

#define  GH3X2X_DRV1_FULL_SCAL_CURRENT_LSB          (4)
#define  GH3X2X_DRV1_FULL_SCAL_CURRENT_MSB          (5)

#define  GH3X2X_WHO_AM_I_LSB                        (5)
#define  GH3X2X_WHO_AM_I_MSB                        (7)

#define  GH3X2X_EFUSE_MODE_LSB                      (0)
#define  GH3X2X_EFUSE_MODE_MSB                      (1)

#define  GH3X2X_EFUSE_CHOOSE_LSB                    (2)
#define  GH3X2X_EFUSE_CHOOSE_MSB                    (3)

#define  GH3X2X_EFUSE_START_LSB                     (4)
#define  GH3X2X_EFUSE_START_MSB                     (4)

#define  GH3X2X_EFUSE_CLK_EN_LSB                    (5)
#define  GH3X2X_EFUSE_CLK_EN_MSB                    (5)




#define   GH3X2X_LED_DRV_AD_REG_REG_ADDR                (0x0692)
#define   GH3X2X_WHO_AM_I_REG_ADDR                      (0x0724)


// efuse_ctrl block, start addr: 0x0700
#define   GH3X2X_EFUSE_CTRL_CMD_REG_ADDR                (0x0700)
#define   GH3X2X_EFUSE_CTRL_PASSWORD_REG_ADDR           (0x0702)
#define   GH3X2X_EFUSE_CTRL_TIMING_CFG_0_REG_ADDR       (0x0704)
#define   GH3X2X_EFUSE_CTRL_TIMING_CFG_1_REG_ADDR       (0x0706)
#define   GH3X2X_EFUSE_CTRL_WDATA_0_REG_ADDR            (0x0708)
#define   GH3X2X_EFUSE_CTRL_WDATA_1_REG_ADDR            (0x070A)
#define   GH3X2X_EFUSE_CTRL_WDATA_2_REG_ADDR            (0x070C)
#define   GH3X2X_EFUSE_CTRL_WDATA_3_REG_ADDR            (0x070E)
#define   GH3X2X_EFUSE_CTRL_RDATA_0_REG_ADDR            (0x0710)
#define   GH3X2X_EFUSE_CTRL_RDATA_1_REG_ADDR            (0x0712)
#define   GH3X2X_EFUSE_CTRL_RDATA_2_REG_ADDR            (0x0714)
#define   GH3X2X_EFUSE_CTRL_RDATA_3_REG_ADDR            (0x0716)
#define   GH3X2X_EFUSE_CTRL_STATUS_REG_ADDR             (0x0718)
#define   GH3X2X_EFUSE_CTRL_REG_MODE_REG_ADDR           (0x071A)
  
/***************************** GH3X2X slot *************************************************/

#define GH3X2X_AFE_SLOT_NUM0                      (0x01 << 0)
#define GH3X2X_AFE_SLOT_NUM1                      (0x01 << 1)
#define GH3X2X_AFE_SLOT_NUM2                      (0x01 << 2)
#define GH3X2X_AFE_SLOT_NUM3                      (0x01 << 3)
#define GH3X2X_AFE_SLOT_NUM4                      (0x01 << 4)
#define GH3X2X_AFE_SLOT_NUM5                      (0x01 << 5)
#define GH3X2X_AFE_SLOT_NUM6                      (0x01 << 6)
#define GH3X2X_AFE_SLOT_NUM7                      (0x01 << 7)
#define GH3X2X_AFE_SLOT_ALL                       (0xFF)

extern GU8 g_uchGh3x2xAfeIntCallBackIsCalled;


extern void GH3X2X_AFE_SetNeedWakeUpGh3x2xFlag(GU8 uchFlag);
extern void hal_gh3x2x_afe_DelayUs(GU16 usUsec);
extern void hal_gh3x2x_afe_DelayMs(GU16 usMsec);

extern void hal_gh3x2x_afe_i2c_init(void);
extern GU8 hal_gh3x2x_afe_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length);
extern GU8 hal_gh3x2x_afe_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length);

extern void hal_gh3x2x_afe_spi_init(void);
extern GU8 hal_gh3x2x_afe_spi_write(GU8 write_buffer[], GU16 length);
extern GU8 hal_gh3x2x_afe_spi_read(GU8 read_buffer[], GU16 length);
extern GU8 hal_gh3x2x_afe_spi_write_F1_and_read(GU8 read_buffer[], GU16 length);
extern void hal_gh3x2x_afe_spi_cs_ctrl(GU8 cs_pin_level);

extern void hal_gh3x2x_afe_reset_pin_init(void);
extern void hal_gh3x2x_afe_reset_pin_ctrl(GU8 pin_level);

extern void hal_gh3x2x_afe_int_init(void);
extern void hal_gh3x2x_afe_int_handler_call_back(void);

extern void GH3X2X_AFE_Log(GCHAR *log_string);





extern void GH3X2X_AFE_ChipPro(void);
extern void GH3X2X_AFE_Init(void);
extern void GH3X2X_AFE_StartSampling(GU8 SlotCfgEnable);
extern void GH3X2X_AFE_StopSampling(void);
extern void GH3X2X_AFE_SwitchSlot(GU8 SlotCfgEnable);

extern void GH3x2xSetPpgCtrlReg(GU8 uchPpgCtrlReg,  GU8 uchSlot, GU16 usValue);
extern GU16 GH3x2xGetPpgCtrlReg(GU8 uchPpgCtrlReg,  GU8 uchSlot);

GU16 GH3X2X_AFE_SetLedDrvCurrent(GU8 uchCurrent_mA, GU8 uchDrvNum, GS8 chECODE);
GF32 GH3X2X_AFE_GetLedDrvCurrent(GU16 usDrvCode, GU8 uchDrvNum, GS8 chECODE);
void GH3X2X_AFE_GetEfuseCode(GS8* pchLedDrvEcode,GS8* pchTiaEcode,GS8* pchEcgEcode);
GF32 GH3X2X_AFE_GetTiaCalCode(GS8 chECODE);
GF32 GH3X2X_AFE_GetEcgCalCode(GS8 chECODE);
GCHAR *GH3X2X_GetAfeLibVersion(void);
GU8 GH3X2X_AFE_GetChipId(void);
GS8 GH3X2X_AFE_CheckChipId(void);
void GH3X2X_AFE_LoadEfuse(GU8* puchEfuseBuffer,GU16 EfuseByteLen);
GS8 GH3X2X_AFE_CheckEfuseCrc(void);
void GH3X2X_AFE_WriteRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usValue);
GU16 GH3X2X_AFE_ReadRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb);
GU8 GH3X2X_AFE_GetSoftEvent(void);
void GH3X2X_AFE_SetSoftEvent(GU8 uchEvent);
void GH3X2X_AFE_ClearSoftEvent(GU8 uchEvent);
void GH3X2X_AFE_StartFunction(GU32 unFuncMode);

void GH3X2X_AFE_SetSlotLedDrvCurrent(GU8 uchSlotCfg, GU8 uchDrvNum, GU8 uchCurrent_mA);
GU8 GH3X2X_AFE_GetSlotLedDrvCurrent(GU8 uchSlotCfg, GU8 uchDrvNum);
/*********************************************** master-slaver interface lib function *********************************/
#define EXAMPLE_LOG(...)                    do {\
                                                char GH3X2X_AFE_example_log[128] = {0};\
                                                snprintf(GH3X2X_AFE_example_log, 128, __VA_ARGS__);\
                                                GH3X2X_AFE_Log((char *)GH3X2X_AFE_example_log);\
                                            } while(0)




#endif















