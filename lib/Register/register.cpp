#include "register.h"

#include <algorithm>

namespace registers {
    Reg::Reg() {
        name = "";
        description = "";
        _canRead = true;
        _canWrite = true;
        _maxValue = INT32_MAX;
        _minValue = INT32_MIN;
    }

    Reg::Reg(std::string name, std::string desc) {
        this->name = name;
        description = desc;
        _canRead = true;
        _canWrite = true;
        _maxValue = INT32_MAX;
        _minValue = INT32_MIN;
    }

    Reg::Reg(std::string name, std::string desc, int def) {
        this->name = name;
        description = desc;
        _canRead = true;
        _canWrite = true;
        _maxValue = INT32_MAX;
        _minValue = INT32_MIN;
        _value = def;
    }

    Reg::Reg(std::string name, std::string desc, int def, int max, int min) {
        this->name = name;
        description = desc;
        _canRead = true;
        _canWrite = true;
        _maxValue = max;
        _minValue = min;
        _value = def;
    }

    Reg::Reg(std::string name, std::string desc, bool read, bool write) {
        this->name = name;
        description = desc;
        _canRead = read;
        _canWrite = write;
        _maxValue = INT32_MAX;
        _minValue = INT32_MIN;
    }

    Reg::Reg(std::string name, std::string desc, bool read, bool write, int def) {
        this->name = name;
        description = desc;
        _canRead = read;
        _canWrite = write;
        _maxValue = INT32_MAX;
        _minValue = INT32_MIN;
        _value = def;
    }

    Reg::Reg(std::string name, std::string desc, bool read, bool write, int def, int max, int min) {
        this->name = name;
        description = desc;
        _canRead = read;
        _canWrite = write;
        _maxValue = max;
        _minValue = min;
        _value = def;
    }

    Reg::~Reg() {

    }

    bool Reg::canRead() {
        return _canRead;
    }

    bool Reg::canWrite() {
        return _canWrite;
    }

    void Reg::setValue(int val) {
        if (_canWrite) _value = std::clamp(val, _minValue, _maxValue);
    }

    void Reg::setFloat(float val) {
        val = std::clamp(val, (float)_minValue, (float)_maxValue);
        int value = *(int*)&val;
        if (_canWrite) _value = value;
    }

    void Reg::forceValue(int val) {
        _value = val;
    }

    void Reg::forceFloat(float val) {
        int value = *(int*)&val;
        _value = value;
    }

    int Reg::getValue() {
        if (_canRead) return _value;
        return 0;
    }

    float Reg::getFloat() {
        if (_canRead) {
            float value = *(float*)&_value;
            return value;
        }
        return 0;
    }

    int Reg::getMaxValue() {
        return _maxValue;
    }

    int Reg::getMinValue() {
        return _minValue;
    }

    

    Registers::Registers() {

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
    }
}