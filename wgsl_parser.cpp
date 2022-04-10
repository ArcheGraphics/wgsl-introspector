//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_parser.h"
#include <memory>

std::vector<std::unique_ptr<AST>> WgslParser::parse(const std::string &code) {
    _initialize(code);

    std::vector<std::unique_ptr<AST>> statements{};
    while (!_isAtEnd()) {
        auto statement = _global_decl_or_directive();
        if (!statement)
            break;
        statements.emplace_back(std::move(statement));
    }
    return statements;
}

std::vector<std::unique_ptr<AST>> WgslParser::parse(const std::vector<Token> &tokens) {
    _initialize(tokens);

    std::vector<std::unique_ptr<AST>> statements{};
    while (!_isAtEnd()) {
        auto statement = _global_decl_or_directive();
        if (!statement)
            break;
        statements.emplace_back(std::move(statement));
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

std::unique_ptr<AST> WgslParser::_global_decl_or_directive() {
    return nullptr;
}

void WgslParser::_function_decl() {}

std::unique_ptr<AST> WgslParser::_compound_statement() {
    // brace_left statement* brace_right
    std::vector<std::unique_ptr<AST>> statements{};
    _consume(Token::Tokens["brace_left"], "Expected '{' for block.");
    while (!_check(Token::Tokens["brace_right"])) {
        auto statement = _statement();
        if (statement)
            statements.emplace_back(std::move(statement));
    }
    _consume(Token::Tokens["brace_right"], "Expected '}' for block.");

    auto ast = std::make_unique<AST>("");
    ast->setChildVec("", std::move(statements));
    return ast;
}

std::unique_ptr<AST> WgslParser::_statement() {
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

    std::unique_ptr<AST> result = nullptr;
    if (_check(Token::Keywords["return"]))
        result = _return_statement();
    else if (_check({Token::Keywords["var"], Token::Keywords["let"]}))
        result = _variable_statement();
    else if (_match(Token::Keywords["discard"])) {
        result = std::make_unique<AST>("discard");
    } else if (_match(Token::Keywords["break"])) {
        result = std::make_unique<AST>("break");
    } else if (_match(Token::Keywords["continue"])) {
        result = std::make_unique<AST>("continue");
    } else {
        auto state = _func_call_statement();
        if (state) {
            result = std::move(state);
        }
        state = _assignment_statement();
        if (state) {
            result = std::move(state);
        }
    }

    if (result != nullptr)
        _consume(Token::Tokens["semicolon"], "Expected ';' after statement.");

    return result;
}

std::unique_ptr<AST> WgslParser::_while_statement() {
    if (!_match(Token::Keywords["while"]))
        return nullptr;
    auto condition = _optional_paren_expression();
    auto block = _compound_statement();

    std::unique_ptr<AST> ast = std::make_unique<AST>("while");
    ast->setChild("condition", std::move(condition));
    ast->setChild("block", std::move(block));
    return ast;
}

std::unique_ptr<AST> WgslParser::_for_statement() {
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
    ast->setChild("init", std::move(init));
    ast->setChild("condition", std::move(condition));
    ast->setChild("increment", std::move(increment));
    ast->setChild("body", std::move(body));
    return ast;
}

std::unique_ptr<AST> WgslParser::_for_init() {
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

std::unique_ptr<AST> WgslParser::_for_increment() {
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

std::unique_ptr<AST> WgslParser::_variable_statement() {
    // variable_decl
    // variable_decl equal short_circuit_or_expression
    // let (ident variable_ident_decl) equal short_circuit_or_expression
    if (_check(Token::Keywords["var"])) {
        auto _var = _variable_decl();
        std::unique_ptr<AST> value = nullptr;
        if (_match(Token::Tokens["equal"]))
            value = _short_circuit_or_expression();

        std::unique_ptr<AST> ast = std::make_unique<AST>("var");
        ast->setChild("var", std::move(_var));
        ast->setChild("value", std::move(value));
        return ast;
    }

    if (_match(Token::Keywords["let"])) {
        auto name = _consume(Token::Tokens["ident"], "Expected name for let.").toString();
        std::unique_ptr<AST> type = nullptr;
        if (_match(Token::Tokens["colon"])) {
            auto typeAttrs = _attribute();
            type = _type_decl();
            type->_child["attributes"] = std::move(typeAttrs);
        }
        _consume(Token::Tokens["equal"], "Expected '=' for let.");
        auto value = _short_circuit_or_expression();

        std::unique_ptr<AST> ast = std::make_unique<AST>("let");
        ast->setChild("type", std::move(type));
        ast->setChild("value", std::move(value));
        ast->setName(name);
        return ast;
    }

    return nullptr;
}

std::unique_ptr<AST> WgslParser::_assignment_statement() {
    // (unary_expression underscore) equal short_circuit_or_expression
    std::unique_ptr<AST> _var = nullptr;

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
    ast->setChild("var", std::move(_var));
    ast->setChild("value", std::move(value));
    return ast;
}

std::unique_ptr<AST> WgslParser::_func_call_statement() {
    // ident argument_expression_list
    if (!_check(Token::Tokens["ident"]))
        return nullptr;

    auto savedPos = _current;
    auto name = _consume(Token::Tokens["ident"], "Expected function name.");
    auto args = _argument_expression_list();

    if (args.empty()) {
        _current = savedPos;
        return nullptr;
    }

    std::unique_ptr<AST> ast = std::make_unique<AST>("call");
    ast->setChildVec("args", std::move(args));
    ast->setName(name._type.name);
    return ast;
}

std::unique_ptr<AST> WgslParser::_loop_statement() {
    // loop brace_left statement* continuing_statement? brace_right
    if (!_match(Token::Keywords["loop"]))
        return nullptr;

    _consume(Token::Tokens["brace_left"], "Expected '{' for loop.");

    // statement*
    std::vector<std::unique_ptr<AST>> statements{};
    auto statement = _statement();
    while (statement != nullptr) {
        statements.emplace_back(std::move(statement));
        statement = _statement();
    }

    // continuing_statement: continuing compound_statement
    std::unique_ptr<AST> continuing = nullptr;
    if (_match(Token::Keywords["continuing"]))
        continuing = _compound_statement();

    _consume(Token::Tokens["brace_right"], "Expected '}' for loop.");

    std::unique_ptr<AST> ast = std::make_unique<AST>("loop");
    ast->setChildVec("statements", std::move(statements));
    ast->setChild("continuing", std::move(continuing));
    return ast;
}

std::unique_ptr<AST> WgslParser::_switch_statement() {
    // switch optional_paren_expression brace_left switch_body+ brace_right
    if (!_match(Token::Keywords["switch"]))
        return nullptr;

    auto condition = _optional_paren_expression();
    _consume(Token::Tokens["brace_left"], "");
    auto body = _switch_body();
    if (body.empty())
        throw std::runtime_error("Expected 'case' or 'default'.");
    _consume(Token::Tokens["brace_right"], "");

    std::unique_ptr<AST> ast = std::make_unique<AST>("switch");
    ast->setChildVec("body", std::move(body));
    ast->setChild("condition", std::move(condition));
    return ast;
}

std::vector<std::unique_ptr<AST>> WgslParser::_switch_body() {
    // case case_selectors colon brace_left case_body? brace_right
    // default colon brace_left case_body? brace_right
    std::vector<std::unique_ptr<AST>> cases{};
    if (_match(Token::Keywords["case"])) {
        _consume(Token::Keywords["case"], "");
        auto selector = _case_selectors();
        _consume(Token::Keywords["colon"], "Exected ':' for switch case.");
        _consume(Token::Tokens["brace_left"], "Exected '{' for switch case.");
        auto body = _case_body();
        _consume(Token::Tokens["brace_right"], "Exected '}' for switch case.");

        std::unique_ptr<AST> ast = std::make_unique<AST>("case");
        ast->setChildVec("body", std::move(body));
        ast->setNameVec(selector);
        cases.emplace_back(std::move(ast));
    }

    if (_match(Token::Keywords["default"])) {
        _consume(Token::Tokens["colon"], "Exected ':' for switch default.");
        _consume(Token::Tokens["brace_left"], "Exected '{' for switch default.");
        auto body = _case_body();
        _consume(Token::Tokens["brace_right"], "Exected '}' for switch default.");

        std::unique_ptr<AST> ast = std::make_unique<AST>("default");
        ast->setChildVec("body", std::move(body));
        cases.emplace_back(std::move(ast));
    }

    if (_check({Token::Keywords["default"], Token::Keywords["case"]})) {
        auto _cases = _switch_body();
        cases.emplace_back(std::move(_cases[0]));
    }

    return cases;
}

std::vector<std::string> WgslParser::_case_selectors() {
    // const_literal (comma const_literal)* comma?
    std::vector<std::string> selectors = {
            _consume(Token::Tokens["const_literal"], "Expected constant literal").toString()};
    while (_match(Token::Tokens["comma"])) {
        selectors.push_back(_consume(Token::Tokens["const_literal"], "Expected constant literal").toString());
    }
    return selectors;
}

std::vector<std::unique_ptr<AST>> WgslParser::_case_body() {
    // statement case_body?
    // fallthrough semicolon
    if (_match(Token::Keywords["fallthrough"])) {
        _consume(Token::Tokens["semicolon"], "");
        return {};
    }

    auto statement = _statement();
    if (statement == nullptr)
        return {};

    auto nextStatement = _case_body();
    if (nextStatement.empty()) {
        std::vector<std::unique_ptr<AST>> result(1);
        result[0] = std::move(statement);
        return result;
    }

    std::vector<std::unique_ptr<AST>> result(2);
    result[0] = std::move(statement);
    result[1] = std::move(nextStatement[0]);
    return result;
}

std::unique_ptr<AST> WgslParser::_if_statement() {
    // if optional_paren_expression compound_statement elseif_statement? else_statement?
    if (!_match(Token::Keywords["if"]))
        return nullptr;

    auto condition = _optional_paren_expression();
    auto block = _compound_statement();

    std::vector<std::unique_ptr<AST>> elseif{};
    if (_match(Token::Keywords["elseif"]))
        elseif = _elseif_statement();

    std::unique_ptr<AST> _else = nullptr;
    if (_match(Token::Keywords["else"]))
        _else = _compound_statement();

    std::unique_ptr<AST> ast = std::make_unique<AST>("if");
    ast->setChild("condition", std::move(condition));
    ast->setChild("block", std::move(block));
    ast->setChildVec("elseif", std::move(elseif));
    ast->setChild("else", std::move(_else));
    return ast;
}

std::vector<std::unique_ptr<AST>> WgslParser::_elseif_statement() {
    // else_if optional_paren_expression compound_statement elseif_statement?
    std::vector<std::unique_ptr<AST>> elseif{};
    auto condition = _optional_paren_expression();
    auto block = _compound_statement();
    auto ast = std::make_unique<AST>("elseif");
    ast->setChild("condition", std::move(condition));
    ast->setChild("block", std::move(block));
    elseif.emplace_back(std::move(ast));
    if (_match(Token::Keywords["elseif"]))
        elseif.emplace_back(std::move(_elseif_statement()[0]));
    return elseif;
}

std::unique_ptr<AST> WgslParser::_return_statement() {
    // return short_circuit_or_expression?
    if (!_match(Token::Keywords["return"]))
        return nullptr;
    auto value = _short_circuit_or_expression();

    auto ast = std::make_unique<AST>("return");
    ast->setChild("value", std::move(value));
    return ast;
}

std::unique_ptr<AST> WgslParser::_short_circuit_or_expression() {
    // short_circuit_and_expression
    // short_circuit_or_expression or_or short_circuit_and_expression
    auto expr = _short_circuit_and_expr();
    while (_match(Token::Tokens["or_or"])) {
        auto ast = std::make_unique<AST>("compareOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", std::move(_short_circuit_and_expr()));
        ast->setName(_previous().toString());
        expr = std::move(ast);
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_short_circuit_and_expr() {
    // inclusive_or_expression
    // short_circuit_and_expression and_and inclusive_or_expression
    auto expr = _inclusive_or_expression();
    while (_match(Token::Tokens["and_and"])) {
        auto ast = std::make_unique<AST>("compareOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", _inclusive_or_expression());
        ast->setName(_previous().toString());
        expr = std::move(ast);
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_inclusive_or_expression() {
    // exclusive_or_expression
    // inclusive_or_expression or exclusive_or_expression
    auto expr = _exclusive_or_expression();
    while (_match(Token::Tokens["or"])) {
        auto ast = std::make_unique<AST>("binaryOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", _exclusive_or_expression());
        ast->setName(_previous().toString());
        expr = std::move(ast);
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_exclusive_or_expression() {
    // and_expression
    // exclusive_or_expression xor and_expression
    auto expr = _and_expression();
    while (_match(Token::Tokens["xor"])) {
        auto ast = std::make_unique<AST>("binaryOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", _and_expression());
        ast->setName(_previous().toString());
        expr = std::move(ast);
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_and_expression() {
    // equality_expression
    // and_expression and equality_expression
    auto expr = _equality_expression();
    while (_match(Token::Tokens["and"])) {
        auto ast = std::make_unique<AST>("binaryOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", _equality_expression());
        ast->setName(_previous().toString());
        expr = std::move(ast);
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_equality_expression() {
    // relational_expression
    // relational_expression equal_equal relational_expression
    // relational_expression not_equal relational_expression
    auto expr = _relational_expression();
    if (_match({Token::Tokens["equal_equal"], Token::Tokens["not_equal"]})) {
        auto ast = std::make_unique<AST>("compareOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", _relational_expression());
        ast->setName(_previous().toString());
        return ast;
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_relational_expression() {
    // shift_expression
    // relational_expression less_than shift_expression
    // relational_expression greater_than shift_expression
    // relational_expression less_than_equal shift_expression
    // relational_expression greater_than_equal shift_expression
    auto expr = _shift_expression();
    while (_match({Token::Tokens["less_than"], Token::Tokens["greater_than"],
                   Token::Tokens["less_than_equal"], Token::Tokens["greater_than_equal"]})) {
        auto ast = std::make_unique<AST>("compareOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", _shift_expression());
        ast->setName(_previous().toString());
        expr = std::move(ast);
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_shift_expression() {
    // additive_expression
    // shift_expression shift_left additive_expression
    // shift_expression shift_right additive_expression
    auto expr = _additive_expression();
    while (_match({Token::Tokens["shift_left"], Token::Tokens["shift_right"]})) {
        auto ast = std::make_unique<AST>("binaryOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", _additive_expression());
        ast->setName(_previous().toString());
        expr = std::move(ast);
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_additive_expression() {
    // multiplicative_expression
    // additive_expression plus multiplicative_expression
    // additive_expression minus multiplicative_expression
    auto expr = _multiplicative_expression();
    while (_match({Token::Tokens["plus"], Token::Tokens["minus"]})) {
        auto ast = std::make_unique<AST>("binaryOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", _multiplicative_expression());
        ast->setName(_previous().toString());
        expr = std::move(ast);
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_multiplicative_expression() {
    // unary_expression
    // multiplicative_expression star unary_expression
    // multiplicative_expression forward_slash unary_expression
    // multiplicative_expression modulo unary_expression
    auto expr = _unary_expression();
    while (_match({Token::Tokens["star"], Token::Tokens["forward_slash"], Token::Tokens["modulo"]})) {
        auto ast = std::make_unique<AST>("binaryOp");
        ast->setChild("left", std::move(expr));
        ast->setChild("right", _unary_expression());
        ast->setName(_previous().toString());
        expr = std::move(ast);
    }
    return expr;
}

std::unique_ptr<AST> WgslParser::_unary_expression() {
    // singular_expression
    // minus unary_expression
    // bang unary_expression
    // tilde unary_expression
    // star unary_expression
    // and unary_expression
    if (_match({Token::Tokens["minus"], Token::Tokens["bang"],
                Token::Tokens["tilde"], Token::Tokens["star"], Token::Tokens["and"]})) {
        auto ast = std::make_unique<AST>("unaryOp");
        ast->setChild("right", _unary_expression());
        ast->setName(_previous().toString());
        return ast;
    }
    return _singular_expression();
}

std::unique_ptr<AST> WgslParser::_singular_expression() {
    // primary_expression postfix_expression ?
    auto expr = _primary_expression();
    auto p = _postfix_expression();
    if (p)
        expr->setChild("postfix", std::move(p));
    return expr;
}

std::unique_ptr<AST> WgslParser::_postfix_expression() {
    // bracket_left short_circuit_or_expression bracket_right postfix_expression?
    if (_match(Token::Tokens["bracket_left"])) {
        auto expr = _short_circuit_or_expression();
        _consume(Token::Tokens["bracket_right"], "Expected ']'.");
        auto p = _postfix_expression();
        if (p)
            expr->setChild("postfix", std::move(p));
        return expr;
    }

    // period ident postfix_expression?
//    if (_match(Token::Tokens["period"])) {
//        auto name = _consume(Token::Tokens["ident"], "Expected member name.");
//        auto p = _postfix_expression();
//        if (p)
//            name.postfix = p;
//        return name;
//    }

    return nullptr;
}

std::unique_ptr<AST> WgslParser::_primary_expression() {
    // ident argument_expression_list?
    if (_match(Token::Tokens["ident"])) {
        auto name = _previous().toString();
        if (_check(Token::Tokens["paren_left"])) {
            auto args = _argument_expression_list();

            auto ast = std::make_unique<AST>("call_expr");
            ast->setName(name);
            ast->setChildVec("args", std::move(args));
            return ast;
        }
        auto ast = std::make_unique<AST>("variable_expr");
        ast->setName(name);
        return ast;
    }

    // const_literal
    if (_match(Token::Tokens["const_literal"])) {
        auto ast = std::make_unique<AST>("literal_expr");
        ast->setName(_previous().toString());
    }

    // paren_expression
    if (_check(Token::Tokens["paren_left"])) {
        return _paren_expression();
    }

    // bitcast less_than type_decl greater_than paren_expression
    if (_match(Token::Keywords["bitcast"])) {
        _consume(Token::Tokens["less_than"], "Expected '<'.");
        auto type = _type_decl();
        _consume(Token::Tokens["greater_than"], "Expected '>'.");
        auto value = _paren_expression();

        auto ast = std::make_unique<AST>("bitcast_expr");
        ast->setChild("type", std::move(type));
        ast->setChild("value", std::move(value));
        return ast;
    }

    // type_decl argument_expression_list
    auto type = _type_decl();
    auto args = _argument_expression_list();

    auto ast = std::make_unique<AST>("typecast_expr");
    ast->setChild("type", std::move(type));
    ast->setChildVec("args", std::move(args));
    return ast;
}

std::vector<std::unique_ptr<AST>> WgslParser::_argument_expression_list() {
    // paren_left ((short_circuit_or_expression comma)* short_circuit_or_expression comma?)? paren_right
    if (!_match(Token::Tokens["paren_left"]))
        return {};

    std::vector<std::unique_ptr<AST>> args{};
    do {
        if (_check(Token::Tokens["paren_right"]))
            break;
        auto arg = _short_circuit_or_expression();
        args.emplace_back(std::move(arg));
    } while (_match(Token::Tokens["comma"]));
    _consume(Token::Tokens["paren_right"], "Expected ')' for agument list");

    return args;
}

std::unique_ptr<AST> WgslParser::_optional_paren_expression() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_paren_expression() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_struct_decl() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_global_variable_decl() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_global_constant_decl() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_const_expression() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_variable_decl() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_enable_directive() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_type_alias() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_type_decl() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_texture_sampler_types() {
    return nullptr;
}

std::unique_ptr<AST> WgslParser::_attribute() {
    return nullptr;
}