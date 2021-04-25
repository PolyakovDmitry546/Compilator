#include "SyntaxAnalyzer.h"

CSyntaxAnalyzer::CSyntaxAnalyzer(CLexer* lexer)
{
	this->lexer = lexer;
	this->errorManager = lexer->errorManager;
}

CSyntaxAnalyzer::~CSyntaxAnalyzer()
{
	delete token;
	delete intType;
	delete realType;
	delete charType;
	delete stringType;
	delete boolType;
}

void CSyntaxAnalyzer::Analyze()
{
	createFictScope();
	getNextToken();
	try
	{
		programme();
	}
	catch(exception){}
	errorManager->PrintErrors();
}

void CSyntaxAnalyzer::getNextToken()
{
	auto oldToken = token;
	token = lexer->getNextToken();
	position = lexer->position;
	delete oldToken;
	//cout << token->ToString() << endl;
}

void CSyntaxAnalyzer::skipTo(set<EOperand> symbols)
{
	token = lexer->skipTo(token, symbols);
}

void CSyntaxAnalyzer::accept(EOperand operand)
{
	if (this->token != nullptr && this->token->m_type == ETokenType::operand &&
		this->token->m_op == operand) 
	{
		getNextToken();
	}
	else
	{
		throw exception();
	}
}

//программа
void CSyntaxAnalyzer::programme() 
{
	scopes.push_back(map<string, SIdentifierTableItem> {});
	try
	{
		accept(EOperand::programsy);
	}
	catch(exception)
	{
		errorManager->AddError(new CSyntaxError(21, position));
	}
	try
	{
		if (token->m_type == ETokenType::ident)
		{
			scopes.back()[token->m_ident] = SIdentifierTableItem{ progs, nullptr };
		}
		name();
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(23, position));
		skipTo(set<EOperand> {semicolon});
	}
	//todo
	try
	{
		accept(EOperand::semicolon);
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(24, position));
	}
	block();
	try
	{
		accept(EOperand::point);
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(22, position));
	}
}

//блок
void CSyntaxAnalyzer::block()
{
	constantSection();
	typeSection();
	variableSection();
	operatorsSection();
}

//раздел констант
void CSyntaxAnalyzer::constantSection()
{
	set<EOperand> s = { semicolon, typesy, varsy, beginsy };
	if (token->m_type == ETokenType::operand && token->m_op == EOperand::constsy)
	{
		accept(EOperand::constsy);
		do
		{
			try
			{
				constantDeclaration();
			}
			catch (exception)
			{
				skipTo(s);
				if (token->m_op != EOperand::semicolon)
					return;
			}
			try
			{
				accept(EOperand::semicolon);
			}
			catch (exception)
			{
				errorManager->AddError(new CSyntaxError(24, position));
				skipTo(s);
				if (token->m_op != EOperand::semicolon)
					return;
			}
		} while (token->m_type == ETokenType::ident);
	}
}

//определение константы
void CSyntaxAnalyzer::constantDeclaration()
{
	string newConst;
	try
	{
		if (token->m_type == ETokenType::ident)
		{
			if (scopes.back().count(token->m_ident) > 0)
			{
				errorManager->AddError(new CSemanticError(32, position));
			}
			else
			{
				newConst = token->m_ident;
				scopes.back()[token->m_ident] = SIdentifierTableItem{ consts, nullptr };
			}
		}
		name();
	}
	catch(exception)
	{
		errorManager->AddError(new CSyntaxError(26, position));
		throw exception();
	}
	try
	{
		accept(EOperand::equal);
	}
	catch(exception)
	{
		errorManager->AddError(new CSyntaxError(25, position));
		throw exception();
	}
	constant(newConst);
}

