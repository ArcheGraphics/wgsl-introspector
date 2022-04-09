//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_scanner.h"

const TokenType Token::TokenEOF = {
        "EOF",
        "token",
        "-1",
        false
};

const std::unordered_map<std::string, std::string> Token::WgslTokens = {
        {"decimal_float_literal", R"((/((-?[0-9]*\.[0-9]+|-?[0-9]+\.[0-9]*)((e|E)(\+|-)?[0-9]+)?f?)|(-?[0-9]+(e|E)(\+|-)?[0-9]+f?)/))"},
        {"hex_float_literal",     R"(/-?0x((([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.[0-9a-fA-F]*)((p|P)(\+|-)?[0-9]+f?)?)|([0-9a-fA-F]+(p|P)(\+|-)?[0-9]+f?))/)"},
        {"int_literal",           "/-?0x[0-9a-fA-F]+|0|-?[1-9][0-9]*/"},
        {"uint_literal",          "/0x[0-9a-fA-F]+u|0u|[1-9][0-9]*u/"},
        {"ident",                 "/[a-zA-Z][0-9a-zA-Z_]*/"},
        {"and",                   "&"},
        {"and_and",               "&&"},
        {"arrow",                 "->"},
        {"attr",                  "@"},
        {"attr_left",             "[["},
        {"attr_right",            "]]"},
        {"forward_slash",         "/"},
        {"bang",                  "!"},
        {"bracket_left",          "["},
        {"bracket_right",         "]"},
        {"brace_left",            "{"},
        {"brace_right",           "}"},
        {"colon",                 ":"},
        {"comma",                 ","},
        {"equal",                 "="},
        {"equal_equal",           "=="},
        {"not_equal",             "!="},
        {"greater_than",          ">"},
        {"greater_than_equal",    ">="},
        {"shift_right",           ">>"},
        {"less_than",             "<"},
        {"less_than_equal",       "<="},
        {"shift_left",            "<<"},
        {"modulo",                "%"},
        {"minus",                 "-"},
        {"minus_minus",           "--"},
        {"period",                "."},
        {"plus",                  "+"},
        {"plus_plus",             "++"},
        {"or",                    "|"},
        {"or_or",                 "||"},
        {"paren_left",            "("},
        {"paren_right",           ")"},
        {"semicolon",             ";"},
        {"star",                  "*"},
        {"tilde",                 "~"},
        {"underscore",            "_"},
        {"xor",                   "^"},
};

const std::vector<std::string> Token::WgslKeywords = {
        "array",
        "atomic",
        "bool",
        "f32",
        "i32",
        "mat2x2",
        "mat2x3",
        "mat2x4",
        "mat3x2",
        "mat3x3",
        "mat3x4",
        "mat4x2",
        "mat4x3",
        "mat4x4",
        "ptr",
        "sampler",
        "sampler_comparison",
        "struct",
        "texture_1d",
        "texture_2d",
        "texture_2d_array",
        "texture_3d",
        "texture_cube",
        "texture_cube_array",
        "texture_multisampled_2d",
        "texture_storage_1d",
        "texture_storage_2d",
        "texture_storage_2d_array",
        "texture_storage_3d",
        "texture_depth_2d",
        "texture_depth_2d_array",
        "texture_depth_cube",
        "texture_depth_cube_array",
        "texture_depth_multisampled_2d",
        "u32",
        "vec2",
        "vec3",
        "vec4",
        "bitcast",
        "block",
        "break",
        "case",
        "continue",
        "continuing",
        "default",
        "discard",
        "else",
        "elseif",
        "enable",
        "fallthrough",
        "false",
        "fn",
        "for",
        "function",
        "if",
        "let",
        "loop",
        "while",
        "private",
        "read",
        "read_write",
        "return",
        "storage",
        "switch",
        "true",
        "type",
        "uniform",
        "var",
        "workgroup",
        "write",
        "r8unorm",
        "r8snorm",
        "r8uint",
        "r8sint",
        "r16uint",
        "r16sint",
        "r16float",
        "rg8unorm",
        "rg8snorm",
        "rg8uint",
        "rg8sint",
        "r32uint",
        "r32sint",
        "r32float",
        "rg16uint",
        "rg16sint",
        "rg16float",
        "rgba8unorm",
        "rgba8unorm_srgb",
        "rgba8snorm",
        "rgba8uint",
        "rgba8sint",
        "bgra8unorm",
        "bgra8unorm_srgb",
        "rgb10a2unorm",
        "rg11b10float",
        "rg32uint",
        "rg32sint",
        "rg32float",
        "rgba16uint",
        "rgba16sint",
        "rgba16float",
        "rgba32uint",
        "rgba32sint",
        "rgba32float"
};

