/**
 * @copyright (c) 2019 - 2024, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_afe_drv.c
 * 
 * @brief   gh3x2x driver example code
 *
 * @author  Gooidx Iot Team
 * 
 * @attention
 *  This file just show how to init and control gh3x2x sampling.
 */
#include "stdio.h"
#include "string.h"
#include "gh3x2x_afe_drv.h"
#include "gh3x2x_afe_user.h"
#include "gh3x2x_afe_ecg.h"
#include "gh3x2x_drv_common.h"
#include "gh3x2x_drv_soft_led_agc.h"
#include "gh3x2x_drv_dump.h"

 
#define SWAP_SIZE_OF_GU16               (2)
#define SWAP_INDEX_OF_NEXT_BYTE         (1)

extern void GH3X2X_SoftLedADJAutoADJInt(void);
extern void GH3X2X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen);
extern void GH3X2X_LedAgcReset(void);
extern void GH3X2X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi);
extern void GH3X2X_LedAgcInit(void);



GU8 g_uchNeedWakeUpGh3x2xAfe = 1; // 0: do not need wake up gh3x2x  1: need wake up gh3x2x
GU8 g_uchGh3x2xAfeIntCallBackIsCalled = 0;  //0: no new interrupt    1:  have new interrupt

/*********************************************** reg cfg package *******************************************/
STGH3X2X_AFEReg GH3X2X_AFE_reg_list[] =
{
    {0x0004, 0x001F}, {0x0006, 0x0204}, {0x000A, 0x0064}, {0x000E, 0x000D}, {0x0100, 0x0100}, {0x0102, 0x0302},
    {0x0104, 0x0804}, {0x010A, 0x1350}, {0x010C, 0x0000}, {0x010E, 0x0004}, {0x0110, 0x0C15}, {0x0112, 0x4544},
    {0x0114, 0x0020}, {0x011E, 0x023F}, {0x0120, 0x0400}, {0x0126, 0x1350}, {0x0128, 0x0000}, {0x012A, 0x0004},
    {0x012C, 0x0C15}, {0x012E, 0x4544}, {0x0130, 0x0020}, {0x013A, 0x033F}, {0x013C, 0x0400}, {0x0142, 0x1350},
    {0x0144, 0x0000}, {0x0146, 0x0004}, {0x0148, 0x0C15}, {0x014C, 0x0020}, {0x0156, 0x013F}, {0x0158, 0x0400},
    {0x015E, 0x1350}, {0x0160, 0x0000}, {0x0162, 0x0004}, {0x0164, 0x0C15}, {0x0168, 0x0020}, {0x0172, 0x003F},
    {0x0174, 0x0400}, {0x017A, 0xC750}, {0x017C, 0x0000}, {0x017E, 0x0004}, {0x0180, 0x0C10}, {0x0182, 0x4244},
    {0x0184, 0x0020}, {0x018A, 0x0030}, {0x018E, 0x0019}, {0x0190, 0x0400}, {0x01EC, 0x01BF}, {0x01EE, 0x01BF},
    {0x01F0, 0x01BF}, {0x01F2, 0x01BF}, {0x01F4, 0x0098}, {0x0200, 0x0120}, {0x0408, 0x0001}, {0x0410, 0x9680},
    {0x0412, 0x0098}, {0x0414, 0xF560}, {0x0416, 0x0090}, {0x0422, 0x0011}, {0x0424, 0x0901}, {0x0426, 0x0102},
    {0x0428, 0x0102}, {0x0502, 0x4C60}, {0x0504, 0x000A}, {0x0588, 0x0008}, {0x0692, 0x0222}, {0x0694, 0x0400},
    {0x0698, 0x2310}, {0x0700, 0x0000}, {0x1000, 0x0000}, {0x1002, 0x3197}, {0x1004, 0x0000}, {0x1006, 0x0F7D},
    {0x1008, 0x2CC9}, {0x100A, 0x0001}, {0x10E0, 0x0000}, {0x10E2, 0x0000}, {0x10E4, 0xFF00}, {0x10E6, 0xFFFF},
    {0x10E8, 0x0202}, {0x10EA, 0x0202}, {0x10EC, 0x0202}, {0x10EE, 0x0202}, {0x1120, 0x0064}, {0x1122, 0x0005},
    {0x1124, 0x0000}, {0x1140, 0x0000}, {0x1142, 0x00C8}, {0x1144, 0x0001}, {0x2000, 0x0001}, {0x2002, 0x0092},
    {0x2022, 0x0002}, {0x2024, 0x3111}, {0x2044, 0x0002}, {0x2046, 0x3111}, {0x20CC, 0x0002}, {0x20CE, 0x7253},
    {0x2880, 0x0005}, {0x2882, 0x0019}, {0x2884, 0x0019}, {0x288C, 0x0019}, {0x3000, 0x0800}, {0x3002, 0x0000},
    {0x3004, 0x9680}, {0x3006, 0x0098}, {0xFFFF, 0x0001}
};

GU16 GH3X2X_AFE_reg_list_len = sizeof(GH3X2X_AFE_reg_list) / sizeof(GH3X2X_AFE_reg_list[0]);


void GH3X2X_SwapLittleBigEndianData(GU8 *puchDataBuffer, GU16 usDataLength)
{
    GU16 usIndex = 0;

    for (usIndex = 0; usIndex < (usDataLength / SWAP_SIZE_OF_GU16); usIndex++)
    {
        puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16] ^= \
                                                puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16 + SWAP_INDEX_OF_NEXT_BYTE];
        puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16 + SWAP_INDEX_OF_NEXT_BYTE] ^= \
                                                puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16];
        puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16] ^= \
                                                puchDataBuffer[usIndex * SWAP_SIZE_OF_GU16 + SWAP_INDEX_OF_NEXT_BYTE];
    }
}


#if ( GH3X2X_AFE_COM_INTERFACE_TYPE == GH3X2X_AFE_COM_INTERFACE_SPI )


/**
 * @fn     static void GH3X2X_AFE_SpiSendCmd(GU8 uchCmd)
 *
 * @brief  Send cmd via spi
 *
 * @attention   None
 *
 * @param[in]   uchCmd      spi cmd
 * @param[out]  None
 *
 * @return  None
 */
#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_SOFTWARE_CS) 
void GH3X2X_AFE_SpiSendCmd(GU8 uchCmd)
{
    GU8 uchBuffArr[1];

    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_LOW);
    uchBuffArr[0] = uchCmd;
    hal_gh3x2x_afe_spi_write(uchBuffArr, 1);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_HIGH);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_END_DELAY_X_US);
}
#endif

#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_HARDWEAR_CS) 
void GH3X2X_AFE_SpiSendCmdHardwareCs(GU8 uchCmd)
{
    GU8 uchBuffArr[1];
    uchBuffArr[0] = uchCmd;
    hal_gh3x2x_afe_spi_write(uchBuffArr, 1);
}
#endif


/**
 * @fn     static void GH3X2X_AFE_SpiWriteReg(GU16 usRegAddr, GU16 usRegValue)
 *
 * @brief  Write register via spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr       register addr
 * @param[in]   usRegValue      register data
 * @param[out]  None
 *
 * @return  None
 */