//константа
void CSyntaxAnalyzer::constant(string newConst)
{
	sign();
	if (token->m_type == ETokenType::value)
	{
		if (token->m_val->m_t == EVarType::vString)
		{
			scopes.back()[newConst].type = stringType;
			getNextToken();
		}
		else
		{
			if (token->m_val->m_t == EVarType::vInt)
			{
				scopes.back()[newConst].type = intType;
			}
			if (token->m_val->m_t == EVarType::vReal)
			{
				scopes.back()[newConst].type = realType;
			}
			unsignedNumber();
		}
	}
	else
	{
		if (token->m_type == ETokenType::ident)
		{
			auto ident = findIdent(token->m_ident);
			if (ident == nullptr)
			{
				errorManager->AddError(new CSemanticError(31, position));
			}
			else
			{
				if (ident->mode != consts)
				{
					errorManager->AddError(new CSemanticError(33, position));
				}
				else
				{
					scopes.back()[newConst].type = ident->type;
				}
			}
		}
		try
		{
			constantName();
		}
		catch (exception)
		{
			errorManager->AddError(new CSyntaxError(26, position));
			throw exception();
		}
	}
}

//имя константы
void CSyntaxAnalyzer::constantName()
{
	name();
}

//строка
//void CSyntaxAnalyzer::str()
//{
//	if (token->m_type == ETokenType::value && token->m_val->m_t == EVarType::vString)
//	{
//		getNextToken();
//	}
//	else
//	{
//		throw exception();
//	}
//}

//знак + | -
void CSyntaxAnalyzer::sign()
{
	if (token->m_type == ETokenType::operand)
	{
		if (token->m_op == EOperand::plus)
		{
			getNextToken();
		}
		else if (token->m_op == EOperand::minus)
		{
			getNextToken();
		}
	}
}

//раздел операторов
void CSyntaxAnalyzer::operatorsSection()
{
	try
	{
		compoundOperator();
	}
	catch (exception)
	{
		skipTo(set<EOperand> {point});
	}
}

//составной оператор
void CSyntaxAnalyzer::compoundOperator()
{
	set<EOperand> s = { semicolon, endsy, beginsy, point };
	try
	{
		accept(EOperand::beginsy);
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(210, position));
	}
	try
	{
		operatorF();
	}
	catch (exception)
	{
		skipTo(s);
		if (token->m_op == EOperand::beginsy || token->m_op == EOperand::point)
			return;
	}
	while (token->m_type == ETokenType::operand && token->m_op == EOperand::semicolon)
	{
		getNextToken();
		if (token->m_op == EOperand::endsy)
		{
			break;
		}
		try 
		{
			operatorF();
		}
		catch (exception)
		{
			skipTo(s);
			if (token->m_op == EOperand::beginsy || token->m_op == EOperand::point)
				return;
		}
	}
	try
	{
		accept(EOperand::endsy);
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(211, position));
	}
}

//оператор
void CSyntaxAnalyzer::operatorF()
{
	unmarkedOperator();
}

//непомеченный оператор
void CSyntaxAnalyzer::unmarkedOperator()
{
	if (token->m_type == ETokenType::ident)
	{
		simpleOperator();
	}
	else if (token->m_type == ETokenType::operand)
	{
		complexOperator();
	}
	else
	{
		errorManager->AddError(new CSyntaxError(215, position));
		throw exception();
	}
}

//простой оператор
void CSyntaxAnalyzer::simpleOperator()
{
	//todo
	assignmentOperator();
}

//оператор присваивания
void CSyntaxAnalyzer::assignmentOperator()
{
	CBaseType* varType = nullptr;
	try
	{
		if (token->m_type == ETokenType::ident)
		{
			auto ident = findIdent(token->m_ident);
			if (ident == nullptr)
			{
				errorManager->AddError(new CSemanticError(31, position));
			}
			else
			{
				if (ident->mode != vars)
				{
					errorManager->AddError(new CSemanticError(35, position));
				}
				else
				{
					varType = ident->type;
				}
			}
		}
		variable();
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(212, position));
		throw exception();
	}
	try
	{
		accept(EOperand::assign);
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(213, position));
		throw exception();
	}
	try
	{
		CBaseType* expressionType = expression();
		if (expressionType == nullptr)
		{
			errorManager->AddError(new CSemanticError(396, position));
		}
		if (varType != nullptr)
		{
			if (varType == realType)
			{
				if (!(expressionType == intType || expressionType == realType))
				{
					errorManager->AddError(new CSemanticError(37, position));
				}
			}
			else if(varType != expressionType)
			{
				errorManager->AddError(new CSemanticError(38, position));
			}
		}
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(214, position));
		throw exception();
	}
}

