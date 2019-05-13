#include <iostream>
#include <string>
#include <cctype>
#include <stack>

using ValueStack = std::stack<double>;
using OperationStack = std::stack<char>;

void ExecuteBranch(OperationStack &opStack, ValueStack &vStack);
size_t ExecuteDigit(const std::string &exp, size_t n, size_t nPos, ValueStack &vStack);
void ExecuteOperator(char op, ValueStack &vStack, OperationStack &opStack);
bool OperatorCausesEvaluation(char prevOp, char op);
void ExecuteOperation(ValueStack &vStack, OperationStack &opStack);
int EvaluateExpression(const std::string &exp);
