#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "renderer/renderer_driver_data.h"

class Resource
{
	RENDERERDRIVER_DATA
public:
	Resource() { m_CurrentModificationDate = m_LoadedModificationDate = 0; m_bLocked = FALSE; }
	virtual ~Resource(){}

	virtual void Create() {}
	
	void SetName(const Name& _name)		{ m_Name = _name; }
	const Name& GetName() const			{ return m_Name; }

	void SetBaseName(const Name& _name) { m_BaseName = _name; }
	const Name& GetBaseName() const { return m_BaseName; }

	void SetPath(const Str& _path)		{ m_Path = _path; }
	const Str& GetPath() const			{ return m_Path; }

	U32		GetCurrentModificationDate() const		{ return m_CurrentModificationDate; }
	void	SetCurrentModificationDate(U32 _date)	{ m_CurrentModificationDate = _date; }

	U32		GetLoadedModificationDate() const { return m_LoadedModificationDate; }
	void	SetLoadedModificationDate(U32 _date) { m_LoadedModificationDate = _date; }

	Bool	IsDirty() const { 
		return m_LoadedModificationDate != m_CurrentModificationDate; 
	}

	virtual void	WriteCache(){}
	virtual Bool	LoadCache(const Str& _path){ return FALSE; }

	virtual void	OnPause()	{}
	virtual void	OnResume()	{}
	virtual void	Dump()		{}
	virtual void	PostLoad()	{}

	void		Lock()			{ m_bLocked = TRUE; }
	Bool		Locked() const	{ return m_bLocked; }
protected:
	Name		m_Name;
	Name		m_BaseName;
	Str			m_Path;
	U32			m_CurrentModificationDate;
	U32			m_LoadedModificationDate;
	Bool		m_bLocked;
};

typedef RefTo<Resource>		ResourceRef;
typedef PtrTo<Resource>		ResourcePtr;
typedef ArrayOf<ResourceRef>	ResourceRefArray;

#endif