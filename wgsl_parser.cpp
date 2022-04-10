//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_parser.h"
#include <memory>

std::vector<AST *> WgslParser::parse(const std::string &code) {
    _initialize(code);

    std::vector<AST *> statements{};
    while (!_isAtEnd()) {
        auto statement = _global_decl_or_directive();
        if (!statement)
            break;
        statements.emplace_back(statement);
    }
    return statements;
}

std::vector<AST *> WgslParser::parse(const std::vector<Token> &tokens) {
    _initialize(tokens);

    std::vector<AST *> statements{};
    while (!_isAtEnd()) {
        auto statement = _global_decl_or_directive();
        if (!statement)
            break;
        statements.emplace_back(statement);
    }
    return statements;
}

void WgslParser::_initialize(const std::string &code) {
    auto scanner = WgslScanner(code);
    _tokens = scanner.scanTokens();
    _current = 0;
}

void WgslParser::_initialize(const std::vector<Token> &tokens) {
    _tokens = tokens;
    _current = 0;
}

void WgslParser::_error(const Token &token, const std::string &message) {

}

bool WgslParser::_isAtEnd() {
    return _current >= _tokens.size() || _peek()._type == Token::TokenEOF;
}

bool WgslParser::_match(const TokenType &types) {
    if (_check(types)) {
        _advance();
        return true;
    }
    return false;
}

bool WgslParser::_match(const std::vector<TokenType> &types) {
    for (const auto &type: types) {
        if (_check(type)) {
            _advance();
            return true;
        }
    }

    return false;
}

bool WgslParser::_check(const TokenType &types) {
    if (_isAtEnd()) return false;
    return _peek()._type == types;
}

bool WgslParser::_check(const std::vector<TokenType> &types) {
    if (_isAtEnd()) return false;
    const auto iter = std::find(types.begin(), types.end(), _peek()._type);
    return iter != types.end();
}

Token WgslParser::_consume(const TokenType &types, const std::string &message) {
    if (_check(types)) return _advance();
    throw std::runtime_error(message);
}

Token WgslParser::_consume(const std::vector<TokenType> &types, const std::string &message) {
    if (_check(types)) return _advance();
    throw std::runtime_error(message);
}

Token WgslParser::_advance() {
    if (!_isAtEnd()) _current++;
    return _previous();
}

Token WgslParser::_peek() {
    return _tokens[_current];
}

Token WgslParser::_previous() {
    return _tokens[_current - 1];
}

AST *WgslParser::_global_decl_or_directive() {
    return nullptr;
}

void WgslParser::_function_decl() {}

AST *WgslParser::_compound_statement() {
    // brace_left statement* brace_right
    std::vector<AST *> statements{};
    _consume(Token::Tokens["brace_left"], "Expected '{' for block.");
    while (!_check(Token::Tokens["brace_right"])) {
        const auto statement = _statement();
        if (statement)
            statements.emplace_back(statement);
    }
    _consume(Token::Tokens["brace_right"], "Expected '}' for block.");

    auto ast = std::make_unique<AST>("");
    auto astPtr = ast.get();
    _astPool.emplace_back(std::move(ast));
    return astPtr;
}

AST *WgslParser::_statement() {
    // semicolon
    // return_statement semicolon
    // if_statement
    // switch_statement
    // loop_statement
    // for_statement
    // func_call_statement semicolon
    // variable_statement semicolon
    // break_statement semicolon
    // continue_statement semicolon
    // discard semicolon
    // assignment_statement semicolon
    // compound_statement

    // Ignore any stand-alone semicolons
    while (_match(Token::Tokens["semicolon"]) && !_isAtEnd());

    if (_check(Token::Keywords["if"]))
        return _if_statement();

    if (_check(Token::Keywords["switch"]))
        return _switch_statement();

    if (_check(Token::Keywords["loop"]))
        return _loop_statement();

    if (_check(Token::Keywords["for"]))
        return _for_statement();

    if (_check(Token::Keywords["while"]))
        return _while_statement();

    if (_check(Token::Tokens["brace_left"]))
        return _compound_statement();

    AST *resultPtr = nullptr;
    if (_check(Token::Keywords["return"]))
        resultPtr = _return_statement();
    else if (_check({Token::Keywords["var"], Token::Keywords["let"]}))
        resultPtr = _variable_statement();
    else if (_match(Token::Keywords["discard"])) {
        auto result = std::make_unique<AST>("discard");
        resultPtr = result.get();
        _astPool.emplace_back(std::move(result));
    } else if (_match(Token::Keywords["break"])) {
        auto result = std::make_unique<AST>("break");
        resultPtr = result.get();
        _astPool.emplace_back(std::move(result));
    } else if (_match(Token::Keywords["continue"])) {
        auto result = std::make_unique<AST>("continue");
        resultPtr = result.get();
        _astPool.emplace_back(std::move(result));
    } else {
        auto state = _func_call_statement();
        if (state) {
            resultPtr = state;
        }
        state = _assignment_statement();
        if (state) {
            resultPtr = state;
        }
    }

    if (resultPtr != nullptr)
        _consume(Token::Tokens["semicolon"], "Expected ';' after statement.");

    return resultPtr;
}

