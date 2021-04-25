#pragma once
#include<string>

using namespace std;

enum EVarType
{
	vInt,
	vReal,
	vChar,
	vString
};

class CVariant
{
public:
	EVarType m_t;
	virtual string ToString() = 0;

protected:
	CVariant(EVarType type);
};

class CIntVariant : public CVariant
{
public:
	int m_val;
	CIntVariant(int val);
	virtual string ToString() override;
};

class CRealVariant : public CVariant
{
public:
	float m_val;
	CRealVariant(float val);
	virtual string ToString() override;
};

class CCharVariant : public CVariant
{
public:
	char m_val;
	CCharVariant(char val);
	virtual string ToString() override;
};

class CStringVariant : public CVariant
{
public:
	string m_val;
	CStringVariant(string val);
	virtual string ToString() override;
};