const std::vector<std::string> Token::WgslReserved = {
        "asm",
        "bf16",
        "const",
        "do",
        "enum",
        "f16",
        "f64",
        "handle",
        "i8",
        "i16",
        "i64",
        "mat",
        "premerge",
        "regardless",
        "typedef",
        "u8",
        "u16",
        "u64",
        "unless",
        "using",
        "vec",
        "void"
};

std::unordered_map<std::string, TokenType> Token::Tokens{};
std::unordered_map<std::string, TokenType> Token::Keywords{};

std::unordered_map<std::string, TokenType> Token::StorageClass{};
std::unordered_map<std::string, TokenType> Token::AccessMode{};
std::unordered_map<std::string, TokenType> Token::SamplerType{};
std::unordered_map<std::string, TokenType> Token::SampledTextureType{};
std::unordered_map<std::string, TokenType> Token::MultisampledTextureType{};
std::unordered_map<std::string, TokenType> Token::StorageTextureType{};
std::unordered_map<std::string, TokenType> Token::DepthTextureType{};
std::unordered_map<std::string, TokenType> Token::TextureType{};
std::unordered_map<std::string, TokenType> Token::TexelFormat{};
std::unordered_map<std::string, TokenType> Token::ConstLiteral{};
std::unordered_map<std::string, TokenType> Token::LiteralOrIdent{};
std::unordered_map<std::string, TokenType> Token::ElementCountExpression{};
std::unordered_map<std::string, TokenType> Token::TemplateTypes{};
std::unordered_map<std::string, TokenType> Token::AttributeName{};

