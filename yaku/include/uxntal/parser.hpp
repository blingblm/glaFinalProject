#pragma once

#include "tokens.hpp"
#include "definitions.hpp"

#include <sstream>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

namespace uxntal {

struct Uxn {
    std::unordered_map<std::string, std::vector<int>> linesForToken;
    bool hasMain = false;
    int lambdaCount = 0;
    std::vector<int> lambdaStack;
};

std::string stripComments(const std::string &text);
std::unordered_map<std::string, std::vector<int>> getLinesForTokens(const std::string& text);

inline Token makeToken(TokenType type, int64_t val, int sz = 1, int r = 0, int k = 0,const std::string &name = "")
{
    Token token;
    token.type = type;
    token.val = val;
    token.sz = sz;
    token.r = r;
    token.k = k;
    token.name = name;
    return token;
}

std::vector<Token> parseUxntalProgram(const std::string& programFile, Uxn& uxn);


}