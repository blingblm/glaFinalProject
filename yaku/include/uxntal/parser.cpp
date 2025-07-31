#include "parser.hpp"
#include <algorithm>
#include <stdexcept>
#include <regex>
#include <vector>

namespace uxntal
{
    // Forward declaration from tokens.cpp
    Token toInstrToken(std::string op_name_str);

    std::string stripComments(const std::string &text) {
        std::string result;
        int parens_count = 0;
        bool in_string = false;

        std::string processed_text = text;

        // std::cout << processed_text << std::endl;

        char prev_ch = '\0';

        for (size_t i = 0; i < processed_text.length(); ++i) {
            char ch = processed_text[i];
            char next_ch = (i + 1 < processed_text.length()) ? processed_text[i + 1] : '\0';

            if (ch == '"') {
                in_string = !in_string;
            }
            
            bool is_comment_start = (!in_string && (prev_ch == ' ' || prev_ch == '\n' || prev_ch == '\0') && ch == '(' &&( next_ch == ' ' || next_ch == '\n'));
            bool is_comment_end = (!in_string && ch == ')' && (prev_ch == ' ' || next_ch == ' ' || prev_ch == '\n' || next_ch == '\n' || next_ch == '\0'));

            // std::cout << ch << ':' << is_comment_start << ' ' << is_comment_end << std::endl;

            if (is_comment_start) {
                parens_count++;
                continue;
            } else if (is_comment_end) {
                if (parens_count > 0) {
                    parens_count--;
                }
                continue;
            }

            if (parens_count == 0) {
                result += ch;
            }
            prev_ch = ch;
        }

        if (parens_count > 0) {
            throw std::runtime_error("Error: Unclosed comment block. Missing ')'");
        }
        
        std::replace(result.begin(), result.end(), '\n', ' ');
        std::replace(result.begin(), result.end(), '\t', ' ');

        std::regex multiple_spaces("\\s+");
        std::string cleaned_result = std::regex_replace(result, multiple_spaces, " ");

        size_t first_char = cleaned_result.find_first_not_of(' ');
        if (std::string::npos == first_char) {
            return "";
        }
        size_t last_char = cleaned_result.find_last_not_of(' ');
        return cleaned_result.substr(first_char, (last_char - first_char + 1));
    }


    std::unordered_map<std::string, std::vector<int>> getLinesForTokens(const std::string &text)
    {
        std::unordered_map<std::string, std::vector<int>> map;
        std::istringstream iss(text);
        std::string line;
        int lineno = 1;
        while (std::getline(iss, line))
        {
            std::istringstream line_stream(line);
            std::string word;
            while (line_stream >> word)
            {
                map[word].push_back(lineno);
            }
            ++lineno;
        }
        return map;
    }

