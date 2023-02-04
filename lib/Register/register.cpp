#include "register.h"

#include <algorithm>

namespace registers {
    Reg::Reg() {
        _type = RegisterType::IntType;
        name = "";
        description = "";
        canSave = true;
        canRead = true;
        canWrite = true;
        maxValue = INT32_MAX;
        minValue = INT32_MIN;
    }

    Reg::Reg(RegisterType type) {
        _type = type;
        name = "";
        description = "";
        canSave = true;
        canRead = true;
        canWrite = true;
        maxValue = INT32_MAX;
        minValue = INT32_MIN;
    }

    Reg::Reg(RegisterType type, std::string name, std::string desc) {
        _type = type;
        this->name = name;
        description = desc;
        canSave = true;
        canRead = true;
        canWrite = true;
        maxValue = INT32_MAX;
        minValue = INT32_MIN;
    }

    Reg::Reg(RegisterType type, std::string name, std::string desc, int def) {
        _type = type;
        this->name = name;
        description = desc;
        canSave = true;
        canRead = true;
        canWrite = true;
        maxValue = INT32_MAX;
        minValue = INT32_MIN;
        _value = def;
    }

    Reg::Reg(RegisterType type, std::string name, std::string desc, int def, int max, int min) {
        _type = type;
        this->name = name;
        description = desc;
        canSave = true;
        canRead = true;
        canWrite = true;
        maxValue = max;
        minValue = min;
        _value = def;
    }

    Reg::Reg(RegisterType type, std::string name, std::string desc, bool read, bool write) {
        _type = type;
        this->name = name;
        description = desc;
        canSave = true;
        canRead = read;
        canWrite = write;
        maxValue = INT32_MAX;
        minValue = INT32_MIN;
    }

    Reg::Reg(RegisterType type, std::string name, std::string desc, bool read, bool write, int def) {
        _type = type;
        this->name = name;
        description = desc;
        canSave = true;
        canRead = read;
        canWrite = write;
        maxValue = INT32_MAX;
        minValue = INT32_MIN;
        _value = def;
    }

    Reg::Reg(RegisterType type, std::string name, std::string desc, bool read, bool write, int def, int max, int min) {
        _type = type;
        this->name = name;
        description = desc;
        canSave = true;
        canRead = read;
        canWrite = write;
        maxValue = max;
        minValue = min;
        _value = def;
    }

    Reg::~Reg() {

    }

    RegisterType Reg::getType() {
        return _type;
    }

    void Reg::setValue(int val) {
        if (canWrite) _value = std::clamp(val, minValue, maxValue);
    }

    void Reg::setFloat(float val) {
        val = std::clamp(val, (float)minValue, (float)maxValue);
        int value = *(int*)&val;
        if (canWrite) _value = value;
    }

    void Reg::forceValue(int val) {
        _value = val;
    }

    void Reg::forceFloat(float val) {
        int value = *(int*)&val;
        _value = value;
    }

    int Reg::getValue() {
        if (canRead) return _value;
        return 0;
    }

    float Reg::getFloat() {
        if (canRead) {
            float value = *(float*)&_value;
            return value;
        }
        return 0;
    }

    

    /*Registers::Registers() {

    }

    Registers::~Registers() {

    }

    void Registers::setRegister(int reg, int value) {
        if (reg < 0) return;
        if (reg >= RegisterCount) return;

        Regs[reg].setValue(value);
    }

    int Registers::getRegister(int reg) {
        if (reg < 0) return 0;
        if (reg >= RegisterCount) return 0;

        return Regs[reg].getValue();
    }*/
}