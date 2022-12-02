#include "port.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/uart.h"

void init_port() {
	// Initialize RP2040
	stdio_init_all();
	//uart_init(UART_ID, BAUD_RATE);
}

void port_sleep_ms(int msec) {
    sleep_ms(msec);
}

int port_getchar_timeout_ms(int msec) {
	int c;
	int loops = 0;
    
	while ((c = getchar_timeout_us(0)) < 0 && loops < msec) {
        sleep_us(1000);
        loops++;
	}

	return c;
}