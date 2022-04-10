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
    explicit AST(const std::string &type) {
        _type = type;
    }

    void setChild(const std::string &name, std::unique_ptr<AST> &&ast) {
        _child[name] = std::move(ast);
    }

    const std::unordered_map<std::string, std::unique_ptr<AST>> &child() const {
        return _child;
    }

    void setChildVec(const std::string &name, std::vector<std::unique_ptr<AST>> &&ast) {
        _childVec[name] = std::move(ast);
    }

    const std::unordered_map<std::string, std::vector<std::unique_ptr<AST>>> &childVec() const {
        return _childVec;
    }

    void setName(const std::string &name) {
        _name = name;
    }

    const std::string &name() const {
        return _name;
    }

    void setNameVec(const std::vector<std::string> &nameVec) {
        _nameVec = nameVec;
    }

    const std::vector<std::string> &nameVec() const {
        return _nameVec;
    }

private:
    friend class WgslParser;

    std::string _type;
    std::unordered_map<std::string, std::unique_ptr<AST>> _child{};
    std::unordered_map<std::string, std::vector<std::unique_ptr<AST>>> _childVec{};
    std::string _name;
    std::vector<std::string> _nameVec;
};


class WgslParser {
public:
    WgslParser() = default;

    std::vector<std::unique_ptr<AST>> parse(const std::string &code);

    std::vector<std::unique_ptr<AST>> parse(const std::vector<Token> &tokens);

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
    std::unique_ptr<AST> _global_decl_or_directive();

    void _function_decl();

    std::unique_ptr<AST> _compound_statement();

    std::unique_ptr<AST> _statement();

    std::unique_ptr<AST> _while_statement();

    std::unique_ptr<AST> _for_statement();

    std::unique_ptr<AST> _for_init();

    std::unique_ptr<AST> _for_increment();

    std::unique_ptr<AST> _variable_statement();

    std::unique_ptr<AST> _assignment_statement();

    std::unique_ptr<AST> _func_call_statement();

    std::unique_ptr<AST> _loop_statement();

    std::unique_ptr<AST> _switch_statement();

    std::vector<std::unique_ptr<AST>> _switch_body();

    std::vector<std::string> _case_selectors();

    std::vector<std::unique_ptr<AST>> _case_body();

    std::unique_ptr<AST> _if_statement();

    std::vector<std::unique_ptr<AST>> _elseif_statement();

    std::unique_ptr<AST> _return_statement();

    std::unique_ptr<AST> _short_circuit_or_expression();

    std::unique_ptr<AST> _short_circuit_and_expr();

    std::unique_ptr<AST> _inclusive_or_expression();

    std::unique_ptr<AST> _exclusive_or_expression();

    std::unique_ptr<AST> _and_expression();

    std::unique_ptr<AST> _equality_expression();

    std::unique_ptr<AST> _relational_expression();

    std::unique_ptr<AST> _shift_expression();

    std::unique_ptr<AST> _additive_expression();

    std::unique_ptr<AST> _multiplicative_expression();

    std::unique_ptr<AST> _unary_expression();

    std::unique_ptr<AST> _singular_expression();

    std::unique_ptr<AST> _postfix_expression();

    std::unique_ptr<AST> _primary_expression();

    std::vector<std::unique_ptr<AST>> _argument_expression_list();

    std::unique_ptr<AST> _optional_paren_expression();

    std::unique_ptr<AST> _paren_expression();

    std::unique_ptr<AST> _struct_decl();

    std::unique_ptr<AST> _global_variable_decl();

    std::unique_ptr<AST> _global_constant_decl();

    std::unique_ptr<AST> _const_expression();

    std::unique_ptr<AST> _variable_decl();

    std::unique_ptr<AST> _enable_directive();

    std::unique_ptr<AST> _type_alias();

    std::unique_ptr<AST> _type_decl();

    std::unique_ptr<AST> _texture_sampler_types();

    std::unique_ptr<AST> _attribute();

private:
    std::vector<Token> _tokens{};
    size_t _current = 0;
};

#endif //WGSL_INTROSPECTOR_WGSL_PARSER_H
