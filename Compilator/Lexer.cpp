#include"Lexer.h"
#pragma warning(disable : 4996)

map<string, EOperand> keys = {
	{"if", ifsy},
	{"do", dosy},
	{"of", ofsy},
	{"or", orsy},
	{"in", insy},
	{"to", tosy},
	{"end", endsy},
	{"var", varsy},
	{"div", divsy},
	{"and", andsy},
	{"not", notsy},
	{"for", forsy},
	{"mod", modsy},
	{"nil", nilsy},
	{"set", setsy},
	{"then", thensy},
	{"else", elsesy},
	{"case", casesy},
	{"file", filesy},
	{"goto", gotosy},
	{"type", typesy},
	{"with", withsy},
	{"begin", beginsy},
	{"while", whilesy},
	{"array", arraysy},
	{"const", constsy},
	{"label", labelsy},
	{"until", untilsy},
	{"downto", downtosy},
	{"packed", packedsy},
	{"record", recordsy},
	{"repeat", repeatsy},
	{"program", programsy},
	{"function", functionsy},
	{"procedure", proceduresy},
};

CLexer::CLexer(string file_name)
{
	this->file_name = file_name;
	try
	{
		in.open(file_name);
	}
	catch (ifstream::failure e)
	{
		std::cerr << "Exception opening file: " << std::strerror(errno) << "\n";
	}
	this->errorManager = new CErrorManager();
	getNextChar();
}

CLexer::~CLexer()
{
	delete errorManager;
}

//считывает следующий символ в переменную ch
void CLexer::getNextChar()
{
	if (in.is_open())
	{
		int length = line.length() - 1;
		if (position.charnumber >= length)
		{
			if (!getline(in, line))
			{
				ch = '\0';
				in.close();
				return;
			}

			position.linenumber++;
			position.charnumber = 0;
		}
		else
		{
			position.charnumber++;
		}
		ch = line[position.charnumber];
		return;
	}
	else
	{
		//подумать над удалением
		CError* error = new CError(0, position);
		errorManager->AddError(error);
	}

	ch = '\0';
	in.close();
	return;
}

void CLexer::passSpace()
{
	//удаление пробелов
	while (ch == ' ' || ch == '\t')
		getNextChar();
}

