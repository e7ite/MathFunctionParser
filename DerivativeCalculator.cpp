// DerivativeCalculator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
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
	int tmp;
	stream >> tmp;

	vec.at(vec.size() - 1).exponent = tmp;

	std::stringstream().swap(stream);
  }
}

std::vector<Piece> FindPieces(const std::string &func)
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

  p.push_back(Piece());

  for (uint32_t i = 0; i < func.size(); i++)
  {
	char index = func.at(i);

	/*Skip spaces*/
	if (index == ' ')
		continue;

	/*If mode is set to PARSE_EXPONENT, directly place into exponent buffer
	  else respectively place input into buffers*/
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

	/*If mode has changed, place data from buffers into its correseponding
	  vector*/
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

	/*if exponent mode is toggled, place exponent into correct vector*/
	if (mode == PARSE_EXPONENT)
	{
	  if (prevMode == PARSE_CONSTANT)
		AppendExponent<Constant>(p.at(p.size() - 1).constants, exponents);
	  else if (prevMode == PARSE_VARIABLE)
		AppendExponent<Variable>(p.at(p.size() - 1).variables, exponents);

	  mode = prevMode;
	}

	/*If current character indicates exponent, place parser into exponent
	  mode*/
	if (index == '^')
	  mode = PARSE_EXPONENT;

	/*if not index 0 and character is a sign, add new piece*/
	if (i && (index == '+' || index == '-'))
	  p.push_back(Piece(index));
  }

  /*Check buffers for any remaining information*/
  switch (mode)
  {
  case PARSE_CONSTANT:
	CollectData<Constant, int>(p.at(p.size() - 1).constants, integers);
	break;

  case PARSE_VARIABLE:
	CollectData<Variable, char>(p.at(p.size() - 1).variables, variables);
	break;

  case PARSE_EXPONENT:
	if (prevMode == PARSE_CONSTANT)
	  AppendExponent<Constant>(p.at(p.size() - 1).constants, exponents);
	else if (prevMode == PARSE_VARIABLE)
	  AppendExponent<Variable>(p.at(p.size() - 1).variables, exponents);
	break;
  }

  return p;
}

/*Currently only modifies first variable*/
void ApplyPowerRule(std::vector<Piece> &p)
{
  std::vector<Piece>::iterator i = p.begin();

  while (i != p.end())
  {
	std::vector<Variable>::iterator var = i->variables.begin();

	if (var == i->variables.end())
	{
		if (i->constants.size())
			i = p.erase(i);

		continue;
	}

	if (!i->constants.size() && var->exponent > 0)
		i->constants.push_back(var->exponent--);
	else
		i->constants.at(0).value *= var->exponent--;

	if (!var->exponent)
		i->variables.erase(var);

	i++;
  }
}

std::string GetDerivative(const std::string &func)
{
  std::stringstream ret;
  std::vector<Piece> pieces;

  /*Parse input*/
  pieces = FindPieces(func);

  /*Apply Power Rule To each piece in the vector*/
  ApplyPowerRule(pieces);

  /*Create a string representation of the vector*/
  for (auto i = pieces.cbegin(); i != pieces.cend(); i++)
  {
	auto constant = i->constants.cbegin();
	auto variable = i->variables.cbegin();

	ret << i->sign;

	while (true)
	{
	  bool cEnd = constant != i->constants.cend();
	  bool vEnd = variable != i->variables.cend();

	  if (!cEnd && !vEnd)
		break;

	  if (cEnd)
	  {
		/*Apply the exponent to any constants*/
		int out = static_cast<int>(
			std::pow(constant->value, constant->exponent)
		);
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
