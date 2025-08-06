#include "parser.hpp"
#include "tokens.hpp"
#include "definitions.hpp"
#include <algorithm>
#include <stdexcept>
#include <regex>
#include <vector>
#include <cstring>

using namespace uxntal;
namespace uxntal
{

    void storeToken(Uxn &uxn, const Token &token) {
        unsigned int byte;
        sscanf(token.name.c_str(), "%2x", &byte);
        if (uxn.programCounter >= 0x10000) throw std::runtime_error("Memory overflow");
        uxn.memory.push_back(static_cast<unsigned char>(byte));
        uxn.programCounter++;
    }


    void resolveSymbols(Uxn &uxn);

    void populateMemoryAndBuildSymbolTable(Uxn &uxn, const std::vector<Token> &tokens) {
        for (size_t i = 0; i < tokens.size(); ++i) {
            const Token &token = tokens[i];

            if (isParentLabel(token)) {
                std::string label = token.name.substr(1);
                uxn.symbolTable[label] = uxn.programCounter;
                uxn.symbolTableLabels[label] = {uxn.programCounter, token};
                continue;
            }

            if (isParentRef(token)) {
                std::string label = token.name.substr(1);
                uxn.deferredSymbols[label].push_back(uxn.programCounter);
                uxn.symbolTableRefs[label].first.push_back(uxn.programCounter);
                uxn.symbolTableRefs[label].second = token.name; 
                

                uxn.memory.push_back(0x00);
                uxn.memory.push_back(0x00);
                uxn.programCounter += 2;
                continue;
            }

            storeToken(uxn, token);
        }
    }

    void tokensToMemory(Uxn& uxn, const std::vector<Token>& tokens) {
        populateMemoryAndBuildSymbolTable(uxn, tokens);
        resolveSymbols(uxn);

        for (const auto& [symbol, labelInfo] : uxn.symbolTableLabels) {
            int pc = labelInfo.first;
            const Token& token = labelInfo.second;

            if (isParentLabel(token)) {
                uxn.reverseSymbolTable[pc] = {token.name, 1};
            }
        }

        for (const auto& [symbol, refInfo] : uxn.symbolTableRefs) {
            const std::vector<int>& pcs = refInfo.first;

            const std::string& token_name = refInfo.second;
            Token temp_token; 
            temp_token.name = token_name;

            if (isParentRef(temp_token)) {
                for (int pc : pcs) {
                    bool isJSR2 = false, isJMP2 = false;

                    if (pc >= 1 && uxn.memory[pc - 1] == 0x2d) isJSR2 = true; // JSI

                    if (pc >= 1 && uxn.memory[pc - 1] == 0x2c) isJMP2 = true; // JMI


                    if (isJSR2) {
                        uxn.reverseSymbolTable[pc] = {token_name, 2};
                    } else if (isJMP2) {
                        uxn.reverseSymbolTable[pc] = {token_name, 3};
                    } else {
                        uxn.reverseSymbolTable[pc] = {token_name, 0};
                    }
                }
            }
        }
    }
Token loadToken(int word_sz, const Uxn& uxn, size_t addr) {
    const std::vector<uint8_t>& memory = uxn.memory;

    if (addr + 12 > memory.size()) {
        throw std::out_of_range("loadToken: address out of range");
    }

    Token token;
    token.type = static_cast<TokenType>(memory[addr]);

    // 这里按照你之前的设计，val是8字节，从addr+1开始小端读取
    std::memcpy(&token.val, &memory[addr + 1], sizeof(int64_t));

    token.sz = memory[addr + 9];
    token.r  = memory[addr + 10];
    token.k  = memory[addr + 11];

    // 读取instr字符串（从addr+12开始，直到遇到\0）
    size_t pos = addr + 12;
    size_t start_instr = pos;
    while (pos < memory.size() && memory[pos] != '\0') pos++;
    if (pos >= memory.size()) throw std::runtime_error("loadToken: instr string not terminated");
    token.instr = std::string(memory.begin() + start_instr, memory.begin() + pos);

    // 读取name字符串（跳过instr末尾的\0后，继续读直到遇到下一个\0）
    pos++; // 跳过instr的'\0'
    size_t start_name = pos;
    while (pos < memory.size() && memory[pos] != '\0') pos++;
    if (pos >= memory.size()) throw std::runtime_error("loadToken: name string not terminated");
    token.name = std::string(memory.begin() + start_name, memory.begin() + pos);

    // 如果你需要根据 word_sz 对 token.val 或其他字段做不同处理，可以在这里添加逻辑

    return token;
}
    void resolveSymbols(Uxn &uxn) {
        for (const auto &[label, addresses] : uxn.deferredSymbols) {
            auto it = uxn.symbolTable.find(label);
            if (it == uxn.symbolTable.end()) {
                throw std::runtime_error("Unresolved symbol: " + label);
            }

            int address = it->second;
            for (int addr : addresses) {
                if (addr + 1 >= uxn.memory.size()) {
                    throw std::runtime_error("Memory overflow on resolving symbol");
                }
                uxn.memory[addr] = (address >> 8) & 0xFF;
                uxn.memory[addr + 1] = address & 0xFF;
            }
        }
    }
}