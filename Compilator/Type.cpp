#include "Type.h"

CBaseType::CBaseType(ETypes type)
{
	m_type = type;
}

CScalarType::CScalarType(EScalarTypes type) : CBaseType(ETypes::scalars)
{
	scalar_type = type;
}

CEnumType::CEnumType() : CBaseType(ETypes::enums)
{

}