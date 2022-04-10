//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef WGSL_INTROSPECTOR_WGSL_PARSER_H
#define WGSL_INTROSPECTOR_WGSL_PARSER_H

#include <utility>
#include "wgsl_scanner.h"

class AST {
public:
    AST(const std::string &type,
        std::unordered_map<std::string, AST*> child = {},
        std::string name = "") {
        _type = type;
        _child = child;
        _name = std::move(name);
    }

    AST* operator[](std::string key) {
        return _child[key];
    }

    std::string name() {
        return _name;
    }

private:
    std::string _type;
    std::unordered_map<std::string, AST*> _child{};
    std::string _name;
};


class WgslParser {
public:
    WgslParser() = default;

    std::vector<AST*> parse(const std::string &code);

    std::vector<AST*> parse(const std::vector<Token> &tokens);

private:
    void _initialize(const std::string &code);

    void _initialize(const std::vector<Token> &tokens);

    void _error(const Token &token, const std::string &message);

    bool _isAtEnd();

    bool _match(const TokenType &types);

    bool _match(const std::vector<TokenType> &types);

    bool _check(const TokenType &types);

    bool _check(const std::vector<TokenType> &types);

    Token _consume(const TokenType &types, const std::string &message);

    Token _consume(const std::vector<TokenType> &types, const std::string &message);

    Token _advance();

    Token _peek();

    Token _previous();

private:
    AST* _global_decl_or_directive();

    void _function_decl();

    AST* _compound_statement();

    AST* _statement();

    AST* _while_statement();

    AST* _for_statement();

    AST* _for_init();

    AST* _for_increment();

    AST* _variable_statement();

    AST* _assignment_statement();

    AST* _func_call_statement();

    AST* _loop_statement();

    AST* _switch_statement();

    AST* _switch_body();

    AST* _case_selectors();

    AST* _case_body();

    AST* _if_statement();

    AST* _elseif_statement();

    AST* _return_statement();

    AST* _short_circuit_or_expression();

    AST* _short_circuit_and_expr();

    AST* _inclusive_or_expression();

    AST* _exclusive_or_expression();

    AST* _and_expression();

    AST* _equality_expression();

    AST* _relational_expression();

    AST* _shift_expression();

    AST* _additive_expression();

    AST* _multiplicative_expression();

    AST* _unary_expression();

    AST* _singular_expression();

    AST* _postfix_expression();

    AST* _primary_expression();

    AST* _argument_expression_list();

    AST* _optional_paren_expression();

    AST* _paren_expression();

    AST* _struct_decl();

    AST* _global_variable_decl();

    AST* _global_constant_decl();

    AST* _const_expression();

    AST* _variable_decl();

    AST* _enable_directive();

    AST* _type_alias();

    AST* _type_decl();

    AST* _texture_sampler_types();

    AST* _attribute();

private:
    std::vector<Token> _tokens{};
    size_t _current = 0;

    std::vector<std::unique_ptr<AST>> _astPool{};
};

#endif //WGSL_INTROSPECTOR_WGSL_PARSER_H
