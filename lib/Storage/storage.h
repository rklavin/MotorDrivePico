#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

#include "FatFs/ff.h"
#include "MotorDrive/config.h"

#define EXE_PARTITION_SIZE (EXE_STORAGE_SIZE / STORAGE_SECTOR_SIZE)
#define USER_PARTITION_SIZE (USER_STORAGE_SIZE / STORAGE_SECTOR_SIZE)

#define EXE_FS  0
#define USER_FS 1

void init_storage ();
FRESULT storage_scan_files (char* path);

// Disk Operations
FRESULT storage_f_open (FIL* fp, const TCHAR* path, BYTE mode);				/* Open or create a file */
FRESULT storage_f_close (FIL* fp);											/* Close an open file object */
FRESULT storage_f_read (FIL* fp, void* buff, UINT btr, UINT* br);			/* Read data from the file */
FRESULT storage_f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* Write data to the file */
FRESULT storage_f_mkdir (const TCHAR* path);								/* Create a sub directory */
FRESULT storage_f_unlink (const TCHAR* path);								/* Delete an existing file or directory */
FRESULT storage_f_rename (const TCHAR* path_old, const TCHAR* path_new);	/* Rename/Move a file or directory */
int storage_f_puts (const TCHAR* str, FIL* fp);								/* Put a string to the file */
TCHAR* storage_f_gets (TCHAR* buff, int len, FIL* fp);						/* Get a string from the file */
FRESULT storage_f_stat (const TCHAR* path, FILINFO* fno);					/* Get file status */

int print_file(char* fileName);

#ifdef __cplusplus
}
#endif