#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include "resource/resource.h"
#include "resource/resource_manager.h"

class TextureManager
{
	SINGLETON_Declare(TextureManager);
	DECLARE_SCRIPT_CLASS(TextureManager);
public:
	TextureManager(){}
	~TextureManager(){}

	void RegisterAtlas(const Str& _path);
	const Str& GetRealTexturePath(const Str& _path) const;
	void GetTexture(const Str& _path, ResourceRef& _outTexture, Vec2f& _outUvMin, Vec2f& _outUVMax);
	void AddTextureInAtlas(const Str& _path, const Str& _atlas, Float _uvminx, Float _uvminy, Float _uvsizex, Float _uvsizey);
protected:
	struct STextureRegister
	{
		Name	m_TexturePath;
		Str		m_AtlasTexturePath;
		Vec2f	m_vUVMin;
		Vec2f	m_vUVMax;
	};

	ArrayOf<STextureRegister>	m_TexturesRegister;
};

#endif