//сложный оператор
void CSyntaxAnalyzer::complexOperator()
{
	if (token->m_type == ETokenType::operand)
	{
		switch (token->m_op)
		{
		case EOperand::beginsy:
			compoundOperator();
			break;
		case EOperand::ifsy:
			conditionalOperator();
			break;
		//case EOperand::casesy:
		//	break;
		//case EOperand::forsy:
		//	break;
		case EOperand::whilesy:
			preconditionedLoopOperator();
			break;
		//case EOperand::repeatsy:
		//	break;
		//case EOperand::withsy:
		//	break;
		default:
			errorManager->AddError(new CSyntaxError(215, position));
			throw exception();
			break;
		}
	}
	else
	{
		throw exception();
	}
}

//условный оператор
void CSyntaxAnalyzer::conditionalOperator()
{
	accept(EOperand::ifsy);
	try
	{
		CBaseType* expressionType = expression();
		if (expressionType != boolType)
		{
			errorManager->AddError(new CSemanticError(36, position));
		}
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(214, position));
		throw exception();
	}
	try
	{
		accept(EOperand::thensy);
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(216, position));
	}
	try
	{
		operatorF();
	}
	catch (exception)
	{
		skipTo(set<EOperand> {elsesy, semicolon});
	}
	if (token->m_type == ETokenType::operand && token->m_op == EOperand::elsesy)
	{
		getNextToken();
		try
		{
			operatorF();
		}
		catch (exception)
		{
			skipTo(set<EOperand> {semicolon});
		}
	}
}

//оператор цикла с предусловием(цикл while)
void CSyntaxAnalyzer::preconditionedLoopOperator()
{
	accept(EOperand::whilesy);
	try
	{
		CBaseType* expressionType = expression();
		if (expressionType != boolType)
		{
			errorManager->AddError(new CSemanticError(36, position));
		}
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(214, position));
		throw exception();
	}
	try
	{
		accept(EOperand::dosy);
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(217, position));
		throw exception();
	}
	try
	{
		operatorF();
	}
	catch (exception)
	{
		skipTo(set<EOperand> {semicolon});
	}
}

//раздел описания типов
void CSyntaxAnalyzer::typeSection()
{
	set<EOperand> s = { semicolon, varsy, beginsy };
	if (token->m_type == ETokenType::operand &&
		token->m_op == EOperand::typesy)
	{
		accept(EOperand::typesy);
		do
		{
			try
			{
				typeDeclaration();
			}
			catch (exception)
			{
				skipTo(s);
				if (token->m_op != EOperand::semicolon)
					return;
			}
			try 
			{
				accept(EOperand::semicolon);
			}
			catch (exception)
			{
				errorManager->AddError(new CSyntaxError(24, position));
				skipTo(s);
				if (token->m_op != EOperand::semicolon)
					return;
			}
		} while (token->m_type == ETokenType::ident);
	}
}

