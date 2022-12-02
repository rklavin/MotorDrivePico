#include "storage.h"

#include <stdlib.h>
#include <stdio.h>

FATFS FatFs[FF_VOLUMES]; // Work area (filesystem object) for logical drives
BYTE work[FF_MAX_SS]; // Working buffer

void init_storage() {
	// Initialize a brand-new disk drive mapped to physical drive 0
	LBA_t plist[] = {EXE_PARTITION_SIZE, USER_PARTITION_SIZE}; // Divide the drive into two partitions
	FRESULT mount_res;
	FRESULT mount_res2;
	FRESULT file_res;

	// Try mounting volumes
	mount_res = f_mount(FatFs, "0:", 0);
	#if (USER_STORAGE_SIZE > 0)
	mount_res2 = f_mount(FatFs + 1, "1:", 0);
	#endif

	if ((mount_res == FR_NO_FILESYSTEM) || (mount_res2 == FR_NO_FILESYSTEM)) {
		printf("No filesystem found on disk, attempting to create...\n");

		// Divide physical drive 0
		file_res = f_fdisk(0, plist, work);

		// Create FAT volume on the logical drive 0
		file_res = f_mkfs("0:", 0, work, sizeof work);

		#if (USER_STORAGE_SIZE > 0)
		// Create FAT volume on the logical drive 1
		file_res = f_mkfs("1:", 0, work, sizeof work);
		#endif

		// Mount volumes
		mount_res = f_mount(FatFs, "0:", 0);
		#if (USER_STORAGE_SIZE > 0)
		mount_res2 = f_mount(FatFs + 1, "1:", 0);
		#endif
	} else {
		printf("Mount Result 1: ");
		printf("%d\n", mount_res);
		printf("Mount Result 2: ");
		printf("%d\n", mount_res2);
		printf("Existing filesystems found on disk\n");
	}
}

// Disk Operations
FRESULT storage_f_open (FIL* fp, const TCHAR* path, BYTE mode) {
	return f_open(fp, path, mode);
}

FRESULT storage_f_close (FIL* fp) {
	return f_close(fp);
}

FRESULT storage_f_read (FIL* fp, void* buff, UINT btr, UINT* br) {
	return f_read(fp, buff, btr, br);
}

FRESULT storage_f_write (FIL* fp, const void* buff, UINT btw, UINT* bw) {
	return f_write(fp, buff, btw, bw);
}

FRESULT storage_f_mkdir (const TCHAR* path) {
	return f_mkdir(path);
}

FRESULT storage_f_unlink (const TCHAR* path) {
	return f_unlink(path);
}

FRESULT storage_f_rename (const TCHAR* path_old, const TCHAR* path_new) {
	return f_rename(path_old, path_new);
}

TCHAR* storage_f_gets (TCHAR* buff, int len, FIL* fp) {
	return f_gets(buff, len, fp);
}

FRESULT storage_f_stat (const TCHAR* path, FILINFO* fno) {
	return f_stat(path, fno);
}