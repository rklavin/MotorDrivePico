#pragma once

#include "motor.h"

#include <string>
#include <vector>

namespace shell {
    // Convert string to integer
    int string_to_int(std::string str);
    float string_to_float(std::string str);

    void shell_setd(std::vector<std::string> args);
    int shell_getd(std::vector<std::string> args);
    void shell_setReg(std::vector<std::string> args, motor::Motor* mtr);
    void shell_setRegFloat(std::vector<std::string> args, motor::Motor* mtr);
    int shell_getReg(std::vector<std::string> args, motor::Motor* mtr);
    float shell_getRegFloat(std::vector<std::string> args, motor::Motor* mtr);
    std::string shell_getRegs(std::vector<std::string> args, motor::Motor* mtr);
    std::string shell_getRegsFloat(std::vector<std::string> args, motor::Motor* mtr);
    void shell_setPWM(std::vector<std::string> args);
}