//описание типов
void CSyntaxAnalyzer::typeDeclaration()
{
	string newType;
	try
	{
		if (token->m_type == ETokenType::ident)
		{
			if (scopes.back().count(token->m_ident) > 0)
			{
				errorManager->AddError(new CSemanticError(32, position));
			}
			else
			{
				newType = token->m_ident;
				scopes.back()[token->m_ident] = SIdentifierTableItem{ types, nullptr };
			}
		}
		name();
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(27, position));
		throw exception();
	}
	try
	{
		accept(EOperand::equal);
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(25, position));
		throw exception();
	}
	try
	{
		if (token->m_type == ETokenType::ident)
		{
			auto ident = findIdent(token->m_ident);
			if (ident == nullptr)
			{
				errorManager->AddError(new CSemanticError(31, position));
			}
			else
			{
				if (ident->mode != types)
				{
					errorManager->AddError(new CSemanticError(34, position));
				}
				else
				{
					scopes.back()[newType].type = ident->type;
				}
			}
		}
		type();
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(27, position));
		throw exception();
	}
}

//тип
void CSyntaxAnalyzer::type()
{
	if (token->m_type == ETokenType::ident)
	{
		simpleType();
	}
	//todo
	else
	{
		throw exception();
	}
}

//простой тип
void CSyntaxAnalyzer::simpleType()
{
	typeName();
}

//имя типа
void CSyntaxAnalyzer::typeName()
{
	name();
}

//раздел описания переменных
void CSyntaxAnalyzer::variableSection()
{
	set<EOperand> s = { semicolon, beginsy };
	if (token->m_type == ETokenType::operand &&
		token->m_op == EOperand::varsy)
	{
		accept(EOperand::varsy);
		do
		{
			try
			{
				variableDeclaration();
			}
			catch (exception)
			{
				skipTo(s);
				if (token->m_op != EOperand::semicolon)
					return;
			}
			try
			{
				accept(EOperand::semicolon);
			}
			catch (exception)
			{
				errorManager->AddError(new CSyntaxError(24, position));
				skipTo(s);
				if (token->m_op != EOperand::semicolon)
					return;
			}
		} while (token->m_type == ETokenType::ident);
	}
}


void CSyntaxAnalyzer::addVars(list<string> &newVariables)
{
	if (token->m_type == ETokenType::ident)
	{
		if (scopes.back().count(token->m_ident) > 0)
		{
			errorManager->AddError(new CSemanticError(32, position));
		}
		else
		{
			newVariables.push_back(token->m_ident);
			scopes.back()[token->m_ident] = SIdentifierTableItem{ vars, nullptr };
		}
	}
}

//описание переменных
void CSyntaxAnalyzer::variableDeclaration()
{
	list<string> newVariables;
	try
	{
		addVars(newVariables);
		name();
		while (token->m_type == ETokenType::operand &&
			token->m_op == EOperand::comma)
		{
			getNextToken();
			addVars(newVariables);
			name();
		}
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(29, position));
		throw exception();
	}
	try
	{
		accept(EOperand::colon);
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(28, position));
		throw exception();
	}
	try
	{
		if (token->m_type == ETokenType::ident)
		{
			auto ident = findIdent(token->m_ident);
			if (ident == nullptr)
			{
				errorManager->AddError(new CSemanticError(31, position));
			}
			else
			{
				if (ident->mode != types)
				{
					errorManager->AddError(new CSemanticError(34, position));
				}
				else
				{
					for(auto i = newVariables.begin(); i != newVariables.end(); ++i)
					scopes.back()[*i].type = ident->type;
				}
			}
		}
		type();
	}
	catch (exception)
	{
		errorManager->AddError(new CSyntaxError(27, position));
		throw exception();
	}
}

//void CSyntaxAnalyzer::enumeratedType()
//{
//	if (token->m_type == ETokenType::operand &&
//		token->m_op == EOperand::leftpar)
//	{
//		accept(EOperand::leftpar);
//		while (token->m_type == ETokenType::ident)
//		{
//			getNextToken();
//			accept(EOperand::comma);
//		}
//		accept(EOperand::rightpar);
//	}
//}

//void CSyntaxAnalyzer::intervalType()
//{
//
//}

