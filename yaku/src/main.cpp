#include <iostream>
#include <fstream>
#include <sstream>
#include "uxntal/parser.hpp"

int main() {
    uxntal::Uxn uxn;
    std::ifstream file("test/hello.tal");
    std::string line;
    std::string source;
    while (std::getline(file, line)) {
        source += line + "\n";  
    }
    auto tokens = uxntal::parseUxntalProgram(source,uxn);

    for (const auto& token : tokens) {
        std::cout << "Token: " << token.name << ", Type: " << (int)(token.type) << "\n";
    }

    return 0;
}
