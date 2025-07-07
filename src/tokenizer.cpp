#include "tokenizer.hpp"
#include <pcre2.h>
#include <iostream>
#include <cctype>
#include <algorithm>
namespace yaku
{

    Tokenizer::Tokenizer() : regex_(nullptr), match_data_(nullptr)
    {
        compile_regex();
        std::cout << "run successfully";
    }

    Tokenizer::~Tokenizer()
    {
        if (match_data_)
            pcre2_match_data_free(match_data_);
        if (regex_)
            pcre2_code_free(regex_);
    }

    void Tokenizer::compile_regex()
    {
        int errornumber;
        PCRE2_SIZE erroroffset;
        const char *pattern =
            R"((^\|[0-9a-fA-F]{4})|)"  // ADDR
            R"((@[a-zA-Z0-9_]+))"      // LABEL
            R"((&[a-zA-Z0-9_]+))"      // REF
            R"((#[0-9a-fA-F]+)|)"      // LIT
            R"(([A-Z][A-Z0-9]{1,3})|)" // INSTR
            R"(([0-9a-fA-F]{2,4})|)"   // RAW
            R"((\s+))";                // EMPTY

        regex_ = pcre2_compile(
            reinterpret_cast<PCRE2_SPTR>(pattern),
            PCRE2_ZERO_TERMINATED,
            0, &errornumber, &erroroffset,
            nullptr);
        match_data_ = pcre2_match_data_create_from_pattern(regex_, nullptr);
    }

    TokenList Tokenizer::tokenize(const std::string &source)
    {
        TokenList tokens;
        size_t offset = 0;
        size_t line = 1;
        size_t line_start = 0;

        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data_);
        const char *src = source.c_str();
        size_t src_length = source.length();

        while (offset < src_length)
        {
            int rc = pcre2_match(
                regex_,
                reinterpret_cast<PCRE2_SPTR>(src + offset),
                src_length - offset,
                0,
                0,
                match_data_,
                nullptr);

            if (rc <= 0)
            {
                if (rc == PCRE2_ERROR_NOMATCH)
                {
                    offset++;
                    continue;
                }
            }
            size_t match_start = offset + ovector[0];
            size_t match_end = offset + ovector[1];

            // 更新行号
            for (size_t i = offset; i < match_start; ++i)
            {
                if (src[i] == '\n')
                {
                    line++;
                    line_start = i + 1;
                }
            }

            // 获取匹配的文本
            std::string token_str(src + match_start, src + match_end);

            // 跳过空白和注释
            if (token_str.empty() || token_str[0] == ';' || isspace(token_str[0]))
            {
                // 更新行号
                for (size_t i = match_start; i < match_end; ++i)
                {
                    if (src[i] == '\n')
                    {
                        line++;
                        line_start = i + 1;
                    }
                }
                offset = match_end;
                continue;
            }

            // 确定token类型
            TokenType type = determine_token_type(token_str);

            // 计算列号
            size_t column = match_start - line_start + 1;

            // 添加到token列表
            tokens.push_back({type, token_str, line, column});
            offset = match_end;
        }

        return tokens;
    }

    TokenType Tokenizer::determine_token_type(const std::string &token)
    {
        if (token.empty())
        {
            return TokenType::EMPTY;
        }

        switch (token[0])
        {
        case '|':
            return TokenType::ADDR; // 地址声明
        case '@':
            return TokenType::LABEL; // 全局标签
        case '&':
            return TokenType::REF; // 局部标签
        case '#':
            return TokenType::LIT; // 字面量

        default:
            bool isUpperORDigital = true;
            // all upper?
            for (char c : token)
            {
                if (!isupper(c) && !isdigit(c))
                {
                    isUpperORDigital = false;
                }
            }
            if (isUpperORDigital)
            {
                return TokenType::INSTR;
            }

            // all hex?
            bool isALLHex = true;
            for (char c : token)
            {
                if (!isxdigit(c))
                {
                    isALLHex = false;
                }
            }
            if (isALLHex)
            {
                return (token.length() <= 2) ? TokenType::RAW : TokenType::MAIN;
            }
            return TokenType::EMPTY;
        }
    }

}