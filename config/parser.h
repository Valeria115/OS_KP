#pragma once
#include <string>

namespace Parser {
    bool load_config(const std::string& filename, int &max_concurrent);
}
