#include "texture_manager.h"

#include "file/base_file.h"

SINGLETON_Define(TextureManager);
BEGIN_SCRIPT_CLASS(TextureManager)
	SCRIPT_STATIC_RESULT_METHOD(GetInstance,TextureManager)
	SCRIPT_VOID_METHOD6(AddTextureInAtlas,Str,Str,Float,Float,Float,Float)
	SCRIPT_VOID_METHOD1(RegisterAtlas,Str)
END_SCRIPT_CLASS

//----------------------------------

void TextureManager::RegisterAtlas(const Str& _path)
{
	Str texturePath = _path;
	texturePath += ".png";
	Str scriptPath = _path;
	scriptPath += ".lua";
	
	Script script;
	if( !script.LoadFromFile(scriptPath)) 
	{
		LOGGER_LogError("Unable to load %s !!\n",scriptPath.GetArray());
		return;
	}

	if( !script.Execute()) 
	{
		LOGGER_LogError("Unable to execute %s !!\n",scriptPath.GetArray());
		return;
	}

	if( !script.Call("LoadAtlas",*this,texturePath) )
	{
		LOGGER_LogError("Unable to load atlas in script %s !! \n",scriptPath.GetArray());
		return;
	}
}

//----------------------------------

const Str& TextureManager::GetRealTexturePath(const Str& _path) const
{
	Name namePath = _path;
	for (U32 i = 0; i < m_TexturesRegister.GetCount(); i++)
	{
		const STextureRegister& reg = m_TexturesRegister[i];
		if (reg.m_TexturePath == namePath)
		{
			return reg.m_AtlasTexturePath;
		}
	}

	return _path;
}

//----------------------------------

void TextureManager::GetTexture(const Str& _path, ResourceRef& _outTexture, Vec2f& _outUvMin, Vec2f& _outUvMax)
{
	Name namePath = _path;
	for(U32 i=0; i<m_TexturesRegister.GetCount(); i++)
	{
		STextureRegister& reg = m_TexturesRegister[i];
		if( reg.m_TexturePath == namePath )
		{
			_outTexture = ResourceManager::GetInstance().GetResourceFromFile<Texture>(reg.m_AtlasTexturePath);
			_outUvMin = reg.m_vUVMin;
			_outUvMax = reg.m_vUVMax;
			if( _outTexture.IsValid() )
			{
				Texture* pTexture = (Texture*)(_outTexture.GetPtr());
				_outUvMin.x *= pTexture->GetUVMaxX();
				_outUvMin.y *= pTexture->GetUVMaxY();
				_outUvMax.x *= pTexture->GetUVMaxX();
				_outUvMax.y *= pTexture->GetUVMaxY();
			}
			return;
		}
	}

	_outTexture = ResourceManager::GetInstance().GetResourceFromFile<Texture>(_path);
	_outUvMin = Vec2f::Zero;
	_outUvMax = Vec2f::One;
	if( _outTexture.IsValid() )
	{
		Texture* pTexture = (Texture*)(_outTexture.GetPtr());
		_outUvMax.x = pTexture->GetUVMaxX();
		_outUvMax.y = pTexture->GetUVMaxY();
	}

	// try load mipmap
	if( _outTexture.IsValid() )
	{
		Texture* pOutTexture = _outTexture.GetCastPtr<Texture>();
		for(S32 i=0; i<4; i++)
		{
			S32 idx = _path.GetLastIndexOfCharacter('.');
			Str startPath = _path.GetTextInsideRange(0,idx-1);
			Str ext = _path.GetTextInsideRange(idx,_path.GetLength()-1);
			Str newPath;
			newPath.SetFrom("%s_%d%s",startPath.GetArray(),i,ext.GetArray());
			Bool bFound = FALSE;
			OldFile file(newPath);
			if( file.Exists())
			{
				ResourceRef newTexture = ResourceManager::GetInstance().GetResourceFromFile<Texture>(newPath);
				if( newTexture.IsValid() )
				{
					Texture* pTexture = newTexture.GetCastPtr<Texture>();
					pOutTexture->AddLevel(pTexture->GetSizeX(),pTexture->GetSizeY(),pTexture->GetData());
					bFound = TRUE;
				}
			}

			if( !bFound )
				break;
		}
	}
}

//----------------------------------

void TextureManager::AddTextureInAtlas(const Str& _path, const Str& _atlas, Float _uvminx, Float _uvminy, Float _uvsizex, Float _uvsizey)
{
	STextureRegister texture;
	S32 idx = _atlas.GetLastIndexOfCharacter('/');
	if( idx < 0 )
		_atlas.GetLastIndexOfCharacter('\\');
	Str texturePath = _atlas.GetTextInsideRange(0,idx);
	texturePath += _path;
	texture.m_TexturePath = texturePath;
	texture.m_AtlasTexturePath = _atlas;
	texture.m_vUVMin.x = _uvminx;
	texture.m_vUVMax.y = 1.f - _uvminy;
	texture.m_vUVMax.x = _uvminx + _uvsizex;
	texture.m_vUVMin.y = 1.f - _uvminy - _uvsizey;
	m_TexturesRegister.AddLastItem(texture);
}
