#include "motor.h"
#include "config.h"

#include <algorithm>
#include <cmath>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#include "Storage/storage.h"
#include "Shell/shell.h"

namespace motor {
    Motor::Motor() {
        buildRegisters();
    }

    Motor::~Motor() {

    }

    int Motor::init() {
        state = motor::MotorState::init;

        int err = loadRegisters();

        return err;
    }

    void Motor::update() {
        // Read Analog Input to Speed Reference Analog register
        updateRefAnalog();

        // Update Speed Reference register
        updateRef();

        updateCurrentLimit();

        updateCurrent();

        updateEnable();

        updateMotorControl();
    }

    void Motor::resetRegisters() {
        Registers.clear();
        buildRegisters();
    }

    int Motor::saveRegisters() {
        UINT bw;    // Stores number of bytes written during file write operation
        FRESULT fr; // File operation result
        FIL Fil;    // File object needed for each open file

        // Create a file
        fr = storage_f_open(&Fil, "0:MotorRegs.txt", FA_WRITE | FA_CREATE_ALWAYS);
        if (fr != FR_OK) return 0;

        std::map<int, registers::Reg>::iterator itr;
        for (itr = Registers.begin(); itr != Registers.end(); itr++) {
            if (!itr->second.canSave) continue;

            std::string data = std::to_string(itr->first) + "\n";
            bw = storage_f_puts(data.c_str(), &Fil);
            if (bw < 0) {
                fr = storage_f_close(&Fil);
                return 0;
            }

            data = std::to_string(itr->second.getValue()) + "\n";
            bw = storage_f_puts(data.c_str(), &Fil);
            if (bw < 0) {
                fr = storage_f_close(&Fil);
                return 0;
            }
        }

        // Close the file
        fr = storage_f_close(&Fil);

        return 1;
    }

    int Motor::loadRegisters() {
        UINT br;    // Stores number of bytes read during file read operation
        FRESULT fr; // File operation result
        FIL Fil;    // File object needed for each open file

        // Create a file
        fr = storage_f_open(&Fil, "0:MotorRegs.txt", FA_READ | FA_OPEN_EXISTING);
        if (fr != FR_OK) return 0;

        TCHAR* buff;
        buff = (TCHAR*)malloc(sizeof(TCHAR) * 256);
        std::string dataStr;

        while(storage_f_gets(buff, 256, &Fil) != 0) {
            dataStr = buff;
            int reg = shell::string_to_int(dataStr);

            if (storage_f_gets(buff, 256, &Fil) == 0) break;
            dataStr = buff;
            int regVal = shell::string_to_int(dataStr);

            Registers[reg].forceValue(regVal);
        }

        free(buff);

        return 1;
    }

    MotorStatus Motor::getStatus() {
        return status;
    }

    MotorState Motor::getState() {
        return state;
    }

    void Motor::updateRefAnalog() {
        state = motor::MotorState::updateSpeedRef;

        if (ADC != NULL) {
            float zero = Registers[RegSpeedRefAnalogScaleZero].getFloat();
            float min = Registers[RegSpeedRefAnalogScaleMin].getFloat();
            float max = Registers[RegSpeedRefAnalogScaleMax].getFloat();
            float deadband = Registers[RegAnalogInDeadband].getFloat();

            float val = ADC->readScaled();
            Registers[RegAnalogInVoltage].forceFloat(val);
            val = std::clamp(val, min, max);

            // Apply deadband
            if (std::abs(val - zero) <= deadband) val = zero;

            if (val >= zero) {
                if (max == zero) 
                    val = 0;
                else
                    val = ((val - zero) / (max - zero)) * 100;
            } else {
                if (min == zero) 
                    val = 0;
                else
                    val = ((val - zero) / (min - zero)) * -100;
            }

            min = -100;
            max = 100;
            val = std::clamp(val, min, max);
            
            Registers[RegSpeedReferenceAnalog].forceFloat(val);
        }
    }

