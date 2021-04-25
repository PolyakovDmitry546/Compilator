#pragma once
#include<string>

#include"Variant.h"

using namespace std;

enum ETokenType
{
	ident,
	operand,
	value
};

enum EOperand
{
	star,
	slash,
	equal,
	comma,
	semicolon,
	colon,
	point,
	arrow,
	leftpar,
	rightpar,
	lbracket,
	rbracket,
	flpar,
	frpar,
	later,
	greater,
	laterequal,
	greaterequal,
	latergreater,
	plus,
	minus,
	lcomment,
	rcomment,
	assign,
	twopoints,
	identsy,
	floatc,
	intc,
	charc,
	casesy,
	elsesy,
	filesy,
	gotosy,
	thensy,
	typesy,
	untilsy,
	dosy,
	withsy,
	ifsy,
	ofsy,
	orsy,
	insy,
	tosy,
	endsy,
	varsy,
	divsy,
	andsy,
	notsy,
	forsy,
	modsy,
	nilsy,
	setsy,
	beginsy,
	whilesy,
	arraysy,
	constsy,
	labelsy,
	downtosy,
	packedsy,
	recordsy,
	repeatsy,
	programsy,
	functionsy,
	proceduresy,
	apostrophe
};

class CToken
{
public:
	ETokenType m_type;
	union
	{
		EOperand m_op;
		CVariant* m_val;
	};
	string m_ident;
	CToken(EOperand oper, string ident);
	CToken(CVariant* val);
	CToken(string ident);
	~CToken();

	string ToString() const;
};