//работать с указателями
CToken* CLexer::getNextToken()
{
	CToken* returnToken = nullptr;

	passSpace();
	//удаление коментариев
	if (ch == '{')
	{
		while (ch != '}')
		{
			getNextChar();
		}
	}

	passSpace();
	if(ch == '/')
	{
		getNextChar();
		//удаление строчных коментариев
		if (ch == '/')
		{
			while (position.charnumber != 0)
			{
				getNextChar();
			}
		}
		else
		{
			returnToken = new CToken(EOperand::slash, "/");
			return returnToken;
		}
	}
	passSpace();

	//определение идентификаторов
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	{
		string ident = "";
		while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
		{
			ident += ch;
			getNextChar();
		}
		if (keys.count(ident) > 0)
		{
			returnToken = new CToken(keys[ident], ident);
		}
		else
		{
			returnToken = new CToken(ident);
		}
	}
	//определение чисел
	else if (ch >= '0' && ch <= '9')
	{
		bool is_int = true;
		string number;
		while (ch >= '0' && ch <= '9')
		{
			number += ch;
			getNextChar();
		}
		if (ch == '.')
		{
			is_int = false;
			number += ch;
			getNextChar();
			while (ch >= '0' && ch <= '9')
			{
				number += ch;
				getNextChar();
			}
		}
		if (is_int)
		{
			int int_number;
			try
			{
				int_number = stoi(number);
				returnToken = new CToken(new CIntVariant(int_number));
			}
			catch (const std::out_of_range& e)
			{
				CError* error = new CLexicalError(11, position);
				errorManager->AddError(error);
				returnToken = new CToken(new CIntVariant(0));
			}
		}
		else
		{
			float real_number;
			try
			{
				real_number = stof(number);
				returnToken = new CToken(new CRealVariant(real_number));
			}
			catch (const std::out_of_range& e)
			{
				CError* error = new CLexicalError(12, position);
				errorManager->AddError(error);
				returnToken = new CToken(new CRealVariant(0));
			}
		}
	}
	else
	{
		//определение операндов
		switch (ch)
		{
		case '<':
		{
			getNextChar();
			if (ch == '=')
			{
				returnToken = new CToken(EOperand::laterequal, "<=");
				getNextChar();
			}
			else if (ch == '>')
			{
				returnToken = new CToken(EOperand::latergreater, "<>");
				getNextChar();
			}
			else
			{
				returnToken = new CToken(EOperand::later, "<");
			}
			break;
		}
		case '>':
		{
			getNextChar();
			if (ch == '=')
			{
				returnToken = new CToken(EOperand::greaterequal, ">=");
				getNextChar();
			}
			else
			{
				returnToken = new CToken(EOperand::greater, ">");
			}
			break;
		}
		case ':':
		{
			getNextChar();
			if (ch == '=')
			{
				returnToken = new CToken(EOperand::assign, ":=");
				getNextChar();
			}
			else
			{
				returnToken = new CToken(EOperand::colon, ":");
			}
			break;
		}
		case ';':
		{
			returnToken = new CToken(EOperand::semicolon, ";");
			getNextChar();
			break;
		}
		case '+':
		{
			returnToken = new CToken(EOperand::plus, "+");
			getNextChar();
			break;
		}
		case '-':
		{
			returnToken = new CToken(EOperand::minus, "-");
			getNextChar();
			break;
		}
		case '*':
		{
			getNextChar();
			if (ch == ')')
			{
				returnToken = new CToken(EOperand::rcomment, "*)");
				getNextChar();
			}
			else
			{
				returnToken = new CToken(EOperand::star, "*");
			}
			break;
		}
		case '=':
		{
			returnToken = new CToken(EOperand::equal, "=");
			getNextChar();
			break;
		}
		case '(':
		{
			getNextChar();
			if (ch == '*')
			{
				returnToken = new CToken(EOperand::lcomment, "(*");
				getNextChar();
			}
			else
			{
				returnToken = new CToken(EOperand::leftpar, "(");
			}
			break;
		}
		case ')':
		{
			returnToken = new CToken(EOperand::rightpar, ")");
			getNextChar();
			break;
		}
		case '{':
		{
			returnToken = new CToken(EOperand::flpar, "}");
			getNextChar();
			break;
		}
		case '}':
		{
			returnToken = new CToken(EOperand::frpar, "{");
			getNextChar();
			break;
		}
		case '[':
		{
			returnToken = new CToken(EOperand::lbracket, "[");
			getNextChar();
			break;
		}
		case ']':
		{
			returnToken = new CToken(EOperand::rbracket, "]");
			getNextChar();
			break;
		}
		case '.':
		{
			getNextChar();
			if (ch == '.')
			{
				returnToken = new CToken(EOperand::twopoints, "..");
				getNextChar();
			}
			else
			{
				returnToken = new CToken(EOperand::point, ".");
			}
			break;
		}
		case ',':
		{
			returnToken = new CToken(EOperand::comma, ",");
			getNextChar();
			break;
		}
		case '^':
		{
			returnToken = new CToken(EOperand::arrow, "^");
			getNextChar();
			break;
		}
		case '\'':
		{
			string str = "";
			getNextChar();
			while (ch != '\'')
			{
				str += ch;
				getNextChar();
			}
			returnToken = new CToken(new CStringVariant(str));
			getNextChar();
			break;
		}
		case '\0':
		{
			break;
		}
		default:
		{
			CError* error = new CLexicalError(13, position);
			errorManager->AddError(error);
			getNextChar();
			returnToken = getNextToken();
			break;
		}
		}
	}
	return returnToken;
}

CToken* CLexer::skipTo(CToken* cur_token, set<EOperand> symbols)
{
	CToken* token = cur_token;
	do
	{
		if (token->m_type == ETokenType::operand && symbols.count(token->m_op) > 0 )
		{
			break;
		}
	} while (token = getNextToken());

	return token;
}

//int main()
//{
//	setlocale(LC_ALL, "Russian");
//	string file_name = "C:\\Users\\м видео\\Desktop\\Compilator\\Debug\\programm3.txt";
//	CToken* token;
//	CErrorManager* errorManager = new CErrorManager();
//	CLexer* lexer = new CLexer(file_name, errorManager);
//	while (token = lexer->getNextToken())
//	{
//		cout << token->ToString() << endl;
//	}
//	errorManager->PrintErrors();
//	//ifstream file;
//	//file.open(file_name);
//	//string str;                               
//	//while (getline(file, str)){
//	//	cout << str;
//	//}
//	//file.close();
//	return 0;
//}