#include "loader_PNG.h"

#include INCLUDE_LIB(LIB_PNG, png.h)

#include "file/base_file.h"
#include "3d/texture.h"
#include "resource_manager.h"

DEFINE_LOADER_CLASS(PNG);

Loader_PNG::Loader_PNG()
{
}

//-----------------------------------------------

Loader_PNG::~Loader_PNG()
{
}

//-----------------------------------------------

void png_read_from_mem (png_structp png_ptr, png_bytep data, png_size_t length)
{
  OldFile *src = (OldFile*)png_get_io_ptr(png_ptr);

  /* Copy data from image buffer */
  //memcpy (data, src->data + src->offset, length);
  src->ReadByte((S8*)data,U32(length));
}

//-----------------------------------------------

void png_error_fn(png_structp png_ptr, png_const_charp msg)
{
	LOGGER_LogError("PNG Error : %s\n",msg);
}

//-----------------------------------------------

void png_warning_fn(png_structp png_ptr, png_const_charp msg)
{
	LOGGER_LogWarning("PNG Warning : %s\n",msg);
}

//-----------------------------------------------

Bool Loader_PNG::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
	OldFile file(_path,OldFile::USE_TYPE_ReadBuffer);
	file.Open();
	if( !file.IsOpen() )
		return FALSE;

	// http://tfc.duke.free.fr/coding/png.html
	/* Check for valid magic number */
	png_byte magic[8];
	file.ReadByte((S8*)magic,sizeof(magic));
	if (!png_check_sig(magic, sizeof (magic)))
	{
		LOGGER_LogError("error PNG : \"%s\" is not a valid PNG image!\n", _path.GetArray());
		file.Close();
		return FALSE;
	}

	/* Create a png read struct */
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		LOGGER_LogError("error PNG : Unable to create read struct!\n");
		file.Close();
		return FALSE;
	}

	/* Create a png info struct */
	png_infop info_ptr = png_create_info_struct (png_ptr);
	if (!info_ptr)
	{
		LOGGER_LogError("error PNG : Unable to create info struct!\n");
		png_destroy_read_struct (&png_ptr, NULL, NULL);
		file.Close();
		return FALSE;
	}

	/* Initialize the setjmp for returning properly after a libpng  error occured */
	int val = setjmp(png_jmpbuf(png_ptr));
	if (val)
	{
		LOGGER_LogError("error PNG : Unable to jump buffer (%d) !\n",val);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		file.Close();
		return NULL;
	}

	/* Setup libpng for using standard C fread() function with our FILE pointer */
	//png_init_io(png_ptr, fp);
	png_set_error_fn(png_ptr, 0, png_error_fn, png_warning_fn);
	png_set_read_fn(png_ptr, (png_voidp *)&file, png_read_from_mem);

	/* Tell libpng that we have already read the magic number */
	png_set_sig_bytes (png_ptr, sizeof (magic));

	/* Read png info */
	png_read_info (png_ptr, info_ptr);

	/* Get some usefull information from header */
	S32 bit_depth = png_get_bit_depth (png_ptr, info_ptr);
	S32 color_type = png_get_color_type (png_ptr, info_ptr);

	/* Convert index color images to RGB images */
	if (color_type & PNG_COLOR_MASK_PALETTE)
		png_set_palette_to_rgb (png_ptr);

	if ((color_type & PNG_COLOR_MASK_ALPHA) == 0) // fill with alpha
		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

	/* Convert 1-2-4 bits grayscale images to 8 bits
	grayscale. */
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	{
		DEBUG_Forbidden("Unsupported\n"); 
		LOGGER_LogError("Unsupported\n");
	}
	// png_set_gray_1_2_4_to_8 (png_ptr);

	if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	if (bit_depth == 16)
		png_set_strip_16 (png_ptr);
	else if (bit_depth < 8)
		png_set_packing (png_ptr);

	/* Update info structure to apply transformations */
	png_read_update_info (png_ptr, info_ptr);

	/* Retrieve updated information */
	U32 w,h;
	w = h = 0;
	png_get_IHDR(png_ptr, info_ptr,(png_uint_32*)&w,(png_uint_32*)&h,(int*)&bit_depth,(int*)&color_type, NULL, NULL, NULL);
	//////////texinfo->width = w;
	//////////texinfo->height = h;

	/* Get image format and components per pixel */
	S32 bytePerPixel = 0;
	Texture::EFormat format = Texture::TEXTUREFORMAT_RGB8;
	switch (color_type)
	{
	case PNG_COLOR_TYPE_GRAY:
		bytePerPixel = 1;
		format = Texture::TEXTUREFORMAT_R8;
		break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:
		bytePerPixel = 2;
		format = Texture::TEXTUREFORMAT_RG8;
		break;

	case PNG_COLOR_TYPE_RGB:
		//bytePerPixel = 3;
		//format = Texture::TEXTUREFORMAT_RGB8;

		// filled with alpha
		bytePerPixel = 4;
		format = Texture::TEXTUREFORMAT_RGBA8;
		break;

	case PNG_COLOR_TYPE_RGB_ALPHA:
		bytePerPixel = 4;
		format = Texture::TEXTUREFORMAT_RGBA8;
		break;

	default:
		DEBUG_Forbidden("Unknown format!");
		break;
	}

	/* We can now allocate memory for storing pixel data */
	////////////texinfo->texels = (GLubyte *)malloc (sizeof (GLubyte) * texinfo->width
	////////////* texinfo->height * texinfo->internalFormat);

	/* Setup a pointer array.  Each one points at the begening of a row. */

	S32 pixelCount = w*h;
	S32 dataSize = pixelCount * bytePerPixel;
	U8* data = NEW U8[dataSize];
	png_bytep* row_pointers = NEW png_bytep[h];

	for (U32 i = 0; i < h; ++i)
	{
		//row_pointers[i] = (png_bytep)(data + ((h - (i + 1)) * w * bytePerPixel));
		row_pointers[i] = (png_bytep)(data + i * w * bytePerPixel);
	}

	/* Read pixel data using row pointers */
	png_read_image(png_ptr, row_pointers);

	/* Finish decompression and release memory */
	png_read_end (png_ptr, NULL);
	png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

	// create resource
	ResourceRef ref = ResourceManager::GetInstance().CreateNewResource<Texture>(_path);
	Texture* pTexture = ref.GetCastPtr<Texture>();
	pTexture->Init(w,h,format, data);
	_outResources.AddLastItem(ref);

	DELETE[] data;
	DELETE[] row_pointers;
	file.Close();

	return TRUE;
}