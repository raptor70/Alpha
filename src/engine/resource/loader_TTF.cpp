#include "loader_TTF.h"

#include "2d/font.h"
#include "resource_manager.h"
#include "file/base_file.h"


#define FT_CONFIG_CONFIG_H  "config/ftconfig.h"
#include INCLUDE_LIB(LIB_FREETYPE, include/ft2build.h)
#include INCLUDE_LIB(LIB_FREETYPE, include/freetype.h)
#include INCLUDE_LIB(LIB_FREETYPE, include/config/ftconfig.h)
#include INCLUDE_LIB(LIB_FREETYPE, include/ftmodapi.h)
#include INCLUDE_LIB(LIB_FREETYPE, include/ftglyph.h)

DEFINE_LOADER_CLASS(TTF);

//-----------------------------------------

class CDistanceFieldFilter
{
public:
	CDistanceFieldFilter(U8* _texture, S32 _textureSize)
	{
		m_TextureSize = _textureSize;
		m_pTexture = _texture;
		m_pDistances = NEW Float[_textureSize*_textureSize];
		m_fMinDist = 1e7f;
		m_fMaxDist = -1e7f;
	}

	~CDistanceFieldFilter()
	{
		DELETE[] m_pDistances;
	}

	void Process();
protected:
	U8* m_pTexture;
	S32 m_TextureSize;
	Float* m_pDistances;
	Float m_fMinDist;
	Float m_fMaxDist;
};

//-----------------------------------------

Loader_TTF::Loader_TTF()
{
}

//-----------------------------------------------

Loader_TTF::~Loader_TTF()
{
}

//-----------------------------------------------

void* FT_MyAlloc( FT_Memory  memory, long  size )
{
	return NEW U8[size];
}

void FT_MyFree( FT_Memory  memory, void* block )
{
	DELETE[] (U8*)block;
}


void* FT_MyRealloc( FT_Memory  memory, long  cur_size, long new_size, void* block )
{
	if (new_size == 0) 
	{
		DELETE[] (U8*)block;
		return NULL;
	}
	else
	{
		void* result = NEW U8[new_size];
		if( block )
		{
			memcpy(result,block,Min(new_size,cur_size));
			DELETE[] (U8*)block;
		}

		return result;
	}
}

unsigned long FT_MyRead( FT_Stream stream, unsigned long offset, unsigned char*  buffer, unsigned long   count )
{
	OldFile* file = (OldFile*)stream->descriptor.pointer;

	 if ( stream->pos != offset )
		file->MoveToAdress(offset);

	if( buffer && file->ReadByte((S8*)buffer,count) )
		return count;

	return 0;
}

void FT_MyClose(FT_Stream  stream)
{
	OldFile* file = (OldFile*)stream->descriptor.pointer;
	file->Close();
    stream->descriptor.pointer = NULL;
    stream->size               = 0;
    stream->base               = 0;
  }