#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_SOFTWARE_CS) 
void GH3X2X_AFE_SpiWriteReg(GU16 usRegAddr, GU16 usRegValue)
{
    GU8 uchBuffArr[7];

    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_LOW);
    uchBuffArr[0] = GH3X2X_AFE_SPI_CMD_WRITE;
    uchBuffArr[1] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegAddr);
    uchBuffArr[2] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegAddr);
    uchBuffArr[3] = 0;
    uchBuffArr[4] = 2;
    uchBuffArr[5] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegValue);
    uchBuffArr[6] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegValue);
    hal_gh3x2x_afe_spi_write(uchBuffArr, 7);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_WAIT_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_HIGH);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_END_DELAY_X_US);
}
#endif

#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_HARDWEAR_CS) 
void GH3X2X_AFE_SpiWriteRegHardwareCs(GU16 usRegAddr, GU16 usRegValue)
{
    GU8 puchWriteBuf[7];
    puchWriteBuf[0] = 0xF0;
    puchWriteBuf[1] = usRegAddr>>8;
    puchWriteBuf[2] = usRegAddr&0x00FF;
    puchWriteBuf[3] = 0;
    puchWriteBuf[4] = 2;
    puchWriteBuf[5] = usRegValue>>8;
    puchWriteBuf[6] = usRegValue&0x00FF;
    hal_gh3x2x_afe_spi_write(puchWriteBuf, 7);
}
#endif







/**
 * @fn     static GU16 GH3X2X_AFE_SpiReadReg(GU16 usRegAddr)
 *
 * @brief  Read register via spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[out]  None
 *
 * @return  register data
 */
 
#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_SOFTWARE_CS) 
GU16 GH3X2X_AFE_SpiReadReg(GU16 usRegAddr)
{
    GU8 uchBuffArr[3] = {0};

    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_LOW);
    uchBuffArr[0] = GH3X2X_AFE_SPI_CMD_WRITE;
    uchBuffArr[1] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegAddr);
    uchBuffArr[2] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegAddr);
    hal_gh3x2x_afe_spi_write(uchBuffArr, 3);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_WAIT_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_HIGH);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_END_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_LOW);
    uchBuffArr[0] = GH3X2X_AFE_SPI_CMD_READ;
    hal_gh3x2x_afe_spi_write(uchBuffArr, 1);
    hal_gh3x2x_afe_spi_read(uchBuffArr, 2);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_WAIT_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_HIGH);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_END_DELAY_X_US);
    return GH3X2X_AFE_MAKEUP_WORD(uchBuffArr[0], uchBuffArr[1]);
}

/**
 * @fn      void GH3X2X_SpiReadRegs(GU16 usRegAddr, GU16 *pusRegValueBuffer, GU8 uchLen)
 *
 * @brief  Read multi register via spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr           register addr
 * @param[in]   pusRegValueBuffer   pointer to register value buffer
 * @param[in]   uchLen              registers len
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AFE_SpiReadRegs(GU16 usRegAddr, GU16 *pusRegValueBuffer, GU8 uchLen)
{
    GU8 uchBuffArr[3] = {0};

    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_LOW);
    uchBuffArr[0] = GH3X2X_AFE_SPI_CMD_WRITE;
    uchBuffArr[1] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegAddr);
    uchBuffArr[2] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegAddr);
    hal_gh3x2x_afe_spi_write(uchBuffArr, 3);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_WAIT_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_HIGH);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_END_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_LOW);
    uchBuffArr[0] = GH3X2X_AFE_SPI_CMD_READ;
    hal_gh3x2x_afe_spi_write(uchBuffArr, 1);
    hal_gh3x2x_afe_spi_read((GU8 *)pusRegValueBuffer, uchLen * 2);
    GH3X2X_SwapLittleBigEndianData((GU8 *)pusRegValueBuffer, (GU16)((GU16)uchLen * 2));
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_WAIT_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_HIGH);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_END_DELAY_X_US);
}

#endif

#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_HARDWEAR_CS) 
GU16 GH3X2X_AFE_SpiReadRegHardwareCs(GU16 usRegAddr)
{
    GU8 puchWriteBuf[7];
    GU8 puchReadBuf[2];
    puchWriteBuf[0] = 0xF0;
    puchWriteBuf[1] = usRegAddr>>8;
    puchWriteBuf[2] = usRegAddr&0x00FF;
    hal_gh3x2x_afe_spi_write(puchWriteBuf, 3);
    hal_gh3x2x_afe_spi_write_F1_and_read(puchReadBuf, 2);
    return ((((GU16)puchReadBuf[0])<<8) + puchReadBuf[1]);
}



void GH3X2X_AFE_SpiReadRegsHardwareCs(GU16 usRegAddr, GU16 *pusRegValueBuffer, GU8 uchLen)
{
    GU8 puchWriteBuf[3];
    puchWriteBuf[0] = 0xF0;
    puchWriteBuf[1] = usRegAddr>>8;
    puchWriteBuf[2] = usRegAddr&0x00FF;
    hal_gh3x2x_afe_spi_write(puchWriteBuf, 3);
    hal_gh3x2x_afe_spi_write_F1_and_read((GU8 *)pusRegValueBuffer, uchLen * 2);
    GH3X2X_SwapLittleBigEndianData((GU8 *)pusRegValueBuffer, (GU16)((GU16)uchLen * 2));
}


#endif





/**
 * @fn     static void GH3X2X_AFE_SpiReadFifo(GU8 *puchDataBuffer, GU16 usLen)
 *
 * @brief  Read fifo via spi
 *
 * @attention   None
 *
 * @param[in]   puchDataBuffer      pointer to buffer for fifo rawdata
 * @param[in]   usLen               fifo bytes len
 * @param[out]  None
 *
 * @return  None
 */
 
#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_SOFTWARE_CS) 
static void GH3X2X_AFE_SpiReadFifo(GU8 *puchDataBuffer, GU16 usLen)
{
    GU8 uchBuffArr[3] = {0};

    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_LOW);
    uchBuffArr[0] = GH3X2X_AFE_SPI_CMD_WRITE;
    uchBuffArr[1] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(GH3X2X_AFE_FIFO_REG_ADDR);
    uchBuffArr[2] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(GH3X2X_AFE_FIFO_REG_ADDR);
    hal_gh3x2x_afe_spi_write(uchBuffArr, 3);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_WAIT_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_HIGH);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_END_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_LOW);
    uchBuffArr[0] = GH3X2X_AFE_SPI_CMD_READ;
    hal_gh3x2x_afe_spi_write(uchBuffArr, 1);
    hal_gh3x2x_afe_spi_read(puchDataBuffer, usLen);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_WAIT_DELAY_X_US);
    hal_gh3x2x_afe_spi_cs_ctrl(GH3X2X_AFE_SPI_CS_CTRL_HIGH);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_SPI_END_DELAY_X_US);
}

#endif


#if (GH3X2X_AFE_SPI_TYPE == GH3X2X_AFE_SPI_HARDWEAR_CS) 
void GH3X2X_AFE_SpiReadFifoHardwareCs(GU8 *puchDataBuffer, GU16 usLen)
{
    GU8 puchWriteBuf[3];
    puchWriteBuf[0] = 0xF0;
    puchWriteBuf[1] = 0xAAAA>>8;
    puchWriteBuf[2] = 0xAAAA&0x00FF;
    hal_gh3x2x_afe_spi_write(puchWriteBuf, 3);
    hal_gh3x2x_afe_spi_write_F1_and_read(puchDataBuffer, usLen);
}
#endif

