#include "parser.h"

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

void Function::ExecuteBranch(OperationStack &opStack, ValueStack &vStack)
{
    while (opStack.top() != '(')
        ExecuteOperation(vStack, opStack);
    
    opStack.pop();
}

size_t Function::ExecuteDigit(const std::string &exp, size_t n, size_t nPos, ValueStack &vStack)
{
    double value = 0;

    while (nPos < n && isdigit(exp[nPos]))
        value = 10 * value + (exp[nPos++] - '0');

    vStack.push(value);

    return nPos;
}

void Function::ExecuteOperator(char op, ValueStack &vStack, OperationStack &opStack)
{
    while (!opStack.empty() && OperatorCausesEvaluation(opStack.top(), op))
        ExecuteOperation(vStack, opStack);

    opStack.push(op);
}

bool Function::OperatorCausesEvaluation(char prevOp, char op)
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

void Function::ExecuteOperation(ValueStack &vStack, OperationStack &opStack)
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

double Function::EvaluateExpression(const std::string &exp)
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

int Function::Solve(int x)
{
	std::string toEvaluate = this->ToString();
	std::string tmp;
	std::vector<size_t> indexes;

	std::stringstream converter;
	converter << x;
	converter >> tmp;
	tmp = "*" + tmp;

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
		toEvaluate = toEvaluate.substr(0, *i) 
			+ tmp + toEvaluate.substr(*i + 1, size);

		for (std::vector<size_t>::iterator j = ++i; j != indexes.end(); ++j)
			++(*j);
			
		size++;	
	}

	return this->EvaluateExpression(toEvaluate);
}

std::string Function::ToString()
{
    std::stringstream ret;
    for (auto i = this->pieces.cbegin(); i != this->pieces.cend(); i++)
    {
        auto constant = i->constants.cbegin();
        auto variable = i->variables.cbegin();

		if (i->sign != '-' && i != this->pieces.cbegin())
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

