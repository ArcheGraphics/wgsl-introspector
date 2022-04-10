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
    ast->setChildVec("", statements);
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

    std::unique_ptr<AST> ast = std::make_unique<AST>("while");
    ast->setChild("condition", condition);
    ast->setChild("block", block);
    auto astPtr = ast.get();
    _astPool.emplace_back(std::move(ast));
    return astPtr;
}

AST *WgslParser::_for_statement() {
    // for paren_left for_header paren_right compound_statement
    if (!_match(Token::Keywords["for"]))
        return nullptr;

    _consume(Token::Tokens["paren_left"], "Expected '('.");

    // for_header: (variable_statement assignment_statement func_call_statement)?
    // semicolon short_circuit_or_expression? semicolon (assignment_statement func_call_statement)?
    auto init = !_check(Token::Tokens["semicolon"]) ? _for_init() : nullptr;
    _consume(Token::Tokens["semicolon"], "Expected ';'.");
    auto condition = !_check(Token::Tokens["semicolon"]) ? _short_circuit_or_expression() : nullptr;
    _consume(Token::Tokens["semicolon"], "Expected ';'.");
    auto increment = !_check(Token::Tokens["paren_right"]) ? _for_increment() : nullptr;

    _consume(Token::Tokens["paren_right"], "Expected ')'.");

    auto body = _compound_statement();

    std::unique_ptr<AST> ast = std::make_unique<AST>("for");
    ast->setChild("init", init);
    ast->setChild("condition", condition);
    ast->setChild("increment", increment);
    ast->setChild("body", body);

    auto astPtr = ast.get();
    _astPool.emplace_back(std::move(ast));
    return astPtr;
}

AST *WgslParser::_for_init() {
    // (variable_statement assignment_statement func_call_statement)?
    auto state = _variable_statement();
    if (state) {
        return state;
    }

    state = _func_call_statement();
    if (state) {
        return state;
    }

    state = _assignment_statement();
    if (state) {
        return state;
    }
    return nullptr;
}

AST *WgslParser::_for_increment() {
    // (assignment_statement func_call_statement)?
    auto state = _func_call_statement();
    if (state) {
        return state;
    }

    state = _assignment_statement();
    if (state) {
        return state;
    }
    return nullptr;
}

AST *WgslParser::_variable_statement() {
    // variable_decl
    // variable_decl equal short_circuit_or_expression
    // let (ident variable_ident_decl) equal short_circuit_or_expression
    if (_check(Token::Keywords["var"])) {
        auto _var = _variable_decl();
        AST *value = nullptr;
        if (_match(Token::Tokens["equal"]))
            value = _short_circuit_or_expression();

        std::unordered_map<std::string, AST *> child{
                {"var",   _var},
                {"value", value}
        };
        std::unique_ptr<AST> ast = std::make_unique<AST>("var");
        auto astPtr = ast.get();
        _astPool.emplace_back(std::move(ast));
        return astPtr;
    }

    if (_match(Token::Keywords["let"])) {
        auto name = _consume(Token::Tokens["ident"], "Expected name for let.").toString();
        AST *type = nullptr;
        if (_match(Token::Tokens["colon"])) {
            auto typeAttrs = _attribute();
            type = _type_decl();
            type->_child["attributes"] = typeAttrs;
        }
        _consume(Token::Tokens["equal"], "Expected '=' for let.");
        auto value = _short_circuit_or_expression();

        std::unique_ptr<AST> ast = std::make_unique<AST>("let");
        ast->setChild("type", type);
        ast->setChild("value", value);
        ast->setName(name);

        auto astPtr = ast.get();
        _astPool.emplace_back(std::move(ast));
        return astPtr;
    }

    return nullptr;
}

AST *WgslParser::_assignment_statement() {
    // (unary_expression underscore) equal short_circuit_or_expression
    AST *_var = nullptr;

    if (_check(Token::Tokens["brace_right"]))
        return nullptr;

    auto isUnderscore = _match(Token::Tokens["underscore"]);
    if (!isUnderscore)
        _var = _unary_expression();

    if (!isUnderscore && _var == nullptr)
        return nullptr;

    _consume(Token::Tokens["equal"], "Expected '='.");

    auto value = _short_circuit_or_expression();

    std::unique_ptr<AST> ast = std::make_unique<AST>("assign");
    ast->setChild("var", _var);
    ast->setChild("value", value);

    auto astPtr = ast.get();
    _astPool.emplace_back(std::move(ast));
    return astPtr;
}

AST *WgslParser::_func_call_statement() {
    // ident argument_expression_list
    if (!_check(Token::Tokens["ident"]))
        return nullptr;

    auto savedPos = _current;
    auto name = _consume(Token::Tokens["ident"], "Expected function name.");
    auto args = _argument_expression_list();

    if (args == nullptr) {
        _current = savedPos;
        return nullptr;
    }

    std::unique_ptr<AST> ast = std::make_unique<AST>("call");
    ast->setChild("args", args);
    ast->setName(name._type.name);

    auto astPtr = ast.get();
    _astPool.emplace_back(std::move(ast));
    return astPtr;
}

AST *WgslParser::_loop_statement() {
    // loop brace_left statement* continuing_statement? brace_right
    if (!_match(Token::Keywords["loop"]))
        return nullptr;

    _consume(Token::Tokens["brace_left"], "Expected '{' for loop.");

    // statement*
    std::vector<AST *> statements{};
    auto statement = _statement();
    while (statement != nullptr) {
        statements.push_back(statement);
        statement = _statement();
    }

    // continuing_statement: continuing compound_statement
    AST *continuing = nullptr;
    if (_match(Token::Keywords["continuing"]))
        continuing = _compound_statement();

    _consume(Token::Tokens["brace_right"], "Expected '}' for loop.");

    std::unique_ptr<AST> ast = std::make_unique<AST>("loop");
    ast->setChildVec("statements", statements);
    ast->setChild("continuing", continuing);

    auto astPtr = ast.get();
    _astPool.emplace_back(std::move(ast));
    return astPtr;
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