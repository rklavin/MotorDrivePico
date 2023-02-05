#pragma once

#include <string>

namespace registers {
    enum RegisterType {
        IntType,
        UintType,
        FloatType,
        BoolType,
    };

    class Reg {
        public:
            Reg();
            Reg(RegisterType type);
            Reg(RegisterType type, std::string name, std::string desc);
            Reg(RegisterType type, std::string name, std::string desc, int def);
            Reg(RegisterType type, std::string name, std::string desc, int def, int max, int min);
            Reg(RegisterType type, std::string name, std::string desc, bool read, bool write);
            Reg(RegisterType type, std::string name, std::string desc, bool read, bool write, int def);
            Reg(RegisterType type, std::string name, std::string desc, bool read, bool write, int def, int max, int min);
            ~Reg();

            std::string name;
            std::string description;

            RegisterType getType();
            bool canRead;
            bool canWrite;
            bool canSave;
            int maxValue;
            int minValue;
            void setValue(int val);
            void setFloat(float val);
            void forceValue(int val);
            void forceFloat(float val);
            int getValue();
            float getFloat();

        private:
            RegisterType _type;
            int _value;
    };

    /*class Registers {
        public:
            Registers();
            ~Registers();

            static const int RegisterCount = 256;

            void setRegister(int reg, int value);
            int getRegister(int reg);

        private:
            Reg Regs[RegisterCount];
    };*/

    //const int RegisterCount = 256;
    //inline int Registers[RegisterCount];
}