#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "resource/resource.h"
#include "resource/resource_manager.h"

#define DEFAULT_TEXTURE_PATH "debug/debug.png"

BEGIN_SUPER_CLASS(Texture,Resource)
public:
	Texture(Bool _bRegister = TRUE);
	virtual ~Texture();

	enum	EWrapping
	{
		TEXTUREWRAP_Repeat,
		TEXTUREWRAP_Clamp
	};

	enum	EType
	{
		TEXTURETYPE_2D,
		TEXTURETYPE_Cubemap
	};

	enum	EFormat
	{
		TEXTUREFORMAT_DepthStencil,
		TEXTUREFORMAT_R8,
		TEXTUREFORMAT_RG8,
		TEXTUREFORMAT_RGB8,
		TEXTUREFORMAT_RGBA8,
		TEXTUREFORMAT_R16F,
		TEXTUREFORMAT_RG16F,
		TEXTUREFORMAT_RGB16F,
		TEXTUREFORMAT_RGBA16F,
		TEXTUREFORMAT_R32F,
		TEXTUREFORMAT_RGB32F,
		TEXTUREFORMAT_RGBA32F,
	};

	struct MipMap
	{
		U32		m_iSizeX;		// downscaled if necessary
		U32		m_iSizeY;		// downscaled if necessary
		U32		m_iRealSizeX;
		U32		m_iRealSizeY;
		Float	m_fUVMaxX;
		Float	m_fUVMaxY;
		U8* m_Data;				// first byte : top-left

		MipMap()
		{
			m_iSizeX = m_iSizeY = m_iRealSizeX = m_iRealSizeY = 0;
			m_fUVMaxX = m_fUVMaxY = 1.f;
			m_Data = NULL;
		}
	};

	static  const Char* GetResourcePrefix() { return "texture"; }

	void	Init(U32 _sizeX, U32 _sizeY, EFormat _format, const U8* _dataToCopy = NULL);
	void	AddLevel(U32 _sizeX, U32 _sizeY, const U8* _dataToCopy);
	
	U32			GetDataSize(S32 _level = 0) const;
	U32			GetSizeX(S32 _level = 0) const			{ return m_MipMaps[_level].m_iSizeX; }
	U32			GetSizeY(S32 _level = 0) const			{ return m_MipMaps[_level].m_iSizeY; }
	U32			GetRealSizeX(S32 _level = 0) const		{ return m_MipMaps[_level].m_iRealSizeX; }
	U32			GetRealSizeY(S32 _level = 0) const		{ return m_MipMaps[_level].m_iRealSizeY; }
	Float		GetUVMaxX(S32 _level = 0) const			{ return m_MipMaps[_level].m_fUVMaxX; }
	Float		GetUVMaxY(S32 _level = 0) const			{ return m_MipMaps[_level].m_fUVMaxY; }
	const U8*	GetData(S32 _level = 0) const			{ return m_MipMaps[_level].m_Data; }
	S32			GetNbLevels() const						{ return m_MipMaps.GetCount(); }

	EFormat		GetFormat() const						{ return m_Format; }
	EType		GetType() const							{ return m_Type; }
	void		SetType(EType _type)					{ m_Type = _type; }
	Bool		IsTransparent() const					{ return m_bIsTransparent; }

	const EWrapping&	GetWrapping() const							{ return m_Wrapping; }
	void				SetWrapping(const EWrapping& _wrap)			{ m_Wrapping = _wrap; }

	void		ReleaseData(S32 _level = 0);

	virtual void	WriteCache() OVERRIDE;
	virtual Bool	LoadCache(const Str& _path) OVERRIDE;

	virtual void	OnPause() OVERRIDE;
	virtual void	OnResume() OVERRIDE;
	virtual void	Dump() OVERRIDE
	{
		/*if( m_MipMaps.GetCount() )
		{
			LOGGER_Log("%s (%dx%d) %f Mo\n",
				m_Name.GetStr().GetArray(),
				m_MipMaps[0].m_iSizeX,
				m_MipMaps[0].m_iSizeY,
				Float(m_MipMaps[0].m_iSizeX * m_MipMaps[0].m_iSizeY * m_MipMaps[0].m_iBytePerPixel)/ (1024.f * 1024.f));
		}*/
	}

protected:
	U32			ComputePow2(U32 _base);

	ArrayOf<MipMap>		m_MipMaps;
	EType				m_Type;
	EFormat				m_Format;
	EWrapping			m_Wrapping;
	Bool				m_bUsed;
	Bool				m_bIsTransparent;
	Bool				m_bRegister;
END_SUPER_CLASS

typedef RefTo<Texture>		TextureRef;
typedef ArrayOf<TextureRef>	TextureRefArray;
typedef PtrTo<Texture>		TexturePtr;
typedef ArrayOf<TexturePtr>	TexturePtrArray;

#endif