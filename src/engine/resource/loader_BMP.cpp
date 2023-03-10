#include "loader_BMP.h"

#include "file/base_file.h"
#include "3d/texture.h"
#include "resource_manager.h"

#include "platform/platform_win32.h"

DEFINE_LOADER_CLASS(BMP);

Loader_BMP::Loader_BMP()
{
}

//-----------------------------------------------

Loader_BMP::~Loader_BMP()
{
}

//-----------------------------------------------

Bool Loader_BMP::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
#ifdef WIN32
	OldFile file(_path,OldFile::USE_TYPE_ReadBuffer);
	file.Open();
	if( !file.IsOpen() )
		return FALSE;

	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	file.ReadByte((S8*)&fileheader,sizeof(fileheader));
	file.ReadByte((S8*)&infoheader,sizeof(infoheader));

	Char* c = "BM";

	if(fileheader.bfType != 0x4D42)
	{
		LOGGER_LogError("%s isn't a BMP file !",_path.GetArray());
		return FALSE;
	}

	S32 bytePerPixel = infoheader.biBitCount / 8;
	S32 pixelCount = infoheader.biWidth*infoheader.biHeight;
	S32 dataSize = pixelCount * bytePerPixel;
	U8* data = NEW U8[dataSize];
	file.MoveToAdress(fileheader.bfOffBits);
	if( !file.ReadByte((S8*)data,dataSize) )
	{
		LOGGER_LogError("%s is a corrupt BMP file !",_path.GetArray());
		return FALSE;;
	}

	// transform BGR to RGB (todo : support BGR ??)
	
	for(S32 i=0 ;i<pixelCount;i++)
	{
		U8 tmp = data[i*bytePerPixel];
		data[i*bytePerPixel] = data[i*bytePerPixel + 2];
		data[i*bytePerPixel + 2] = tmp;
	}

	Texture::EFormat format = Texture::TEXTUREFORMAT_RGB8;
	switch (bytePerPixel)
	{
	case 1: format = Texture::TEXTUREFORMAT_R8;
	case 2: format = Texture::TEXTUREFORMAT_RG8;
	case 3: format = Texture::TEXTUREFORMAT_RGB8;
	case 4: format = Texture::TEXTUREFORMAT_RGBA8;
	default: DEBUG_Forbidden("Unknown format!"); break;
	}

	// create resource
	ResourceRef ref = ResourceManager::GetInstance().CreateNewResource<Texture>(_path);
	Texture* pTexture = ref.GetCastPtr<Texture>();
	pTexture->Init(infoheader.biWidth,infoheader.biHeight, format, data);
	_outResources.AddLastItem(ref);

	DELETE[] data;

	file.Close();
	

	return TRUE;
#else
	return FALSE;
#endif
}
