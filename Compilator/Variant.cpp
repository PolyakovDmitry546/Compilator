#include "Variant.h"


CVariant::CVariant(EVarType type)
{
	m_t = type;
}


CIntVariant::CIntVariant(int val) :CVariant(EVarType::vInt)
{
	m_val = val;
}

string CIntVariant::ToString()
{
	return to_string(m_val);
}


CRealVariant::CRealVariant(float val) :CVariant(EVarType::vReal)
{
	m_val = val;
}

string CRealVariant::ToString()
{
	return to_string(m_val);
}


CCharVariant::CCharVariant(char val) : CVariant(EVarType::vChar)
{
	m_val = val;
}

string CCharVariant::ToString()
{
	return to_string(m_val);
}


CStringVariant::CStringVariant(string val) : CVariant(EVarType::vString)
{
	m_val = val;
}

string CStringVariant::ToString()
{
	return m_val;
}