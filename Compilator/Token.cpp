#include "Token.h"


CToken::CToken(EOperand oper, string ident)
{
	m_op = oper;
	m_ident = ident;
	m_type = ETokenType::operand;
}

CToken::CToken(CVariant* val)
{
	m_val = val;
	m_type = ETokenType::value;
}

CToken::CToken(string ident)
{
	m_ident = ident;
	m_type = ETokenType::ident;
}

CToken::~CToken()
{
	if (m_type == ETokenType::value)
	{
		delete m_val;
	}
}

string CToken::ToString() const
{
	if (m_type == ETokenType::ident)
	{
		return m_ident;
	}
	else if (m_type == ETokenType::value)
	{
		return m_val->ToString();
	}
	else
	{
		return m_ident;
	}
}