    void Motor::updateRef() {
        state = motor::MotorState::updateSpeedRef;

        static int time = to_ms_since_boot(get_absolute_time());
        float  dt = (to_ms_since_boot(get_absolute_time()) - time) / 1000.0;

        checkDigitalInReferenceSource();

        float val;
        switch(Registers[RegSpeedReferenceSource].getValue()) {
            case motor::SpeedReferenceSource::AnalogIn:
                val = Registers[RegSpeedReferenceAnalog].getFloat();
                break;

            case motor::SpeedReferenceSource::SpeedRefReg:
                val = Registers[RegSpeedReferenceValue].getFloat();
                break;

            case motor::SpeedReferenceSource::InternalSet1:
                val = Registers[RegInternalSetpoint1].getFloat();
                break;

            case motor::SpeedReferenceSource::InternalSet2:
                val = Registers[RegInternalSetpoint2].getFloat();
                break;

            case motor::SpeedReferenceSource::InternalSet3:
                val = Registers[RegInternalSetpoint3].getFloat();
                break;

            case motor::SpeedReferenceSource::InternalSet4:
                val = Registers[RegInternalSetpoint4].getFloat();
                break;

            case motor::SpeedReferenceSource::InternalSet5:
                val = Registers[RegInternalSetpoint5].getFloat();
                break;

            default:
                break;
        }

        float rate = Registers[RegMotorOutputRate].getFloat() * dt;
        float ramp = Registers[RegSpeedReference].getFloat();
        if ((ramp + rate) <= val) {
            ramp += rate;
        }
        else if ((ramp - rate) >= val) {
            ramp -= rate;
        }
        else {
            ramp = val;
        }

        // Prevent ramp if not run forward/reverse
        bool forwardUsed = checkDigitalFunction(DigitalInFunction::RunForward, RegRunForward);
        bool reverseUsed = checkDigitalFunction(DigitalInFunction::RunReverse, RegRunReverse);
        float maxRamp = 100;
        float minRamp = -100;
        
        if (forwardUsed) {
            maxRamp = Registers[RegRunForward].getValue() ? 100 : 0;
        }
        if (reverseUsed) {
            minRamp = Registers[RegRunReverse].getValue() ? -100 : 0;
        }
        ramp = std::clamp(ramp, minRamp, maxRamp);

        // Prevent enable if forward/reverse inhibit is on
        forwardUsed = checkDigitalFunction(DigitalInFunction::ForwardInhibit, RegForwardInhibit);
        reverseUsed = checkDigitalFunction(DigitalInFunction::ReverseInhibit, RegReverseInhibit);

        if (forwardUsed) {
            maxRamp = Registers[RegForwardInhibit].getValue() ? 0 : 100;
        }
        if (reverseUsed) {
            minRamp = Registers[RegReverseInhibit].getValue() ? 0 : -100;
        }
        ramp = std::clamp(ramp, minRamp, maxRamp);

        Registers[RegSpeedReference].forceFloat(ramp);

        time = to_ms_since_boot(get_absolute_time());
    }

    void Motor::updateCurrentLimit() {
        state = motor::MotorState::updateCurrentLimit;

        if (DAC != NULL) {
            float val = Registers[RegCurrentLimitValue].getFloat();
            val = (val / maxCurrent) * 3.3;
            DAC->write(val);
        }
    }

    void Motor::updateCurrent() {
        state = motor::MotorState::updateCurrentLimit;

        // Select ADC using 0-3 instead of gpio for some reason
        adc_select_input(PIN_ADC_IPROPI - 26);

        const float conversionFactor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        float val = result * conversionFactor;
        val = (val / 3.3) * maxCurrent;

        Registers[RegMotorCurrentValue].forceFloat(val);
    }

    void Motor::updateEnable() {
        state = motor::MotorState::updateEnable;

        int val;

        switch(Registers[RegMotorEnableSource].getValue()) {
            case motor::MotorEnableSource::DigitalIn:
                val = checkDigitalInMotorEnable();
                break;

            case motor::MotorEnableSource::MotorEnableReg:
                val = Registers[RegMotorEnabled].getValue();
                break;

            default:
                break;
        }

        Registers[RegMotorEnabled].forceValue(val);
    }

