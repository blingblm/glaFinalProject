#include "common.hpp"
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
namespace yaku {

class Tokenizer {
public:
    Tokenizer();
    ~Tokenizer();
    TokenList tokenize(const std::string& source);

private:
    pcre2_code* regex_;
    pcre2_match_data* match_data_;
    void compile_regex();
    TokenType determine_token_type(const std::string &token);

};

} 


