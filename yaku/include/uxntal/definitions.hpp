#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <tuple>

namespace uxntal
{

    enum class TokenType : int
    {
        MAIN = 0,
        LIT = 1,
        INSTR = 2,
        LABEL = 3,
        REF = 4,
        IREF = 5,
        RAW = 6,
        ADDR = 7,
        PAD = 8,
        LAMBDA = 9,
        EMPTY = 10,
        UNKNOWN = 11,
        PLACEHOLDER = 12,
        INCLUDE = 13,
        STR = 14,
        LD = 15,
        ST = 16
    };

    inline const std::unordered_map<char, int> refTypes = {
        {'.', 0}, {',', 1}, {';', 2}, {'-', 3}, {'_', 4}, {'=', 5}, {'I', 6}};

    inline const std::array<const char *, 32> opcodes{
        "LIT", "INC", "POP", "NIP", "SWP", "ROT", "DUP", "OVR",
        "EQU", "NEQ", "GTH", "LTH", "JMP", "JCN", "JSR", "STH",
        "LDZ", "STZ", "LDR", "STR", "LDA", "STA", "DEI", "DEO",
        "ADD", "SUB", "MUL", "DIV", "AND", "ORA", "EOR", "SFT"};

    inline std::unordered_map<std::string, int> opcodeIdx = []
    {
        std::unordered_map<std::string, int> m;
        for (size_t i = 0; i < opcodes.size(); ++i)
        {
            m[opcodes[i]] = i;
        }
        m["BRK"] = 0x00;
        m["LIT"] = 0x80;
        m["JCI"] = 0x20;
        m["JMI"] = 0x40;
        m["JSI"] = 0x60;
        return m;
    }();

    inline const std::unordered_map<std::string, std::pair<int, int>> stack_operations = {
        {"POP", {1, 0}}, {"NIP", {2, 1}}, {"SWP", {2, 2}},
        {"ROT", {3, 3}}, {"DUP", {1, 2}}, {"OVR", {2, 3}},
        {"STH", {1, 0}}};

    inline const std::unordered_map<std::string, std::pair<int, int>> cmp_operations = {
        {"EQU", {2, 1}}, {"NEQ", {2, 1}}, {"GTH", {2, 1}}, {"LTH", {2, 1}}};

    inline const std::unordered_map<std::string, std::tuple<int, int, int>> mem_operations = {
        {"LDA", {2, 0, 1}}, {"STA", {2, 1, 0}}, {"LDR", {1, 0, 1}},
        {"STR", {1, 1, 0}}, {"LDZ", {1, 0, 1}}, {"STZ", {1, 1, 0}}};

    inline const std::unordered_map<std::string, bool> commutative_binary_ops = {
        {"ADD", true}, {"MUL", true}, {"AND", true}, {"ORA", true},
        {"EOR", true}, {"NEQ", true}, {"EQU", true}};

    inline const std::unordered_map<std::string, bool> alu_ops = {
        {"INC", true}, {"DIV", true}, {"SUB", true}, {"SFT", true},
        {"LTH", true}, {"GTH", true}, {"ADD", true}, {"MUL", true},
        {"AND", true}, {"ORA", true}, {"EOR", true}, {"NEQ", true},
        {"EQU", true}};
}