    void Motor::updateMotorControl() {
        state = motor::MotorState::updateMotorControl;

        bool enabled = (bool)Registers[RegMotorEnabled].getValue();
        float speedRef = Registers[RegSpeedReference].getFloat();

        // Set direction of motor depending on speed reference
        gpio_put(PIN_MOTOR_MODE, (speedRef >= 0));

        // Enable motor driver
        gpio_put(PIN_MOTOR_SLEEP, enabled);
        
        // Set PWM
        setPWM(true, 20000, enabled ? std::abs(speedRef) : 0);
    }

    int Motor::checkDigitalInMotorEnable() {
        int val = 0;

        if (Registers[RegDigitaInput1Function].getValue() == motor::DigitalInFunction::MotorEnable) {
            val = gpio_get(PIN_DIN_1);
        }

        if (Registers[RegDigitaInput2Function].getValue() == motor::DigitalInFunction::MotorEnable) {
            val |= gpio_get(PIN_DIN_2);
        }

        if (Registers[RegDigitaInput3Function].getValue() == motor::DigitalInFunction::MotorEnable) {
            val |= gpio_get(PIN_DIN_3);
        }

        if (Registers[RegDigitaInput4Function].getValue() == motor::DigitalInFunction::MotorEnable) {
            val |= gpio_get(PIN_DIN_4);
        }

        if (Registers[RegDigitaInput5Function].getValue() == motor::DigitalInFunction::MotorEnable) {
            val |= gpio_get(PIN_DIN_5);
        }

        return val;
    }

    int Motor::checkDigitalInReferenceSource() {
        int val = 0;
        int dins[5] = { PIN_DIN_1, PIN_DIN_2, PIN_DIN_3, PIN_DIN_4, PIN_DIN_5 };
        int dinregs[5] = { RegDigitaInput1Function, RegDigitaInput2Function, RegDigitaInput3Function, RegDigitaInput4Function, RegDigitaInput5Function};

        for (int i = 0; i < 5; i += 1) {
            if (!gpio_get(dins[i])) continue;

            switch(Registers[dinregs[i]].getValue()) {
                case motor::DigitalInFunction::SelectAnalogIn:
                    val = motor::SpeedReferenceSource::AnalogIn;
                    break;

                case motor::DigitalInFunction::SelectSpeedRefReg:
                    val = motor::SpeedReferenceSource::SpeedRefReg;
                    break;

                case motor::DigitalInFunction::SelectInternalSet1:
                    val = motor::SpeedReferenceSource::InternalSet1;
                    break;

                case motor::DigitalInFunction::SelectInternalSet2:
                    val = motor::SpeedReferenceSource::InternalSet2;
                    break;

                case motor::DigitalInFunction::SelectInternalSet3:
                    val = motor::SpeedReferenceSource::InternalSet3;
                    break;

                case motor::DigitalInFunction::SelectInternalSet4:
                    val = motor::SpeedReferenceSource::InternalSet4;
                    break;

                case motor::DigitalInFunction::SelectInternalSet5:
                    val = motor::SpeedReferenceSource::InternalSet5;
                    break;

                default:
                    continue;
                    break;
            }

            Registers[RegSpeedReferenceSource].setValue(val);
        }

        return 1;
    }

    bool Motor::checkDigitalFunction(DigitalInFunction func, int reg) {
        int val = 0;
        bool used = false;

        if (Registers[RegDigitaInput1Function].getValue() == func) {
            val = gpio_get(PIN_DIN_1);
            used = true;
        }

        if (Registers[RegDigitaInput2Function].getValue() == func) {
            val |= gpio_get(PIN_DIN_2);
            used = true;
        }

        if (Registers[RegDigitaInput3Function].getValue() == func) {
            val |= gpio_get(PIN_DIN_3);
            used = true;
        }

        if (Registers[RegDigitaInput4Function].getValue() == func) {
            val |= gpio_get(PIN_DIN_4);
            used = true;
        }

        if (Registers[RegDigitaInput5Function].getValue() == func) {
            val |= gpio_get(PIN_DIN_5);
            used = true;
        }

        if (used) Registers[reg].forceValue(val);

        return used;
    }

