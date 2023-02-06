/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "lib/FatFs/ff.h"			/* Obtains integer types */
#include "lib/FatFs/diskio.h"		/* Declarations of disk functions */

#include <string.h>

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "config.h"

#ifndef PICO_FLASH_SIZE_BYTES
#warning PICO_FLASH_SIZE_BYTES not set
#endif

/* Definitions of physical drive number for each drive */
#define DEV_ROM_1	0	// Drive 1 in ROM used to store executing script
#define DEV_ROM_2	1	// Drive 2 in ROM for use in executing script

DSTATUS ROM_1_Status;
DSTATUS ROM_2_Status;

#define ROM_1_SIZE (256 * 1024)
#define ROM_2_SIZE (256 * 1024)

#define XIP_EXE_STORAGE_BASE XIP_BASE + C_STORAGE_SIZE
#define XIP_USER_STORAGE_BASE XIP_BASE + C_STORAGE_SIZE + EXE_STORAGE_SIZE
#define EXE_STORAGE_BASE C_STORAGE_SIZE
#define USER_STORAGE_BASE C_STORAGE_SIZE + EXE_STORAGE_SIZE

extern char __flash_binary_end;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = 0;

    switch (pdrv) {
    case DEV_ROM_1:
        return ROM_1_Status;

    /*case DEV_ROM_2:
        return ROM_2_Status;*/
    }

    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    //result = PICO_FLASH_SIZE_BYTES;
    //result = XIP_NOCACHE_NOALLOC_BASE;
    //result = FLASH_SECTOR_SIZE;

    switch (pdrv) {
    case DEV_ROM_1:
        // First check if there is enough ROM space, all sizes totalled should be less than or equal to device flash

        // Clear init flag
        ROM_1_Status &= ~STA_NOINIT;

        // Check if actual C firmware size is less than that specified in the config
        if ((__flash_binary_end - XIP_BASE) <= C_STORAGE_SIZE) return ROM_1_Status |= STA_NOINIT;

        return ROM_1_Status;

    /*case DEV_ROM_2:
        // Clear init flag
        ROM_2_Status &= 0b110;

        // Check if actual C firmware size is less than that specified in the config
        if ((__flash_binary_end - XIP_BASE) <= C_STORAGE_SIZE) return ROM_2_Status |= STA_NOINIT;

        return ROM_2_Status;*/
    }

    // Return no init if any other drive is initialized
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,		/* Physical drive nmuber to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    LBA_t sector,	/* Start sector in LBA */
    UINT count		/* Number of sectors to read */
)
{
    size_t buffsize = count * STORAGE_SECTOR_SIZE;

    switch (pdrv) {
    case DEV_ROM_1:
        // Copy from flash to buffer
        memcpy(buff, (BYTE*)(XIP_EXE_STORAGE_BASE + (sector * STORAGE_SECTOR_SIZE)), buffsize);

        return RES_OK;

    /*case DEV_ROM_2:
        // Copy from flash to buffer
        memcpy(buff, (BYTE*)(XIP_USER_STORAGE_BASE + (sector * FLASH_SECTOR_SIZE)), buffsize);

        return RES_OK;*/
    }

    return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
    const BYTE *buff,	/* Data to be written */
    LBA_t sector,		/* Start sector in LBA */
    UINT count			/* Number of sectors to write */
)
{
    uint32_t interrupts;
    size_t buffsize = count * STORAGE_SECTOR_SIZE;

    switch (pdrv) {
    case DEV_ROM_1:
        // Copy from buffer to flash
        interrupts = save_and_disable_interrupts();
        flash_range_erase(EXE_STORAGE_BASE + (sector * STORAGE_SECTOR_SIZE), buffsize);
        flash_range_program(EXE_STORAGE_BASE + (sector * STORAGE_SECTOR_SIZE), buff, buffsize);
        restore_interrupts(interrupts);

        return RES_OK;

    /*case DEV_ROM_2:
        // Copy from buffer to flash
        interrupts = save_and_disable_interrupts();
        flash_range_erase(USER_STORAGE_BASE + (sector * FLASH_SECTOR_SIZE), buffsize);
        flash_range_program(USER_STORAGE_BASE + (sector * FLASH_SECTOR_SIZE), buff, buffsize);
        restore_interrupts(interrupts);

        return RES_OK;*/
    }

    return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,		/* Physical drive nmuber (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    LBA_t* sector_count = (LBA_t*)buff;
    WORD* sector_size = (WORD*)buff;

    switch (pdrv) {
    case DEV_ROM_1:
        switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT:
            *sector_count = (LBA_t)((PICO_FLASH_SIZE_BYTES - C_STORAGE_SIZE) / STORAGE_SECTOR_SIZE);
            return RES_OK;

        case GET_SECTOR_SIZE:
            *sector_size = (WORD)(STORAGE_SECTOR_SIZE);
            return RES_OK;

        case CTRL_TRIM:
            // TODO: implement later
            // Erase data from sectors in buff[0] to buff[1]
            return RES_OK;
        }

        return RES_PARERR;

    /*case DEV_ROM_2:
        switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT:
            *sector_count = (LBA_t)(PICO_FLASH_SIZE_BYTES / FLASH_SECTOR_SIZE);
            return RES_OK;

        case GET_SECTOR_SIZE:
            *sector_size = (WORD)(FLASH_SECTOR_SIZE);
            return RES_OK;

        case CTRL_TRIM:
            // TODO: implement later
            // Erase data from sectors in buff[0] to buff[1]
            return RES_OK;
        }

        return RES_PARERR;*/
    }

    return RES_PARERR;
}


DWORD get_fattime(void) {
    // If board has wifi get internet time?
    // Get time at startup for RTC, then use that, don't keep fetching the time
    return ((2022 - 1980) << 25) | ((12) << 21) | ((1) << 16) | ((12) << 11) | ((0) << 5) | (0 / 2);
}


#if (USER_STORAGE_SIZE > 0)
PARTITION VolToPart[FF_VOLUMES] = {
    {0, 1},     /* "0:" ==> 1st partition on the pd#0 */
    {0, 2}      /* "1:" ==> 2nd partition on the pd#0 */
};
#else
PARTITION VolToPart[FF_VOLUMES] = {
    {0, 1},     /* "0:" ==> 1st partition on the pd#0 */
    {0, 0}      /* "1:" ==> 2nd partition on the pd#0 */
};
#endif