void Token::initialize() {
    for (const auto &token: Token::WgslTokens) {
        if (token.first == "decimal_float_literal" ||
            token.first == "hex_float_literal" ||
            token.first == "int_literal" ||
            token.first == "uint_literal" ||
            token.first == "ident") {
            Token::Tokens[token.first] = TokenType{
                    token.first,
                    "token",
                    token.second,
                    true,
            };
        } else {
            Token::Tokens[token.first] = TokenType{
                    token.first,
                    "token",
                    token.second,
                    false,
            };
        }
    }

    for (const auto &keyword: Token::WgslKeywords) {
        Token::Keywords[keyword] = TokenType{
                keyword,
                "token",
                keyword,
                false,
        };
    }

    for (const auto &keyword: Token::WgslReserved) {
        Token::Keywords[keyword] = TokenType{
                keyword,
                "token",
                keyword,
                false,
        };
    }

    // WGSL grammar has a few keywords that have different token names than the strings they
    // represent. Aliasing them here.
    Token::Keywords["int32"] = Token::Keywords["i32"];
    Token::Keywords["uint32"] = Token::Keywords["u32"];
    Token::Keywords["float32"] = Token::Keywords["f32"];
    Token::Keywords["pointer"] = Token::Keywords["ptr"];

    // The grammar has a few rules where the rule can match to any one of a given set of keywords
    // or tokens. Defining those here.
    Token::StorageClass["function"] = Token::Keywords["function"];
    Token::StorageClass["private"] = Token::Keywords["private"];
    Token::StorageClass["workgroup"] = Token::Keywords["workgroup"];
    Token::StorageClass["uniform"] = Token::Keywords["uniform"];
    Token::StorageClass["storage"] = Token::Keywords["storage"];

    Token::AccessMode["read"] = Keywords["read"];
    Token::AccessMode["write"] = Keywords["write"];
    Token::AccessMode["read_write"] = Keywords["read_write"];

    Token::SamplerType["sampler"] = Keywords["sampler"];
    Token::SamplerType["sampler_comparison"] = Keywords["sampler_comparison"];

    Token::SampledTextureType["texture_1d"] = Keywords["texture_1d"];
    Token::SampledTextureType["texture_2d"] = Keywords["texture_2d"];
    Token::SampledTextureType["texture_2d_array"] = Keywords["texture_2d_array"];
    Token::SampledTextureType["texture_3d"] = Keywords["texture_3d"];
    Token::SampledTextureType["texture_cube"] = Keywords["texture_cube"];
    Token::SampledTextureType["texture_cube_array"] = Keywords["texture_cube_array"];
    for (const auto &type: SampledTextureType) {
        Token::TextureType[type.first] = type.second;
    }

    Token::MultisampledTextureType["texture_multisampled_2d"] = Keywords["texture_multisampled_2d"];
    for (const auto &type: MultisampledTextureType) {
        Token::TextureType[type.first] = type.second;
    }

    Token::StorageTextureType["texture_storage_1d"] = Keywords["texture_storage_1d"];
    Token::StorageTextureType["texture_storage_2d"] = Keywords["texture_storage_2d"];
    Token::StorageTextureType["texture_storage_2d_array"] = Keywords["texture_storage_2d_array"];
    Token::StorageTextureType["texture_storage_3d"] = Keywords["texture_storage_3d"];
    for (const auto &type: StorageTextureType) {
        Token::TextureType[type.first] = type.second;
    }

    Token::DepthTextureType["texture_depth_2d"] = Keywords["texture_depth_2d"];
    Token::DepthTextureType["texture_depth_2d_array"] = Keywords["texture_depth_2d_array"];
    Token::DepthTextureType["texture_depth_cube"] = Keywords["texture_depth_cube"];
    Token::DepthTextureType["texture_depth_cube_array"] = Keywords["texture_depth_cube_array"];
    Token::DepthTextureType["texture_depth_multisampled_2d"] = Keywords["texture_depth_multisampled_2d"];
    for (const auto &type: DepthTextureType) {
        Token::TextureType[type.first] = type.second;
    }

    Token::TexelFormat["r8unorm"] = Keywords["r8unorm"];
    Token::TexelFormat["r8snorm"] = Keywords["r8snorm"];
    Token::TexelFormat["r8uint"] = Keywords["r8uint"];
    Token::TexelFormat["r8sint"] = Keywords["r8sint"];
    Token::TexelFormat["r16uint"] = Keywords["r16uint"];
    Token::TexelFormat["r16sint"] = Keywords["r16sint"];
    Token::TexelFormat["r16float"] = Keywords["r16float"];
    Token::TexelFormat["rg8unorm"] = Keywords["rg8unorm"];
    Token::TexelFormat["rg8snorm"] = Keywords["rg8snorm"];
    Token::TexelFormat["rg8uint"] = Keywords["rg8uint"];
    Token::TexelFormat["rg8sint"] = Keywords["rg8sint"];
    Token::TexelFormat["r32uint"] = Keywords["r32uint"];
    Token::TexelFormat["r32sint"] = Keywords["r32sint"];
    Token::TexelFormat["r32float"] = Keywords["r32float"];
    Token::TexelFormat["rg16uint"] = Keywords["rg16uint"];
    Token::TexelFormat["rg16sint"] = Keywords["rg16sint"];
    Token::TexelFormat["rg16float"] = Keywords["rg16float"];
    Token::TexelFormat["rgba8unorm"] = Keywords["rgba8unorm"];
    Token::TexelFormat["rgba8unorm_srgb"] = Keywords["rgba8unorm_srgb"];
    Token::TexelFormat["rgba8snorm"] = Keywords["rgba8snorm"];
    Token::TexelFormat["rgba8uint"] = Keywords["rgba8uint"];
    Token::TexelFormat["rgba8sint"] = Keywords["rgba8sint"];
    Token::TexelFormat["bgra8unorm"] = Keywords["bgra8unorm"];
    Token::TexelFormat["bgra8unorm_srgb"] = Keywords["bgra8unorm_srgb"];
    Token::TexelFormat["rgb10a2unorm"] = Keywords["rgb10a2unorm"];
    Token::TexelFormat["rg11b10float"] = Keywords["rg11b10float"];
    Token::TexelFormat["rg32uint"] = Keywords["rg32uint"];
    Token::TexelFormat["rg32sint"] = Keywords["rg32sint"];
    Token::TexelFormat["rg32float"] = Keywords["rg32float"];
    Token::TexelFormat["rgba16uint"] = Keywords["rgba16uint"];
    Token::TexelFormat["rgba16sint"] = Keywords["rgba16sint"];
    Token::TexelFormat["rgba16float"] = Keywords["rgba16float"];
    Token::TexelFormat["rgba32uint"] = Keywords["rgba32uint"];
    Token::TexelFormat["rgba32sint"] = Keywords["rgba32sint"];
    Token::TexelFormat["rgba32float"] = Keywords["rgba32float"];

    Token::ConstLiteral["int_literal"] = Token::Tokens["int_literal"];
    Token::ConstLiteral["uint_literal"] = Token::Tokens["uint_literal"];
    Token::ConstLiteral["decimal_float_literal"] = Token::Tokens["decimal_float_literal"];
    Token::ConstLiteral["hex_float_literal"] = Token::Tokens["hex_float_literal"];
    Token::ConstLiteral["true"] = Keywords["true"];
    Token::ConstLiteral["false"] = Keywords["false"];

    Token::LiteralOrIdent["int_literal"] = Token::Tokens["int_literal"];
    Token::LiteralOrIdent["uint_literal"] = Token::Tokens["uint_literal"];
    Token::LiteralOrIdent["decimal_float_literal"] = Token::Tokens["decimal_float_literal"];
    Token::LiteralOrIdent["hex_float_literal"] = Token::Tokens["hex_float_literal"];
    Token::LiteralOrIdent["ident"] = Token::Tokens["ident"];

    Token::ElementCountExpression["int_literal"] = Token::Tokens["int_literal"];
    Token::ElementCountExpression["uint_literal"] = Token::Tokens["uint_literal"];
    Token::ElementCountExpression["ident"] = Token::Tokens["ident"];

    Token::TemplateTypes["vec2"] = Keywords["vec2"];
    Token::TemplateTypes["vec3"] = Keywords["vec3"];
    Token::TemplateTypes["vec4"] = Keywords["vec4"];
    Token::TemplateTypes["mat2x2"] = Keywords["mat2x2"];
    Token::TemplateTypes["mat2x3"] = Keywords["mat2x3"];
    Token::TemplateTypes["mat2x4"] = Keywords["mat2x4"];
    Token::TemplateTypes["mat3x2"] = Keywords["mat3x2"];
    Token::TemplateTypes["mat3x3"] = Keywords["mat3x3"];
    Token::TemplateTypes["mat3x4"] = Keywords["mat3x4"];
    Token::TemplateTypes["mat4x2"] = Keywords["mat4x2"];
    Token::TemplateTypes["mat4x3"] = Keywords["mat4x3"];
    Token::TemplateTypes["mat4x4"] = Keywords["mat4x4"];
    Token::TemplateTypes["atomic"] = Keywords["atomic"];
    Token::TemplateTypes["bitcast"] = Keywords["bitcast"];
    for (const auto &type: TextureType) {
        Token::TemplateTypes[type.first] = type.second;
    }

    // The grammar calls out 'block', but attribute grammar is defined to use a 'ident'.
    // The attribute grammar should be ident | block.
    Token::AttributeName["ident"] = Token::Tokens["ident"];;
    Token::AttributeName["block"] = Keywords["block"];
}