#endif

#if ( GH3X2X_AFE_COM_INTERFACE_TYPE == GH3X2X_AFE_COM_INTERFACE_I2C )

/**
 * @fn     static void GH3X2X_AFE_I2cSendCmd(GU8 uchCmd)
 *
 * @brief  Send cmd via i2c
 *
 * @attention   None
 *
 * @param[in]   uchCmd      i2c cmd
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AFE_I2cSendCmd(GU8 uchCmd)
{
    GU8 uchBuffArr[3] = {0};

    uchBuffArr[0] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(GH3X2X_AFE_I2C_CMD_ADDR);
    uchBuffArr[1] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(GH3X2X_AFE_I2C_CMD_ADDR);
    uchBuffArr[2] = uchCmd;
    
    hal_gh3x2x_afe_i2c_write(GH3X2X_AFE_I2C_DEVICE_ADDR, uchBuffArr, 3);
}

/**
 * @fn     static void GH3X2X_AFE_I2cWriteReg(GU16 usRegAddr, GU16 usRegValue)
 *
 * @brief  Write register via i2c
 *
 * @attention   None
 *
 * @param[in]   usRegAddr       register addr
 * @param[in]   usRegValue      register data
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AFE_I2cWriteReg(GU16 usRegAddr, GU16 usRegValue)
{
    GU8 uchBuffArr[4] = {0};

    uchBuffArr[0] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegAddr);
    uchBuffArr[1] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegAddr);
    uchBuffArr[2] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegValue);
    uchBuffArr[3] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegValue);
    hal_gh3x2x_afe_i2c_write(GH3X2X_AFE_I2C_DEVICE_ADDR, uchBuffArr, 4);
}



/**
 * @fn     static GU16 GH3X2X_I2cReadReg(GU16 usRegAddr)
 *
 * @brief  Read register via i2c
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[out]  None
 *
 * @return  register data
 */
GU16 GH3X2X_AFE_I2cReadReg(GU16 usRegAddr)
{
    GU8 uchCmdBuffArr[2] = {0};
    GU8 uchReadBuffArr[2] = {0};

    uchCmdBuffArr[0] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegAddr);
    uchCmdBuffArr[1] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegAddr);
    hal_gh3x2x_afe_i2c_read(GH3X2X_AFE_I2C_DEVICE_ADDR, uchCmdBuffArr, 2, uchReadBuffArr, 2);
    return GH3X2X_AFE_MAKEUP_WORD(uchReadBuffArr[0], uchReadBuffArr[1]);
}

/**
 * @fn    void GH3X2X_I2cReadRegs(GU16 usRegAddr, GU16 *pusRegValueArr, GU8 uchLen)
 *
 * @brief  Read multi register via i2c
 *
 * @attention   None
 *
 * @param[in]   usRegAddr           register addr
 * @param[in]   pusRegValueBuffer   pointer to register value buffer
 * @param[in]   uchLen              registers len
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_I2cReadRegs(GU16 usRegAddr, GU16 *pusRegValueArr, GU8 uchLen)
{
    GU8 uchBuffArr[2] = {0};

    uchBuffArr[0] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegAddr);
    uchBuffArr[1] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegAddr);
    hal_gh3x2x_afe_i2c_read(GH3X2X_AFE_I2C_DEVICE_ADDR, uchBuffArr, 2, \
                        (GU8 *)pusRegValueArr, uchLen * 2);
    GH3X2X_SwapLittleBigEndianData((GU8 *)pusRegValueArr, (GU16)uchLen * 2);
}




/**
 * @fn     static void GH3X2X_I2cReadFifo(GU8 *puchDataBuffer, GU16 usLen)
 *
 * @brief  Read fifo via i2c
 *
 * @attention   None
 *
 * @param[in]   puchDataBuffer      pointer to buffer for fifo rawdata
 * @param[in]   usLen               fifo bytes len
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AFE_I2cReadFifo(GU8 *puchDataBuffer, GU16 usLen)
{
    GU8 uchCmdBuffArr[2] = {0};

    uchCmdBuffArr[0] = GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(GH3X2X_AFE_FIFO_REG_ADDR);
    uchCmdBuffArr[1] = GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(GH3X2X_AFE_FIFO_REG_ADDR);
    hal_gh3x2x_afe_i2c_read(GH3X2X_AFE_I2C_DEVICE_ADDR, uchCmdBuffArr, 2, puchDataBuffer, usLen);
}

#endif

/**
 * @fn     void GH3X2X_WriteRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usValue)
 *
 * @brief  write register bit field via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[in]   uchLsb         lsb of bit field
 * @param[in]   uchMsb         msb of bit field
 * @param[in]   usRegValue     register data
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AFE_WriteRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usValue)
{
    GU16 usMakData = ((((GU16)0x0001) << (uchMsb - uchLsb + 1)) - 1) << uchLsb;
    GU16 usRegData = 0;
    usRegData = GH3X2X_AFE_READ_REG(usRegAddr);
    GH3X2X_VAL_CLEAR_BIT(usRegData, usMakData);
    GH3X2X_VAL_SET_BIT(usRegData, (usValue << uchLsb) & usMakData);
    GH3X2X_AFE_WRITE_REG(usRegAddr, usRegData);
}

/**
 * @fn     GU16 GH3X2X_ReadRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb)
 *
 * @brief  Read register bit field via i2c or spi
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[in]   uchLsb         lsb of bit field
 * @param[in]   uchMsb         msb of bit field
 * @param[out]  None
 *
 * @return  register bit field data
 */
GU16 GH3X2X_AFE_ReadRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb)
{
    GU16 usMakData = ((((GU16)0x0001) << (uchMsb - uchLsb + 1)) - 1) << uchLsb;
    GU16 usRegData = 0;
    usRegData = GH3X2X_AFE_READ_REG(usRegAddr);
    GH3X2X_AFE_VAL_GET_BIT(usRegData, usMakData);
    usRegData >>= uchLsb;
    return usRegData;
}


void GH3X2X_AFE_SetNeedWakeUpGh3x2xFlag(GU8 uchFlag)
{
    g_uchNeedWakeUpGh3x2xAfe = uchFlag;
}

GU8 GH3X2X_AFE_GetNeedWakeUpGh3x2xFlag(void)
{
    return g_uchNeedWakeUpGh3x2xAfe;
}



void GH3X2X_AFE_ExitLowPowerMode(void)
{
    GH3X2X_AFE_SEND_CMD(GH3X2X_AFE_CMD_WAKEUP);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_WAKE_UP_CMD_DELAY_X_US);
}

void GH3X2X_AFE_EnterLowPowerMode(void)
{
    GH3X2X_AFE_SEND_CMD(GH3X2X_AFE_CMD_SLEEP);
    GH3X2X_AFE_SetNeedWakeUpGh3x2xFlag(1);
}


void GH3X2X_AFE_ClearEvent(GU16 usEvent)
{
    GH3X2X_AFE_WRITE_REG(GH3X2X_AFE_REG_INT_EVENT_ADDR,usEvent);
}








/**
 * @fn     void GH3X2X_AFE_SendRegPackage(STGH3X2X_AFEReg* GH3X2X_AFE_reg_list,  GU16 reg_list_len)
 *
 * @brief  send register package (for initialize GH3X2X_AFE chip)
 *
 * @attention   
 *
 * @param[in]  GH3X2X_AFE_reg_list      reg list that will be written to GH3X2X_AFE
 * @param[in]  reg_list_len         reg count that contained in reg list
 * @param[out] None
 *
 * @return  None
 */
