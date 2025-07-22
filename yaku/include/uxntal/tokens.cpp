#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <variant> 
#include <algorithm> 
#include "tokens.hpp"
#include "parser.hpp"
namespace uxntal{
std::pair<TokenType, int> getTokenTypeAndMode(const std::vector<Token>& tokens, int idx) {
    if (idx < 0 || idx >= tokens.size()) {
        throw std::out_of_range("Index out of bounds in getTokenTypeAndMode");
    }
    const Token& token = tokens[idx];
    int mode = token.sz * 4 + token.r * 2 + token.k;
    return {token.type, mode};
}
bool sameWordSzAndStack(const Token& t1, const Token& t2) {
    return (t1.sz == t2.sz && t1.r == t2.r);
}
inline bool operator==(const Token& lhs, const Token& rhs) {
    return lhs.type == rhs.type &&
           lhs.val == rhs.val &&
           lhs.sz == rhs.sz &&
           lhs.r == rhs.r &&
           lhs.k == rhs.k &&
           lhs.name == rhs.name;
}

inline bool operator!=(const Token& lhs, const Token& rhs) {
    return !(lhs == rhs);
}
bool tokensAreEqual(const std::vector<Token>& tokens, int i, const std::vector<int>& idxs) {
    for (size_t k_pair = 0; k_pair < idxs.size() / 2; ++k_pair) {
        int idx_l = idxs[k_pair * 2];
        int idx_r = idxs[k_pair * 2 + 1];
        if (i + idx_l >= tokens.size() || i + idx_r >= tokens.size() || i + idx_l < 0 || i + idx_r < 0) {
            throw std::out_of_range("Index out of bounds in tokensAreEqual");
        }
        if (tokens[i + idx_l] != tokens[i + idx_r]) { 
            return false;
        }
    }
    return true;
}

bool tokenEqual(const Token& t1, const Token& t2) {
    return t1 == t2;
}
bool noKeep(const Token& t) {
    return t.k == 0;
}
bool isLit(const Token& t) {
    return t.type == TokenType::LIT;
}

bool isPadding(const Token& t) {
    return t.type == TokenType::PAD;
}
bool isOp(const Token& t) {
    return t.type == TokenType::INSTR && alu_ops.count(t.name);
}
bool isCommBinOp(const Token& t) {
    return t.type == TokenType::INSTR && commutative_binary_ops.count(t.name);
}
bool hasKeepMode(const std::vector<Token>& tokens, int i, const std::vector<int>& idxs) {
    for (int idx : idxs) {
        if (i + idx >= tokens.size() || i + idx < 0) {
            throw std::out_of_range("Index out of bounds in hasKeepMode");
        }
        if (tokens[i + idx].k == 1) {
            return true;
        }
    }
    return false;
}
int getWordSz(const Token& token) {
    return token.sz;
}
int getStackMode(const Token& token) {
    return token.r;
}
Token toInstrToken(std::string op_name_str) {
    int word_sz = 1;
    int r = 0;
    int k = 0;

    size_t pos_2 = op_name_str.find('2');
    if (pos_2 != std::string::npos) {
        word_sz = 2;
        op_name_str.erase(pos_2, 1);
    }
    size_t pos_r = op_name_str.find('r');
    if (pos_r != std::string::npos) {
        r = 1;
        op_name_str.erase(pos_r, 1);
    }
    size_t pos_k = op_name_str.find('k');
    if (pos_k != std::string::npos) {
        k = 1;
        op_name_str.erase(pos_k, 1);
    }
    return makeToken(TokenType::INSTR, 0, word_sz, r, k, op_name_str);
}
bool isInstr(const Token& token, const std::string& op_name) {
    return (token.type == TokenType::INSTR &&
            token.name.rfind(op_name, 0) == 0);
}
bool isStore(const Token& token) {
    return !isInstr(token, "STH") && isInstr(token, "ST");
}
bool isLoad(const Token& token) {
    return isInstr(token, "LD");
}
bool isCondJump(const Token& token) {
    return (isInstr(token, "JCI") || isInstr(token, "JCN"));
}
bool isJump(const Token& token) {
    return (isInstr(token, "JMI") || isInstr(token, "JMP"));
}
bool isCall(const Token& token) {
    return (isInstr(token, "JSI") || isInstr(token, "JSR"));
}

bool isRef(const Token& token, int parent_or_child_mode = -1) {
    bool res = (token.type == TokenType::REF);
    if (parent_or_child_mode != -1) { // Check if defined
        res = res && (token.val == parent_or_child_mode);
    }
    return res;
}

bool isParentRef(const Token& t) {
    return isRef(t, 0); 
}

bool isChildRef(const Token& t) {
    return isRef(t, 1); 
}

bool isLabel(const Token& token, int parent_or_child_mode = -1) {
    bool res = (token.type == TokenType::LABEL);
    if (parent_or_child_mode != -1) { 
        res = res && (token.val == parent_or_child_mode);
    }
    return res;
}
bool isParentLabel(const Token& t) {
    return isLabel(t, 2); 
}
bool isChildLabel(const Token& t) {
    return isLabel(t, 1); 
}
bool hasName(const Token& token, const std::string& name) {
    return (token.name == name);
}
bool isChild(const Token& token) {
    if (token.type == TokenType::LABEL) {
        return (token.val == 1); 
    }
    else if (token.type == TokenType::REF) {
        return (token.val == 1); 
    }
    else {
        throw std::runtime_error("Not a label or reference " );
    }
}
bool isParent(const Token& token) {
    if (token.type == TokenType::LABEL) {
        return (token.val == 2); 
    }
    else if (token.type == TokenType::REF) {
        return (token.val == 0); 
    }
    else {
        throw std::runtime_error("Not a label or reference ");
    }
}
}