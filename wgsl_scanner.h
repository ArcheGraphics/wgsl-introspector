//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef WGSL_INTROSPECTOR_WGSL_SCANNER_H
#define WGSL_INTROSPECTOR_WGSL_SCANNER_H

#include <string>
#include <unordered_map>
#include <regex>

class Token {
public:
    static const std::unordered_map<std::string, std::regex> WgslTokens;
    static const std::vector<std::string> WgslKeywords;
    static const std::vector<std::string> WgslReserved;

    static void initialize();

private:
    static std::unordered_map<std::string, Token> Tokens;

public:
    Token(std::string type, std::string lexeme, std::string line);

    const std::string &toString();

private:
    std::string _type;
    std::string _lexeme;
    std::string _line;
};

#endif //WGSL_INTROSPECTOR_WGSL_SCANNER_H
