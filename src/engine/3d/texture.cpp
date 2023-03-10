#include "texture.h"

#include "file/base_file.h"
#include "renderer/renderer.h"

Texture::Texture(Bool _bRegister /*= TRUE*/):SUPER()
{
	m_bUsed = FALSE;
	m_bIsTransparent = FALSE;
	m_Wrapping = TEXTUREWRAP_Clamp;
	m_Type = TEXTURETYPE_2D;
	m_Format = TEXTUREFORMAT_RGBA8;
	m_bRegister = _bRegister;

	if(m_bRegister)
		RendererDatas::GetInstance().RegisterTexture(this);
}

//----------------------------------

Texture::~Texture()
{
	if (m_bRegister)
		RendererDatas::GetInstance().UnregisterTexture(this);
		
	for(U32 i=0;i<m_MipMaps.GetCount();i++)
		if (m_MipMaps[i].m_Data)
		{
			DELETE[] m_MipMaps[i].m_Data;
			m_MipMaps[i].m_Data = NULL;
		}
}

//----------------------------------

void	Texture::Init(U32 _sizeX, U32 _sizeY, EFormat _format, const U8* _dataToCopy /*=NULL*/)
{
	DEBUG_Require(m_MipMaps.GetCount()==0);
	m_Format = _format;
	AddLevel(_sizeX,_sizeY,_dataToCopy);
}

//----------------------------------

U32 BytePerPixel(Texture::EFormat _format)
{
	U32 bytePerPixel = 0;
	switch (_format)
	{
		case Texture::TEXTUREFORMAT_R8:		bytePerPixel = 1;		break;
		case Texture::TEXTUREFORMAT_RG8:		bytePerPixel = 2;		break;
		case Texture::TEXTUREFORMAT_RGB8:		bytePerPixel = 3;		break;
		case Texture::TEXTUREFORMAT_RGBA8:		bytePerPixel = 4;		break;
		case Texture::TEXTUREFORMAT_R16F:		bytePerPixel = 1 * 2;	break;
		case Texture::TEXTUREFORMAT_RG16F:		bytePerPixel = 2 * 2;	break;
		case Texture::TEXTUREFORMAT_RGB16F:	bytePerPixel = 3 * 2;	break;
		case Texture::TEXTUREFORMAT_RGBA16F:	bytePerPixel = 4 * 2;	break;
		case Texture::TEXTUREFORMAT_R32F:		bytePerPixel = 1 * 4;	break;
		case Texture::TEXTUREFORMAT_RGB32F:	bytePerPixel = 3 * 4;	break;
		case Texture::TEXTUREFORMAT_RGBA32F:	bytePerPixel = 4 * 4;	break;
		case Texture::TEXTUREFORMAT_DepthStencil:	bytePerPixel = 4 * 4;	break;
		default: DEBUG_Forbidden("Unknown format"); break;
	}
	return bytePerPixel;
}

//----------------------------------

