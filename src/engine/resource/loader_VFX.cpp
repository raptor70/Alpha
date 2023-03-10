#include "loader_VFX.h"

#include "file/base_file.h"
#include "resource_manager.h"

DEFINE_LOADER_CLASS(VFX);
DEFINE_LOADER_CLASS(PFX);
DEFINE_LOADER_CLASS(HFX);

Loader_VFX::Loader_VFX()
{
}

//-----------------------------------------------

Loader_VFX::~Loader_VFX()
{
}

//-----------------------------------------------

Bool Loader_VFX::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
	OldFile file(_path,OldFile::USE_TYPE_ReadBuffer);
	file.Open();
	if( !file.IsOpen() )
	{
		LOGGER_LogError("File %s not found !!\n",_path.GetArray());
		return FALSE;
	}

	// load text
	Str content;
	file.ReadCompleteTextFile(content);
	file.Close();

	// create resource
	ResourceRef ref = ResourceManager::GetInstance().CreateNewResource<ShaderSource>(_path);
	ref.GetCastPtr<ShaderSource>()->Init(GetShaderType(),content);
	_outResources.AddLastItem(ref);

	return TRUE;
}
