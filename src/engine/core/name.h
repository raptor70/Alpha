#ifndef __NAME_H__
#define __NAME_H__

#include "str.h"

class Name
{
public:
	Name();
	Name(const Char* _in);
	Name(const Str& _in);
	Name(const Name& _other);
	~Name();

	const Name& operator=(const Name& _other);
	Bool operator==(const Name& _other) const;
	Bool operator!=(const Name& _other) const;
	Bool operator<(const Name& _other) const { return m_ID < _other.m_ID; }
	U32 GetID() const {return m_ID; }
	const Str& GetStr() const {return m_Str; }

protected:
	U32 ComputeID(const Char* _in);

	U32	m_ID;
	Str m_Str;

public:
	static Name Null;
};

//------------------------------------------------------

static U32 GetHash(const Name& _key)
{
	return (U32(_key.GetID()));
}

typedef ArrayOf<Name>	NameArray;

#endif