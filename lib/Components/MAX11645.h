#pragma once

#include <string>
#include "components.h"
#include "hardware/i2c.h"

namespace components {
    class MAX11645 : public component {
        const uint8_t I2C_READ = 0x1;
        const uint8_t I2C_WRITE = 0x0;

        // Register bit
        const uint8_t SETUP_MODE = 1;
        const uint8_t CONFIG_MODE = 0;

        // Reference Voltage Source bits
        const uint8_t REF_SRC_VDD = 0;
        const uint8_t REF_SRC_EXTERNAL = 0;
        const uint8_t REF_SRC_INTERNAL = 1;

        // Reference Pin bits
        const uint8_t REF_PIN_NC = 0;
        const uint8_t REF_PIN_INPUT = 1;
        const uint8_t REF_PIN_OUTPUT = 1;

        // Internal Reference State bits
        const uint8_t REF_STATE_OFF = 0;
        const uint8_t REF_STATE_ON = 1;

        // Clock mode bit
        const uint8_t CLOCK_INTERNAL = 0;
        const uint8_t CLOCK_EXTERNAL = 1;

        // Polarity mode bit
        const uint8_t POLARITY_UNIPOLAR = 0;
        const uint8_t POLARITY_BIPOLAR  = 1;

        // Reset bit
        const uint8_t RES_RESET = 0;
        const uint8_t RES_NO_ACTION = 1;

        // Setup Mode Bits
        const uint8_t SET_REGISTER_BIT  = 0x08;
        const uint8_t SET_SEL2_BIT      = 0x07;
        const uint8_t SET_SEL1_BIT      = 0x06;
        const uint8_t SET_SEL0_BIT      = 0x05;
        const uint8_t SET_CLOCK_BIT     = 0x04;
        const uint8_t SET_POLARITY_BIT  = 0x03;
        const uint8_t SET_RESET_BIT     = 0x02;
        const uint8_t SET_X_BIT         = 0x01;

        // Scan Modes
        const uint8_t SCAN_MODE_0 = 0;  // Scans up from AIN0 to the input selected by CS0
        const uint8_t SCAN_MODE_1 = 1;  // Converts the input selected by CS0 eight times (see Tables 3 and 4)
        const uint8_t SCAN_MODE_2 = 2;  // Reserved. Do not use
        const uint8_t SCAN_MODE_3 = 3;  // Converts the input selected by CS0

        // Channel Selection
        const uint8_t CS_AIN0_POS   = 0;
        const uint8_t CS_AIN0_NEG   = 1;

        // Input Mode
        const uint8_t INPUT_SINGLE  = 1;
        const uint8_t INPUT_DIFF    = 0;

        // Configuration Mode Bits
        const uint8_t CONFIG_REGISTER_BIT   = 0x08;
        const uint8_t CONFIF_SCAN1_BIT      = 0x07;
        const uint8_t CONFIG_SCAN0_BIT      = 0x06;
        const uint8_t CONFIG_X0_BIT         = 0x05;
        const uint8_t CONFIG_X1_BIT         = 0x04;
        const uint8_t CONFIG_X2_BIT         = 0x03;
        const uint8_t CONFIG_CS0_BIT        = 0x02;
        const uint8_t CONFIG_INPUT_BIT      = 0x01;

        public:
            MAX11645(std::string name);
            ~MAX11645();
            int init();
            int write(double val);
            double read();
            int16_t read16();
            double readScaled();

            i2c_inst_t* bus = NULL;
            uint8_t address = 0b0110110;

        private:
            void setWriteAddress();
            void setReadAddress();
    };
}