Bool Loader_TTF::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
	FT_Memory  memory = NEW FT_MemoryRec_;
	memory->alloc = &FT_MyAlloc;
	memory->free = &FT_MyFree;
	memory->realloc = &FT_MyRealloc;

	OldFile file(_path,OldFile::USE_TYPE_ReadBuffer);
	file.Open();
	if( !file.IsOpen() )
	{
		LOGGER_LogError("Unable to open file %s !\n",_path.GetArray());
		return FALSE;
	}

	FT_Stream stream = NEW FT_StreamRec_;
	stream->descriptor.pointer = &file;
	stream->size  = (U32)file.GetSize();
    stream->pos   = 0;
    stream->base  = 0;
    stream->read  = &FT_MyRead;
    stream->close = &FT_MyClose;
	
	FT_Library library; 
	if( FT_New_Library(memory, &library) )
	{
		LOGGER_LogError("Couldn't init freetype !");
		return FALSE;
	}

	FT_Add_Default_Modules(library);

	FT_Face face;
	FT_Open_Args  args;
	args.flags    = FT_OPEN_STREAM;
    args.pathname = (char*)_path.GetArray();
    args.stream   = stream;
	if(FT_Open_Face(library, &args, 0, &face))  
	{
		LOGGER_LogError("Could not open font %s\n",_path.GetArray());
		return FALSE;
	}

	ResourceRef textureRef = ResourceManager::GetInstance().CreateNewResource<Texture>(_path);
	_outResources.AddLastItem(textureRef);
	Texture* pTexture = ((Texture*)textureRef.GetPtr());

	ResourceRef fontRef = ResourceManager::GetInstance().CreateNewResource<Font>(_path);
	_outResources.AddLastItem(fontRef);
	Font* pFont = ((Font*)fontRef.GetPtr());
	pFont->SetTexture(textureRef);

	static S32 sizes[] = { 256/*, 128, 64, 32, 16/*, 8, 4, 2*/ };
	for(S32 baseSizeIdx = 0; baseSizeIdx<_countof(sizes); baseSizeIdx++)
	{
		S32 nbGlyphPerLine = 16;
		S32 baseSize = sizes[baseSizeIdx];
		Font::SGlyphSize& glyphSize = pFont->AddGlyphSize(Float(baseSize));
		S32 textureSize = baseSize * nbGlyphPerLine;
		U8* atlas_buffer = NEW U8[textureSize * textureSize];
		for (S32 i = 0; i < textureSize * textureSize; i+=1)
		{
			atlas_buffer[i] = 0;
		}
		FT_Set_Pixel_Sizes(face, 0, baseSize);
		FT_Glyph_Metrics glyph_metrics[256];
		for(S32 c=0; c<256; c++)
		{
			if(FT_Load_Char(face, c, FT_LOAD_RENDER)) 
			{
				LOGGER_LogError("Could not load character %d in font %s\n",c,_path.GetArray());
				return FALSE;
			}

			glyph_metrics[c] = face->glyph->metrics;

			S32 startx = (c % nbGlyphPerLine) * baseSize;
			S32 starty = (c / nbGlyphPerLine) * baseSize;
			S32 width = Min(baseSize,(S32)face->glyph->bitmap.width);
			S32 height = Min(baseSize,(S32)face->glyph->bitmap.rows);

			for(S32 y=0;y<height;y++)
			for(S32 x=0;x<width;x++)
			{
				U32 idx = y*width + x;
				S32 i = ((starty + y) * textureSize + startx + x);			
				atlas_buffer[i] = face->glyph->bitmap.buffer[idx] > 128 ? 255 : 0;
			}

			// add glyph info per size
			{
				Float fTextureSize = (Float)textureSize;
				Vec2f uvMin((Float)startx/fTextureSize,(Float)starty/fTextureSize);
				Vec2f uvMax = uvMin + Vec2f((Float)width/fTextureSize,(Float)height/fTextureSize);

				// 64 points per pixel
				Float factor = 1.f/((Float)baseSize * 64.f);
				Float advance = (Float)glyph_metrics[c].horiAdvance * factor;
				Vec2f bearing((Float)glyph_metrics[c].horiBearingX,(Float)glyph_metrics[c].horiBearingY);
				bearing *= factor;
				Vec2f size((Float)glyph_metrics[c].width,(Float)glyph_metrics[c].height);
				size *= factor;
				pFont->AddGlyph(glyphSize,(U8)c,uvMin,uvMax,advance,bearing,size,Float(baseSize));
			}
		}

		CDistanceFieldFilter filter(atlas_buffer, textureSize);
		filter.Process();

		if(baseSizeIdx == 0)
			pTexture->Init(textureSize,textureSize,Texture::TEXTUREFORMAT_R8, atlas_buffer);
		else
			pTexture->AddLevel(textureSize, textureSize, atlas_buffer);
		DELETE[] atlas_buffer;
	}
	
	FT_Done_Face(face);
	FT_Done_Library(library);

	DELETE memory;
	DELETE stream;

	return TRUE;
}

//------------------------------------------------------

