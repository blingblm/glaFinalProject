#pragma once
#include "definitions.hpp"
#include <string>

namespace uxntal {

struct Token {
    TokenType type;
    int64_t   val = 0;
    int       sz  = 1;
    int       r   = 0;
    int       k   = 0;
    std::string name;
};

} // namespace uxntal