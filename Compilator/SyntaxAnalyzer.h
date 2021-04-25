#pragma once

#include "Lexer.h"
#include "Type.h"

#include <list>

//способ передачи параметра
//enum EPassingMethod
//{
//	link,
//	value
//};

//способ использования идентификатора
enum EIdentUseMode
{
	progs,
	types,
	consts,
	vars,
	procs,
	funcs
};

//параметр функции или процедуры
//struct SIdentifierParametr
//{
//	SIdentifierTableItem ident;
//	EPassingMethod method;
//
//};

//элемент таблицы идентификаторов
struct SIdentifierTableItem
{
	EIdentUseMode mode;
	CBaseType* type;
	//list<SIdentifierParametr> parameters;
};



class CSyntaxAnalyzer
{
public:
	CSyntaxAnalyzer(CLexer* lexer);
	~CSyntaxAnalyzer();
	void Analyze();

private:
	CToken* token;
	CLexer* lexer;
	STextPosition position;
	CErrorManager* errorManager;

	CScalarType* intType;
	CScalarType* realType;
	CScalarType* charType;
	CScalarType* stringType;
	CEnumType* boolType;
	//области видимости
	list<map<string, SIdentifierTableItem>> scopes;

	void skipTo(set<EOperand> symbols);
	void createFictScope();
	SIdentifierTableItem* findIdent(string token);

	void getNextToken();
	void accept(EOperand operand);

	void programme();
	void block();

	void operatorsSection();
	void compoundOperator();
	void operatorF();
	void unmarkedOperator();
	void simpleOperator();
	void complexOperator();
	void assignmentOperator();
	void conditionalOperator();
	void preconditionedLoopOperator();

	void constantSection();
	void constantDeclaration();
	void constant(string newConst);
	CBaseType* unsignedConstant();
	void sign();
	void constantName();

	void typeSection();
	void typeDeclaration();
	void type();
	void simpleType();
	void typeName();

	void variableSection();
	void addVars(list<string> &newVariables);
	void variableDeclaration();

	CBaseType* expression();
	CBaseType* simpleExpression();
	CBaseType* term();
	CBaseType* factor();
	CBaseType* unsignedNumber();
	void unsignedReal();
	void unsignedInteger();

	void variable();
	void variableName();
	void name();
	void identifier();
};

int main();
