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
//#include "Components/DRV8876.h"
#include "Components/MAX11645.h"
#include "Components/MCP47A1.h"

#include "Console/console.h"
#include "Shell/shell.h"
#include "motor.h"

// TODO: Move serial communication to core 2
//       Core 1 to only be used for motor logic

//void init_board();
void init_config();

int main(void) {
	// Main loop
	int err = 0;
	bool readAnalog = false;
	bool readScaled = false;
	bool streamRegisters = false;

	init_board();

	/*for (int i = 0; i < 5; i += 1) {
		printf("%d\n", 5 - i);
		interface_sleep_ms(1000);
	}*/

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
	//components::DRV8876 MotorDriver("Motor Driver");
	components::MAX11645 ADC("Analog to Digital Converter");
	ADC.bus = i2c0;
	ADC.scale_M = 12.288;
	components::MCP47A1 DAC("Digital to Analog Converter");
	//DAC.bus = i2c0;

	motor::Motor mtr;
	mtr.ADC = &ADC;
	mtr.DAC = &DAC;

	printf("Init Components...\n");
	printf("Init ADC...\n");
	err = ADC.init();
	if (err != 1)
		printf("ADC Init error\n");

	printf("Init DAC...\n");
	err &= DAC.init();
	if (err != 1)
		printf("DAC Init error\n");

	printf("Init Motor...\n");
	err &= mtr.init();
	if (err != 1)
		printf("Motor Init error\n");

	if (err == 1)
		printf("Init Complete\n");
	else
		printf("Component Init Failed\n");

	int time = to_ms_since_boot(get_absolute_time());
	int streamTime = to_ms_since_boot(get_absolute_time());
	std::vector<std::string> streamCommand;

	while (true) {
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
				else if (cmd == "GETALLREGS") {
					std::string got = shell::shell_getRegsAll(command, &mtr);
					//printf("Registers: ");
					printf(got.c_str());
					printf("\n");
				}
				else if (cmd == "STREAMREGS") {
					streamCommand.clear();
					streamCommand = command;
					streamRegisters = !streamRegisters;
				}
			}

			command.clear();
		}

		mtr.update();

		if (readAnalog) {
			double val = ADC.read();
			printf("ADC Value: %f\n", val);
		}

		if (readScaled) {
			double val = ADC.readScaled();
			printf("ADC Scaled Value: %f\n", val);
		}
		
		if (streamRegisters) {
			if (to_ms_since_boot(get_absolute_time()) - streamTime > 50) {
				streamTime = to_ms_since_boot(get_absolute_time());
				//std::vector<std::string> dummy = { "STREAMREGS" };
				std::string got = shell::shell_getRegsAny(streamCommand, &mtr);
				printf(got.c_str());
				printf("\n");
			}
		}

		sleep_ms(5);
	}

	return 0;
}

/*void init_board() {
	// Initialize RP2040
	stdio_init_all();
	//uart_init(UART_ID, BAUD_RATE);
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