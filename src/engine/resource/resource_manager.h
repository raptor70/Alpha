#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include	"resource.h"
#include	"loader.h"

//------------------------------------------

class ResourceManager
{
	SINGLETON_Declare(ResourceManager);

public:
	ResourceManager();
	~ResourceManager();

	void	CleanResources();
	void	OnPause();
	void	OnResume();

	void	Update(Float _dTime);

	//----------------

	template<class T>
	ResourceRef	GetResourceFromFile(const Str& _path)
	{
		Name name = ComputeResourceName<T>(_path);
		ResourceRef resource = LoadResource(_path, name, CreateNewResource<T>(_path));
		if (resource.IsValid())
		{
			resource->SetPath(_path);
			resource->PostLoad();
		}
		return resource;
	}

	//----------------
	
	template<class T>
	ResourceRef	GetVirtualResource(const Char* _pName, Bool _bOnlyCreate = FALSE)
	{
		U32 idx = GetResourceIdx(ComputeResourceName<T>(_pName));		
		if (!_bOnlyCreate  && idx != -1)
			return m_daResources[idx];
		
		DEBUG_RequireMessage(idx == -1, "Resource already created");
		return m_daResources.AddLastItem(CreateNewResource<T>(_pName));
	}

	//----------------

	template<class T>
	ResourceRef CreateNewResource(const Str& _name)
	{
		ResourceRef newRef = NEW T;
		newRef->SetName(ComputeResourceName<T>(_name));
		newRef->SetBaseName(_name);
		newRef->Create();
		return newRef;
	}

	//----------------

	template<class T>
	Name ComputeResourceName(const Str& _path)
	{
		Str resourceStrName = T::GetResourcePrefix();
		resourceStrName += "_";

		S32 idx = _path.GetWordLastIndex("bin");
		Str pathToAdd;
		if (idx >= 0)
		{
			Str relativePath = _path.GetTextInsideRange(idx + 4, _path.GetLength() - 1);
			pathToAdd = relativePath;
		}
		else
			pathToAdd = _path;

		pathToAdd.Replace('/', '_');
		pathToAdd.Replace('\\', '_');
		pathToAdd.Replace('.', '_');
		resourceStrName += pathToAdd;

		return resourceStrName;
	}

	//----------------

protected:
	ResourceRef	LoadResource(const Str& _path, const Name& _resourceName, ResourceRef _tmpResourceForCache);
	void	CheckResourceValidity(Float _dTime);
	U32		GetResourceIdx(const Name& _name) const;

	//----------------

public:

	template<class T>
	void Dump()
	{
		Str prefix = T::GetResourcePrefix();
		LOGGER_Log("--------------------------\n");
		LOGGER_Log(" DUMP %s\n",prefix.GetArray());
		LOGGER_Log("--------------------------\n");
		for(U32 i=0; i<m_daResources.GetCount(); i++)
		{
			Name name = ComputeResourceName<T>(m_daResources[i]->GetPath());
			if( m_daResources[i]->GetName() == name )
			{
				m_daResources[i]->Dump();
			}
		}
		LOGGER_Log("--------------------------\n");
	}

protected:
	ResourceRefArray		m_daResources;
	U32				m_iCurrentCheckResource;
};

#endif