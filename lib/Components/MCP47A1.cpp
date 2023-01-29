#include "MCP47A1.h"

#include <algorithm>

namespace components {
    MCP47A1::MCP47A1(std::string name) : component(name) {

    }

    MCP47A1::~MCP47A1() {

    }

    int MCP47A1::init() {
        // Nothing to initialize
        return 1;
    }

    int MCP47A1::write(double val) {
        if (bus == NULL) return 0;

        // Set R/W bit in address for writing
        //setWriteAddress();
        
        uint8_t data = CMD_WRITE;
        int bytes_written = i2c_write_blocking(bus, address, &data, sizeof(uint8_t), true);

        // Clamp input to 0-3.3V
        val = std::clamp(val, 0.0, 3.3);
        // Scale to 0-63 for register
        val = (val / 3.3) * 63.0;

        // Convert to int and add 6-bit mask
        data = val;
        data = data & 0x3F;

        bytes_written = i2c_write_blocking(bus, address, &data, sizeof(uint8_t), false);

        return bytes_written;
    }

    double MCP47A1::read() {
        if (bus == NULL) return 0;

        // Set R/W bit in address for reading
        //setReadAddress();
        
        uint8_t data = CMD_WRITE;
        int bytes_written = i2c_write_blocking(bus, address, &data, sizeof(uint8_t), true);

        i2c_read_blocking(bus, address, &data, sizeof(uint8_t), false);

        double val = data;
        val = (val / 63.0) * 3.3;

        return val;
    }

    void MCP47A1::setWriteAddress() {
        address &= ~1;
        address += I2C_WRITE;
    }

    void MCP47A1::setReadAddress() {
        address &= ~1;
        address += I2C_READ;
    }
}