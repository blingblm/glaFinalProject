#include "parser.hpp"
#include <algorithm>
#include <stdexcept>

namespace uxntal
{
    std::vector<Token> parseUxntalText(const std::string &input);
    std::string stripComments(const std::string &text)
    {
        std::string result;
        bool in_paren_comment = false;

        for (size_t i = 0; i < text.size(); ++i)
        {
            char c = text[i];

            if (in_paren_comment)
            {
                if (c == ')')
                {
                    in_paren_comment = false;
                    result += ' ';
                }
                continue;
            }

            if (c == '(')
            {
                in_paren_comment = true;
                result += ' ';
                continue;
            }

            result += c;
        }

        return result;
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
        // std::cout << "lines map:\n";
        // for (const auto& [key, lines] : map) {
        //     std::cout << key << " : ";
        //     for (auto n : lines) {
        //         std::cout << n << " ";
        //     }
        //     std::cout << "\n";
        // }
        return map;
    }

    std::vector<Token> parseUxntalProgram(const std::string &Text, Uxn &uxn)
    {
        std::cout << "Program text size: " << Text.size() << std::endl;
        std::unordered_map<std::string, std::vector<int>> linesMap = getLinesForTokens(Text);
        uxn.linesForToken.insert(linesMap.begin(), linesMap.end());
        std::string TextNoComments = stripComments(Text);
        // std::cout << "Text: " << TextNoComments << std::endl;

        return parseUxntalText(TextNoComments);
    }

    std::vector<Token> parseUxntalText(const std::string &input)
    {
        std::istringstream iss(input);
        std::string token;
        std::vector<Token> tokens;
        bool isString = false;
        while (iss >> token)
        {
            if (token.empty())
                continue;
            if (token[0] == '#')
            {
                int v = std::stoi(token.substr(1), nullptr, 16);
                int sz = token.length() == 3 ? 1 : 2;
                tokens.push_back(makeToken(TokenType::LIT, v, sz, 0, 0, token));
            }
            else if (opcodeIdx.count(token))
            {
                tokens.push_back(makeToken(TokenType::INSTR, opcodeIdx.at(token), 1, 0, 0, token));
            }
            else if (token == "|0100")
            {
                tokens.push_back(makeToken(TokenType::MAIN, 0, 1, 0, 0, token));
            }
            else if (token[0] == '@')
            {
                tokens.push_back(makeToken(TokenType::LABEL, 0, 2, 0, 0, token.substr(1)));
            }
            else if (token[0] == '&')
            {
                tokens.push_back(makeToken(TokenType::LABEL, 0, 1, 0, 0, token.substr(1)));
            }
            else if (token[0] == '|')
            {
                tokens.push_back(makeToken(TokenType::ADDR, 2, std::stoi(token.substr(1), nullptr, 16), 2));
            }
            else if (token[0] == '$')
            {
                tokens.push_back(makeToken(TokenType::PAD, 0, std::stoi(token.substr(1), nullptr, 16), 1));
            }
            else if (token[0] == ',')
            {
                std::string labelName = token.substr(1);
                tokens.push_back(makeToken(TokenType::LABEL, 0, 1, 0, 0, labelName));
            }
            else
            {
                tokens.push_back(makeToken(TokenType::UNKNOWN, 0, 0, 0, 0, token));
            }
        }

        return tokens;
    }

}
