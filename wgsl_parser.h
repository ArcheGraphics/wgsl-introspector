//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef WGSL_INTROSPECTOR_WGSL_PARSER_H
#define WGSL_INTROSPECTOR_WGSL_PARSER_H

#include "wgsl_scanner.h"

class AST {
public:
    AST(const std::string &type, const std::vector<std::string> &options) {
        _type = type;
        _options = options;
    }

private:
    std::string _type;
    std::vector<std::string> _options;
};


class WgslParser {
public:
    WgslParser() = default;

    std::vector<AST> parse(const std::string &code);

    std::vector<AST> parse(const std::vector<Token> &tokens);

private:
    void _initialize(const std::string &code);

    void _initialize(const std::vector<Token> &tokens);

    void _error(const Token &token, const std::string &message);

    bool _isAtEnd();

    void _match(const std::string &types);

    void _consume(const std::string &types, const std::string &message);

    void _check(const std::string &types);

    Token _advance();

    Token _peek();

    Token _previous();

private:
    std::optional<AST> _global_decl_or_directive();

    void _function_decl();

    void _compound_statement();

    void _statement();

    void _while_statement();

    void _for_statement();

    void _for_init();

    void _for_increment();

    void _variable_statement();

    void _assignment_statement();

    void _func_call_statement();

    void _loop_statement();

    void _switch_statement();

    void _switch_body();

    void _case_selectors();

    void _case_body();

    void _if_statement();

    void _elseif_statement();

    void _return_statement();

    void _short_circuit_or_expression();

    void _short_circuit_and_expr();

    void _inclusive_or_expression();

    void _exclusive_or_expression();

    void _and_expression();

    void _equality_expression();

    void _relational_expression();

    void _shift_expression();

    void _additive_expression();

    void _multiplicative_expression();

    void _unary_expression();

    void _singular_expression();

    void _postfix_expression();

    void _primary_expression();

    void _argument_expression_list();

    void _optional_paren_expression();

    void _paren_expression();

    void _struct_decl();

    void _global_variable_decl();

    void _global_constant_decl();

    void _const_expression();

    void _variable_decl();

    void _enable_directive();

    void _type_alias();

    void _type_decl();

    void _texture_sampler_types();

    void _attribute();

private:
    std::vector<Token> _tokens{};
    size_t _current = 0;
};

#endif //WGSL_INTROSPECTOR_WGSL_PARSER_H
