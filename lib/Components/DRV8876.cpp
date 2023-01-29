#include "DRV8876.h"

namespace components {
    DRV8876::DRV8876(std::string name) : component(name) {

    }

    DRV8876::~DRV8876() {

    }

    int DRV8876::init() {
        return 0;
    }

    int DRV8876::write(double val) {
        // Write value out to DAC
        // Turn on EN/PH depending on direction
            // Use PWM on EM to control speed
            // PH = 0 = reverse
            // PH = 1 = forward
        // Turn on nSleep?
        if (vref == NULL) return 0;
        if (bus == NULL) return 0;
        return vref->write(val);
    }

    double DRV8876::read() {
        // Read motor current from ipropi
        // 0-3.3V (0-4096) is 0-3.626A
        adc_select_input(ipropi_adc);
        return adc_read();
    }

    void DRV8876::sleep() {
        gpio_put(sleep_pin, false);
    }

    void DRV8876::wake() {
        gpio_put(sleep_pin, true);
    }

    void DRV8876::brake() {
        gpio_put(enable_pin, false);
        gpio_put(mode_pin, false);
    }

    void DRV8876::forward() {
        gpio_put(enable_pin, true);
        gpio_put(mode_pin, true);
    }

    void DRV8876::reverse() {
        gpio_put(enable_pin, true);
        gpio_put(mode_pin, false);
    }

    bool DRV8876::getFault() {
        return gpio_get(fault_pin);
    }
}