#include "shell.h"
#include "Register/register.h"
#include "Interface/interface.h"

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"

namespace shell {
    // Convert string to integer
    int string_to_int(std::string str) {
        int res = 0;
        int i = 0;
        bool neg = false;

        if (str[i] == '-') {
            neg = true;
            i += 1;
        }

        while(str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + (str[i] - '0');
            i += 1;
        }

        if (neg) res = res * -1;

        return res;
    }

    // Convert string to float
    float string_to_float(std::string str) {
        float res = 0;
        float dec = 0;
        int i = 0;
        float j = 1;
        bool neg = false;

        if (str[i] == '-') {
            neg = true;
            i += 1;
        }

        while(str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + (str[i] - '0');
            i += 1;
        }

        if (str[i] == '.') {
            i += 1;
            while(str[i] >= '0' && str[i] <= '9') {
                dec = dec * 10 + (str[i] - '0');
                i += 1;
                j *= 10;
            }

            dec = dec / j;
            res += dec;
        }

        if (neg) res = res * -1;

        return res;
    }

    void shell_setd(std::vector<std::string> args) {
        // set a digital output
        // first argument is gpio number
        // second argument is 1/0 = on/off
        if (args.size() < 3) return;
        
        int gpio = string_to_int(args[1]);
        int out = string_to_int(args[2]);
        gpio_put(gpio, out);
    }

    int shell_getd(std::vector<std::string> args) {
        // set a digital output
        // first argument is gpio number
        // second argument is 1/0 = on/off
        if (args.size() < 2) return 0;
        
        int gpio = string_to_int(args[1]);
        return gpio_get(gpio);
    }

    void shell_setReg(std::vector<std::string> args, motor::Motor* mtr) {
        if (args.size() < 3) return;

        int reg = string_to_int(args[1]);
        int value = string_to_int(args[2]);

        if (mtr->Registers.find(reg) == mtr->Registers.end()) {
            return;
        } else {
            mtr->Registers[reg].setValue(value);
        }
    }

    void shell_setRegFloat(std::vector<std::string> args, motor::Motor* mtr) {
        if (args.size() < 3) return;

        int reg = string_to_int(args[1]);
        float value = string_to_float(args[2]);

        if (mtr->Registers.find(reg) == mtr->Registers.end()) {
            return;
        } else {
            mtr->Registers[reg].setFloat(value);
        }
    }

    int shell_getReg(std::vector<std::string> args, motor::Motor* mtr) {
        if (args.size() < 2) return 0;

        int reg = string_to_int(args[1]);

        if (mtr->Registers.find(reg) == mtr->Registers.end()) {
            return 0;
        } else {
            return mtr->Registers[reg].getValue();
        }
    }

    float shell_getRegFloat(std::vector<std::string> args, motor::Motor* mtr) {
        if (args.size() < 2) return 0;

        int reg = string_to_int(args[1]);

        if (mtr->Registers.find(reg) == mtr->Registers.end()) {
            return 0;
        } else {
            return mtr->Registers[reg].getFloat();
        }
    }
    
    std::string shell_getRegs(std::vector<std::string> args, motor::Motor* mtr) {
        if (args.size() < 2) return 0;

        int regs = args.size() - 1;
        std::string res;

        for (int i = 0; i < regs; i += 1) {
            int reg = string_to_int(args[i + 1]);
            res += std::to_string(reg) + " ";
            if (mtr->Registers.find(reg) == mtr->Registers.end()) {
                res += std::to_string(0);
            } else {
                res += std::to_string(mtr->Registers[reg].getValue());
            }

            if (i + 1 < regs) res += " ";
        }

        return res;
    }
    
