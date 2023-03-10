#include "loader_TIF.h"


#include INCLUDE_LIB(LIB_TIFF, libtiff/tiffio.h)
#include INCLUDE_LIB(LIB_TIFF, libtiff/tiffiop.h)

#include "3d/texture.h"
#include "resource_manager.h"
#include "file/base_file.h"

DEFINE_LOADER_CLASS(TIF);

void TifErrorHandler(const char* module, const char* fmt, va_list ap){}
void TifWarningHandler(const char* module, const char* fmt, va_list ap){}

static OldFile* g_pCurrentFile = NULL;

tmsize_t TifReadProc(thandle_t fd, void* buf, tmsize_t size)
{
	OldFile* file = (OldFile*)fd;
	if( file->ReadByte((S8*)buf,U32(size)) )
		return size;

	return 0;
}

static tmsize_t TifWriteProc(thandle_t fd, void* buf, tmsize_t size)
{
	DEBUG_Forbidden("TODO");
	return 0;
}

uint64 TifSeekProc(thandle_t fd, uint64 off, int whence)
{
	OldFile* file = (OldFile*)fd;
	switch(whence)
	{
		case SEEK_SET:
			file->MoveToAdress((U32)off);
			break;
		default:
			DEBUG_Forbidden("TODO");
			break;
	}

	return off;
}

int TifCloseProc(thandle_t fd)
{
	OldFile* file = (OldFile*)fd;
	file->Close();
	return 0;
}

uint64 TifSizeProc(thandle_t fd)
{
	OldFile* file = (OldFile*)fd;
	return file->GetSize();
}

int TifMapProc(thandle_t fd, void** pbase, toff_t* psize){ return 0; }
void TifUnmapProc(thandle_t fd, void* base, toff_t size){}

Loader_TIF::Loader_TIF()
{
}

//-----------------------------------------------

Loader_TIF::~Loader_TIF()
{
}

//-----------------------------------------------

Bool Loader_TIF::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
	TIFFSetErrorHandler(TifErrorHandler);
	TIFFSetWarningHandler(TifWarningHandler);

	OldFile file(_path.GetArray(),OldFile::USE_TYPE_ReadBuffer);
	file.Open();
	if( !file.IsOpen() )
	{
		LOGGER_LogError("Unable to open file %s !\n",_path.GetArray());
		return FALSE;
	}

	TIFF* 	tif = TIFFClientOpen(
		_path.GetArray(),
		"r",
	    (thandle_t) &file,
	    TifReadProc, TifWriteProc,
	    TifSeekProc, TifCloseProc, TifSizeProc,
	    TifMapProc, TifUnmapProc);
	if (tif)
	{
        tif->tif_fd = (thandle_t)&file;
		g_pCurrentFile = &file;
        TIFFRGBAImage img;
        char emsg[1024];
        if (TIFFRGBAImageBegin(&img, tif, 0, emsg)) 
		{
            U8* raster = NEW U8[img.width * img.height * 4];
            if (TIFFRGBAImageGet(&img, (uint32*)raster, img.width, img.height)) 
			{
				// Convert to data (libtiff get 32bit pixels)
				U32 bytePerPixel = img.bitspersample * img.samplesperpixel / 8;
				U8* data = NEW U8[img.width * img.height * bytePerPixel];
				for(U32 y=0;y<img.height;y++)
					for(U32 x=0;x<img.width;x++)
						for(U32 b=0;b<bytePerPixel; b++)
							data[y*img.width*bytePerPixel+x*bytePerPixel+b] = raster[y*img.width*4+x*4+b];

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
				pTexture->Init(img.width,img.height, format,data);
				_outResources.AddLastItem(ref);

				// delete data
				DELETE[] data;
            }
            DELETE[] raster;
            TIFFRGBAImageEnd(&img);
        } 
		else
        {
			LOGGER_LogError("TIF error(%s) : %s\n",_path.GetArray(), emsg);
			TIFFClose(tif);
            g_pCurrentFile = NULL;
			return FALSE;
		}

        TIFFClose(tif);
        g_pCurrentFile = NULL;
		return TRUE;
	}
	return FALSE;
}
