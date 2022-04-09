//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_parser.h"

std::vector<AST> WgslParser::parse(const std::string &code) {
    _initialize(code);

    std::vector<AST> statements{};
    while (!_isAtEnd()) {
        auto statement = _global_decl_or_directive();
        if (!statement)
            break;
        statements.emplace_back(statement.value());
    }
    return statements;
}

std::vector<AST> WgslParser::parse(const std::vector<Token> &tokens) {
    _initialize(tokens);

    std::vector<AST> statements{};
    while (!_isAtEnd()) {
        auto statement = _global_decl_or_directive();
        if (!statement)
            break;
        statements.emplace_back(statement.value());
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

void WgslParser::_match(const std::string &types) {}

void WgslParser::_consume(const std::string &types, const std::string &message) {}

void WgslParser::_check(const std::string &types) {}

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

std::optional<AST> WgslParser::_global_decl_or_directive() {}

void WgslParser::_function_decl() {}

void WgslParser::_compound_statement() {}

void WgslParser::_statement() {}

void WgslParser::_while_statement() {}

void WgslParser::_for_statement() {}

void WgslParser::_for_init() {}

void WgslParser::_for_increment() {}

void WgslParser::_variable_statement() {}

void WgslParser::_assignment_statement() {}

void WgslParser::_func_call_statement() {}

void WgslParser::_loop_statement() {}

void WgslParser::_switch_statement() {}

void WgslParser::_switch_body() {}

void WgslParser::_case_selectors() {}

void WgslParser::_case_body() {}

void WgslParser::_if_statement() {}

void WgslParser::_elseif_statement() {}

void WgslParser::_return_statement() {}

void WgslParser::_short_circuit_or_expression() {}

void WgslParser::_short_circuit_and_expr() {}

void WgslParser::_inclusive_or_expression() {}

void WgslParser::_exclusive_or_expression() {}

void WgslParser::_and_expression() {}

void WgslParser::_equality_expression() {}

void WgslParser::_relational_expression() {}

void WgslParser::_shift_expression() {}

void WgslParser::_additive_expression() {}

void WgslParser::_multiplicative_expression() {}

void WgslParser::_unary_expression() {}

void WgslParser::_singular_expression() {}

void WgslParser::_postfix_expression() {}

void WgslParser::_primary_expression() {}

void WgslParser::_argument_expression_list() {}

void WgslParser::_optional_paren_expression() {}

void WgslParser::_paren_expression() {}

void WgslParser::_struct_decl() {}

void WgslParser::_global_variable_decl() {}

void WgslParser::_global_constant_decl() {}

void WgslParser::_const_expression() {}

void WgslParser::_variable_decl() {}

void WgslParser::_enable_directive() {}

void WgslParser::_type_alias() {}

void WgslParser::_type_decl() {}

void WgslParser::_texture_sampler_types() {}

void WgslParser::_attribute() {}