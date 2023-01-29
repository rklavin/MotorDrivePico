#pragma once

#include <string>

namespace components {
    class component {
        public:
            component(std::string name);
            ~component();

            std::string Name;
            double scale_M = 0;
            double scale_B = 0;

            virtual int init();
            virtual int write(double val);
            virtual double read();
    };
}