#pragma once

#include <memory>
#include <variant>
#include <ostream>

class AstNode {
public:
    virtual ~AstNode() = default;
};

class AstValueNode : public AstNode {
public:
    virtual ~AstValueNode() = default;

    virtual int GetIntValue() = 0;
    virtual double GetDoubleValue() = 0;

    virtual void DisplayTree(std::ostream& os, unsigned spaces, unsigned back = 0) const = 0;
};

class AstLiteralValueNode : public AstValueNode {
public:
    ~AstLiteralValueNode() = default;
    AstLiteralValueNode(int value) : value(value) {}
    AstLiteralValueNode(double value) : value(value) {}

    int GetIntValue() override;
    double GetDoubleValue() override;
    void DisplayTree(std::ostream& os, unsigned spaces, unsigned back = 0) const override;

private:
    std::variant<int, double> value;
};

class AstUnaryOperatorNode : public AstValueNode {
public:
    enum class Type {
        Identity, Negation
    };

    ~AstUnaryOperatorNode() = default;
    AstUnaryOperatorNode(Type type, std::unique_ptr<AstValueNode> operand) : 
        type(type), operand(std::move(operand)) {}

    int GetIntValue() override;
    double GetDoubleValue() override;
    void DisplayTree(std::ostream& os, unsigned spaces, unsigned back = 0) const override;

private:
    Type type;
    std::unique_ptr<AstValueNode> operand;
};

class AstBinaryOperatorNode : public AstValueNode {
public:
    enum class Type {
        Addition, Subtraction, Multiplication, Division
    };

    ~AstBinaryOperatorNode() = default;
    AstBinaryOperatorNode(Type type, std::unique_ptr<AstValueNode> leftOperand, std::unique_ptr<AstValueNode> rightOperand) :
        type(type), leftOperand(std::move(leftOperand)), rightOperand(std::move(rightOperand)) {}

    int GetIntValue() override;
    double GetDoubleValue() override;
    void DisplayTree(std::ostream& os, unsigned spaces, unsigned back = 0) const override;

private:
    Type type;
    std::unique_ptr<AstValueNode> leftOperand;
    std::unique_ptr<AstValueNode> rightOperand;
};