#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <algorithm>
#include <cmath>

using Exponent = int;
using ValueStack = std::stack<double>;
using OperationStack = std::stack<char>;

struct Constant
{
    int value;
    Exponent exponent;

    Constant() = default;
    Constant(int value = 1, Exponent exponent = 1) 
    : value(value), exponent(exponent) {}
};

struct Variable
{
    char value;
    Exponent exponent;

    Variable() = default;
    Variable(char value = 'x', Exponent exponent = 1)
        : value(value), exponent(exponent) {}
};

struct Piece
{
    char sign = '+';
    std::vector<Constant> constants;
    std::vector<Variable> variables;

    Piece() = default;
    Piece(char sign) : sign(sign) {}
};

class Function
{
	std::vector<Piece> pieces;

public:
    /*Constructors*/
    Function() = default;
    Function(const std::string &func) { this->pieces = FindPieces(func); }

    /*Input Methods*/
    std::vector<Piece> FindPieces(const std::string &func);

    /*Output Methods*/
    std::vector<Piece>& GetPieces() { return this->pieces; }
    double Solve(double x) const;
    std::string ToString() const;
    const Piece* GetPiece(size_t index) const { return &*(this->pieces.begin() + index); }
    double operator()(double x) const { return this->Solve(x); } 

    /*Overloaded Operators*/
    Piece& operator[](size_t index) { return this->pieces.at(index); }

private:
    /*Private Helper Methods For Parser*/
    template <typename VecT, typename InputT>
    void CollectData(std::vector<VecT> &vec, std::stringstream &stream);
    template <typename T>
    void AppendExponent(std::vector<T> &vec, std::stringstream &stream);

    /*Private Helper Methods For Solver*/
    bool OperatorCausesEvaluation(char prevOp, char op) const;
    size_t ExecuteExponent(const std::string &exp, size_t n, 
        size_t nPos, ValueStack &vStack) const;
    size_t ExecuteDigit(const std::string &exp, size_t n, 
	    size_t nPos, bool negative, ValueStack &vStack) const;
    void ExecuteOperator(char op, ValueStack &vStack,
        OperationStack &opStack) const;
    void ExecuteOperation(ValueStack &vStack, OperationStack &opStack) const;
    void ExecuteBranch(OperationStack &opStack, ValueStack &vStack) const;
public:
    double EvaluateExpression(const std::string &exp) const;
};