void GH3X2X_AFE_SendRegPackage(STGH3X2X_AFEReg* GH3X2X_AFE_reg_list,  GU16 reg_list_len)
{
    GU16 luwIndex  = 0;

    if ((GH3X2X_AFE_reg_list != NULL) && (reg_list_len > 0))
    {
        for (luwIndex = 0; luwIndex < reg_list_len; luwIndex++)
        {
            if (GH3X2X_AFE_REG_IS_VALID(GH3X2X_AFE_reg_list->usRegAddr))
            {
                GH3X2X_AFE_WRITE_REG(GH3X2X_AFE_reg_list->usRegAddr, GH3X2X_AFE_reg_list->usRegData); 
            }            
            GH3X2X_AFE_reg_list++;
        }
    }

    if(GH3X2X_AFE_PRO_TYPE == GH3X2X_AFE_PRO_BY_POLLING)
    {
        //int pin should not action when poling mode
        GH3X2X_AFE_WRITE_REG(GH3X2X_AFE_REG_INT_EVENT_EN_ADDR, 0); 
    }

}



/**
 * @fn     GS8 GH3X2X_HardReset(void)
 *
 * @brief  Gh3x2x softreset via i2c/spi, can read&write reg with gh3x2x after reset
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR    reset pin control function not register
 */
void GH3X2X_AFE_Reset(void)
{

    GH3X2X_AFE_ClearEvent(0xFFFF);
    #if(GH3X2X_AFE_RESET_TYPE == GH3X2X_AFE_RESET_HARDWARE)
    hal_gh3x2x_afe_reset_pin_ctrl(0);
    hal_gh3x2x_afe_DelayUs(GH3X2X_AFE_HARD_RESET_DELAY_X_US); 
    hal_gh3x2x_afe_reset_pin_ctrl(1); 
    #else
    GH3X2X_AFE_SEND_CMD(GH3X2X_AFE_CMD_RESET);
    #endif
    hal_gh3x2x_afe_DelayMs(10);
}



/**
 * @fn     GS8 GH3X2X_CommunicateConfirm(void)
 *
 * @brief  Communication operation interface confirm
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AFE_CommunicateConfirm(void)
{
    GU16 uchReadData;
    GU16 uchWriteData;
   
    if (GH3X2X_AFE_READ_REG(GH3X2X_AFE_REG_CHIP_READY_CODE_REG_ADDR) == 0xAA55)
    {
        uchReadData = GH3X2X_AFE_READ_REG(GH3X2X_AFE_REG_SLOT_TMR0_REG_ADDR);
        EXAMPLE_LOG("slot_time0 reg = %d\r\n", (int)uchReadData);
        uchWriteData = ~uchReadData;
        GH3X2X_AFE_WRITE_REG(GH3X2X_AFE_REG_SLOT_TMR0_REG_ADDR, uchWriteData);
        uchReadData = GH3X2X_AFE_READ_REG(GH3X2X_AFE_REG_SLOT_TMR0_REG_ADDR);
        EXAMPLE_LOG("slot_time0 reg = %d\r\n", (int)uchReadData);
        if (uchWriteData == uchReadData)
        {
            uchWriteData = ~uchReadData;
            GH3X2X_AFE_WRITE_REG(GH3X2X_AFE_REG_SLOT_TMR0_REG_ADDR, uchWriteData);
        }
        else
        {
            EXAMPLE_LOG("[%s]read fail or write fail!!! \r\n", __FUNCTION__);
            while(1);
        }
    }
    else
    {
        EXAMPLE_LOG("[%s]ready code reg is error (0x%04x)!!! \r\n", __FUNCTION__,GH3X2X_AFE_READ_REG(GH3X2X_AFE_REG_CHIP_READY_CODE_REG_ADDR));
        while(1);
    }

    EXAMPLE_LOG("[%s]Comunication confirm ok. \r\n", __FUNCTION__);
        
   
    

}







/*********************************************** init function*******************************************/

/**
 * @fn     void GH3X2X_AFE_Init(void)
 *
 * @brief  init GH3X2X_AFE chip
 *
 * @attention   
 *
 * @param[in]  None
 * @param[out] None
 *
 * @return  None
 */
GU16 gh3x2x_i2c_addr = GH3X2X_AFE_I2C_DEVICE_BASE;

void GH3X2X_AFE_Init(void)
{
    /* Step 1: init communicate interface. */
    #if(GH3X2X_AFE_COM_INTERFACE_TYPE == GH3X2X_AFE_COM_INTERFACE_SPI)
    hal_gh3x2x_afe_spi_init();
    #else
    hal_gh3x2x_afe_i2c_init();
    #endif

    /* Step 2: init interupt pin  */
    #if(GH3X2X_AFE_PRO_TYPE == GH3X2X_AFE_PRO_BY_INT)
    hal_gh3x2x_afe_int_init();
    #endif
    

    /* Step 3: Reset GH3X2X_AFE chip */
    #if(GH3X2X_AFE_RESET_TYPE == GH3X2X_AFE_RESET_HARDWARE)
    hal_gh3x2x_afe_reset_pin_init();
    #endif
    GH3X2X_AFE_Reset();

    GH3X2X_AFE_ExitLowPowerMode();
    
    /* Step 4: Check conmunication and chip id is ok or not */
    GH3X2X_AFE_CommunicateConfirm();
	GH3X2X_AFE_CheckChipId();
	GH3X2X_AFE_CheckEfuseCrc();


    /* Step 5: Init GH3X2X_AFE chip */
    GH3X2X_AFE_SendRegPackage(GH3X2X_AFE_reg_list, GH3X2X_AFE_reg_list_len);

    /* Step 6: agc module reset and set parm*/
//    GH3X2X_LedAgcReset();
//    GH3X2X_LedAgcPramWrite(GH3X2X_AFE_reg_list, GH3X2X_AFE_reg_list_len);

    /* Step 7: enter low power mode */
    GH3X2X_AFE_EnterLowPowerMode();
    EXAMPLE_LOG("afe code version: %s\r\n",GH3X2X_VERSION_STRING);

}

/*********************************************** sample control handle function*******************************************/

/**
 * @fn     void GH3X2X_AFE_StartSampling(GU8 SlotCfgEnable)
 *
 * @brief  start sample
 *
 * @attention   
 *
 * @param[in]  SlotCfgEnable slot enable bit  bit0 control slot0, bit1 control slot1, bit2 control slot2...
 * @param[out] None
 *
 * @return  None
 */
void GH3X2X_AFE_StartSampling(GU8 SlotCfgEnable)
{
    GH3X2X_AFE_ExitLowPowerMode();

    //agc init
//    GH3X2X_LedAgcInit();
    
    //write slot enable
    GH3X2X_AFE_WRITE_REG(GH3X2X_AFE_REG_SLOT_EN_ADDR, (GU16)SlotCfgEnable);
    //start sample
    GH3X2X_AFE_WriteRegBitField(GH3X2X_AFE_REG_START_ADDR,GH3X2X_AFE_REG_START_LSB,GH3X2X_AFE_REG_START_MSB,1);
     
    GH3X2X_AFE_EnterLowPowerMode();
}