//выражение
CBaseType* CSyntaxAnalyzer::expression()
{
	CBaseType* type1 = simpleExpression();
	CBaseType* type = type1;
	if (token->m_type == ETokenType::operand && (token->m_op == EOperand::latergreater ||
		token->m_op == EOperand::later || token->m_op == EOperand::laterequal ||
		token->m_op == EOperand::greaterequal || token->m_op == EOperand::greater ||
		token->m_op == EOperand::equal || token->m_op == EOperand::insy))
	{
		EOperand operand = token->m_op;
		getNextToken();
		CBaseType* type2 = simpleExpression();
		if (type2 == nullptr)
		{
			type = nullptr;
		}
		else if (type1 != type2 && !((type1 == intType && type2 == realType) 
			|| (type1 == realType && type2 == intType)))
		{
			errorManager->AddError(new CSemanticError(397, position));
		}
		else if(type != nullptr)
		{
			type = boolType;
		}
	}

	return type;
}

//операция отношения
//void CSyntaxAnalyzer::relationshipOperation()
//{
//	switch (token->m_op)
//	{
//	case EOperand::latergreater:
//		getNextToken();
//		break;
//	case EOperand::later:
//		getNextToken();
//		break;
//	case EOperand::laterequal:
//		getNextToken();
//		break;
//	case EOperand::greaterequal:
//		getNextToken();
//		break;
//	case EOperand::greater:
//		getNextToken();
//		break;
//	case EOperand::equal:
//		getNextToken();
//		break;
//	case EOperand::insy:
//		getNextToken();
//		break;
//	default:
//		break;
//	}
//}

//простое выражение
CBaseType* CSyntaxAnalyzer::simpleExpression()
{
	sign();
	CBaseType* type1 = term();
	while (token->m_type == ETokenType::operand && (token->m_op == EOperand::plus ||
		token->m_op == EOperand::minus || token->m_op == EOperand::orsy))
	{
		EOperand operand = token->m_op;
		getNextToken();
		CBaseType* type2 = term();
		if (operand == EOperand::orsy)
		{
			if (type1 != boolType || type2 != boolType)
			{
				errorManager->AddError(new CSemanticError(392, position));
			}
		}
		else if (operand == EOperand::minus)
		{
			if (!(type1 == intType || type2 == intType || type1 == realType || type2 == realType))
			{
				errorManager->AddError(new CSemanticError(395, position));
			}
			else if (type2 == realType)
			{
				type1 == type2;
			}
		}
		else
		{
			if (!(type1 == intType || type2 == intType || type1 == realType || type2 == realType) &&
				!(type1 == stringType && type2 == stringType))
			{
				errorManager->AddError(new CSemanticError(395, position));
			}
			if (type2 == realType)
			{
				type1 == type2;
			}
		}
	}

	return type1;
}

//арифметическое выражение
//void CSyntaxAnalyzer::arithmeticExpression()
//{
//	sign();
//	term();
//	while (token->m_op == EOperand::plus || token->m_op == EOperand::minus)
//	{
//		getNextToken();
//		term();
//	}
//}

//слагаемое
CBaseType* CSyntaxAnalyzer::term()
{
	CBaseType* type1 = factor();
	while (token->m_type == ETokenType::operand && (token->m_op == EOperand::star || 
		token->m_op == EOperand::slash || token->m_op == EOperand::modsy || 
		token->m_op == EOperand::divsy || token->m_op == EOperand::andsy))
	{
		EOperand operand = token->m_op;
		getNextToken();
		CBaseType* type2 = factor();
		if (operand == EOperand::modsy || operand == EOperand::divsy)
		{
			if (type1 != intType || type2 != intType)
			{
				errorManager->AddError(new CSemanticError(391, position));
			}
		}
		else if (operand == EOperand::andsy)
		{
			if (type1 != boolType || type2 != boolType)
			{
				errorManager->AddError(new CSemanticError(392, position));
			}
		}
		else if (operand == EOperand::star)
		{
			if (!(type1 == intType || type2 == intType || type1 == realType || type2 ==realType))
			{
				errorManager->AddError(new CSemanticError(393, position));
			}
			else if (type2 == realType)
			{
				type1 == type2;
			}
		}
		else if (operand == EOperand::slash)
		{
			if (!(type1 == intType || type2 == intType || type1 == realType || type2 == realType))
			{
				errorManager->AddError(new CSemanticError(394, position));
			}
			else if (type2 == realType)
			{
				type1 == type2;
			}
		}
	}

	return type1;
}

