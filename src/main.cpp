#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "../include/tokenizer.hpp"

using namespace std;

string readFile(const string& filePath) {
    ifstream file(filePath);
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

int main(int argc, char* argv[]) {
        string source = readFile(argv[1]);
        yaku::Tokenizer tokenizer;
        auto tokens = tokenizer.tokenize(source);
    return 0;
}