/*=========================================================================

  PicoLink

  config.h

  This file defines various board specific parameters

=========================================================================*/

#pragma once

// The size of storage to be set aside for the C firmware in bytes
// This needs to be at least enough to store the uf2 file, recommended at least 1 MB
#define C_STORAGE_SIZE (2 * 1024 * 1024)

// The size of storage to be set aside for user installed scripts in bytes
// This storage space address will start immediately following the C Storage space
// This can be any size above 0 kB, recommended at least 1 MB
#define EXE_STORAGE_SIZE (4 * 1024 * 1024)

// The size of storage to be set aside for use in user installed scripts in bytes
// This storage space address will start immediately following the EXE Storage space
// This is optional, can be set to 0 kB
#define USER_STORAGE_SIZE (2 * 1024 * 1024)

// The size of a sector in the storage medium in bytes
// Allowable sizes are 512, 1024, 2048 or 4096 bytes
#define STORAGE_SECTOR_SIZE 4096


//#define UART_ID uart0
//#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
//#define UART_TX_PIN 0
//#define UART_RX_PIN 1