/**
 * @fn     void GH3X2X_AFE_StopSampling(void)
 *
 * @brief  stop sample
 *
 * @attention   
 *
 * @param[in]  None
 * @param[out] None
 *
 * @return  None
 */
void GH3X2X_AFE_StopSampling(void)
{
    GH3X2X_AFE_ExitLowPowerMode();

    //stop sample
    GH3X2X_AFE_WriteRegBitField(GH3X2X_AFE_REG_START_ADDR,GH3X2X_AFE_REG_START_LSB,GH3X2X_AFE_REG_START_MSB,0);

    GH3X2X_AFE_EnterLowPowerMode();
}

/**
 * @fn     void GH3X2X_AFE_SwitchSlot(GU8 SlotCfgEnable)
 *
 * @brief  switch sample slot.
 *
 * @attention   this function can only be used during sampling, after call GH3X2X_AFE_StartSampling
 *
 * @param[in]  SlotCfgEnable slot enable bit  bit0 control slot0, bit1 control slot1, bit2 control slot2...
 * @param[out] None
 *
 * @return  None
 */
void GH3X2X_AFE_SwitchSlot(GU8 SlotCfgEnable)
{
    GH3X2X_AFE_ExitLowPowerMode();
            
    //write slot enable
    GH3X2X_AFE_WRITE_REG(GH3X2X_AFE_REG_SLOT_EN_ADDR, (GU16)SlotCfgEnable);
    
    GH3X2X_AFE_EnterLowPowerMode();
}

/********************************* interrupt handle:fifo read and analysis ***************************/
#define GH3X2X_AFE_READ_FIFO_BUF_LEN (800)
GU32 GH3X2X_AFE_ReadFifoBuffer[(GU16)GH3X2X_AFE_READ_FIFO_BUF_LEN];

/*
    FIFO rawdata decode
*/
//typedef struct
//{
//    GU32 uiAdcCode;                      //sampling rawdata of ADC
//    GU8  ubSlotNo;                       //slot number
//    GU8  ubAdcNo;                        //adc number
//    GU8  ubFlagLedAdjIsAgc_EcgRecover;   //adj flag of ppg data or fast recover flag of ecg data
//    GU8  ubFlagLedAdjAgcUp;            //adj up flag of ppg data
//}StFifoDataInformation;
 typedef struct
{
    GU32 uiAdcCode:24;                      //sampling rawdata of ADC
    GU32 Res:1;
    GU32 ubFlagLedAdjAgcUp:1;            //adj down flag of ppg data   0: down  1:up
    GU32 ubFlagLedAdjIsAgc_EcgRecover:1;   //adj flag of ppg data or fast recover flag of ecg data   
    GU32 ubAdcNo:2;                        //adc number
    GU32 ubSlotNo:3;                       //slot number   
}StFifoDataInformation;
/**
 * @fn     GU32 EndienRev32(GU32 luiTempData)
 *
 * @brief  reverse data endien
 *
 * @attention   
 *
 * @param[in]  luiTempData input data
 * @param[out] None
 *
 * @return  output data
 */
static GU32 EndienRev32(GU32 luiTempData)
{
    GU8 *lpubIn;
    GU8 *lpubOut;
    
    GU32 luiOutData;
    
    lpubIn = (GU8 *)(&luiTempData);
    lpubOut = (GU8 *)(&luiOutData);
    
    for(GU8 i = 0; i < 4; i ++)
    {
        lpubOut[i] = lpubIn[3 - i];
    }
    
    return luiOutData;
}

/**
 * @fn     void GH3X2X_AFE_FifoDataAnalysis(GU16 luwFifoDataCnt)
 *
 * @brief  analysis fifo data
 *
 * @attention   
 *
 * @param[in]  luwFifoDataCnt    the number of rawdata 
 * @param[out] None
 *
 * @return  None
 */
void GH3X2X_AFE_FifoDataAnalysis(GU16 luwFifoDataCnt)
{
    StFifoDataInformation* stTempFifoInfo;
    GU16 luwFifoCnt = 0;
    
    GU32 luiTempFifoData = 0;

    EXAMPLE_LOG("Received rawdata sample num = %d \r\n", (int)luwFifoDataCnt);
    
    for(luwFifoCnt = 0; luwFifoCnt < luwFifoDataCnt; luwFifoCnt++)
    {
        luiTempFifoData = EndienRev32(GH3X2X_AFE_ReadFifoBuffer[luwFifoCnt]);  //big endien -> little endien
        stTempFifoInfo = (StFifoDataInformation*)&luiTempFifoData;
//        stTempFifoInfo->uiAdcCode = ((luiTempFifoData >> 0) & 0x00FFFFFF);
//        stTempFifoInfo.ubSlotNo =  ((luiTempFifoData >> 29) & 0x00000007);
//        stTempFifoInfo.ubAdcNo = ((luiTempFifoData >> 27) & 0x00000003);
//        stTempFifoInfo.ubFlagLedAdjIsAgc_EcgRecover = ((luiTempFifoData >> 26) & 0x00000001);
//        stTempFifoInfo.ubFlagLedAdjAgcUp = ((luiTempFifoData >> 25) & 0x00000001);
        if(stTempFifoInfo->ubAdcNo < 2)
        EXAMPLE_LOG("Received rawdata:slot:%d,adc:%d,data:%d\r\n", stTempFifoInfo->ubSlotNo, stTempFifoInfo->ubAdcNo, \
                                                                    stTempFifoInfo->uiAdcCode);
        /******************************** add your fifo date process code******************************/
        //you can combine frame rawdata according to stTempFifoInfo
        //and then excute algorithm calculate
        
        
    }
}


/**
 * @fn     void GH3X2X_AFE_ChipPro(void)
 *
 * @brief  interrupt handle function
 *
 * @attention   
 *
 * @param[in]  None
 * @param[out] None
 *
 * @return  None
 */
