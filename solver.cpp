#include "solver.h"

void ExecuteBranch(OperationStack &opStack, ValueStack &vStack)
{
    while (opStack.top() != '(')
        ExecuteOperation(vStack, opStack);
    
    opStack.pop();
}

size_t ExecuteDigit(const std::string &exp, size_t n, size_t nPos, ValueStack &vStack)
{
    double value = 0;

    while (nPos < n && isdigit(exp[nPos]))
        value = 10 * value + (exp[nPos++] - '0');

    vStack.push(value);

    return nPos;
}

void ExecuteOperator(char op, ValueStack &vStack, OperationStack &opStack)
{
    while (!opStack.empty() && OperatorCausesEvaluation(opStack.top(), op))
        ExecuteOperation(vStack, opStack);

    opStack.push(op);
}

bool OperatorCausesEvaluation(char prevOp, char op)
{
    switch (op)
    {
        case '+':
        case '-':
            return prevOp != '(';
        case '*':
        case '/':
            return prevOp == '*' || prevOp == '/';
        case ')':
            return true;
    }
    return false;
}

void ExecuteOperation(ValueStack &vStack, OperationStack &opStack)
{
    double rightOperand = vStack.top();
    vStack.pop();
    
    double leftOperand = vStack.top();
    vStack.pop();
    
    char operation = opStack.top();
    opStack.pop();
    
    double result;
    switch (operation)
    {
        case '+':
            result = leftOperand + rightOperand;
            break;
        case '-':
            result = leftOperand - rightOperand;
            break;
        case '*':
            result = leftOperand * rightOperand;
            break;
        case '/':
            result = leftOperand / rightOperand;
            break;
    }

    vStack.push(result);
}

int EvaluateExpression(const std::string &exp)
{
    OperationStack opStack;
    ValueStack vStack;
    size_t nPos = 0;
    size_t size = exp.size();

    opStack.push('(');

    while (nPos <= size)
    {
        if (nPos == size || exp[nPos] == ')')
        {
            ExecuteBranch(opStack, vStack);
            nPos++;
        }
        else if (isdigit(exp[nPos]))
            nPos = ExecuteDigit(exp, size, nPos, vStack);
        else
            ExecuteOperator(exp[nPos++], vStack, opStack);
    }

    return vStack.top();
}