    std::string shell_getRegsFloat(std::vector<std::string> args, motor::Motor* mtr) {
        if (args.size() < 2) return 0;

        int regs = args.size() - 1;
        std::string res;

        for (int i = 0; i < regs; i += 1) {
            int reg = string_to_int(args[i + 1]);
            res += std::to_string(reg) + " ";
            if (mtr->Registers.find(reg) == mtr->Registers.end()) {
                res += std::to_string(0);
            } else {
                res += std::to_string(mtr->Registers[reg].getFloat());
            }

            if (i + 1 < regs) res += " ";
        }

        return res;
    }
    
    std::string shell_getRegsAll(std::vector<std::string> args, motor::Motor* mtr) {
        if (args.size() < 1) return 0;

        std::string res;
        std::map<int, registers::Reg>::iterator itr;

        for (itr = mtr->Registers.begin(); itr != mtr->Registers.end(); itr++) {
            int reg = itr->first;

            // Add register index
            res += std::to_string(reg) + " ";

            // Add register value
            if (mtr->Registers[reg].getType() == registers::RegisterType::IntType) {
                res += std::to_string(mtr->Registers[reg].getValue());
            }
            else if (mtr->Registers[reg].getType() == registers::RegisterType::FloatType) {
                res += std::to_string(mtr->Registers[reg].getFloat());
            }
            else if (mtr->Registers[reg].getType() == registers::RegisterType::BoolType) {
                res += std::to_string(mtr->Registers[reg].getValue());
            }

            res += " ";
        }

        return res;
    }
    
    std::string shell_getRegsAny(std::vector<std::string> args, motor::Motor* mtr) {
        // If no registers were added, try sending all
        if (args.size() < 2) return shell_getRegsAll(args, mtr);

        int regs = args.size() - 1;
        std::string res;

        for (int i = 0; i < regs; i += 1) {
            int reg = string_to_int(args[i + 1]);
            res += std::to_string(reg) + " ";
            if (mtr->Registers.find(reg) == mtr->Registers.end()) {
                res += std::to_string(0);
            } else {
                if (mtr->Registers[reg].getType() == registers::RegisterType::IntType) {
                    res += std::to_string(mtr->Registers[reg].getValue());
                }
                else if (mtr->Registers[reg].getType() == registers::RegisterType::FloatType) {
                    res += std::to_string(mtr->Registers[reg].getFloat());
                }
                else if (mtr->Registers[reg].getType() == registers::RegisterType::BoolType) {
                    res += std::to_string(mtr->Registers[reg].getValue());
                }
            }

            if (i + 1 < regs) res += " ";
        }

        return res;
    }
    
    void shell_setPWM(std::vector<std::string> args) {
        // arg 1 = gpio num
        // arg 2 = start/stop = 1/0
        // arg 3 = frequency in Hz
        // arg 4 = duty cycle in 0-100%

        if (args.size() < 5) return;

        int gpio = string_to_int(args[1]);
        int ena = string_to_int(args[2]);
        double freq = string_to_int(args[3]);
        double duty = string_to_int(args[4]);

        //printf("GPIO Arg: %d\n", gpio);
        //printf("ena Arg: %d\n", ena);
        //printf("freq Arg: %d\n", freq);
        //printf("duty Arg: %d\n", duty);

        uint slice_num = pwm_gpio_to_slice_num(gpio);
        uint chan_num = pwm_gpio_to_channel(gpio);

        // exit early if turning off
        if (ena == 0) {
            pwm_set_enabled(slice_num, false);
            return;
        }

        // Set period of 4 cycles (0 to 3 inclusive)
        // 8ns per cycle
        if (freq == 0) return;
        freq = ((1 / freq) * 1000000000) / 8;
        int cycles = freq - 1;
        pwm_set_wrap(slice_num, cycles);

        // Set channel A output high for one cycle before dropping
        // number of cycles to stay on, duty cycle
        duty = duty / 100;
        duty = freq * duty;
        int duty_cycle = duty;
        pwm_set_chan_level(slice_num, chan_num, duty_cycle);

        // Set the PWM running
        pwm_set_enabled(slice_num, true);
    }
}