void GH3X2X_AFE_ChipPro(void)
{
    GU16 luwGH3X2X_AFEFifoLen = 0;
        GU16 luwGotEvent = 0;


#if (GH3X2X_AFE_PRO_TYPE == GH3X2X_AFE_PRO_BY_INT)
    if(0 == g_uchGh3x2xAfeIntCallBackIsCalled)
    {
        EXAMPLE_LOG("InterruptProcess:invalid call, do not process.\r\n");
        return;
    }
    g_uchGh3x2xAfeIntCallBackIsCalled = 0;  
        
#endif

     if (GH3X2X_AFE_GetNeedWakeUpGh3x2xFlag())
    {
        GH3X2X_AFE_ExitLowPowerMode();
    }


    luwGH3X2X_AFEFifoLen = 0;

    //read event
    luwGotEvent = GH3X2X_AFE_READ_REG(GH3X2X_AFE_REG_INT_EVENT_ADDR);  
    EXAMPLE_LOG("Received event 0x%04X\r\n", luwGotEvent);
    //clear event
    GH3X2X_AFE_WRITE_REG(GH3X2X_AFE_REG_INT_EVENT_ADDR,luwGotEvent);  //clear event

    //read fifo use
    luwGH3X2X_AFEFifoLen = GH3X2X_AFE_READ_REG(GH3X2X_AFE_REG_FIFO_USE_ADDR);
    if(luwGH3X2X_AFEFifoLen > GH3X2X_AFE_READ_FIFO_BUF_LEN)
    {
        luwGH3X2X_AFEFifoLen = GH3X2X_AFE_READ_FIFO_BUF_LEN;
    }

    // read fifo data
    if(0 != luwGH3X2X_AFEFifoLen)
    {
        GH3X2X_AFE_READ_FIFO((GU8 *)GH3X2X_AFE_ReadFifoBuffer,luwGH3X2X_AFEFifoLen * 4);
    } 
    //ECG_LEAD OFF DET
    GU16 luwGH3X2X_AFEFifoByteLen = luwGH3X2X_AFEFifoLen * 4;
    luwGotEvent &= (~GH3X2X_AFE_EVENT_BIT(GH3X2X_AFE_EVENT_LEAD_OFF));
    #if USE_GH3X2X_AFE_ECG_LEAD_LIB
    GU8 lubRet = GH3X2X_LeadOffDetect((GU8 *)GH3X2X_AFE_ReadFifoBuffer, &luwGH3X2X_AFEFifoByteLen, 0);
     if(lubRet != 0)
    {
        luwGotEvent |= (GH3X2X_AFE_EVENT_BIT(GH3X2X_AFE_EVENT_LEAD_OFF));
    }
    
    EXAMPLE_LOG("before Rs luwGH3X2X_AFEFifoLen = %d\r\n",luwGH3X2X_AFEFifoLen);
    GH3X2X_AFE_EcgRsHandle((GU8 *)GH3X2X_AFE_ReadFifoBuffer, &luwGH3X2X_AFEFifoByteLen,0,500);
    luwGH3X2X_AFEFifoLen = luwGH3X2X_AFEFifoByteLen/4;
    EXAMPLE_LOG("after Rs luwGH3X2X_AFEFifoLen = %d\r\n",luwGH3X2X_AFEFifoLen);
    #endif
    
    
    
    
    
    if(0 != (luwGotEvent & GH3X2X_AFE_EVENT_BIT(GH3X2X_AFE_EVENT_WEAR_ON)))
    {
        EXAMPLE_LOG("Received wear on event\r\n");
    }

    if(0 != (luwGotEvent & GH3X2X_AFE_EVENT_BIT(GH3X2X_AFE_EVENT_WEAR_OFF)))
    {
        EXAMPLE_LOG("Received wear off event\r\n");
    }

    if(0 != (luwGotEvent & GH3X2X_AFE_EVENT_BIT(GH3X2X_AFE_EVENT_LEAD_ON)))
    {
        EXAMPLE_LOG("Received lead on event\r\n");
        GH3X2X_AFE_Leadon_hook();
    }

    if(0 != (luwGotEvent & GH3X2X_AFE_EVENT_BIT(GH3X2X_AFE_EVENT_LEAD_OFF)))
    {
        EXAMPLE_LOG("Received lead off event\r\n");
        GH3X2X_AFE_Leadoff_hook();

    }

    if(0 != (luwGotEvent & GH3X2X_AFE_EVENT_BIT(GH3X2X_AFE_EVENT_LED_TUNE_FAIL)))
    {
        EXAMPLE_LOG("Received tune fail event\r\n");
    }

    if(0 != (luwGotEvent & GH3X2X_AFE_EVENT_BIT(GH3X2X_AFE_EVENT_LED_TUNE_DONE)))
    {
        EXAMPLE_LOG("Received tune done event\r\n");
    }


    //you can add some code to adjust ppg sample parameter, such as led drv current    GU16 usTempDrvCurrent;
    
    //GH3x2xSetPpgCtrlReg(GH3X2X_AFE_PPG_CTRL_REG_AGC_EN,1,0);
    //GH3x2xSetPpgCtrlReg(GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_DRV0,1,60);
    //EXAMPLE_LOG("change slot0 drv0 current.\r\n");
    //EXAMPLE_LOG("Get slot0 drv0 current = %d.\r\n",(int)GH3x2xGetPpgCtrlReg(GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_DRV0,0));


    
    if(0 != (luwGotEvent & (GH3X2X_AFE_EVENT_LED_TUNE_FAIL | GH3X2X_AFE_EVENT_LED_TUNE_DONE)))
    {
//        GH3X2X_SoftLedADJAutoADJInt();
    }
//    GH3X2X_LedAgcProcess((GU8 *)GH3X2X_AFE_ReadFifoBuffer, luwGH3X2X_AFEFifoLen * 4);
    

    
    GH3X2X_AFE_EnterLowPowerMode();
    

    if(0 != luwGH3X2X_AFEFifoLen)
    {
        GH3X2X_AFE_FifoDataAnalysis(luwGH3X2X_AFEFifoLen);
    }                
}


void GH3X2X_AFE_SetSlotLedDrvCurrent(GU8 uchSlotCfg, GU8 uchDrvNum, GU8 uchCurrent_mA)
{
    GS8 pchLedDrvEcode;
    GS8 pchTiaEcode;
    GS8 pchEcgEcode;
    GU16 uchCurrent_Code;
    
    GH3X2X_AFE_ExitLowPowerMode();
    
    GH3X2X_AFE_GetEfuseCode(&pchLedDrvEcode, &pchTiaEcode, &pchEcgEcode);
    uchCurrent_Code = GH3X2X_AFE_SetLedDrvCurrent(uchCurrent_mA, uchDrvNum, pchLedDrvEcode);
    
    GH3x2xSetPpgCtrlReg(uchDrvNum == 0?GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_DRV0:GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_DRV1, uchSlotCfg, uchCurrent_Code);
    GH3X2X_AFE_EnterLowPowerMode();
    
}

GU8 GH3X2X_AFE_GetSlotLedDrvCurrent(GU8 uchSlotCfg, GU8 uchDrvNum)
{
    GS8 pchLedDrvEcode;
    GS8 pchTiaEcode;
    GS8 pchEcgEcode;
    GU16 uchCurrent_Code;
    GU8 uchCurrent_mA;
    
    GH3X2X_AFE_ExitLowPowerMode();
    
    GH3X2X_AFE_GetEfuseCode(&pchLedDrvEcode, &pchTiaEcode, &pchEcgEcode);
    
    uchCurrent_Code = GH3x2xGetPpgCtrlReg(uchDrvNum == 0?GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_DRV0:GH3X2X_AFE_PPG_CTRL_REG_LED_CURRENT_DRV1, uchSlotCfg);
    uchCurrent_mA = GH3X2X_AFE_GetLedDrvCurrent(uchCurrent_Code, uchDrvNum, pchLedDrvEcode);
    GH3X2X_AFE_EnterLowPowerMode();
    return uchCurrent_mA;
}


const GU16 g_usGH3x2xAfeSlotRegBase[8] = 
{
    0x0100,
    0x011C,
    0x0138,
    0x0154,
    0x0170,
    0x018C,
    0x01A8,
    0x01C4,
};


const STGh3x2xPpgCtrlReg g_pstGH3x2xPpgCtrlReg[] = 
{
    {0x10, 9, 8},              //0: agc en
    {0x22, 7, 0},              //1: led current up limit
    {0x22, 15, 8},               //2: led current down limit
    {0x1E, 7, 0},               //3: led current drv0
    {0x20, 7, 0},               //4: led current drv1
    {0x12, 3, 0},               //5: rx0 tia gain
    {0x12, 7, 4},               //6: rx1 tia gain
    {0x12, 11, 8},               //7: rx2 tia gain
    {0x12, 15, 12},               //8: rx3 tia gain
    {0x10, 7,6},               //9: agc src
    {0x1A, 1,0},               //10: rx0 bg cancel
    {0x1A, 3,2},               //11: rx1 bg cancel    
    {0x1A, 5,4},               //12: rx2 bg cancel
    {0x1A, 7,6},               //13: rx2 bg cancel
    {0x0A, 15,8},               //14: sr mulipliper
    {0x0A, 5,1},               //15: ecg en& rx0~rx1 en
};

