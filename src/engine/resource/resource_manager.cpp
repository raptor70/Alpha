#include "resource_manager.h"

#include "file/base_file.h"

SINGLETON_Define(ResourceManager);

ResourceManager::ResourceManager()
{
	m_iCurrentCheckResource = 0;
}

//------------------------------------------

ResourceManager::~ResourceManager()
{
}

//------------------------------------------

ResourceRef	ResourceManager::LoadResource(const Str& _path ,const Name& _resourceName, ResourceRef _tmpResourceForCache)
{
	// get modification date
	OldFile file(_path);
	U32 modifDate = file.GetModificationDate();

	// try to find it
	U32 resIdx = GetResourceIdx(_resourceName);
	if(resIdx != -1)
	{
		if(m_daResources[resIdx]->GetLoadedModificationDate() == modifDate)
		{
			// already loaded, reuse it
			return m_daResources[resIdx];
		}

		// resource found but need to be reloaded. Remove old one
		m_daResources.RemoveItems(resIdx,1);
	}

#ifdef USE_CREATE_DATA_CACHE
	// try load from cache
	Bool loadFromCache = _tmpResourceForCache->LoadCache(_path);
	if( loadFromCache )
	{
		if( _tmpResourceForCache->GetLoadedModificationDate() == modifDate)
		{
			m_daResources.AddLastItem(_tmpResourceForCache);
			return _tmpResourceForCache;
		}
	}
#endif

	// create & load new one
	LoaderRef loader = LoaderFactory::GetInstance().GetNewLoader(_path.GetExtension());
	if( loader.IsValid() )
	{
		ResourceRefArray resources;
		loader->LoadToResources(_path, resources);
		S32 searchResource = -1;
		for(U32 i=0; i<resources.GetCount(); i++)
		{
			resources[i]->SetCurrentModificationDate(modifDate);
			resources[i]->SetLoadedModificationDate(modifDate);
#ifdef USE_CREATE_DATA_CACHE
			resources[i]->WriteCache();
#endif

			m_daResources.AddLastItem(resources[i]);
			if(resources[i]->GetName() == _resourceName )
				searchResource = i;
		}

		if( searchResource >= 0 )
		{
			return resources[searchResource];
		}
	}

	// error to load resource
	LOGGER_LogError("Unable to load %s\n", _resourceName.GetStr().GetArray());
	return ResourceRef();
}

//------------------------------------------

U32 ResourceManager::GetResourceIdx(const Name& _name) const
{
	for (U32 i = 0; i < m_daResources.GetCount(); i++)
	{
		if (m_daResources[i].IsValid() && m_daResources[i]->GetName() == _name)
		{
			return i;
		}
	}

	return -1;
}

//------------------------------------------

void	ResourceManager::CleanResources()
{
	Bool bResourceRemoved = FALSE;
	do
	{
		bResourceRemoved = FALSE;
		for (S32 i = m_daResources.GetCount() - 1; i >= 0; i--)
		{
			if (m_daResources[i].GetRefCount() == 1 && !m_daResources[i]->Locked())
			{
				m_daResources.RemoveItems(i, 1);
				bResourceRemoved = TRUE;
			}
		}
	} while (bResourceRemoved);
}

//------------------------------------------

void	ResourceManager::OnPause()
{
	for(S32 i=m_daResources.GetCount()-1; i>=0; i--)
	{
		m_daResources[i]->OnPause();
	}
}
	
//------------------------------------------

void	ResourceManager::OnResume()
{
	for(S32 i=m_daResources.GetCount()-1; i>=0; i--)
	{
		m_daResources[i]->OnResume();
	}
}

//------------------------------------------

void	ResourceManager::Update(Float _dTime)
{
	CheckResourceValidity(_dTime);
}

//------------------------------------------

void	ResourceManager::CheckResourceValidity(Float _dTime)
{
	U32 maxCount = Min<S32>(5,m_daResources.GetCount());
	if (maxCount == 0)
		return;

	for(U32 curCount = 0; curCount<maxCount; curCount++)
	{
		m_iCurrentCheckResource++;
		if (m_iCurrentCheckResource >= m_daResources.GetCount())
		{
			m_iCurrentCheckResource = 0;
		}

		ResourceRef& curRes = m_daResources[m_iCurrentCheckResource];
		const Str& path = curRes->GetPath();
		if (!path.IsEmpty())
		{
			OldFile file(path);
			curRes->SetCurrentModificationDate(file.GetModificationDate());
		}
	}
}