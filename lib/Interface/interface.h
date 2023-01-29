/*=========================================================================

  MotorDrive

  port.h

  This file translates various board specific functions

=========================================================================*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdio.h"

// Board specific initializations
void init_board();

// Sleep for a duration of milliseconds
void interface_sleep_ms(int msec);

// Get character input
int interface_getchar_timeout_ms(int msec);

// Read line until delimiter or buffer size limit
int interface_getline_timeout_ms(int msec, char del, char* buff, int buffSize);

// Print a single character without newline or carriage return
int interface_putchar_raw(int c);

// Return a 32-bit integer from a pointer, corrects for misalignment
int32_t interface_get_int32(void* ptr);

// Return a 16-bit integer from a pointer, corrects for misalignment
int16_t interface_get_int16(void* ptr);

// Convert string to integer
int str_to_int(const char* buff);

#ifdef __cplusplus
}
#endif