    void Motor::setPWM(int enable, double frequency, double duty) {
        int gpio = PIN_MOTOR_ENABLE;

        uint slice_num = pwm_gpio_to_slice_num(gpio);
        uint chan_num = pwm_gpio_to_channel(gpio);

        // exit early if turning off
        if (enable == 0) {
            pwm_set_enabled(slice_num, false);
            return;
        }

        // Set period of 4 cycles (0 to 3 inclusive)
        // 8ns per cycle
        if (frequency == 0) return;
        frequency = ((1 / frequency) * 1000000000) / 8;
        int cycles = frequency - 1;
        pwm_set_wrap(slice_num, cycles);

        // Set channel A output high for one cycle before dropping
        // number of cycles to stay on, duty cycle
        duty = duty / 100;
        duty = frequency * duty;
        int duty_cycle = duty;
        pwm_set_chan_level(slice_num, chan_num, duty_cycle);

        // Set the PWM running
        pwm_set_enabled(slice_num, true);
    }

    int Motor::getRawFloat(float val) {
        return *(int*)&val;
    }

    void Motor::buildRegisters() {
        std::string name;
        std::string desc;

        name = "Speed Reference Source";
        desc = "R/W - Enum - Set the Speed Reference Source";
        Registers[RegSpeedReferenceSource] = registers::Reg(registers::RegisterType::IntType, name, desc, motor::SpeedReferenceSource::SpeedRefReg);
        
        name = "Speed Reference Value";
        desc = "R/W - Float - Speed reference when SpeedReferenceSource is set to register, -100-100%";
        Registers[RegSpeedReferenceValue] = registers::Reg(registers::RegisterType::FloatType, name, desc, getRawFloat(0.0), 100, -100);
        //Registers[RegSpeedReferenceValue].canSave = false;
        
        name = "Current Limit Value";
        desc = "R/W - Float - Set the current limit for the motor, in Amps";
        Registers[RegCurrentLimitValue] = registers::Reg(registers::RegisterType::FloatType, name, desc, getRawFloat(1.0), getRawFloat(maxCurrent), 0);
        
        name = "Digital Input 1 Function";
        desc = "R/W - Enum - Set the function of Digial Input 1";
        Registers[RegDigitaInput1Function] = registers::Reg(registers::RegisterType::IntType, name, desc, motor::DigitalInFunction::MotorEnable);
        
        name = "Digital Input 2 Function";
        desc = "R/W - Enum - Set the function of Digial Input 2";
        Registers[RegDigitaInput2Function] = registers::Reg(registers::RegisterType::IntType, name, desc, motor::DigitalInFunction::RunForward);
        
        name = "Digital Input 3 Function";
        desc = "R/W - Enum - Set the function of Digial Input 3";
        Registers[RegDigitaInput3Function] = registers::Reg(registers::RegisterType::IntType, name, desc, motor::DigitalInFunction::NA);
        
        name = "Digital Input 4 Function";
        desc = "R/W - Enum - Set the function of Digial Input 4";
        Registers[RegDigitaInput4Function] = registers::Reg(registers::RegisterType::IntType, name, desc, motor::DigitalInFunction::NA);
        
        name = "Digital Input 5 Function";
        desc = "R/W - Enum - Set the function of Digial Input 5";
        Registers[RegDigitaInput5Function] = registers::Reg(registers::RegisterType::IntType, name, desc, motor::DigitalInFunction::NA);
        
        name = "Motor Current";
        desc = "R - Float - Current in Amps the motor is drawing";
        Registers[RegMotorCurrentValue] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, false);
        Registers[RegMotorCurrentValue].canSave = false;
        
