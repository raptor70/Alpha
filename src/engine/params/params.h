#ifndef __PARAMS_H__
#define __PARAMS_H__

class Params
{
	DECLARE_SCRIPT_CLASS(Params)
public:
	Params();
	Params(const Name& _name);



	enum Type
	{
		TYPE_None = -1,
		TYPE_Null = 0,
#define PARAM_TYPE(_type)	TYPE_##_type,
#include "params_type.h"
#undef PARAM_TYPE

		TYPE_COUNT
	};

	static const Char* TypeToChar(Type _type)
	{
		switch (_type)
		{
#define PARAM_TYPE(_type)	case TYPE_##_type : return #_type;
#include "params_type.h"
#undef PARAM_TYPE
		}

		return "";
	}

	static Type CharToType(const Char* _text)
	{
#define PARAM_TYPE(_type)	if(strcmp(_text,#_type) == 0) return TYPE_##_type;
#include "params_type.h"
#undef PARAM_TYPE

		return TYPE_None;
	}

	Params& operator=(const Params& _other)
	{
		m_vValue = _other.m_vValue;
		m_sValue = _other.m_sValue;
		m_Name = _other.m_Name;
		m_Type = _other.m_Type;

		return *this;
	}

#define DECLARE_PARAM_COPY(_type)	 \
	void			Set##_type(const _type& _value); \
	_type			Get##_type() const;

#define DECLARE_PARAM(_type)	 \
	void			Set##_type(const _type& _value); \
	const _type&	Get##_type() const;

	DECLARE_PARAM_COPY(Bool)
	DECLARE_PARAM(Float)
	DECLARE_PARAM_COPY(S32)
	DECLARE_PARAM(Vec2f)
	DECLARE_PARAM(Vec3f)
	DECLARE_PARAM(Vec4f)
	DECLARE_PARAM(Str)
	DECLARE_PARAM_COPY(Color)

	void		SetNull()		{ m_Type = TYPE_Null; }
	Bool		IsNull() const	{ return (m_Type == TYPE_Null); }

	void		FromStr(Type _type, const Str& _value);
	Str			ToStr() const;

	Type		GetType() const { return m_Type;  }

	void		SetName( const Name& _name)		{ m_Name = _name; }
	const Name& GetName() const					{ return m_Name; }

protected:
	Vec4f	m_vValue;
	Str		m_sValue;
	Name	m_Name;
	Type	m_Type;
};


#endif