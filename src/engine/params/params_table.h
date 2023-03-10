#ifndef __PARAMS_TABLE_H__
#define __PARAMS_TABLE_H__

#include "params.h"

class ParamsTable
{
	DECLARE_SCRIPT_CLASS(ParamsTable)
public:
	ParamsTable()
	{
		m_bChanged = FALSE;
	}

	ParamsTable& operator=(const ParamsTable& _other);

	Bool		ExistsParam(const Name& _name) const { return GetValue(_name) != NULL; }

	void	LoadFromFile(std::vector< uint8_t >* _data = NULL);
	void	SaveToFile();
	void	Reset() { m_Values.RemoveAllItems(); m_ValuesPtr.Flush();  }
	
	Params*		GetModifiableValue(const Name& _name);
	const Params*	GetValue(const Name& _name) const;
	const Params*	GetValue(U32 _idx) const { return (m_ValuesPtr[_idx]); }
	Params*		GetOrAddValue(const Name& _name);
	U32				GetNbValues() const { return m_ValuesPtr.GetCount(); }

	Bool			HasChanged() const { return m_bChanged;  }
	void			ResetChanged() { m_bChanged = FALSE; }

protected:
	typedef HashOf<Name, Params> ParamHash;
	ParamHash				m_Values;
	ArrayOf<Params*>		m_ValuesPtr; // used for a linear access to all values
	
	Bool					m_bChanged;
};


#endif