void CDistanceFieldFilter::Process()
{
	// compute distance brute force
	Vec2i curPos;
	Vec2i otherPos;
	S32 start = 0, stop = 0;
	for (curPos.x = 0; curPos.x < m_TextureSize; curPos.x++)
		for (curPos.y = 0; curPos.y < m_TextureSize; curPos.y++)
		{
			S32 idx = curPos.x + curPos.y * m_TextureSize;
			U8 curVal = m_pTexture[idx];
			Float fDist = 1e7f;
			S32 maxDist = 6;
			Float fMaxDist = 3.f;
			for (S32 otherDist = 1; otherDist <= maxDist; otherDist++)
			{
				// top line
				otherPos.y = curPos.y - otherDist;
				if (otherPos.y >= 0)
				{
					start = Max<S32>(0, curPos.x - otherDist);
					stop = Min<S32>(m_TextureSize - 1, curPos.x + otherDist);
					for (otherPos.x = start; otherPos.x <= stop; otherPos.x++)
					{
						S32 otherIdx = otherPos.x + otherPos.y * m_TextureSize;
						if (curVal != m_pTexture[otherIdx])
						{
							Vec2i deltaPos = otherPos - curPos;
							Float fOtherDist = deltaPos.GetLength();
							fDist = Min(fDist, fOtherDist);
						}
					}
				}

				// bot line
				otherPos.y = curPos.y + otherDist;
				if (otherPos.y < m_TextureSize)
				{
					start = Max<S32>(0, curPos.x - otherDist);
					stop = Min<S32>(m_TextureSize - 1, curPos.x + otherDist);
					for (otherPos.x = start; otherPos.x <= stop; otherPos.x++)
					{
						S32 otherIdx = otherPos.x + otherPos.y * m_TextureSize;
						if (curVal != m_pTexture[otherIdx])
						{
							Vec2i deltaPos = otherPos - curPos;
							Float fOtherDist = deltaPos.GetLength();
							fDist = Min(fDist, fOtherDist);
						}
					}
				}

				// left line
				otherPos.x = curPos.x - otherDist;
				if (otherPos.x >= 0)
				{
					start = Max<S32>(0, curPos.y - otherDist + 1);
					stop = Min<S32>(m_TextureSize - 1, curPos.y + otherDist - 1);
					for (otherPos.y = start; otherPos.y <= stop; otherPos.y++)
					{
						S32 otherIdx = otherPos.x + otherPos.y * m_TextureSize;
						if (curVal != m_pTexture[otherIdx])
						{
							Vec2i deltaPos = otherPos - curPos;
							Float fOtherDist = deltaPos.GetLength();
							fDist = Min(fDist, fOtherDist);
						}
					}
				}

				// right line
				otherPos.x = curPos.x + otherDist;
				if (otherPos.x < m_TextureSize)
				{
					start = Max<S32>(0, curPos.y - otherDist + 1);
					stop = Min<S32>(m_TextureSize - 1, curPos.y + otherDist - 1);
					for (otherPos.y = start; otherPos.y <= stop; otherPos.y++)
					{
						S32 otherIdx = otherPos.x + otherPos.y * m_TextureSize;
						if (curVal != m_pTexture[otherIdx])
						{
							Vec2i deltaPos = otherPos - curPos;
							Float fOtherDist = deltaPos.GetLength();
							fDist = Min(fDist, fOtherDist);
						}
					}
				}

				// found
				if (fDist<1e6f)
				{
					fDist *= (curVal == 0) ? -1.f : 1.f;
					m_fMinDist = Min(m_fMinDist, fDist);
					m_fMaxDist = Max(m_fMaxDist, fDist);
					m_pDistances[idx] = fDist;
					break;
				}

				if (otherDist == maxDist)
				{
					m_pDistances[idx] = 0.f;
					break;
				}
			}
		}

	// change value
	Float fInvRange = 1.f / (m_fMaxDist - m_fMinDist);
	for (S32 i = 0; i < m_TextureSize*m_TextureSize; i++)
	{
		if (m_pDistances[i] != 0.f)
			m_pTexture[i] = U8(255.f*fInvRange*(m_pDistances[i]- m_fMinDist));
	}
}
