#pragma once

#include <map>
#include <string>

#include "Register/register.h"
#include "Components/components.h"
#include "Components/MAX11645.h"
#include "Components/MCP47A1.h"

namespace motor {
    enum MotorStatus {
        idle,
        run,
        brake,
        fault,
        stop,
    };

    enum MotorState {
        init,
        updateSpeedRef,
        updateCurrentLimit,
        updateCurrent,
        updateEnable,
        updateMotorControl,
    };

    enum DigitalInFunction {
        NA,
        MotorEnable,
        RunForward,
        RunReverse,
        ForwardInhibit,
        ReverseInhibit,
        SelectAnalogIn,
        SelectSpeedRefReg,
        SelectInternalSet1,
        SelectInternalSet2,
        SelectInternalSet3,
        SelectInternalSet4,
        SelectInternalSet5,
    };

    enum SpeedReferenceSource {
        AnalogIn,
        SpeedRefReg,
        InternalSet1,
        InternalSet2,
        InternalSet3,
        InternalSet4,
        InternalSet5,
    };

    enum MotorEnableSource {
        DigitalIn,
        MotorEnableReg,
    };

    class Motor {
        public:
            Motor();
            ~Motor();

            std::map<int, registers::Reg> Registers;

            components::MAX11645* ADC = NULL;
            components::MCP47A1* DAC = NULL;
            
            const int RegSpeedReferenceSource       = 0;    // R/W  - Enum - Set the Speed Reference Source
            const int RegSpeedReferenceValue        = 1;    // R/W  - Float - Speed reference when SpeedReferenceSource is set to 1, -100-100%
            const int RegCurrentLimitValue          = 2;    // R/W  - Float - Set the current limit for the motor, in Amps
            const int RegDigitaInput1Function       = 3;    // R/W  - Enum - Set the function of Digial Input 1
            const int RegDigitaInput2Function       = 4;    // R/W  - Enum - Set the function of Digial Input 2
            const int RegDigitaInput3Function       = 5;    // R/W  - Enum - Set the function of Digial Input 3
            const int RegDigitaInput4Function       = 6;    // R/W  - Enum - Set the function of Digial Input 4
            const int RegDigitaInput5Function       = 7;    // R/W  - Enum - Set the function of Digial Input 5
            const int RegMotorCurrentValue          = 8;    // R    - Float - Current in Amps the motor is drawing
            const int RegMotorOutputRate            = 9;    // R/W  - Float - Control how fast the speed setpoint ramps to the new reference, in %/sec
            const int RegMotorEnabled               = 10;   // R/W  - Bool - Motor Enable status, 0 = Motor output is off, 1 = Motor output is on
            const int RegMotorEnableSource          = 11;   // R/W  - Enum - Set the Motor Enable Source
            const int RegInternalSetpoint1          = 12;   // R/W  - Float - Internal Speed Reference Setpoint 1
            const int RegInternalSetpoint2          = 13;   // R/W  - Float - Internal Speed Reference Setpoint 2
            const int RegInternalSetpoint3          = 14;   // R/W  - Float - Internal Speed Reference Setpoint 3
            const int RegInternalSetpoint4          = 15;   // R/W  - Float - Internal Speed Reference Setpoint 4
            const int RegInternalSetpoint5          = 16;   // R/W  - Float - Internal Speed Reference Setpoint 5
            const int RegSpeedReferenceAnalog       = 17;   // R    - Float - Speed reference from Analog Input, -100-100%
            const int RegSpeedReference             = 18;   // R    - Float - Speed reference used as the speed setpoint, -100-100%
            const int RegSpeedRefAnalogScaleMax     = 19;   // R/W  - Float - Voltage level for 100% speed, -10V - 10V
            const int RegSpeedRefAnalogScaleMin     = 20;   // R/W  - Float - Voltage level for -100% speed, -10V - 10V
            const int RegSpeedRefAnalogScaleZero    = 21;   // R/W  - Float - Voltage level for 0% speed, -10V - 10V
            const int RegAnalogInVoltage            = 22;   // R    - Float - Voltage level on the analog input
            const int RegAnalogInDeadband           = 23;   // R/W  - Float - Voltage window around the zero level to be ignored
            const int RegRunForward                 = 24;   // R    - Bool - Run Forward status, 0 = Not allowed, 1 = Allowed
            const int RegRunReverse                 = 25;   // R    - Bool - Run Reverse status, 0 = Not allowed, 1 = Allowed
            const int RegForwardInhibit             = 26;   // R    - Bool - Forward Inhibit status, 0 = Not inhibited, 1 = Inihibited
            const int RegReverseInhibit             = 27;   // R    - Bool - Reverse Inhibit status, 0 = Not inhibited, 1 = Inihibited
            const int RegDigitalInput1              = 28;   // R    - Bool - Status of Digital Input 1, 0 = Off, 1 = On
            const int RegDigitalInput2              = 29;   // R    - Bool - Status of Digital Input 2, 0 = Off, 1 = On
            const int RegDigitalInput3              = 30;   // R    - Bool - Status of Digital Input 3, 0 = Off, 1 = On
            const int RegDigitalInput4              = 31;   // R    - Bool - Status of Digital Input 4, 0 = Off, 1 = On
            const int RegDigitalInput5              = 32;   // R    - Bool - Status of Digital Input 5, 0 = Off, 1 = On

            int init();
            void update();
            void resetRegisters();
            int saveRegisters();
            int loadRegisters();
            MotorStatus getStatus();
            MotorState getState();

            const float maxCurrent = (3.3 / 910.0) * 1000.0;

        private:
            MotorStatus status;
            MotorState state;

            int getRawFloat(float val);
            void buildRegisters();
            void updateRefAnalog();
            void updateRef();
            void updateCurrentLimit();
            void updateCurrent();
            void updateEnable();
            void updateMotorControl();
            void updateIO();

            int checkDigitalInMotorEnable();
            int checkDigitalInReferenceSource();
            // Returns true if any digital inputs are assigned the function in func
            // Forces the result of the digital inputs into register reg
            bool checkDigitalFunction(DigitalInFunction func, int reg);
            void setPWM(int enable, double frequency, double duty);
    };
}