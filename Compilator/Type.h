#pragma once
#include <list>
#include <string>

using namespace std;

enum ETypes
{
	scalars,
	limiteds,
	enums,
	arrays,
	references,
	sets,
	files,
	records
};

enum EScalarTypes
{
	tInteger,
	tReal,
	tChar,
	tString
};

class CBaseType
{
public:
	ETypes m_type;
	CBaseType(ETypes type);
};

class CScalarType : public CBaseType
{
public:
	EScalarTypes scalar_type;
	CScalarType(EScalarTypes type);
};

class CEnumType : public CBaseType
{
public:
	list<string> const_names;
	CEnumType();
};