/*=========================================================================

  PicoLink

  port.h

  This file translates various board specific functions

=========================================================================*/

#pragma once

// Board specific initializations
void init_port();

// Sleep for a duration of milliseconds
void port_sleep_ms(int msec);

// Get character input
int port_getchar_timeout_ms(int msec);