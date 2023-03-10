#include "loader_LUA.h"

#include "resource_manager.h"
#include "2d/sprite.h"
#include "file/base_file.h"

DEFINE_LOADER_CLASS(LUA);

Loader_LUA::Loader_LUA()
{
}

//-----------------------------------------------

Loader_LUA::~Loader_LUA()
{
}

//-----------------------------------------------

Bool Loader_LUA::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
	ResourceRef scriptRef = ResourceManager::GetInstance().CreateNewResource<Script>(_path);
	Script* script = scriptRef.GetCastPtr<Script>();
	if( !script->LoadFromFile(_path)) 
	{
		LOGGER_LogError("Unable to load %s !!\n",_path.GetArray());
		return FALSE;
	}

	if( !script->Execute())
	{
		LOGGER_LogError("Unable to execute %s !!\n",_path.GetArray());
		return FALSE;
	}

	_outResources.AddLastItem(scriptRef);

	if(script->HasFunction("LoadSprite") )
	{
		ResourceRef ref = ResourceManager::GetInstance().CreateNewResource<Sprite>(_path);
		Sprite* sprite = (Sprite*)ref.GetPtr();

		Str texPath = _path.GetTextInsideRange(0,_path.GetLength()-4);
		texPath += "png";
		sprite->GetMaterial()->LoadTexture(texPath);

		if( !script->Call("LoadSprite",*sprite) )
		{
			LOGGER_LogError("Unable to load sprite in script %s !! \n",_path.GetArray());
			return FALSE;
		}

		// load sound events
		Str sndPath = _path.GetTextInsideRange(0,_path.GetLength()-5);
		sndPath += "_snd.lua";
		OldFile file(sndPath);
		if( file.Exists() )
		{
			Script sndscript;
			if( !sndscript.LoadFromFile(sndPath) )
			{
				LOGGER_LogError("Unable to load %s !!\n",sndPath.GetArray());
				return FALSE;
			}

			if( !sndscript.Execute()) 
			{
				LOGGER_LogError("Unable to execute %s !!\n",sndPath.GetArray());
				return FALSE;
			}

			if( !sndscript.Call("LoadSpriteSound",*sprite) )
			{
				LOGGER_LogError("Unable to load sound event to sprite in script %s !! \n",sndPath.GetArray());
				return FALSE;
			}
		}

		_outResources.AddLastItem(ref);
	}

	return TRUE;
}