//множитель
CBaseType* CSyntaxAnalyzer::factor()
{
	CBaseType* type = nullptr;
	if (token->m_type == ETokenType::value)
	{
		type = unsignedConstant();
	}
	else if (token->m_type == ETokenType::operand)
	{
		if (token->m_op == EOperand::leftpar)
		{
			getNextToken();
			type = expression();
			accept(EOperand::rightpar);
		}
		else if(token->m_op == EOperand::notsy)
		{
			getNextToken();
			type = factor();
			if (type != boolType)
			{
				errorManager->AddError(new CSemanticError(392, position));
			}
		}
		else
		{
			type = unsignedConstant();
		}
	}
	else
	{
		auto ident = findIdent(token->m_ident);
		if (ident == nullptr)
		{
			errorManager->AddError(new CSemanticError(31, position));
			ident = new SIdentifierTableItem{ EIdentUseMode::vars, nullptr };
			scopes.back()[token->m_ident] = *ident;
		}
		if (ident->mode == EIdentUseMode::vars)
		{
			type = ident->type;
			variable();
		}
		else if (ident->mode == EIdentUseMode::consts)
		{
			type = ident->type;
			unsignedConstant();
		}
		else
		{
			errorManager->AddError(new CSemanticError(39, position));
			throw exception();
		}
	}

	return type;
}

//константа без знака
CBaseType* CSyntaxAnalyzer::unsignedConstant()
{
	CBaseType* type = nullptr;
	if (token->m_type == ETokenType::operand)
	{
		if (token->m_op == EOperand::nilsy)
		{
			getNextToken();
		}
		else
		{
			throw exception();
		}
	}
	else if (token->m_type == ETokenType::ident)
	{
		auto ident = findIdent(token->m_ident);
		if (ident != nullptr)
		{
			if (ident->mode == consts)
			{
				type = ident->type;
			}
			else
			{
				errorManager->AddError(new CSemanticError(33, position));
			}
		}
		constantName();
	}
	else
	{
		if (token->m_val->m_t == EVarType::vString)
		{
			type = stringType;
			getNextToken();
		}
		else
		{
			type = unsignedNumber();
		}
	}

	return type;
}

//операция умножения
//void CSyntaxAnalyzer::multiplicationOperation()
//{
//	switch (token->m_op)
//	{
//	case EOperand::star:
//		accept(EOperand::star);
//		break;
//	case EOperand::slash:
//		accept(EOperand::slash);
//		break;
//	case EOperand::modsy:
//		accept(EOperand::modsy);
//		break;
//	case EOperand::divsy:
//		accept(EOperand::divsy);
//		break;
//	default:
//		break;
//	}
//}

//число без знака
CBaseType* CSyntaxAnalyzer::unsignedNumber()
{
	CBaseType* type = nullptr;
	if (token->m_type == ETokenType::value)
	{
		if (token->m_val->m_t == EVarType::vReal)
		{
			type = realType;
			unsignedReal();
		}
		else if(token->m_val->m_t == EVarType::vInt)
		{
			type = intType;
			unsignedInteger();
		}
		else
		{
			throw exception();
		}
	}
	else
	{
		throw exception();
	}

	return type;
}

//вещественное без знака
void CSyntaxAnalyzer::unsignedReal()
{
	//todo
	if (token->m_type == ETokenType::value && token->m_val->m_t == EVarType::vReal)
	{
		getNextToken();
	}
}

//целое без знака
void CSyntaxAnalyzer::unsignedInteger()
{
	if (token->m_type == ETokenType::value && token->m_val->m_t == EVarType::vInt)
	{
		getNextToken();
	}
}

