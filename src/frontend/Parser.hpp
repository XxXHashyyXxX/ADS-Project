#pragma once

#include "Lexer.hpp"
#include "Ast.hpp"
#include <ostream>

class Parser {
public:
    static std::unique_ptr<AstValueNode> ParseExpression(const Tokens& tokens);
    static void DisplayTree(std::ostream& os, const AstValueNode& tree);

private:
    static Tokens ParseExpressionToOnp(const Tokens& tokens);
    static int GetOperatorPrecedence(const Token::Type& type);
};