#include "ff.h"
#include "diskio.h"
#include "ff_gen_drv.h"
#include "user_qspi_flash.h"

#define DEV_RAM     0   /* Give Ramdisk to physical drive 0 */
#define DEV_MMC     1   /* Give MMC/SD card to physical drive 1 */
#define DEV_USB     2   /* Give USB MSD to physical drive 2 */
/* Private variables ---------------------------------------------------------*/
extern disk_drv_t  g_disk;
#define QSPI_FLASH 0

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  I/O control access
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Command code
  * @param  *buff: Data buffer pointer
  * @retval DRESULT: Operation result
  */
#if FF_USE_DISK_IOCTL == 1
DRESULT disk_ioctl (
    BYTE pdrv,
    BYTE cmd,
    void *buff
)
{
  DRESULT res;
  switch(cmd)
  {
      case GET_SECTOR_COUNT:
          *(DWORD *)buff = 4096;
          break;
      case GET_SECTOR_SIZE:
          *(WORD *)buff = 4096;
          break;
      case GET_BLOCK_SIZE:
          *(DWORD *)buff = 1;
          break;
  }
  res = RES_OK;
  return res;
}
#endif /* _USE_IOCTL == 1 */

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buff pointer
  * @param  sector: Sector address(LBA)
  * @param  count: Number of sectors(1..128)
  * @retval DRESULT: Operation result
  */
#if FF_USE_DISK_WRITE == 1
DRESULT disk_write (
    BYTE pdrv,
    const BYTE *buff,
    DWORD sector,
    UINT count
)
{
  DRESULT res;
  uint32_t write_addr = 0;
 

	if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误
	if(pdrv == QSPI_FLASH)
	{	
		for(;count>0;count--)
		{
			write_addr = sector<<12;
			//SPI_FLASH_Sector_Erase(write_addr);
			for(uint32_t i = 0; i < 16;i++)
			{
				//SPI_FLASH_program(write_addr+(SPI_FLASH_PAGE_SIZE*i),(uint8_t *)(buff+SPI_FLASH_PAGE_SIZE*i),SPI_FLASH_PAGE_SIZE);
				
			}
			sector++;
			buff+=SPI_FLASH_SECTOR_SIZE;
		}
		res= RES_OK;
	}
	else
	{
		printf("!!!disk_write ERR\r\n");
		res= RES_ERROR;
	}
   return res;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer pointer
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors(1..128)
  * @retval DRESULT: Operation result
  */
DRESULT disk_read (
    BYTE pdrv,
    BYTE *buff,
    DWORD sector,
    UINT count
)
{
  DRESULT res;
  if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误
	if(pdrv == QSPI_FLASH)
	{
		for(;count>0;count--)
		{
			//SPI_FLASH_Read(sector*SPI_FLASH_SECTOR_SIZE,buff,SPI_FLASH_SECTOR_SIZE);
			sector++;
			buff+=SPI_FLASH_SECTOR_SIZE;
		}	
		 res = RES_OK;
	}
	else
	{
		printf("!!!disk_read ERR\r\n");
		 res = RES_ERROR;
	}	

  return res;
}

/**
  * @brief  Disk initializes
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_initialize (
    BYTE pdrv
)
{
  DSTATUS stat = RES_OK;
  if(pdrv == QSPI_FLASH)
	{
		//SPI_FLASH_init();
		stat = RES_OK;	
	}
	else
	{
		printf("!!!disk_initialize ERR\r\n");
		stat = RES_PARERR;
	}	
    
  return stat;
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_status (
    BYTE pdrv
)
{
  DSTATUS stat;
	if(QSPI_FLASH == pdrv)
	{
		stat = RES_OK;
		
	}
	else
	{
		printf("extern flash err\r\n");
		 stat = RES_PARERR;
	}



  return stat;
}
