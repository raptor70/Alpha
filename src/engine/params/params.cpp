#include "params.h"

#define SCRIPT_ADDPARAM(_type) SCRIPT_VOID_METHOD1(Set##_type,_type)

BEGIN_SCRIPT_CLASS(Params)
	SCRIPT_ADDPARAM(Bool)
	SCRIPT_ADDPARAM(Float)
	SCRIPT_ADDPARAM(S32)
	SCRIPT_ADDPARAM(Vec2f)
	SCRIPT_ADDPARAM(Vec3f)
	SCRIPT_ADDPARAM(Vec4f)
	SCRIPT_ADDPARAM(Str)
	SCRIPT_ADDPARAM(Color)
END_SCRIPT_CLASS

//------------------------------

Params::Params()
{
	m_vValue= Vec4f::Zero;
	m_sValue= Str::Void;
	m_Name = Name::Null;
	m_Type = TYPE_Null;
}

//------------------------------

Params::Params(const Name& _name) :Params()
{
	m_Name = _name;
}

//------------------------------

#define BEGIN_ADDPARAM(_type) \
void	Params::Set##_type(const _type& _value) \
{ \
	m_Type = TYPE_##_type;

#define END_ADDPARAM }

//------------------------------

#define BEGIN_GETPARAM(_type) \
const _type&	Params::Get##_type() const\
{ \
	DEBUG_Require(m_Type==TYPE_##_type);

#define BEGIN_GETPARAM_COPY(_type) \
_type	Params::Get##_type() const\
{

#define END_GETPARAM \
}


//------------------------------

BEGIN_ADDPARAM(Bool)
	m_vValue.x = _value?1.f:0.f;
END_ADDPARAM
BEGIN_ADDPARAM(Float)
	m_vValue.x = _value;
END_ADDPARAM
BEGIN_ADDPARAM(S32)
	m_vValue.x = (Float)_value;
END_ADDPARAM
BEGIN_ADDPARAM(Vec2f)
	m_vValue.x = _value.x;
	m_vValue.y = _value.y;
END_ADDPARAM
BEGIN_ADDPARAM(Vec3f)
	m_vValue.x = _value.x;
	m_vValue.y = _value.y;
	m_vValue.z = _value.z;
END_ADDPARAM
BEGIN_ADDPARAM(Vec4f)
	m_vValue.x = _value.x;
	m_vValue.y = _value.y;
	m_vValue.z = _value.z;
	m_vValue.w = _value.w;
END_ADDPARAM
BEGIN_ADDPARAM(Str)
	m_sValue = _value;
END_ADDPARAM
BEGIN_ADDPARAM(Color)
	m_vValue.x = _value.r;
	m_vValue.y = _value.g;
	m_vValue.z = _value.b;
	m_vValue.w = _value.a;
END_ADDPARAM

//------------------------------

BEGIN_GETPARAM_COPY(Bool)
	return (m_vValue.x==0.f)?FALSE:TRUE;
END_GETPARAM
BEGIN_GETPARAM_COPY(S32)
	return (S32)ROUND(m_vValue.x);
END_GETPARAM
BEGIN_GETPARAM(Float)
	return m_vValue.x;
END_GETPARAM
BEGIN_GETPARAM(Vec2f)
	return m_vValue.xy;
END_GETPARAM
BEGIN_GETPARAM(Vec3f)
	return m_vValue.xyz;
END_GETPARAM
BEGIN_GETPARAM(Vec4f)
	return m_vValue;
END_GETPARAM
BEGIN_GETPARAM(Str)
	return m_sValue;
END_GETPARAM
BEGIN_GETPARAM_COPY(Color)
	return Color(m_vValue.x, m_vValue.y, m_vValue.z, m_vValue.w);
END_GETPARAM

//------------------------------

void	Params::FromStr(Type _type, const Str& _value)
{
	DEBUG_Require(m_Type == TYPE_Null || m_Type == _type);
	switch (_type)
	{
	case TYPE_Bool:		SetBool(_value == "TRUE" ? TRUE : FALSE); break;
	case TYPE_S32:		SetS32(_value.GetInteger()); break;
	case TYPE_Float:	SetFloat(_value.ToFloat()); break;
	case TYPE_Vec2f:
	{
		StrArray values;
		_value.Split(";", values);
		SetVec2f(Vec2f(values[0].ToFloat(), values[1].ToFloat()));
		break;
	}
	case TYPE_Vec3f:
	{
		StrArray values;
		_value.Split(";", values);
		SetVec3f(Vec3f(values[0].ToFloat(), values[1].ToFloat(), values[2].ToFloat()));
		break;
	}
	case TYPE_Vec4f:
	{
		StrArray values;
		_value.Split(";", values);
		SetVec4f(Vec4f(values[0].ToFloat(), values[1].ToFloat(), values[2].ToFloat(), values[3].ToFloat()));
		break;
	}
	case TYPE_Str: SetStr(_value); break;
	case TYPE_Color:
	{
		StrArray values;
		_value.Split(";", values);
		if(values.GetCount()==4)
			SetColor(Color(values[0].ToU32(), values[1].ToU32(), values[2].ToU32(), (S32)values[3].ToU32()));
		else
			SetColor(Color(values[0].ToU32(), values[1].ToU32(), values[2].ToU32()));
		break;
	}
	}
}

//------------------------------

Str	Params::ToStr() const
{
	Str result;
	switch (m_Type)
	{
	case TYPE_Bool:		return GetBool() ? "TRUE" : "FALSE";
	case TYPE_S32:		return result.SetFrom("%d", GetS32());
	case TYPE_Float:	return result.SetFrom("%f", GetFloat());
	case TYPE_Vec2f:	
	{
		const Vec2f& value = GetVec2f();
		return result.SetFrom("%f;%f", value.x, value.y);
	}
	case TYPE_Vec3f:
	{
		const Vec3f& value = GetVec3f();
		return result.SetFrom("%f;%f;%f", value.x, value.y, value.z);
	}
	case TYPE_Vec4f:
	{
		const Vec4f& value = GetVec4f();
		return result.SetFrom("%f;%f;%f;%f", value.x, value.y, value.z, value.w);
	}
	case TYPE_Str:
	{
		return GetStr();
	}
	case TYPE_Color:
	{
		const Color& value = GetColor();
		return result.SetFrom("%d;%d;%d;%d", 
			ROUNDINT(value.r * 255.f), 
			ROUNDINT(value.g * 255.f), 
			ROUNDINT(value.b * 255.f), 
			ROUNDINT(value.a * 255.f));
	}
	}

	return "null";
}