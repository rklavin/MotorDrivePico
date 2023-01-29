#pragma once

#include <string>
#include "components.h"
#include "hardware/i2c.h"

namespace components {
    class MCP47A1 : public component {
        const uint8_t I2C_READ = 0x1;
        const uint8_t I2C_WRITE = 0x0;

        const uint8_t CMD_WRITE = 0x0;

        public:
            MCP47A1(std::string name);
            ~MCP47A1();
            int init();
            int write(double val);
            double read();

            i2c_inst_t* bus = NULL;
            uint8_t address = 0b0101110;

        private:
            void setWriteAddress();
            void setReadAddress();
    };
}