        name = "Motor Output Ramp";
        desc = "R/W - Float - Control how fast the speed setpoint ramps to the new reference, in %/sec";
        Registers[RegMotorOutputRate] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(10.0), INT32_MAX, 0);

        name = "Motor Enabled";
        desc = "R/W - Enum - Motor Enable status, 0 = Motor output is off, 1 = Motor output is on";
        Registers[RegMotorEnabled] = registers::Reg(registers::RegisterType::IntType, name, desc, true, true, 0, 1, 0);
        Registers[RegMotorEnabled].canSave = false;

        name = "Motor Enable Source";
        desc = "R/W - Enum - Set the Motor Enable Source";
        Registers[RegMotorEnableSource] = registers::Reg(registers::RegisterType::IntType, name, desc, true, true, motor::MotorEnableSource::MotorEnableReg);

        name = "Internal Speed Reference Setpoint 1";
        desc = "R/W - Float - Internal Speed Reference Setpoint 1";
        Registers[RegInternalSetpoint1] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(0.0), 100, -100);

        name = "Internal Speed Reference Setpoint 2";
        desc = "R/W - Float - Internal Speed Reference Setpoint 2";
        Registers[RegInternalSetpoint2] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(0.0), 100, -100);

        name = "Internal Speed Reference Setpoint 3";
        desc = "R/W - Float - Internal Speed Reference Setpoint 3";
        Registers[RegInternalSetpoint3] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(0.0), 100, -100);

        name = "Internal Speed Reference Setpoint 4";
        desc = "R/W - Float - Internal Speed Reference Setpoint 4";
        Registers[RegInternalSetpoint4] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(0.0), 100, -100);

        name = "Internal Speed Reference Setpoint 5";
        desc = "R/W - Float - Internal Speed Reference Setpoint 5";
        Registers[RegInternalSetpoint5] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(0.0), 100, -100);

        name = "Speed Reference Analog Input";
        desc = "R - Float - Speed reference from Analog Input, -100-100%";
        Registers[RegSpeedReferenceAnalog] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, false, getRawFloat(0.0), 100, -100);
        Registers[RegSpeedReferenceAnalog].canSave = false;

        name = "Speed Reference";
        desc = "R - Float - Speed reference used as the speed setpoint, -100-100%";
        Registers[RegSpeedReference] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, false, getRawFloat(0.0), 100, -100);
        Registers[RegSpeedReference].canSave = false;

        name = "Analog Scale Max";
        desc = "R/W - Float - Voltage level for 100% speed, -10V - 10V";
        Registers[RegSpeedRefAnalogScaleMax] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(10.0), 10, -10);

        name = "Analog Scale Min";
        desc = "R/W - Float - Voltage level for -100% speed, -10V - 10V";
        Registers[RegSpeedRefAnalogScaleMin] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(-10.0), 10, -10);

        name = "Analog Scale Zero";
        desc = "R/W - Float - Voltage level for 0% speed, -10V - 10V";
        Registers[RegSpeedRefAnalogScaleZero] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(0.0), 10, -10);

        name = "Analog Input Voltage";
        desc = "R - Float - Voltage level on the analog input";
        Registers[RegAnalogInVoltage] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, false);
        Registers[RegAnalogInVoltage].canSave = false;

        name = "Analog Input Deadband";
        desc = "R/W - Float - Voltage window around the zero level to be ignored";
        Registers[RegAnalogInDeadband] = registers::Reg(registers::RegisterType::FloatType, name, desc, true, true, getRawFloat(0.0));

        name = "Run Forward Status";
        desc = "R - Bool - Run Forward status, 0 = Not allowed, 1 = Allowed";
        Registers[RegRunForward] = registers::Reg(registers::RegisterType::BoolType, name, desc, true, false, 0);

        name = "Run Reverse Status";
        desc = "R - Bool - Run Reverse status, 0 = Not allowed, 1 = Allowed";
        Registers[RegRunReverse] = registers::Reg(registers::RegisterType::BoolType, name, desc, true, false, 0);

        name = "Forward Inhibit Status";
        desc = "R - Bool - Forward Inhibit status, 0 = Not inhibited, 1 = Inihibited";
        Registers[RegForwardInhibit] = registers::Reg(registers::RegisterType::BoolType, name, desc, true, false, 0);

        name = "Reverse Inhibit Status";
        desc = "R - Bool - Reverse Inhibit status, 0 = Not inhibited, 1 = Inihibited";
        Registers[RegReverseInhibit] = registers::Reg(registers::RegisterType::BoolType, name, desc, true, false, 0);
        
        //name = "";
        //desc = "";
        //Registers[RegSpeedReferenceSource] = registers::Reg(name, desc);
    }
}