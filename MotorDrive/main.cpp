#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"

#include "config.h"
#include "Interface/interface.h"
#include "Storage/storage.h"

#include "Components/components.h"
#include "Components/DRV8876.h"
#include "Components/MAX11645.h"
#include "Components/MCP47A1.h"

#include "Console/console.h"
#include "Shell/shell.h"
#include "motor.h"

//void led_blinking_task(int blink);
void init_config();

int main(void) {
	// Main loop
	char cmd = 'z';
	char buff[256];

	UINT bw;
	FRESULT fr;
	FIL Fil;			// File object needed for each open file
	int err = 0;
	bool readAnalog = false;
	bool readScaled = false;

	init_board();

	for (int i = 0; i < 5; i += 1) {
		printf("%d\n", 5 - i);
		interface_sleep_ms(1000);
	}

	// Initialize GPIO
	printf("GPIO Init...\n");
	init_config();
	printf("GPIO Init Complete\n");

	printf("ADC Init...\n");
	adc_init();
	adc_gpio_init(PIN_ADC_IPROPI);
	printf("ADC Init Complete\n");

	printf("I2C Init...\n");
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(PIN_SDA0, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL0, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA0);
    gpio_pull_up(PIN_SCL0);
	printf("I2C Init Complete\n");

	/*for (int i = 0; i < 5; i += 1) {
		printf("%d\n", 5 - i);
		interface_sleep_ms(1000);
	}*/

	// Initialize filesystem
	printf("File System Init...\n");
	init_storage();
	printf("File System Complete\n");

	// Components
	components::DRV8876 MotorDriver("Motor Driver");
	components::MAX11645 ADC("Analog to Digital Converter");
	ADC.bus = i2c0;
	ADC.scale_M = 12.288;
	components::MCP47A1 DAC("Digital to Analog Converter");

	motor::Motor mtr;
	mtr.ADC = &ADC;
	mtr.DAC = &DAC;

	printf("Init Components...\n");
	err = ADC.init();
	err &= DAC.init();
	err &= mtr.init();
	if (err == 1)
		printf("Init Complete\n");

	int time = to_ms_since_boot(get_absolute_time());

	while (true) {
		double val;

		if (to_ms_since_boot(get_absolute_time()) - time > 100) {
			std::vector<std::string> command = console::split_command_and_args(20);
			time = to_ms_since_boot(get_absolute_time());

			if (command.size() > 0) {
				std::string cmd = command[0];
				for (int i = 0; i < cmd.length(); i += 1) {
					cmd[i] = toupper(cmd[i]);
				}

				//printf("Received Command: ");
				//printf(cmd.c_str());
				//printf("\n");
				//printf("%d\n", err);

				if (cmd == "SETD") {
					shell::shell_setd(command);
				}
				else if (cmd == "GETD") {
					int got = shell::shell_getd(command);
					//printf("GPIO: %d\n", got);
					printf("%d\n", got);
				}
				else if (cmd == "SETREG") {
					shell::shell_setReg(command, &mtr);
				}
				else if (cmd == "SETREGFLOAT") {
					shell::shell_setRegFloat(command, &mtr);
				}
				else if (cmd == "GETREG") {
					int got = shell::shell_getReg(command, &mtr);
					//printf("Register: %d\n", got);
					printf("%d\n", got);
				}
				else if (cmd == "GETREGFLOAT") {
					float got = shell::shell_getRegFloat(command, &mtr);
					//printf("Register: %f\n", got);
					printf("%f\n", got);
				}
				else if (cmd == "GETREGS") {
					std::string got = shell::shell_getRegs(command, &mtr);
					//printf("Registers: ");
					printf(got.c_str());
					printf("\n");
				}
				else if (cmd == "GETREGSFLOAT") {
					std::string got = shell::shell_getRegsFloat(command, &mtr);
					//printf("Registers: ");
					printf(got.c_str());
					printf("\n");
				}
				else if (cmd == "READRAW") {
					readAnalog = !readAnalog;
				}
				else if (cmd == "READSCALED") {
					readScaled = !readScaled;
				}
				else if (cmd == "SETPWM") {
					shell::shell_setPWM(command);
				}
				else if (cmd == "SAVEREGS") {
					mtr.saveRegisters();
				}
				else if (cmd == "LOADREGS") {
					mtr.loadRegisters();
				}
				else if (cmd == "RESETREGS") {
					mtr.resetRegisters();
				}
			}

			command.clear();
		}

		mtr.update();

		// cmd = (char)interface_getchar_timeout_ms(100);
		// char* fileName;
		// int err;
		// double val;
		// switch(cmd) {
		// 	/*case 'y':
		// 		interface_getline_timeout_ms(100, '\n', buff, 256);
		// 		printf("YModem receive error: ");
		// 		printf("%d\n", err);
		// 		break;

		// 	case 's':
		// 		interface_getline_timeout_ms(100, '\n', buff, 256);
		// 		err = print_file(buff);
		// 		printf("YModem send error: ");
		// 		printf("%d\n", err);
		// 		break;*/

		// 	case 's':
		// 		// set a digital output
		// 		break;

		// 	case 'r':
		// 		// read a digital input
		// 		break;

		// 	case 'l':
        // 		strcpy(buff, "/");
		// 		storage_scan_files(buff);
		// 		break;

		// 	case 'm':
		// 		interface_getline_timeout_ms(100, '\n', buff, 256);
		// 		fr = storage_f_open(&Fil, buff, FA_WRITE | FA_CREATE_ALWAYS);	// Create a file
		// 		if (fr == FR_OK) {
		// 			//storage_f_write(&Fil, "It works!\r\n", 11, &bw);	// Write data to the file
		// 			fr = storage_f_close(&Fil);							// Close the file
		// 		} else {
		// 			printf("File write result: ");
		// 			printf("%d\n", fr);
		// 		}
		// 		break;

		// 	case 'd':
		// 		interface_getline_timeout_ms(100, '\n', buff, 256);
		// 		fr = f_unlink(buff);
		// 		printf("File unlink result: ");
		// 		printf("%d\n", fr);
		// 		break;

		// 	/*case 'r':
		// 		interface_getline_timeout_ms(100, '\n', buff, 256);
		// 		//CodeRun(buff);
		// 		break;*/

		// 	case 'i':
		// 		// try reading ADC
		// 		err = ADC.init();
		// 		printf("ADC Init: %d\n", err);
		// 		break;

		// 	case 'a':
		// 		// try reading ADC
		// 		//val = ADC.read();
		// 		//printf("ADC Value: %f\n", val);
		// 		readAnalog = !readAnalog;
		// 		break;

		// 	case 'b':
		// 		// try reading ADC
		// 		//val = ADC.readScaled();
		// 		//printf("ADC Scaled Value: %f\n", val);
		// 		readScaled = !readScaled;
		// 		break;
		// }

		if (readAnalog) {
			val = ADC.read();
			printf("ADC Value: %f\n", val);
		}

		if (readScaled) {
			val = ADC.readScaled();
			printf("ADC Scaled Value: %f\n", val);
		}

		sleep_ms(5);

		/*strcpy(buff, "");
		interface_getline_timeout_ms(100, '\n', buff, 256);
		if (!strcmp(buff, "")) printf(buff);*/
		//led_blinking_task(500);
	}

	while (true) {
		//led_blinking_task(250);
	}

	return 0;
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
/*void led_blinking_task(int blink)
{
	static uint32_t start_ms = 0;
	static bool led_state = false;

	// Blink every interval ms
	int time = to_ms_since_boot(get_absolute_time()); 
	if ((time - start_ms) < blink) return; // not enough time
	start_ms = time;

	gpio_put(PICO_DEFAULT_LED_PIN, led_state);
	led_state = 1 - led_state; // toggle
}*/

void init_config() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    //gpio_init(PIN_MOTOR_ENABLE);
    //gpio_set_dir(PIN_MOTOR_ENABLE, GPIO_OUT);
    gpio_init(PIN_MOTOR_MODE);
    gpio_set_dir(PIN_MOTOR_MODE, GPIO_OUT);
    gpio_init(PIN_MOTOR_SLEEP);
    gpio_set_dir(PIN_MOTOR_SLEEP, GPIO_OUT);

    gpio_init(PIN_DIN_5);
    gpio_set_dir(PIN_DIN_5, GPIO_OUT);
    //gpio_init(PIN_DIN_2);
    //gpio_set_dir(PIN_DIN_2, GPIO_OUT);

    gpio_set_function(PIN_MOTOR_ENABLE, GPIO_FUNC_PWM);
    gpio_set_function(PIN_DIN_4, GPIO_FUNC_PWM);
}