void GH3x2xSetPpgCtrlReg(GU8 uchPpgCtrlReg,  GU8 uchSlot, GU16 usValue)
{
    GH3X2X_AFE_WriteRegBitField(g_usGH3x2xAfeSlotRegBase[uchSlot] + g_pstGH3x2xPpgCtrlReg[uchPpgCtrlReg].uchAddrOffset, g_pstGH3x2xPpgCtrlReg[uchPpgCtrlReg].uchLsb, g_pstGH3x2xPpgCtrlReg[uchPpgCtrlReg].uchMsb, usValue);
}

GU16 GH3x2xGetPpgCtrlReg(GU8 uchPpgCtrlReg,  GU8 uchSlot)
{
    return GH3X2X_AFE_ReadRegBitField(g_usGH3x2xAfeSlotRegBase[uchSlot] + g_pstGH3x2xPpgCtrlReg[uchPpgCtrlReg].uchAddrOffset, g_pstGH3x2xPpgCtrlReg[uchPpgCtrlReg].uchLsb, g_pstGH3x2xPpgCtrlReg[uchPpgCtrlReg].uchMsb);
}

/**
 * @fn  void GH3X2X_AFE_GetEfuseCode(GS8* pchLedDrvEcode,GS8* pchTiaEcode,GS8* pchEcgEcode)
 *
 * @brief: Get Efuse Code
 *
 * @attention   
 *
 * @param[in]    
 * @param[out]  pchLedDrvEcode
 *              pchTiaEcode
 *              pchEcgEcode
 * @return  :  none
 */
void GH3X2X_AFE_GetEfuseCode(GS8* pchLedDrvEcode,GS8* pchTiaEcode,GS8* pchEcgEcode)
{
    GU16 usRegVal;
    GH3X2X_AFE_WRITE_REG(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,0x0020);
    GH3X2X_AFE_WRITE_REG(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,0x0030);

    while(0 == GH3X2X_AFE_ReadRegBitField(GH3X2X_EFUSE_CTRL_STATUS_REG_ADDR,0,0));
    usRegVal = GH3X2X_AFE_READ_REG(GH3X2X_EFUSE_CTRL_RDATA_1_REG_ADDR);
    *pchLedDrvEcode = (GS8)GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegVal);
    *pchTiaEcode    = (GS8)GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegVal);
    *pchEcgEcode    = (GS8)GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(GH3X2X_AFE_READ_REG(GH3X2X_EFUSE_CTRL_RDATA_2_REG_ADDR));
}


/**
 * @fn  GU16 GH3X2X_AFE_SetLedDrvCurrent(GU8 uchCurrent_mA,GU8 uchDrvNum, GS8 chECODE)
 *
 * @brief: led drv calibration
 *
 * @attention   
 *
 * @param[in]   GU8 uchCurrent_mA:led drv current, unit: mA
                GU8 uchDrvNum    :led Drv num,0 or 1
                GS8 chECODE      :Led Drv Ecode from efuse
 * @param[out] None
 *
 * @return  :  Drv code
 */
GU16 GH3X2X_AFE_SetLedDrvCurrent(GU8 uchCurrent_mA, GU8 uchDrvNum, GS8 chECODE)
{
    GF32 fEr;
    GF32 fY;
    GU8  uchN;
    GU16 usDrvCode;
    GU16 usDrvScale;
    
    if(uchDrvNum == 0)
    {
        fY = 0.1;
        usDrvScale = GH3X2X_AFE_ReadRegBitField(GH3X2X_LED_DRV_AD_REG_REG_ADDR,GH3X2X_DRV0_FULL_SCAL_CURRENT_LSB,GH3X2X_DRV0_FULL_SCAL_CURRENT_MSB);
    }
    else
    {
        fY = 0.01;
        usDrvScale = GH3X2X_AFE_ReadRegBitField(GH3X2X_LED_DRV_AD_REG_REG_ADDR,GH3X2X_DRV1_FULL_SCAL_CURRENT_LSB,GH3X2X_DRV1_FULL_SCAL_CURRENT_MSB);
        
    }
    uchN = 1 << (3 - usDrvScale);    
    
    fEr = (GF32)((GF32)chECODE/256) + 1.0f;
    usDrvCode = (GU16)(0.5f+((GF32)fEr*255*(GF32)uchN)/(0.225f/((GF32)uchCurrent_mA/1000) - fY));
    
    return usDrvCode;
}


/**
 * @fn  GF32 GH3X2X_AFE_GetLedDrvCurrent(GU16 usDrvCode, GU8 uchDrvNum, GS8 chECODE)
 *
 * @brief: get led drv calibration current
 *
 * @attention   
 *
 * @param[in]  GU16 usDrvCode   :led drv code
                GU8 uchDrvNum    :led Drv num,0 or 1
                GS8 chECODE      :Led Drv Ecode from efuse
 * @param[out] None
 *
 * @return  :  calibration current
 */

GF32 GH3X2X_AFE_GetLedDrvCurrent(GU16 usDrvCode, GU8 uchDrvNum, GS8 chECODE)
{
    GF32 fEr;
    GF32 fY;
    GU8  uchN;
    GF32 fDrvCurrent;
    GU16 usDrvScale;
       
    if(uchDrvNum == 0)
    {
        fY = 0.1;
        usDrvScale = GH3X2X_AFE_ReadRegBitField(GH3X2X_LED_DRV_AD_REG_REG_ADDR,GH3X2X_DRV0_FULL_SCAL_CURRENT_LSB,GH3X2X_DRV0_FULL_SCAL_CURRENT_MSB);
    }
    else
    {
        fY = 0.01;
        usDrvScale = GH3X2X_AFE_ReadRegBitField(GH3X2X_LED_DRV_AD_REG_REG_ADDR,GH3X2X_DRV1_FULL_SCAL_CURRENT_LSB,GH3X2X_DRV1_FULL_SCAL_CURRENT_MSB);
    }
    uchN = 1 << (3 - usDrvScale);    
    fEr = (GF32)((GF32)chECODE/256) + 1;
    fDrvCurrent = 0.225f/(fEr*255*(GF32)uchN/(GF32)usDrvCode + fY);
    return fDrvCurrent*1000;

}

/**
 * @fn   GF32 GH3X2X_AFE_GetTiaCalCode(GS8 chECODE)
 
 *
 * @brief:  Get Tia Calibration Code
 *
 * @attention   
 *
 * @param[in]  GS8 chECODE      :Tia Ecode from efuse
 * @param[out] None
 *
 * @return  :  calibration Code
 */
GF32 GH3X2X_AFE_GetTiaCalCode(GS8 chECODE)
{
    GF32 fEr;
    fEr = (GF32)((GF32)chECODE/512) + 1;
    return fEr;
}

/**
 * @fn   GF32 GH3X2X_AFE_GetEcgCalCode(GS8 chECODE)
 
 *
 * @brief:  Get ECG Calibration Code
 *
 * @attention   
 *
 * @param[in]  GS8 chECODE      :ECG Ecode from efuse
 * @param[out] None
 *
 * @return  :  calibration Code
 */
