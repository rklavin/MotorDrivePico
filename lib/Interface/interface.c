#include "interface.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/uart.h"

void init_board() {
	// Initialize RP2040
	stdio_init_all();
	//uart_init(UART_ID, BAUD_RATE);
}

void interface_sleep_ms(int msec) {
    sleep_ms(msec);
}

int interface_getchar_timeout_ms(int msec) {
	int c;
	int loops = 0;
    
	while ((c = getchar_timeout_us(0)) < 0 && loops < msec) {
        sleep_us(1000);
        loops++;
	}

	return c;
}

// Read line until delimiter or buffer size limit
int interface_getline_timeout_ms(int msec, char del, char* buff, int buffSize) {
	if (buffSize < 1) return 0;

	char c;
	int loops = 0;
	int i = 0;
	int res = 0;
	bool endSpace = false;

	while (i < buffSize) {
		c = (char)getchar_timeout_us(0);

		if (c == del) {
			res = 1;
			break;
		}

		// Ignore leading spaces
		if (c != ' ') endSpace = true;
		
		if (endSpace) {
			buff[i] = c;
			i += 1;
		}

        loops++;
		if (loops >= msec) {
			res = 0;
			break;
		}

        sleep_us(1000);
	}

	// if res == 1, add \0 for string null
	if (res == 1) {
		if (i >= buffSize) i -= 1;
		buff[i] = '\0';
	}

	return res;
}

int interface_putchar_raw(int c) {
    return putchar_raw(c);
}

// Return a 32-bit integer from a pointer, corrects for misalignment
int32_t interface_get_int32(void* ptr) {
	uint32_t res = 0;

    // Check if pointer is already 4-byte aligned
    if (((int32_t)ptr % 4) == 0) return *(int32_t*)ptr;

    // Check if pointer is 2-byte aligned
    if (((int32_t)ptr % 2) == 0) {
		uint16_t lower = *(uint16_t*)ptr;
		uint16_t upper = *(uint16_t*)(ptr + sizeof(uint16_t));
		res = lower + (upper << 16);
        return (int32_t)res;
    }

    // Otherwise pointer is on an odd byte
	res = *(uint8_t*)ptr;

    ptr += sizeof(uint8_t);
    res += (*(uint8_t*)ptr << 8);

    ptr += sizeof(uint8_t);
    res += (*(uint8_t*)ptr << 16);

    ptr += sizeof(uint8_t);
    res += (*(uint8_t*)ptr << 24);

    return (int32_t)res;
}

// Return a 16-bit integer from a pointer, corrects for misalignment
int16_t interface_get_int16(void* ptr) {
	uint32_t res = 0;

    // Check if pointer is already 2-byte aligned
    if (((int32_t)ptr % 2) == 0) return *(int16_t*)ptr;

    // Otherwise pointer is on an odd byte
	res = *(uint8_t*)ptr;

    ptr += sizeof(uint8_t);
    res += (*(uint8_t*)ptr << 8);

    return (int32_t)res;
}

// Convert string to integer
int str_to_int(const char* buff) {
    int i = 0;

    while(*buff >= '0' && *buff <= '9') {
        i = i * 10 + (*buff - '0');
        buff += 1;
    }

    return i;
}