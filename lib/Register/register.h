#pragma once

#include <string>

namespace registers {
    class Reg {
        public:
            Reg();
            Reg(std::string name, std::string desc);
            Reg(std::string name, std::string desc, int def);
            Reg(std::string name, std::string desc, int def, int max, int min);
            Reg(std::string name, std::string desc, bool read, bool write);
            Reg(std::string name, std::string desc, bool read, bool write, int def);
            Reg(std::string name, std::string desc, bool read, bool write, int def, int max, int min);
            ~Reg();

            std::string name;
            std::string description;

            bool canRead();
            bool canWrite();
            void setValue(int val);
            void setFloat(float val);
            void forceValue(int val);
            void forceFloat(float val);
            int getValue();
            float getFloat();
            int getMaxValue();
            int getMinValue();

        private:
            int _value;
            int _maxValue;
            int _minValue;
            bool _canRead;
            bool _canWrite;

            // TODO: add data type, e.g. int, float, ...
            // TODO: add saveable flag, if false will not save to disk during saves
    };

    class Registers {
        public:
            Registers();
            ~Registers();

            static const int RegisterCount = 256;

            void setRegister(int reg, int value);
            int getRegister(int reg);

        private:
            Reg Regs[RegisterCount];
    };

    //const int RegisterCount = 256;
    //inline int Registers[RegisterCount];
}