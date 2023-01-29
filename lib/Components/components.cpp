#include "components.h"

namespace components {
    component::component(std::string name) {
        Name = name;
    }

    component::~component() {

    }

    int component::init() {
        return 1;
    }

    int component::write(double val) {
        return 0;
    }

    double component::read() {
        return 0;
    }
}