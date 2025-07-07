#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <pcre2.h>
#include <stdexcept>
namespace yaku {
using Byte = uint8_t;
using Word = uint16_t;
using Address = uint16_t;
enum class TokenType 
{LIT, INSTR ,LABEL ,REF ,RAW ,ADDR ,EMPTY,MAIN};
struct Token 
{
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
};//定义token

using TokenList = std::vector<Token>;

} 


