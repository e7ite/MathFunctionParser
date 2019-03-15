// DerivativeCalculator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

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

template <typename VecT, typename InputT>
void CollectData(std::vector<VecT> &vec, std::stringstream &stream)
{
  if (stream.rdbuf()->in_avail())
  {
    InputT tmp;
    stream >> tmp;

    vec.push_back(tmp);

    std::stringstream().swap(stream);
  }
}

template <typename T>
void AppendExponent(std::vector<T> &vec, std::stringstream &stream)
{
  if (stream.rdbuf()->in_avail())
  {
    int tmp = 0;
    vec.at(vec.size() - 1).exponent = tmp;

    std::stringstream().swap(stream);
  }
}

std::vector<Piece> FindPiece(const std::string &func)
{
  std::vector<Piece> p;
  enum Mode : char
  {
    PARSE_SIGN,
    PARSE_CONSTANT,
    PARSE_VARIABLE,
    PARSE_EXPONENT
  };
  std::stringstream integers, variables, exponents;
  Mode mode = PARSE_SIGN;
  Mode prevMode = mode;
  bool exponentToggled = false;

  p.push_back(Piece());

  for (uint32_t i = 0; i < func.size(); i++)
  {
    char index = func.at(i);

    if (mode != PARSE_EXPONENT)
    {
      prevMode = mode;

      if (isdigit(index))
      {
        mode = PARSE_CONSTANT;

        integers << index;
      }
      else if (isalpha(index))
      {
        mode = PARSE_VARIABLE;

        variables << index;
      }
      else if (index == '+' || index == '-')
      {
        mode = PARSE_SIGN;

        if (!i)
          p.at(p.size() - 1).sign = index;
      }
    } 
    else
    {
      exponents << index;
    }

    if (mode != prevMode)
    {
      switch (prevMode)
      {
        case PARSE_CONSTANT:
          CollectData<Constant, int>(p.at(p.size() - 1).constants, integers);
          break;
        
        case PARSE_VARIABLE:
          CollectData<Variable, char>(p.at(p.size() - 1).variables, variables);
          break;
      }
    }

    if (exponentToggled)
    {
      int tmp;
      exponents << index;
      exponents >> tmp;

      if (prevMode == PARSE_CONSTANT)
        AppendExponent<Constant>(p.at(p.size() - 1).constants, exponents);
      else if (prevMode == PARSE_VARIABLE)
        AppendExponent<Variable>(p.at(p.size() - 1).variables, exponents);

      mode = prevMode;
      exponentToggled = false;
    }

    if (index == '^')
    {
      mode = PARSE_EXPONENT;
      exponentToggled = true;
    }

    if (i && (index == '+' || index == '-'))
      p.push_back(Piece(index));
  }

  switch (mode)
  {
  case PARSE_CONSTANT:
    CollectData<Constant, int>(p.at(p.size() - 1).constants, integers);
    break;

  case PARSE_VARIABLE:
    CollectData<Variable, char>(p.at(p.size() - 1).variables, variables);
    break;

  case PARSE_EXPONENT:
    int tmp;
    exponents >> tmp;

    if (prevMode == PARSE_CONSTANT)
      AppendExponent<Constant>(p.at(p.size() - 1).constants, exponents);
    else if (prevMode == PARSE_VARIABLE)
      AppendExponent<Variable>(p.at(p.size() - 1).variables, exponents);
    break;
  }

  return p;
}


std::string ProduceDerivatve(std::string &func)
{
  return "";
}

std::string GetDerivative(const std::string &func)
{
  std::stringstream ret;
  std::vector<Piece> pieces;
  uint32_t i = 0;

  pieces = FindPiece(func);

  for (auto i = pieces.cbegin(); i != pieces.cend(); i++)
  {
    auto constant = i->constants.crbegin();
    auto variable = i->variables.crbegin();

    ret << i->sign;

    while (true)
    {
      bool cEnd = constant != i->constants.crend();
      bool vEnd = variable != i->variables.crend();

      if (!cEnd && !vEnd)
        break;

      if (cEnd)
      {
        int out = pow(constant->value, constant->exponent);
        ret << out;

        constant++;
      }

      if (vEnd)
      {
        if (variable->exponent != 1)
          ret << variable->value << "^" << variable->exponent;
        else
          ret << variable->value;

        variable++;
      }
    }
  }

  return ret.str();
}

int main()
{
  std::string fOfX;
  std::cout << "Enter a function\n";
  std::cin >> fOfX;
  std::cout << "Derivative of f(x): " << GetDerivative(fOfX) << '\n';
}