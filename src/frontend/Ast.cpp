#include "Ast.hpp"

static inline void checkSpaces(unsigned spaces, unsigned back) {
    if(back > spaces) throw std::invalid_argument("Too much backspaces");
}

int AstLiteralValueNode::GetIntValue()
{
    if(std::holds_alternative<int>(value)) return std::get<int>(value);
    if(std::holds_alternative<double>(value)) return static_cast<int>(std::get<double>(value));
    throw std::runtime_error("Value not initialized");
}

double AstLiteralValueNode::GetDoubleValue()
{
    if(std::holds_alternative<int>(value)) return static_cast<double>(std::get<int>(value));
    if(std::holds_alternative<double>(value)) return std::get<double>(value);
    throw std::runtime_error("Value not initialized");
}

void AstLiteralValueNode::DisplayTree(std::ostream &os, unsigned spaces, unsigned back) const
{
    checkSpaces(spaces, back);
    unsigned tabs = spaces - back;

    os << std::string(tabs, '\t');

    if(std::holds_alternative<int>(value)) {
        os << "Int: " << std::get<int>(value) << "\n";
    }
    else if(std::holds_alternative<double>(value)) {
        os << "Double: " << std::get<double>(value) << "\n";
    }
    else {
        os << "Incorrect value type\n";
    }
}

int AstUnaryOperatorNode::GetIntValue()
{
    if(!operand) throw std::runtime_error("Operand not initialized");

    switch(type) {
        case Type::Identity:
            return operand->GetIntValue();
        case Type::Negation:
            return -operand->GetIntValue();
        default:
            throw std::runtime_error("Invalid operator type");
    }
}

double AstUnaryOperatorNode::GetDoubleValue()
{
    if(!operand) throw std::runtime_error("Operand not initialized");

    switch(type) {
        case Type::Identity:
            return operand->GetDoubleValue();
        case Type::Negation:
            return -operand->GetDoubleValue();
        default:
            throw std::runtime_error("Invalid operator type");
    }
}

void AstUnaryOperatorNode::DisplayTree(std::ostream &os, unsigned spaces, unsigned back) const
{
    checkSpaces(spaces, back);
    unsigned tabs = spaces - back;
    os << std::string(tabs, '\t') << "Unary ";

    switch(type) {
        case Type::Identity:
            os << "+:";
            break;
        case Type::Negation:
            os << "-:";
            break;
    }

    operand->DisplayTree(os, spaces+1, spaces);
}

int AstBinaryOperatorNode::GetIntValue()
{
    if(!leftOperand || !rightOperand) throw std::runtime_error("Operands not initialized");

    switch(type) {
        case Type::Addition:
            return leftOperand->GetIntValue() + rightOperand->GetIntValue();
        case Type::Subtraction:
            return leftOperand->GetIntValue() - rightOperand->GetIntValue();
        case Type::Multiplication:
            return leftOperand->GetIntValue() * rightOperand->GetIntValue();
        case Type::Division:
            if(rightOperand->GetIntValue() == 0) throw std::runtime_error("Division by zero");
            return leftOperand->GetIntValue() / rightOperand->GetIntValue();
        default:
            throw std::runtime_error("Invalid operator type");
    }
}

double AstBinaryOperatorNode::GetDoubleValue()
{
    if(!leftOperand || !rightOperand) throw std::runtime_error("Operands not initialized");

    switch(type) {
        case Type::Addition:
            return leftOperand->GetDoubleValue() + rightOperand->GetDoubleValue();
        case Type::Subtraction:
            return leftOperand->GetDoubleValue() - rightOperand->GetDoubleValue();
        case Type::Multiplication:
            return leftOperand->GetDoubleValue() * rightOperand->GetDoubleValue();
        case Type::Division:
            if(rightOperand->GetDoubleValue() == 0) throw std::runtime_error("Division by zero");
            return leftOperand->GetDoubleValue() / rightOperand->GetDoubleValue();
        default:
            throw std::runtime_error("Invalid operator type");
    }
}

void AstBinaryOperatorNode::DisplayTree(std::ostream &os, unsigned spaces, unsigned back) const
{
    checkSpaces(spaces, back);
    unsigned tabs = spaces - back;

    leftOperand->DisplayTree(os, spaces+1, 0);

    os << std::string(tabs, '\t') << "Binary ";
    switch(type) {
        case Type::Addition:
            os << "+:";
            break;
        case Type::Subtraction:
            os << "-:";
            break;
        case Type::Multiplication:
            os << "*:";
            break;
        case Type::Division:
            os << "/:";
            break;
    }
    os << '\n';
    
    rightOperand->DisplayTree(os, spaces+1, 0);
}