void	Texture::AddLevel(U32 _sizeX, U32 _sizeY, const U8* _dataToCopy)
{
	//transparent check
	// 19/02/2017 - Commented due to MagicaVoxel last transparent pixel
	/*if( _dataToCopy && m_MipMaps.GetCount() == 0 && _bytePerPixel == 4 && !m_bIsFloat )
	{
		U32 nbPixels = _sizeX*_sizeY;
		for(U32 i=0; i<nbPixels; i++)
		{
			if( _dataToCopy[i * _bytePerPixel + 3] != 255 )
			{
				m_bIsTransparent = TRUE;
				break;
			}
		}
	}*/

	MipMap& mipmap = m_MipMaps.AddLastItem(MipMap());
	Float downscaleRatio = 1.f;
#ifdef USE_FORCE_POWER_OF_TWO_TEXTURES
	U32 pow2X = ComputePow2(_sizeX);
	U32 pow2Y = ComputePow2(_sizeY);

	U32 maxpow2 = Max(pow2X,pow2Y);
	U32 maxSize = Renderer::GetInstance()->GetMaxTextureSize(); // in driver now
	if( maxpow2 > maxSize )
	{
		downscaleRatio = ((Float)maxSize)/((Float)maxpow2);
	}

	mipmap.m_iSizeX = (U32)ROUND(pow2X * downscaleRatio);
	mipmap.m_iSizeY = (U32)ROUND(pow2Y * downscaleRatio);
	mipmap.m_iRealSizeX = pow2X;
	mipmap.m_iRealSizeY = pow2Y;
	mipmap.m_fUVMaxX = ((Float)_sizeX)/((Float)pow2X);
	mipmap.m_fUVMaxY = ((Float)_sizeY)/((Float)pow2Y);
#else
	mipmap.m_iSizeX = _sizeX;
	mipmap.m_iSizeY = _sizeY;
	mipmap.m_iRealSizeX = _sizeX;
	mipmap.m_iRealSizeY = _sizeY;
	mipmap.m_fUVMaxX = 1.f;
	mipmap.m_fUVMaxY = 1.f;
#endif


	if( !_dataToCopy )
		mipmap.m_Data = NULL; // driver will create an empty texture
	else
	{
		U32 bytePerPixel = BytePerPixel(m_Format);
		U32 size = mipmap.m_iSizeX*mipmap.m_iSizeY* bytePerPixel;
		mipmap.m_Data = NEW U8[size];

		memset(mipmap.m_Data,0,size);
	
		U32 maxX = (U32)ROUND(_sizeX * downscaleRatio);
		U32 maxY = (U32)ROUND(_sizeY * downscaleRatio);
		Float fMaxX = (Float)maxX;
		Float fMaxY = (Float)maxY;
		Float fSizeX = (Float)_sizeX;
		Float fSizeY = (Float)_sizeY;
		Float factorX = fSizeX / fMaxX;
		Float factorY = fSizeY / fMaxY;

		if( downscaleRatio == 1.f )
		{
			for(U32 y=0; y<maxY; y++)
			{
				memcpy(mipmap.m_Data + y * mipmap.m_iSizeX * bytePerPixel,_dataToCopy + (y*_sizeX) * bytePerPixel,_sizeX * bytePerPixel);
			}
		}
		else
		{
			Float fy=0.f;
			U8* curDataY = mipmap.m_Data;
			for(U32 y=0; y<maxY; y++,fy+=factorY)
			{
				U8* curData = curDataY;
				Float fx=0.f;
				U32 srcy = _sizeX * (U32)ROUND(fy);
				const U8* curDataToCopy = _dataToCopy + srcy * bytePerPixel;
				for(U32 x=0; x<maxX; x++,fx+=factorX)
				{
					U32 srcx = (U32)ROUND(fx);
					memcpy(curData,curDataToCopy + (srcx * bytePerPixel), bytePerPixel);
					curData += bytePerPixel;
				}

				curDataY += mipmap.m_iSizeX * bytePerPixel;
			}
		}
	}
}

//----------------------------------

U32		Texture::GetDataSize(S32 _level /*= 0*/) const
{
	const Texture::MipMap& mipmap = m_MipMaps[_level];
	return mipmap.m_iSizeX * mipmap.m_iSizeY * BytePerPixel(m_Format);
}

//----------------------------------

void	Texture::ReleaseData(S32 _level /*= 0*/)
{
	Texture::MipMap& mipmap = m_MipMaps[_level];
	if (mipmap.m_Data) // store texture only on GPU
	{
		DELETE[] mipmap.m_Data;
		mipmap.m_Data = NULL;
	}
}

//----------------------------------

