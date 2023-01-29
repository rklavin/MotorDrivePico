#pragma once

#include <string>
#include <vector>

namespace console {
    std::vector<std::string> split_command_and_args(int timeout_ms = 0);
}