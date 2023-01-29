#pragma once

#include <string>
#include "components.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "MCP47A1.h"

namespace components {
    class DRV8876 : public component {
        public:
            DRV8876(std::string name);
            ~DRV8876();
            int init();
            int write(double val);
            double read();

            void sleep();
            void wake();
            void brake();
            void forward();
            void reverse();
            bool getFault();

            i2c_inst_t* bus = NULL;
            uint8_t address = 0b01101101;

            MCP47A1* vref = NULL;

            uint8_t enable_pin;
            uint8_t mode_pin;
            uint8_t sleep_pin;
            uint8_t fault_pin;
            uint8_t ipropi_adc;
    };
}