void	Texture::WriteCache()
{
	Str cachePath;
	cachePath.SetFrom("%s.bin",m_Name.GetStr().GetArray());
	OldFile file(cachePath,OldFile::USE_TYPE (OldFile::USE_TYPE_Write | OldFile::USE_TYPE_Cache));
	file.Open(TRUE); 
	file.WriteU16(CACHE_VERSION);
	file.WriteU32(m_LoadedModificationDate);
	file.WriteBool(m_bIsTransparent);
	file.WriteU32(m_Type);
	file.WriteU32(m_Format);
	file.WriteS32(m_MipMaps.GetCount());
	for(U32 i=0; i<m_MipMaps.GetCount(); i++ )
	{
		const MipMap& m = m_MipMaps[i];
		file.WriteU32(m.m_iSizeX);
		file.WriteU32(m.m_iSizeY);
		file.WriteU32(m.m_iRealSizeX);
		file.WriteU32(m.m_iRealSizeY);
		file.WriteFloat(m.m_fUVMaxX);
		file.WriteFloat(m.m_fUVMaxY);

		U32 size = m.m_iSizeX*m.m_iSizeY*BytePerPixel(m_Format);
		file.WriteByte((S8*)m.m_Data,size);
	}
	file.Close();
}

//----------------------------------

Bool	Texture::LoadCache(const Str& _path)
{
    Str cachePath;
	cachePath.SetFrom("%s.bin",m_Name.GetStr().GetArray());
	OldFile file(cachePath,OldFile::USE_TYPE (OldFile::USE_TYPE_ReadBuffer | OldFile::USE_TYPE_Cache));
	if( file.Exists() )
	{
		file.Open(); 
		if( file.IsOpen() )
		{
			if( file.ReadU16() != CACHE_VERSION)
			{
				file.Close();
				return FALSE;
			}

			m_LoadedModificationDate = file.ReadU32();
			m_CurrentModificationDate = m_LoadedModificationDate;
			m_bIsTransparent = file.ReadBool();
			m_Type = (EType)file.ReadU32();
			m_Format = (EFormat)file.ReadU32();
			S32 count = file.ReadS32();
			m_MipMaps.SetItemCount(count);
			for(U32 i=0; i<m_MipMaps.GetCount(); i++ )
			{
				MipMap& m = m_MipMaps[i];
				m.m_iSizeX = file.ReadU32();
				m.m_iSizeY = file.ReadU32();
				m.m_iRealSizeX = file.ReadU32();
				m.m_iRealSizeY = file.ReadU32();
				m.m_fUVMaxX = file.ReadFloat();
				m.m_fUVMaxY = file.ReadFloat();

				U32 size = m.m_iSizeX*m.m_iSizeY*BytePerPixel(m_Format);
				m.m_Data = NEW U8[size];
				file.ReadByte((S8*)m.m_Data,size);
			}
			file.Close();

			return TRUE;
		}
	}

	return FALSE;
}

//----------------------------------

U32		Texture::ComputePow2(U32 _base)
{
	U32 res = 1;
	for(S32 i=0;i<30;i++)
	{
		res = res << 1;
		if( res >= _base )
			return res;
	}

	return _base;
}

//----------------------------------

void	Texture::OnPause()
{
	if (m_bRegister)
		RendererDatas::GetInstance().UnregisterTexture(this);
}
	
//----------------------------------

void	Texture::OnResume()
{
    /*if( m_Id < 0 )
    {
    	if( !LoadCache(m_Path) )
    	{
			ResourceRefArray resources;
			LoaderRef loader = LoaderFactory::GetInstance().GetNewLoader(m_Path.GetExtension());
			if( loader.IsValid() )
			{
				loader->LoadToResources(m_Path, resources);
				for(U32 i=0; i<resources.GetCount(); i++)
				{
					if(resources[i]->GetName() == m_Name )
					{
						Texture* texture = ((Texture*)(resources[i].GetPtr()));
						m_Id = texture->m_Id;
						texture->m_Id = -1;
						return;
					}
				}
			}
    	}
    }*/
}