AST *WgslParser::_while_statement() {
    if (!_match(Token::Keywords["while"]))
        return nullptr;
    auto condition = _optional_paren_expression();
    auto block = _compound_statement();

    std::unordered_map<std::string, AST *> child{
            {"condition", condition},
            {"block", block}
    };
    std::unique_ptr<AST> ast = std::make_unique<AST>("while", child);
    auto astPtr = ast.get();
    _astPool.emplace_back(std::move(ast));
    return astPtr;
}

AST *WgslParser::_for_statement() {
    return nullptr;
}

AST *WgslParser::_for_init() {
    return nullptr;
}

AST *WgslParser::_for_increment() {
    return nullptr;
}

AST *WgslParser::_variable_statement() {
    return nullptr;
}

AST *WgslParser::_assignment_statement() {
    return nullptr;
}

AST *WgslParser::_func_call_statement() {
    return nullptr;
}

AST *WgslParser::_loop_statement() {
    return nullptr;
}

AST *WgslParser::_switch_statement() {
    return nullptr;
}

AST *WgslParser::_switch_body() {
    return nullptr;
}

AST *WgslParser::_case_selectors() {
    return nullptr;
}

AST *WgslParser::_case_body() {
    return nullptr;
}

AST *WgslParser::_if_statement() {
    return nullptr;
}

AST *WgslParser::_elseif_statement() {
    return nullptr;
}

AST *WgslParser::_return_statement() {
    return nullptr;
}

AST *WgslParser::_short_circuit_or_expression() {
    return nullptr;
}

AST *WgslParser::_short_circuit_and_expr() {
    return nullptr;
}

AST *WgslParser::_inclusive_or_expression() {
    return nullptr;
}

AST *WgslParser::_exclusive_or_expression() {
    return nullptr;
}

AST *WgslParser::_and_expression() {
    return nullptr;
}

AST *WgslParser::_equality_expression() {
    return nullptr;
}

AST *WgslParser::_relational_expression() {
    return nullptr;
}

AST *WgslParser::_shift_expression() {
    return nullptr;
}

AST *WgslParser::_additive_expression() {
    return nullptr;
}

AST *WgslParser::_multiplicative_expression() {
    return nullptr;
}

AST *WgslParser::_unary_expression() {
    return nullptr;
}

AST *WgslParser::_singular_expression() {
    return nullptr;
}

AST *WgslParser::_postfix_expression() {
    return nullptr;
}

AST *WgslParser::_primary_expression() {
    return nullptr;
}

AST *WgslParser::_argument_expression_list() {
    return nullptr;
}

AST *WgslParser::_optional_paren_expression() {
    return nullptr;
}

AST *WgslParser::_paren_expression() {
    return nullptr;
}

AST *WgslParser::_struct_decl() {
    return nullptr;
}

AST *WgslParser::_global_variable_decl() {
    return nullptr;
}

AST *WgslParser::_global_constant_decl() {
    return nullptr;
}

AST *WgslParser::_const_expression() {
    return nullptr;
}

AST *WgslParser::_variable_decl() {
    return nullptr;
}

AST *WgslParser::_enable_directive() {
    return nullptr;
}

AST *WgslParser::_type_alias() {
    return nullptr;
}

AST *WgslParser::_type_decl() {
    return nullptr;
}

AST *WgslParser::_texture_sampler_types() {
    return nullptr;
}

AST *WgslParser::_attribute() {
    return nullptr;
}