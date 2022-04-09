//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_scanner.h"

const std::unordered_map<std::string, std::regex> Token::WgslTokens = {
        {"decimal_float_literal", std::regex(
                R"((/((-?[0-9]*\.[0-9]+|-?[0-9]+\.[0-9]*)((e|E)(\+|-)?[0-9]+)?f?)|(-?[0-9]+(e|E)(\+|-)?[0-9]+f?)/))")},
        {"hex_float_literal",     std::regex(
                R"(/-?0x((([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.[0-9a-fA-F]*)((p|P)(\+|-)?[0-9]+f?)?)|([0-9a-fA-F]+(p|P)(\+|-)?[0-9]+f?))/)")},
        {"int_literal",           std::regex("/-?0x[0-9a-fA-F]+|0|-?[1-9][0-9]*/")},
        {"uint_literal",          std::regex("/0x[0-9a-fA-F]+u|0u|[1-9][0-9]*u/")},
        {"ident",                 std::regex("/[a-zA-Z][0-9a-zA-Z_]*/")}
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

std::unordered_map<std::string, Token> Token::Tokens{};

void Token::initialize() {
    for (const auto &token: Token::WgslTokens) {

    }
}

Token::Token(std::string type, std::string lexeme, std::string line) :
        _type(std::move(type)),
        _lexeme(std::move(lexeme)),
        _line(std::move(line)) {
}

const std::string &Token::toString() {
    return _lexeme;
}