GF32 GH3X2X_AFE_GetEcgCalCode(GS8 chECODE)
{
    GF32 fEr;
    fEr = (GF32)((GF32)chECODE/1024) + 1;
    return fEr;
}


/**
 * @fn     GCHAR *GH3X2X_GetAfeLibVersion(void)
 *
 * @brief  Get afe driver version
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  library version string
 */
GCHAR *GH3X2X_GetAfeLibVersion(void)
{
    return (GCHAR *)GH3X2X_VERSION_STRING;
}


GU8 GH3X2X_AFE_GetChipId(void)
{
	return (GU8)GH3X2X_AFE_ReadRegBitField(GH3X2X_WHO_AM_I_REG_ADDR,GH3X2X_WHO_AM_I_LSB,GH3X2X_WHO_AM_I_MSB);
}

GS8 GH3X2X_AFE_CheckChipId(void)
{
    GS8 chRet = GH3X2X_AFE_RET_OK;
	if(GH3020_ID != GH3X2X_AFE_GetChipId())
	{
		EXAMPLE_LOG("wrong chip id!!\r\n");
		chRet = GH3X2X_AFE_RET_GENERIC_ERROR;
	}
    return chRet;
}

GU8 _crc8(GU8 *ptr,GS32 len)
{
  GS32 i;
  GU8 crc = 0x00;
  while(len--)
  {
    crc ^= *ptr++;
    for(i=8;i>0;--i)
    {
      if(crc&0x80)
      {
        crc = (crc<<1)^0x31;
      }
      else
      {
        crc = (crc<<1);
      }
      crc = crc & 0xff;
    }
  }
  return (GU8)(crc);
}

void GH3X2X_AFE_LoadEfuse(GU8* puchEfuseBuffer,GU16 EfuseByteLen)
{
	if(EfuseByteLen < 24)
	{
		EXAMPLE_LOG("EfuseByteLen is too short!!\r\n");
		return;
	}
	
    GU16 usRegVal;
	GH3X2X_AFE_WriteRegBitField(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,GH3X2X_EFUSE_MODE_LSB,GH3X2X_EFUSE_MODE_MSB,0);
	GH3X2X_AFE_WriteRegBitField(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,GH3X2X_EFUSE_CHOOSE_LSB,GH3X2X_EFUSE_CHOOSE_MSB,0);
	GH3X2X_AFE_WriteRegBitField(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,GH3X2X_EFUSE_CLK_EN_LSB,GH3X2X_EFUSE_CLK_EN_MSB,1);
	GH3X2X_AFE_WriteRegBitField(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,GH3X2X_EFUSE_START_LSB,GH3X2X_EFUSE_START_MSB,1);
	while(0 == GH3X2X_AFE_ReadRegBitField(GH3X2X_EFUSE_CTRL_STATUS_REG_ADDR,0,0));
	for(GU8 i = 0; i < 4; i++)
	{
	   usRegVal = GH3X2X_AFE_READ_REG(GH3X2X_EFUSE_CTRL_RDATA_0_REG_ADDR + 2 * i);
	   puchEfuseBuffer[2*i] 	= (GU8)GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegVal);
	   puchEfuseBuffer[2*i + 1] = (GU8)GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegVal);
	}

	GH3X2X_AFE_WriteRegBitField(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,GH3X2X_EFUSE_CHOOSE_LSB,GH3X2X_EFUSE_CHOOSE_MSB,1);
	GH3X2X_AFE_WriteRegBitField(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,GH3X2X_EFUSE_START_LSB,GH3X2X_EFUSE_START_MSB,1);
	while(0 == GH3X2X_AFE_ReadRegBitField(GH3X2X_EFUSE_CTRL_STATUS_REG_ADDR,0,0));
	for(GU8 i = 0; i < 4; i++)
	{
	   usRegVal = GH3X2X_AFE_READ_REG(GH3X2X_EFUSE_CTRL_RDATA_0_REG_ADDR + 2 * i);
	   puchEfuseBuffer[8 + 2*i] 	= (GU8)GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegVal);
	   puchEfuseBuffer[8 + 2*i + 1] = (GU8)GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegVal);
	}

	GH3X2X_AFE_WriteRegBitField(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,GH3X2X_EFUSE_CHOOSE_LSB,GH3X2X_EFUSE_CHOOSE_MSB,2);
	GH3X2X_AFE_WriteRegBitField(GH3X2X_EFUSE_CTRL_CMD_REG_ADDR,GH3X2X_EFUSE_START_LSB,GH3X2X_EFUSE_START_MSB,1);
	while(0 == GH3X2X_AFE_ReadRegBitField(GH3X2X_EFUSE_CTRL_STATUS_REG_ADDR,0,0));
	for(GU8 i = 0; i < 4; i++)
	{
	   usRegVal = GH3X2X_AFE_READ_REG(GH3X2X_EFUSE_CTRL_RDATA_0_REG_ADDR + 2 * i);
	   puchEfuseBuffer[16 + 2*i]	 = (GU8)GH3X2X_AFE_GET_LOW_BYTE_FROM_WORD(usRegVal);
	   puchEfuseBuffer[16 + 2*i + 1] = (GU8)GH3X2X_AFE_GET_HIGH_BYTE_FROM_WORD(usRegVal);
	}

	
}

GS8 GH3X2X_AFE_CheckEfuseCrc(void)
{
    GS8 chRet = GH3X2X_AFE_RET_OK;
	GU8  uchEfuseBuffArr[24];
    GH3X2X_AFE_LoadEfuse(uchEfuseBuffArr,24);
	GU8 uchCrcArr[3];
	uchCrcArr[0] = _crc8(uchEfuseBuffArr,7);
	uchCrcArr[1] = _crc8((uchEfuseBuffArr + 8),3);
	uchCrcArr[2] = _crc8((uchEfuseBuffArr + 12),10);
	if(uchCrcArr[0] != uchEfuseBuffArr[7] || uchCrcArr[1] != uchEfuseBuffArr[11] ||uchCrcArr[2] != uchEfuseBuffArr[22])
	{
		EXAMPLE_LOG("Efuse crc error!!\r\n");
		chRet = GH3X2X_AFE_RET_GENERIC_ERROR;
	}
    return chRet;
}

 
//__weak void GH3X2X_SoftLedADJAutoADJInt(void){}
//__weak void GH3X2X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen){}
//__weak void GH3X2X_LedAgcReset(void){}
//__weak void GH3X2X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi){}
//__weak void GH3X2X_LedAgcInit(void){}





void GH3x2xSetAgcReg(GU8 uchAgcReg,  GU8 uchSlot, GU16 usValue)
{
    GH3x2xSetPpgCtrlReg(uchAgcReg, uchSlot, usValue);
}

GU16 GH3x2xGetAgcReg(GU8 uchAgcReg,  GU8 uchSlot)
{
    return GH3x2xGetPpgCtrlReg(uchAgcReg, uchSlot);
}



void *GH3X2X_Memset(void* pDest, GCHAR chVal, GU32 unByteSize)
{
    return memset(pDest, chVal, unByteSize);
}


void *GH3X2X_Memcpy(void *pDest, const void *pSrc, GU32 unByteSize)
{
    return memcpy(pDest, pSrc, unByteSize);
}

























