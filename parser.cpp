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

int Function::Solve(int x)
{
    int sum = 0;
    for (const auto &i : this->pieces)
    {
        auto constant = i.constants.cbegin();
        auto variable = i.variables.cbegin();

        while (true)
		{
			bool cEnd = constant != i.constants.cend();
			bool vEnd = variable != i.variables.cend();

			if (!cEnd && !vEnd)
				break;

            if (vEnd)
            {
             	/*Apply the exponent to any variables*/
                if (!sum)
                    sum += static_cast<int>(
                        pow(x, variable->exponent)
                    );
                else
                    sum *= static_cast<int>(
                        pow(x, variable->exponent)
                    );

                variable++;
            }

			if (cEnd)
			{
                /*Apply the exponent to any constants*/
				if (!sum)
                    sum += static_cast<int>(
                        pow(constant->value, constant->exponent)
                    );
                else
                    sum *= static_cast<int>(
                        pow(constant->value, constant->exponent)
                    );

				constant++;
            }
        }
    }

    return sum;
}

std::string Function::ToString()
{
    std::stringstream ret;
    for (auto i = this->pieces.cbegin(); i != this->pieces.cend(); i++)
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

/*Currently only modifies first variable*/
void ApplyPowerRule(Function &func)
{
    std::vector<Piece>::iterator i = func().begin();
	
	while (i != func().end())
    {
		std::vector<Variable>::iterator var = i->variables.begin();

		if (var == i->variables.end())
		{
			if (i->constants.size())
				i = func().erase(i);
				
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
