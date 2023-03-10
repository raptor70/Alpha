#include "loader_DB.h"

#include "database/database.h"

DEFINE_LOADER_CLASS(DB);

Loader_DB::Loader_DB()
{
}

//-----------------------------------------------

Loader_DB::~Loader_DB()
{
}

//-----------------------------------------------

Bool Loader_DB::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
	// create resource
	ResourceRef ref = ResourceManager::GetInstance().CreateNewResource<Database>(_path);
	if( !((Database*)ref.GetPtr())->Open() )
	{
		LOGGER_LogError("Unable to open %s !!\n",_path.GetArray());
		return FALSE;
	}

	_outResources.AddLastItem(ref);

	return TRUE;
}
