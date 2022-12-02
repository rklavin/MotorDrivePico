#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//#include "FatFs/ff.h"

#include "port.h"
#include "config.h"
#include "storage/storage.h"

int main(void) {
	init_port();

	for (int i = 0; i < 15; i += 1) {
		printf("%d\n", 15 - i);
		port_sleep_ms(1000);
	}

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    //gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    //gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

	// Initialize filesystem
	init_storage();

	// Test
	UINT bw;
	FRESULT fr;
	FIL Fil;			// File object needed for each open file
	/*fr = storage_f_open(&Fil, "0:newfile.txt", FA_WRITE | FA_CREATE_ALWAYS);	// Create a file
	if (fr == FR_OK) {
		storage_f_write(&Fil, "It works!\r\n", 11, &bw);	// Write data to the file
		fr = storage_f_close(&Fil);							// Close the file
	} else {
		printf("File write result: ");
		printf("%d\n", fr);
	}*/

	//sleep_ms(15000);

    char line[100]; // Line buffer
	int bytes_read;
	fr = storage_f_open(&Fil, "0:newfile.txt", FA_READ);	// Open a file
	if (fr == FR_OK) {
		//f_write(&Fil, "It works!\r\n", 11, &bw);	// Write data to the file
        //fr = storage_f_read(&Fil, line, sizeof line, &bytes_read); // Read a chunk of data from the source file
		storage_f_gets(line, sizeof line, &Fil);
		printf("%s", line);
		printf("\n");
		fr = storage_f_close(&Fil);							// Close the file
	} else {
		printf("File read result: ");
		printf("%d\n", fr);
	}

	while (true) {
		printf("Hello World!\n");
		port_sleep_ms(1000);
	}

	return 0;
}
