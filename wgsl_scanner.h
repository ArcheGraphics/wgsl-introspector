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
#include <utility>

struct TokenType {
    std::string name;
    std::string type;
    std::string rule;
    bool isRegex;

    bool operator==(TokenType& t) const {
        return name == t.name;
    }
};

class Token {
public:
    static const TokenType TokenEOF;
    static const std::unordered_map<std::string, std::string> WgslTokens;
    static const std::vector<std::string> WgslKeywords;
    static const std::vector<std::string> WgslReserved;

    static std::unordered_map<std::string, TokenType> Tokens;
    static std::unordered_map<std::string, TokenType> Keywords;

    static std::unordered_map<std::string, TokenType> StorageClass;
    static std::unordered_map<std::string, TokenType> AccessMode;
    static std::unordered_map<std::string, TokenType> SamplerType;
    static std::unordered_map<std::string, TokenType> SampledTextureType;
    static std::unordered_map<std::string, TokenType> MultisampledTextureType;
    static std::unordered_map<std::string, TokenType> StorageTextureType;
    static std::unordered_map<std::string, TokenType> DepthTextureType;
    static std::unordered_map<std::string, TokenType> TextureType;
    static std::unordered_map<std::string, TokenType> TexelFormat;
    static std::unordered_map<std::string, TokenType> ConstLiteral;
    static std::unordered_map<std::string, TokenType> LiteralOrIdent;
    static std::unordered_map<std::string, TokenType> ElementCountExpression;
    static std::unordered_map<std::string, TokenType> TemplateTypes;
    static std::unordered_map<std::string, TokenType> AttributeName;

    static void initialize();

public:
    Token(TokenType type, std::string lexeme, size_t line);

    const std::string &toString();

private:
    friend class WgslScanner;
    TokenType _type;
    std::string _lexeme;
    size_t _line;
};

//MARK: - WgslScanner
class WgslScanner {
public:
    explicit WgslScanner(std::string source);

    void scanTokens();

    bool scanToken();

    static std::optional<TokenType> _findToken(const std::string &lexeme);

    static bool _match(const std::string &lexeme, const std::string &rule);

    static bool _match(const std::string &lexeme, const std::regex &rule);

    static bool _isWhitespace(const std::string &c);

    bool _isAtEnd();

    std::string _advance(size_t amount = 0);

    std::string _peekAhead(size_t offset = 0);

    void _addToken(const TokenType &type);

private:
    std::string _source;
    std::vector<Token> _tokens{};
    size_t _start = 0;
    size_t _current = 0;
    size_t _line = 1;
};

#endif //WGSL_INTROSPECTOR_WGSL_SCANNER_H
