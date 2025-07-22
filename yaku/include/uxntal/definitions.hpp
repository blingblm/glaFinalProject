#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <unordered_map>

namespace uxntal
{

    enum class TokenType : int
    {
        MAIN = 0, LIT, INSTR, LABEL, REF, IREF,RAW,ADDR,PAD,EMPTY,UNKNOWN
    };

    inline const std::array<const char *, 7> refChars{".", ",", ";", "-", "_", "=", "I"};
    inline const std::array<int, 7> refWordSz{1, 1, 2, 1, 1, 1, 2};

    inline const std::array<const char *, 33> opcodes{
        "LIT", "INC", "POP", "NIP", "SWP", "ROT", "DUP", "OVR", "EQU", "NEQ", "GTH", "LTH", "JMP", "JCN", "JSR", "STH", "LDZ", "STZ", "LDR", "STR", "LDA", "STA", "DEI", "DEO",
        "ADD", "SUB", "MUL", "DIV", "AND", "ORA", "EOR", "SFT", "BRK"};

    inline const std::unordered_map<std::string, int> opcodeIdx = []
    {
        std::unordered_map<std::string, int> m;
        for (size_t i = 0; i < opcodes.size(); ++i)
            m[opcodes[i]] = i;
        return m;
    }();

    inline const std::unordered_map<std::string, int> immediateOpcode = {
        {"BRK", 0x00}, {"LIT", 0x80}, {"JCI", 0x20}, {"JMI", 0x40}, {"JSI", 0x60}};

    inline const std::unordered_map<std::string, std::pair<int, int>> stack_operations = {
        {"POP", {1, 0}}, {"NIP", {2, 1}}, {"SWP", {2, 2}}, {"ROT", {3, 3}}, {"DUP", {1, 2}}, {"OVR", {2, 3}}, {"STH", {1, 0}},{"INC",{1, 1}},{"DEO", {2, 0}},{"DEI", {1, 1}}, {"ADD ",{2, 1}},{"SUB", {2, 1}},
        {"MUL",{2, 1}},{"DIV",{2, 1}},{"AND",{2, 1}},{"ORA",{2, 1}},{"EOR",{2, 1}},{"SFT",{2, 1}}};

    inline const std::unordered_map<std::string, std::pair<int, int>> cmp_operations = {
        {"EQU", {2, 1}}, {"NEQ", {2, 1}}, {"GTH", {2, 1}}, {"LTH", {2, 1}}};

    inline const std::unordered_map<std::string, std::tuple<int, int, int>> mem_operations = {
        {"LDA", {2, 0, 1}}, {"STA", {2, 1, 0}}, {"LDR", {1, 0, 1}}, {"STR", {1, 1, 0}}, {"LDZ", {1, 0, 1}}, {"STZ", {1, 1, 0}}};

    inline const std::unordered_map<std::string, bool> commutative_binary_ops = {
        {"ADD", true}, {"MUL", true}, {"AND", true}, {"ORA", true}, {"EOR", true}, {"NEQ", true}, {"EQU", true}};

    inline const std::unordered_map<std::string, bool> alu_ops = {
        {"INC", true}, {"DIV", true}, {"SUB", true}, {"SFT", true}, {"LTH", true}, {"GTH", true}};

}