Token::Token(TokenType type, std::string lexeme, size_t line) :
        _type(std::move(type)),
        _lexeme(std::move(lexeme)),
        _line(line) {
}

const std::string &Token::toString() {
    return _lexeme;
}

//MARK: - WgslScanner
WgslScanner::WgslScanner(std::string source) : _source(std::move(source)) {}

void WgslScanner::scanTokens() {
    while (!_isAtEnd()) {
        _start = _current;
        if (!scanToken())
            throw std::invalid_argument("Invalid syntax at line ${this._line}");
    }
}

bool WgslScanner::scanToken() {
    // Find the longest consecutive set of characters that match a rule.
    auto lexeme = _advance();

    // Skip line-feed, adding to the line counter.
    if (lexeme == "\n") {
        _line++;
        return true;
    }

    // Skip whitespace
    if (_isWhitespace(lexeme)) {
        return true;
    }

    if (lexeme == "/") {
        // If it's a // comment, skip everything until the next line-feed.
        if (_peekAhead() == "/") {
            while (lexeme != "\n") {
                if (_isAtEnd())
                    return true;
                lexeme = _advance();
            }
            // skip the linefeed
            _line++;
            return true;
        } else if (_peekAhead() == "*") {
            // If it's a /* block comment, skip everything until the matching */,
            // allowing for nested block comments.
            _advance();
            ssize_t commentLevel = 1;
            while (commentLevel > 0) {
                if (_isAtEnd())
                    return true;
                lexeme = _advance();
                if (lexeme == "\n") {
                    _line++;
                } else if (lexeme == "*") {
                    if (_peekAhead() == "/") {
                        _advance();
                        commentLevel--;
                        if (commentLevel == 0) {
                            return true;
                        }
                    }
                } else if (lexeme == "/") {
                    if (_peekAhead() == "*") {
                        _advance();
                        commentLevel++;
                    }
                }
            }
            return true;
        }
    }

    std::optional<TokenType> matchToken = std::nullopt;
    for (;;) {
        auto matchedToken = _findToken(lexeme);

        // The exception to "longest lexeme" rule is '>>'. In the case of 1>>2, it's a shift_right.
        // In the case of array<vec4<f32>>, it's two greater_than's (one to close the vec4,
        // and one to close the array).
        // I don't know of a great way to resolve this, so '>>' is special-cased and if
        // there was a less_than up to some number of tokens previously, and the token prior to
        // that is a keyword that requires a '<', then it will be split into two greater_than's;
        // otherwise it's a shift_right.
        if (lexeme == ">" && _peekAhead() == ">") {
            bool foundLessThan = false;
            size_t ti = _tokens.size() - 1;
            for (size_t count = 0; count < 4 && ti >= 0; ++count, --ti) {
                if (_tokens[ti]._type == Token::Tokens["less_than"]) {
                    const auto iter = Token::TemplateTypes.find(_tokens[ti - 1]._type.name);
                    if (ti > 0 && iter != Token::TemplateTypes.end()) {
                        foundLessThan = true;
                    }
                    break;
                }
            }
            // If there was a less_than in the recent token history, then this is probably a
            // greater_than.
            if (foundLessThan) {
                _addToken(matchedToken.value());
                return true;
            }
        }

        // The current lexeme may not match any rule, but some token types may be invalid for
        // part of the string but valid after a few more characters.
        // For example, 0x.5 is a hex_float_literal. But as it's being scanned,
        // "0" is a int_literal, then "0x" is invalid. If we stopped there, it would return
        // the int_literal "0", but that's incorrect. So if we look forward a few characters,
        // we'd get "0x.", which is still invalid, followed by "0x.5" which is the correct
        // hex_float_literal. So that means if we hit an non-matching string, we should look
        // ahead up to two characters to see if the string starts matching a valid rule again.
        if (!matchedToken) {
            std::string lookAheadLexeme = lexeme;
            size_t lookAhead = 0;
            size_t maxLookAhead = 2;
            for (size_t li = 0; li < maxLookAhead; ++li) {
                lookAheadLexeme += _peekAhead(li);
                matchedToken = _findToken(lookAheadLexeme);
                if (matchedToken) {
                    lookAhead = li;
                    break;
                }
            }

            if (!matchedToken) {
                if (!matchToken)
                    return false;
                _current--;
                _addToken(matchToken.value());
                return true;
            }

            lexeme = lookAheadLexeme;
            _current += lookAhead + 1;
        }

        matchToken = matchedToken;

        if (_isAtEnd())
            break;

        lexeme += _advance();
    }

    // We got to the end of the input stream. Then the token we've ready so far is it.
    if (matchToken == std::nullopt)
        return false;

    _addToken(matchToken.value());
    return true;
}

