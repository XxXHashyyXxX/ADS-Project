#include "Parser.hpp"
#include <stack>

std::unique_ptr<AstValueNode> Parser::ParseExpression(const Tokens &tokens)
{
    auto onpTokens = ParseExpressionToOnp(tokens);
    std::stack<std::unique_ptr<AstValueNode>> nodes;

    using Type  = Token::Type;
    using Arity = Token::OperatorArity;

    auto pop1 = [&]() -> std::unique_ptr<AstValueNode> {
        if (nodes.empty()) throw std::runtime_error("RPN underflow");
        auto t = std::move(nodes.top()); nodes.pop(); return t;
    };

    for (auto it = onpTokens.begin(); it != onpTokens.end(); ++it) {
        // literały
        if (std::holds_alternative<int>(it->value)) {
            nodes.push(std::make_unique<AstLiteralValueNode>(std::get<int>(it->value)));
            continue;
        }
        if (std::holds_alternative<double>(it->value)) {
            nodes.push(std::make_unique<AstLiteralValueNode>(std::get<double>(it->value)));
            continue;
        }

        // operator (arity w variant)
        if (std::holds_alternative<Arity>(it->value)) {
            Arity a = std::get<Arity>(it->value);

            switch (it->type) {
                case Type::OperatorPlus:
                    if (a == Arity::Unary) {
                        auto rhs = pop1();
                        nodes.push(std::unique_ptr<AstValueNode>(
                            new AstUnaryOperatorNode(AstUnaryOperatorNode::Type::Identity, std::move(rhs))));
                    } else {
                        auto r = pop1(); auto l = pop1();
                        nodes.push(std::unique_ptr<AstValueNode>(
                            new AstBinaryOperatorNode(AstBinaryOperatorNode::Type::Addition, std::move(l), std::move(r))));
                    }
                    break;

                case Type::OperatorMinus:
                    if (a == Arity::Unary) {
                        auto rhs = pop1();
                        nodes.push(std::unique_ptr<AstValueNode>(
                            new AstUnaryOperatorNode(AstUnaryOperatorNode::Type::Negation, std::move(rhs))));
                    } else {
                        auto r = pop1(); auto l = pop1();
                        nodes.push(std::unique_ptr<AstValueNode>(
                            new AstBinaryOperatorNode(AstBinaryOperatorNode::Type::Subtraction, std::move(l), std::move(r))));
                    }
                    break;

                case Type::OperatorStar: {
                    auto r = pop1(); auto l = pop1();
                    nodes.push(std::unique_ptr<AstValueNode>(
                        new AstBinaryOperatorNode(AstBinaryOperatorNode::Type::Multiplication, std::move(l), std::move(r))));
                    break;
                }
                case Type::OperatorSlash: {
                    auto r = pop1(); auto l = pop1();
                    nodes.push(std::unique_ptr<AstValueNode>(
                        new AstBinaryOperatorNode(AstBinaryOperatorNode::Type::Division, std::move(l), std::move(r))));
                    break;
                }
                default: break; // ignoruj np. nawiasy/monostate
            }
        }
        // inne tokeny (np. nawiasy) ignorujesz w ONP
    }

    if (nodes.size() != 1) throw std::runtime_error("Invalid RPN");
    return std::move(nodes.top());
}

void Parser::DisplayTree(std::ostream& os, const AstValueNode &tree)
{
    tree.DisplayTree(os, 0, 0);
}

Tokens Parser::ParseExpressionToOnp(const Tokens &tokens)
{
    Tokens outTokens;
    outTokens.reserve(tokens.size());
    std::stack<Token> operators;

    auto arityPrecedence = [](const Token& token) {
        int p = GetOperatorPrecedence(token.type);
        if(std::holds_alternative<Token::OperatorArity>(token.value) &&
                std::get<Token::OperatorArity>(token.value) == Token::OperatorArity::Unary) {
            p += 10;
        }

        return p;
    };

    for(auto it = tokens.begin(); it != tokens.end(); ++it) {
        switch(it->type) {
            case Token::Type::Invalid:
            case Token::Type::LiteralDouble:
            case Token::Type::LiteralInt:
                outTokens.push_back(*it);
                break;
            case Token::Type::ParenthesisLeft:
                operators.push(*it);
                break;
            case Token::Type::ParenthesisRight:
                while(!operators.empty() && operators.top().type != Token::Type::ParenthesisLeft) {
                    auto top = operators.top();
                    operators.pop();
                    outTokens.push_back(top);
                }

                if(operators.empty() || operators.top().type != Token::Type::ParenthesisLeft) 
                    throw std::runtime_error("Invalid parenthesis match");
                
                operators.pop();
                break;
            case Token::Type::OperatorMinus:
            case Token::Type::OperatorPlus:
            case Token::Type::OperatorSlash:
            case Token::Type::OperatorStar:
                auto currentPrecedence = arityPrecedence(*it);
                while(!operators.empty() && 
                        operators.top().type != Token::Type::ParenthesisLeft && 
                        (arityPrecedence(operators.top()) > currentPrecedence ||
                        (arityPrecedence(operators.top()) == currentPrecedence && 
                        std::get<Token::OperatorArity>(it->value) == Token::OperatorArity::Binary))) {
                    auto op = operators.top();
                    operators.pop();
                    outTokens.push_back(op);
                }
                operators.push(*it);
                break;
        }
    }

    while(!operators.empty()) {
        auto token = operators.top();
        operators.pop();
        outTokens.push_back(token);
    }

    return outTokens;
}

int Parser::GetOperatorPrecedence(const Token::Type &type)
{
    switch(type) {
        case Token::Type::Invalid:
        case Token::Type::LiteralDouble:
        case Token::Type::LiteralInt:
        case Token::Type::ParenthesisLeft:
        case Token::Type::ParenthesisRight:
            throw std::runtime_error("Token not precedenceable");
        case Token::Type::OperatorPlus:
        case Token::Type::OperatorMinus:
            return 0;
        case Token::Type::OperatorStar:
        case Token::Type::OperatorSlash:
            return 1;
        default:
            throw std::runtime_error("Unrecognized token type");
    }
}