    static std::vector<Token> parseTokenString(const std::string& tokenStr, Uxn& uxn) {
        std::vector<Token> result_tokens;

        if (tokenStr.empty() || tokenStr == "[" || tokenStr == "]") {
            return result_tokens;
        }

        if (tokenStr[0] == '~') {
            result_tokens.push_back(makeToken(TokenType::INCLUDE, 0, 0, 0, 0, tokenStr.substr(1)));
            return result_tokens;
        }

        if (tokenStr[0] == '%') {
            throw std::runtime_error("Error: Macros are not yet supported.");
        }

        if (tokenStr[0] == '#') {
            std::string hexStr = tokenStr.substr(1);
            if (hexStr.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos) {
                throw std::runtime_error("Error: Constant " + tokenStr + " contains invalid hex digit.");
            }
            if (hexStr.length() != 2 && hexStr.length() != 4) {
                throw std::runtime_error("Error: Constant " + tokenStr + " must be two or four hex digits.");
            }
            result_tokens.push_back(makeToken(TokenType::LIT, std::stoi(hexStr, nullptr, 16), (hexStr.length() == 2) ? 1 : 2, 0, 0, tokenStr));
            return result_tokens;
        }

        if (tokenStr.front() == '"') {
            std::string content = tokenStr.substr(1, tokenStr.length() > 1 ? tokenStr.length() - 2 : 0);
            for(char c : content) {
                result_tokens.push_back(makeToken(TokenType::RAW, static_cast<int64_t>(c), 1, 0, 0, ""));
            }
            return result_tokens;
        }

        if (tokenStr[0] == '@') {
            result_tokens.push_back(makeToken(TokenType::LABEL, 2, 0, 0, 0, tokenStr.substr(1)));
            return result_tokens;
        }
        if (tokenStr[0] == '&') {
            result_tokens.push_back(makeToken(TokenType::LABEL, 1, 0, 0, 0, tokenStr.substr(1)));
            return result_tokens;
        }

        if (tokenStr == "|0100" || tokenStr == "|100") {
            result_tokens.push_back(makeToken(TokenType::MAIN, 0, 1, 0, 0, tokenStr));
            return result_tokens;
        }
        if (tokenStr[0] == '|') {
            std::string addrStr = tokenStr.substr(1);
            if (addrStr.empty()) throw std::runtime_error("Invalid absolute padding token: " + tokenStr);
            result_tokens.push_back(makeToken(TokenType::ADDR, std::stoi(addrStr, nullptr, 16), 2, 0, 0, tokenStr));
            return result_tokens;
        }

        if (tokenStr[0] == '$') {
            std::string padStr = tokenStr.substr(1);
            if (padStr.empty()) throw std::runtime_error("Invalid relative padding token: " + tokenStr);
            result_tokens.push_back(makeToken(TokenType::PAD, std::stoi(padStr, nullptr, 16), 1, 0, 0, tokenStr));
            return result_tokens;
        }

        if (tokenStr == "?{") {
            int lambda_count = uxn.lambdaCount++;
            uxn.lambdaStack.push_back(lambda_count);
            result_tokens.push_back(toInstrToken("JCI2"));
            result_tokens.push_back(makeToken(TokenType::REF, 6, 1, 0, 0, std::to_string(lambda_count) + "_LAMBDA"));
            return result_tokens;
        }
        if (tokenStr[0] == '?') {
            std::string val = tokenStr.substr(1);
            if (val.empty()) throw std::runtime_error("Invalid conditional jump token: " + tokenStr);
            int is_child = (val.find('&') != std::string::npos || val.find('/') != std::string::npos);
            if(is_child) val.erase(std::remove_if(val.begin(), val.end(), [](char c){ return c == '&' || c == '/'; }), val.end());
            result_tokens.push_back(toInstrToken("JCI2"));
            result_tokens.push_back(makeToken(TokenType::REF, 6, is_child, 0, 0, val));
            return result_tokens;
        }
        if (tokenStr[0] == '!') {
            std::string val = tokenStr.substr(1);
            if (val.empty()) throw std::runtime_error("Invalid jump token: " + tokenStr);
            int is_child = (val.find('&') != std::string::npos || val.find('/') != std::string::npos);
            if(is_child) val.erase(std::remove_if(val.begin(), val.end(), [](char c){ return c == '&' || c == '/'; }), val.end());
            result_tokens.push_back(toInstrToken("JMI2"));
            result_tokens.push_back(makeToken(TokenType::REF, 6, is_child, 0, 0, val));
            return result_tokens;
        }

        if (tokenStr == "{") {
            int lambda_count = uxn.lambdaCount++;
            uxn.lambdaStack.push_back(lambda_count);
            result_tokens.push_back(toInstrToken("JSI2"));
            result_tokens.push_back(makeToken(TokenType::REF, 6, 1, 0, 0, std::to_string(lambda_count) + "_LAMBDA"));
            return result_tokens;
        }
        if (tokenStr == "}") {
            if (uxn.lambdaStack.empty()) throw std::runtime_error("Unmatched '}' macro.");
            int lambda_count = uxn.lambdaStack.back();
            uxn.lambdaStack.pop_back();
            result_tokens.push_back(makeToken(TokenType::LABEL, 1, 0, 0, 0, std::to_string(lambda_count) + "_LAMBDA"));
            return result_tokens;
        }

        static const std::regex instr_regex("^[A-Z]{3}[2kr]*$");
        if (std::regex_match(tokenStr, instr_regex)) {
            std::string instr_name = tokenStr.substr(0, 3);
            if (opcodeIdx.count(instr_name)) {
                result_tokens.push_back(toInstrToken(tokenStr));
                return result_tokens;
            }
        }
        
        static const std::regex hex_regex("^[a-f0-9]+$");
        if(std::regex_match(tokenStr, hex_regex)) {
            if(tokenStr.length() == 2 || tokenStr.length() == 4) {
                result_tokens.push_back(makeToken(TokenType::RAW, std::stoi(tokenStr, nullptr, 16), tokenStr.length() / 2, 0, 0, tokenStr));
                return result_tokens;
            } else {
                 throw std::runtime_error("Invalid raw hex number (must be 2 or 4 digits): " + tokenStr);
            }
        }

        result_tokens.push_back(toInstrToken("JSI2"));
        result_tokens.push_back(makeToken(TokenType::REF, 6, 0, 0, 0, tokenStr));
        return result_tokens;
    }


    std::vector<Token> parseUxntalProgram(const std::string &programFile, Uxn &uxn)
    {
        std::ifstream file(programFile);
        if (!file.is_open()) {
            throw std::runtime_error("Can't open file: " + programFile);
        }
        std::string programText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        std::unordered_map<std::string, std::vector<int>> linesMap = getLinesForTokens(programText);
        uxn.linesForToken.insert(linesMap.begin(), linesMap.end());
        
        std::string textNoComments = stripComments(programText);

        std::istringstream iss(textNoComments);
        std::string tokenStr;
        std::vector<Token> tokenList;

        while (iss >> tokenStr) {
            auto parsed_tokens = parseTokenString(tokenStr, uxn);
            for (const auto& token : parsed_tokens) {
                if (token.type == TokenType::MAIN) {
                    uxn.hasMain = true;
                }
                tokenList.push_back(token);
            }
        }

        std::vector<Token> mergedTokenList;
        for (const auto& token : tokenList) {
            if (token.type == TokenType::INCLUDE) {
                auto included_tokens = parseUxntalProgram(token.name, uxn);
                if (included_tokens.empty() && !token.name.empty()) {
                    std::cerr << "Warning: Included file is empty or failed to parse: " << token.name << std::endl;
                }
                mergedTokenList.insert(mergedTokenList.end(), included_tokens.begin(), included_tokens.end());
            } else {
                mergedTokenList.push_back(token);
            }
        }

        return mergedTokenList;
    }
}