std::optional<TokenType> WgslScanner::_findToken(const std::string &lexeme) {
    for (const auto &name: Token::Keywords) {
        if (_match(lexeme, name.second.rule)) {
            return name.second;
        }
    }
    for (const auto &name: Token::Tokens) {
        if (name.second.isRegex) {
            if (_match(lexeme, std::regex(name.second.rule))) {
                return name.second;
            }
        } else {
            if (_match(lexeme, name.second.rule)) {
                return name.second;
            }
        }
    }
    return std::nullopt;
}

bool WgslScanner::_match(const std::string &lexeme, const std::string &rule) {
    if (rule == lexeme) {
        return true;
    }
    return false;
}

bool WgslScanner::_match(const std::string &lexeme, const std::regex &rule) {
    if (std::regex_search(lexeme, rule))
        return true;
    return false;
}

bool WgslScanner::_isAtEnd() {
    return _current >= _source.size();
}

bool WgslScanner::_isWhitespace(const std::string &c) {
    return c == " " || c == "\t" || c == "\r";
}

std::string WgslScanner::_advance(size_t amount) {
    const auto &c = _source.substr(_current);
    amount++;
    _current += amount;
    return c;
}

std::string WgslScanner::_peekAhead(size_t offset) {
    if (_current + offset >= _source.size()) return "\0";
    return _source.substr(_current + offset);
}

void WgslScanner::_addToken(const TokenType &type) {
    const auto &text = _source.substr(_start, _current - _start);
    _tokens.emplace_back(type, text, _line);
}