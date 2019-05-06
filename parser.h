#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

using Exponent = int;

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
	int Solve(int x);
	std::string ToString();
    Piece* GetPiece(size_t index) { return &*(this->pieces.begin() + index); }

    /*Overloaded Operators*/
    Piece& operator[](size_t index) { return this->pieces.at(index); }
    std::vector<Piece>& operator()() { return this->pieces; }

private:
    /*Private Helper Methods*/
    template <typename VecT, typename InputT>
    void CollectData(std::vector<VecT> &vec, std::stringstream &stream);
    template <typename T>
    void AppendExponent(std::vector<T> &vec, std::stringstream &stream);
};
