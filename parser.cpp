#include "parser.hpp"

template <typename VecT, typename InputT>
void Function::CollectData(std::vector<VecT> &vec, std::stringstream &stream)
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
void Function::AppendExponent(std::vector<T> &vec, std::stringstream &stream)
{
    if (stream.rdbuf()->in_avail())
    {
		int tmp;
		stream >> tmp;

		vec.at(vec.size() - 1).exponent = tmp;

		std::stringstream().swap(stream);
    }
}

size_t Function::ExecuteDigit(const std::string &exp, size_t n, 
	size_t nPos, bool negative, ValueStack &vStack) const
{
	bool floatingPoint = false;
    double value = 0;
	std::stringstream converter;

	/*Get corresponding value including higher powers of 10 */
    while (nPos < n)
	{
		if (!floatingPoint && exp[nPos] == '.')
		{
			floatingPoint = true;
			converter << exp[nPos++];
			continue;
		}

		if (isdigit(exp[nPos]))
			converter << exp[nPos++]; 
		else
			break;
	}

	converter >> value;

    vStack.push(negative ? -value : value);

    return nPos;
}

void Function::ExecuteOperator(char op, ValueStack &vStack,
	OperationStack &opStack) const
{
    while (!opStack.empty() && OperatorCausesEvaluation(opStack.top(), op))
        ExecuteOperation(vStack, opStack);

    opStack.push(op);
}

void Function::ExecuteOperation(ValueStack &vStack, OperationStack &opStack) const
{
	char operandCount = 0;
	double operands[2];
	while (operandCount < 2)
		if (vStack.size())
		{
			operands[operandCount++] = vStack.top();			
			vStack.pop();
		}
		else 
			break;

    char operation = opStack.top();
    opStack.pop();
    
    double result;
	switch (operandCount)
	{
		case 2:
			switch (operation)
			{
				case '+':
					result = operands[1] + operands[0];
					break;
				case '-':
					result = operands[1] - operands[0];
					break;
				case '*':
					result = operands[1] * operands[0];
					break;
				case '/':
					result = operands[1] / operands[0];
					break;
				case '^':
					result = pow(operands[1], operands[0]);
					break;
			}
			break;
		case 1:
			if (operation == '-')
				result = -operands[0];
			break;
		default:
			return;
	}
	
    vStack.push(result);
}

void Function::ExecuteBranch(OperationStack &opStack, ValueStack &vStack) const
{	
    while (opStack.top() != '(')
        ExecuteOperation(vStack, opStack);
    
    opStack.pop();
}

bool Function::OperatorCausesEvaluation(char prevOp, char op) const
{
    switch (op)
    {
        case '+':
        case '-':
		/*Do not execute if we are not at a closed parentheses*/
            return prevOp != '('; 
        case '*':
        case '/':
            return prevOp == '*' || prevOp == '/';
        case ')':
            return true;
    }
	
    return false;
}

double Function::EvaluateExpression(const std::string &exp) const
{
    OperationStack opStack;
    ValueStack vStack;
    size_t nPos = 0;
    size_t size = exp.size();
	bool neg = false;

    opStack.push('(');

    while (nPos <= size)
    {
		if (exp[nPos] == '_')
		{
			neg = true;
			nPos++;
		}
        else if (nPos == size || exp[nPos] == ')')
        {
            ExecuteBranch(opStack, vStack);
			nPos++;
        }
        else if (isdigit(exp[nPos]))
		{
            nPos = ExecuteDigit(exp, size, nPos, neg, vStack);
			neg = false;
		}
        else
            ExecuteOperator(exp[nPos++], vStack, opStack);
    }

    return vStack.top();
}

double Function::Solve(double x) const
{
	std::string toEvaluate = this->ToString();
	std::string xString = std::to_string(x);

	if (xString == "nan" || xString == "infinity" || xString == "inf")
		return 0.0;

	std::vector<size_t> indexes;

	if (x < 0)
		xString[0] = '_';

	size_t index = toEvaluate.find("x");
	size_t size = toEvaluate.size();

	while (index != std::string::npos 
		&& std::find(indexes.begin(),
		indexes.end(), index) == indexes.end())
	{
		indexes.push_back(index);

		index = toEvaluate.substr(index + 1, size).find("x") + index + 1;		
	}

	std::vector<size_t>::iterator i = indexes.begin();

	while (i != indexes.end())
	{
		std::string tmp = xString;

		if (this->GetPiece(std::distance(indexes.begin(), i))->constants.size())
			tmp = "*" + tmp;

		size_t xSize = tmp.size() - 1;

		toEvaluate = toEvaluate.substr(0, *i) 
			+ tmp + toEvaluate.substr(*i + 1, size);

		for (std::vector<size_t>::iterator j = ++i; j != indexes.end(); ++j)
			(*j) += xSize;
			
		size += xSize;	
	}

	return this->EvaluateExpression(toEvaluate);
}

std::string Function::ToString() const
{
    std::stringstream ret;
    for (auto i = this->pieces.cbegin(); i != this->pieces.cend(); i++)
    {
        auto constant = i->constants.cbegin();
        auto variable = i->variables.cbegin();

		if (i != this->pieces.cbegin() || i->sign == '-')
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
					pow(constant->value, constant->exponent)
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
	
	if (ret.rdbuf()->in_avail())
  	    return ret.str();
	else
        return "0";
}

std::vector<Piece> Function::FindPieces(const std::string &func)
{
    enum Mode : char
    {
		PARSE_SIGN,
		PARSE_CONSTANT,
		PARSE_VARIABLE,
		PARSE_EXPONENT
	};
    std::vector<Piece> p { Piece() };
    std::stringstream integers, variables, exponents;
    Mode mode = PARSE_SIGN;
    Mode prevMode = mode;

    for (uint32_t i = 0; i < func.size(); i++)
    {
		char index = func.at(i);

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
				this->CollectData<Constant, int>(p.at(p.size() - 1).constants, integers);
    			break;
			case PARSE_VARIABLE:
				this->CollectData<Variable, char>(p.at(p.size() - 1).variables, variables);
	    		break;
			}
		}

		/*If exponent mode is toggled, place exponent into correct vector*/
		if (mode == PARSE_EXPONENT)
		{
			if (prevMode == PARSE_CONSTANT)
				this->AppendExponent<Constant>(p.at(p.size() - 1).constants, exponents);
			else if (prevMode == PARSE_VARIABLE)
				this->AppendExponent<Variable>(p.at(p.size() - 1).variables, exponents);

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