//переменная
void CSyntaxAnalyzer::variable()
{
	//todo
	variableName();
}

//имя переменной
void CSyntaxAnalyzer::variableName()
{
	name();
}

//имя
void CSyntaxAnalyzer::name()
{
	identifier();
}

//идентификатор
void CSyntaxAnalyzer::identifier()
{
	if (token->m_type == ETokenType::ident)
	{
		getNextToken();
	}
	else
	{
		throw exception();
	}
}

//логическое выражение
//void CSyntaxAnalyzer::booleanExpression()
//{
//	//todo
//}

//простое логическое выражение
//void CSyntaxAnalyzer::simpleBooleanExpression()
//{
//	logicalTerm();
//	while (token->m_type == ETokenType::operand && token->m_op == EOperand::orsy)
//	{
//		accept(EOperand::orsy);
//		logicalTerm();
//	}
//}

//логическое слагаемое
//void CSyntaxAnalyzer::logicalTerm()
//{
//	logicalFactor();
//	while (token->m_type == ETokenType::operand && token->m_op == EOperand::andsy)
//	{
//		accept(EOperand::andsy);
//		logicalFactor();
//	}
//}

//логический множитель
//void CSyntaxAnalyzer::logicalFactor()
//{
//	if (token->m_type == ETokenType::ident)
//	{
//		//todo
//	}
//	else if (token->m_type == ETokenType::operand)
//	{
//		if (token->m_op == EOperand::notsy)
//		{
//			logicalFactor();
//		}
//		else if (token->m_op == EOperand::leftpar)
//		{
//			accept(EOperand::leftpar);
//			booleanExpression();
//			accept(EOperand::rightpar);
//		}
//		else
//		{
//			throw exception();
//		}
//	}
//	else
//	{
//		throw exception();
//	}
//}

SIdentifierTableItem* CSyntaxAnalyzer::findIdent(string name)
{
	for (auto i = scopes.rbegin(); i != scopes.rend(); ++i)
	{
		if (i->count(name) == 1)
		{
			return &(i->at(name));
		}
	}

	return nullptr;
}

//создание фиктивной области видимости
void CSyntaxAnalyzer::createFictScope()
{
	//фиктивная область видимости
	map<string, SIdentifierTableItem> fictScope;

	intType = new CScalarType(EScalarTypes::tInteger);
	realType = new CScalarType(EScalarTypes::tReal);
	charType = new CScalarType(EScalarTypes::tChar);
	stringType = new CScalarType(EScalarTypes::tString);
	boolType = new CEnumType();
	boolType->const_names.push_back("true");
	boolType->const_names.push_back("false");

	fictScope["integer"] = SIdentifierTableItem{ EIdentUseMode::types, intType };
	fictScope["real"] = SIdentifierTableItem{ EIdentUseMode::types, realType };
	fictScope["char"] = SIdentifierTableItem{ EIdentUseMode::types, charType };
	fictScope["string"] = SIdentifierTableItem{ EIdentUseMode::types, stringType };
	fictScope["boolean"] = SIdentifierTableItem{ EIdentUseMode::types, boolType };

	fictScope["true"] = SIdentifierTableItem{ EIdentUseMode::consts, boolType };
	fictScope["false"] = SIdentifierTableItem{ EIdentUseMode::consts, boolType };
	fictScope["maxint"] = SIdentifierTableItem{ EIdentUseMode::consts, intType };

	scopes.push_back(fictScope);
}


int main()
{
	setlocale(LC_ALL, "Russian");

	string file_name = "C:\\Users\\м видео\\Desktop\\Compilator\\Debug\\programm4.txt";
	CLexer* lexer = new CLexer(file_name);
	CSyntaxAnalyzer* syntax_semantic = new CSyntaxAnalyzer(lexer);
	syntax_semantic->Analyze();
	delete lexer;
	delete syntax_semantic;

	return 0;
}