#include "MAX11645.h"

namespace components {
    MAX11645::MAX11645(std::string name) : component(name) {

    }

    MAX11645::~MAX11645() {

    }

    int MAX11645::init() {
        if (bus == NULL) return 0;

        // Set R/W bit in address for writing
        //setWriteAddress();
        
        // Build setup byte
        uint8_t setup_byte;
        setup_byte = setup_byte | (SETUP_MODE << (SET_REGISTER_BIT - 1));
        //setup_byte = setup_byte | (REF_SRC_INTERNAL << (SET_SEL2_BIT - 1));
        //setup_byte = setup_byte | (REF_PIN_OUTPUT << (SET_SEL1_BIT - 1));
        //setup_byte = setup_byte | (REF_STATE_ON << (SET_SEL0_BIT - 1));
        setup_byte = setup_byte | (REF_SRC_INTERNAL << (SET_SEL2_BIT - 1));
        setup_byte = setup_byte | (REF_PIN_OUTPUT << (SET_SEL1_BIT - 1));
        setup_byte = setup_byte | (REF_STATE_ON << (SET_SEL0_BIT - 1));
        setup_byte = setup_byte | (CLOCK_INTERNAL << (SET_CLOCK_BIT - 1));
        setup_byte = setup_byte | (POLARITY_BIPOLAR << (SET_POLARITY_BIT - 1));
        setup_byte = setup_byte | (RES_NO_ACTION << (SET_RESET_BIT - 1));

        // Send setup byte
        int bytes_written = i2c_write_blocking(bus, address, &setup_byte, 1, false);
        if (bytes_written <= 0) return bytes_written;
        
        // Build configuration byte
        uint8_t config_byte = 0;
        config_byte = config_byte | (CONFIG_MODE << (CONFIG_REGISTER_BIT - 1));
        //setup_byte = setup_byte | (SCAN_MODE_0 << (CONFIF_SCAN1_BIT - 1));
        config_byte = config_byte | (SCAN_MODE_0 << (CONFIG_SCAN0_BIT - 1));
        //setup_byte = setup_byte | (REF_STATE_ON << (CONFIG_X0_BIT - 1));
        //setup_byte = setup_byte | (CLOCK_EXTERNAL << (CONFIG_X1_BIT - 1));
        //setup_byte = setup_byte | (POLARITY_BIPOLAR << (CONFIG_X2_BIT - 1));
        config_byte = config_byte | (CS_AIN0_POS << (CONFIG_CS0_BIT - 1));
        config_byte = config_byte | (INPUT_DIFF << (CONFIG_INPUT_BIT - 1));

        // Send configuration byte
        bytes_written = i2c_write_blocking(bus, address, &config_byte, 1, false);
        if (bytes_written <= 0) return bytes_written;

        return 1;
    }

    int MAX11645::write(double val) {
        // Nothing to write
        return 0;
    }

    double MAX11645::read() {
        if (bus == NULL) return 0;

        // Set R/W bit in address for reading
        //setReadAddress();

        uint8_t MSB[2];

        int bytes_read = i2c_read_blocking(bus, address, MSB, sizeof(uint8_t) * 2, false);
        if (bytes_read <= 0) return bytes_read;

        // Read 12-bit number
        int16_t res = (((MSB[0] << 8) | MSB[1]) & 0xFFF);

        if (res >= 0x800) res = res - 4096;

        return res;
    }

    int16_t MAX11645::read16() {
        return read();
    }

    double MAX11645::readScaled() {
        double res = read16();
        res = ((res / 2048) * scale_M) + scale_B;

        return res;
    }

    void MAX11645::setWriteAddress() {
        address &= ~1;
        address += I2C_WRITE;
    }

    void MAX11645::setReadAddress() {
        address &= ~1;
        address += I2C_READ;
    }
}