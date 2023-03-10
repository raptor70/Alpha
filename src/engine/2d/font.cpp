#include "font.h"

#include "resource/resource_manager.h"
#include "3d/texture_manager.h"
#include "file/base_file.h"

// -----------------------------------------

Font::SGlyph	LinearInterpolation(const Font::SGlyph& _a, const Font::SGlyph& _b, Float _ratio)
{
	if (_ratio < Float_Eps)
		return _a;
	else if (_ratio >(1.f - Float_Eps))
		return _b;

	Font::SGlyph result;
	result.m_vUVMin = LinearInterpolation(_a.m_vUVMin, _b.m_vUVMin, _ratio);
	result.m_vUVMax = LinearInterpolation(_a.m_vUVMax, _b.m_vUVMax, _ratio);
	result.m_fAdvance = LinearInterpolation(_a.m_fAdvance, _b.m_fAdvance, _ratio);
	result.m_vBearing = LinearInterpolation(_a.m_vBearing, _b.m_vBearing, _ratio);
	result.m_vSize = LinearInterpolation(_a.m_vSize, _b.m_vSize, _ratio);
	return result;
}

//----------------------------------

Font::Font()
{
}

//----------------------------------

Font::~Font()
{
}

//----------------------------------

void	Font::WriteCache()
{
	Str cachePath;
	cachePath.SetFrom("%s.bin",m_Name.GetStr().GetArray());
	OldFile file(cachePath, OldFile::USE_TYPE(OldFile::USE_TYPE_Write | OldFile::USE_TYPE_Cache));
	file.Open(TRUE);
	file.WriteU16(CACHE_VERSION);
	file.WriteU32(m_LoadedModificationDate);
	file.WriteU32(m_Glyphs.GetCount());
	for (U32 g = 0; g < m_Glyphs.GetCount(); g++)
	{
		SGlyphSize& size = m_Glyphs[g];
		file.WriteFloat(size.m_Size);
		file.WriteFloat(size.m_MaxY);
		file.WriteFloat(size.m_MinY);
		for (S32 i = 0; i < 256; i++)
		{
			const SGlyph& g = size.m_Chars[i];
			file.WriteFloat(g.m_vUVMin.x);
			file.WriteFloat(g.m_vUVMin.y);
			file.WriteFloat(g.m_vUVMax.x);
			file.WriteFloat(g.m_vUVMax.y);
			file.WriteFloat(g.m_fAdvance);
			file.WriteFloat(g.m_vBearing.x);
			file.WriteFloat(g.m_vBearing.y);
			file.WriteFloat(g.m_vSize.x);
			file.WriteFloat(g.m_vSize.y);
		}
	}
	file.Close();
}

//----------------------------------

Bool	Font::LoadCache(const Str& _path)
{
	Str cachePath;
	cachePath.SetFrom("%s.bin",m_Name.GetStr().GetArray());
	OldFile file(cachePath, OldFile::USE_TYPE(OldFile::USE_TYPE_Read | OldFile::USE_TYPE_Cache));
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
			U32 count = file.ReadU32();
			m_Glyphs.SetItemCount(count);
			for (U32 g = 0; g < count; g++)
			{
				SGlyphSize& glyphSize = m_Glyphs[g];
				glyphSize.m_Size = file.ReadFloat();
				glyphSize.m_MaxY = file.ReadFloat();
				glyphSize.m_MinY = file.ReadFloat();
				for (S32 i = 0; i < 256; i++)
				{
					glyphSize.m_Chars[i].m_vUVMin.x = file.ReadFloat();
					glyphSize.m_Chars[i].m_vUVMin.y = file.ReadFloat();
					glyphSize.m_Chars[i].m_vUVMax.x = file.ReadFloat();
					glyphSize.m_Chars[i].m_vUVMax.y = file.ReadFloat();
					glyphSize.m_Chars[i].m_fAdvance = file.ReadFloat();
					glyphSize.m_Chars[i].m_vBearing.x = file.ReadFloat();
					glyphSize.m_Chars[i].m_vBearing.y = file.ReadFloat();
					glyphSize.m_Chars[i].m_vSize.x = file.ReadFloat();
					glyphSize.m_Chars[i].m_vSize.y = file.ReadFloat();
				}
			}
			file.Close();

			Vec2f uvmin,uvmax;
			TextureManager::GetInstance().GetTexture(_path,m_Texture,uvmin,uvmax);

			return TRUE;
		}
	}

	return FALSE;
}

//----------------------------------

void	Font::ComputeGlyphSizeRatio(Float _size, SGlyphSizeRatio& _ratio) const
{
	U32 count = m_Glyphs.GetCount();
	DEBUG_Require(count>0);

	// first case, greater than max size
	if (_size > m_Glyphs[0].m_Size || count == 1)
	{
		_ratio.m_FirstIndex = _ratio.m_SecondIndex = 0;
		_ratio.m_Ratio = 0.f;
		return;
	}

	// second case, lower than min size
	if (_size < m_Glyphs[count-1].m_Size)
	{
		_ratio.m_FirstIndex = _ratio.m_SecondIndex = count - 1;
		_ratio.m_Ratio = 0.f;
		return;
	}

	// final case, at least 2 size, between 2 of them
	_ratio.m_SecondIndex = 0;
	while (_ratio.m_SecondIndex < m_Glyphs.GetCount()&& m_Glyphs[_ratio.m_SecondIndex].m_Size > _size)
	{
		_ratio.m_SecondIndex++;
	}
	_ratio.m_FirstIndex = _ratio.m_SecondIndex - 1;
	Float a = m_Glyphs[_ratio.m_FirstIndex].m_Size;
	Float b = m_Glyphs[_ratio.m_SecondIndex].m_Size;
	_ratio.m_Ratio = (_size - a) / (b - a);
}

//----------------------------------

void	Font::AddGlyph(SGlyphSize& _glyphSize, U8 _char,const Vec2f& _uvmin,const Vec2f& _uvmax, Float _advance, const Vec2f& _bearing, const Vec2f& _size, Float _refSize)
{
	DEBUG_Require(_char<256);
	_glyphSize.m_Chars[_char].m_vUVMin = _uvmin;
	_glyphSize.m_Chars[_char].m_vUVMax = _uvmax;
	_glyphSize.m_Chars[_char].m_fAdvance = _advance;
	_glyphSize.m_Chars[_char].m_vBearing = _bearing;
	_glyphSize.m_Chars[_char].m_vSize = _size;

	_glyphSize.m_MaxY = Max(_glyphSize.m_MaxY,_size.y-_bearing.y);
	_glyphSize.m_MinY = Min(_glyphSize.m_MinY,-_bearing.y);
}

//----------------------------------

void	Font::ComputeGlyphSizeMinMaxY(const SGlyphSizeRatio& _ratio, Float& _outMinY, Float& _outMaxY) const
{
	_outMinY = LinearInterpolation(m_Glyphs[_ratio.m_FirstIndex].m_MinY, m_Glyphs[_ratio.m_SecondIndex].m_MinY, _ratio.m_Ratio);
	_outMaxY = LinearInterpolation(m_Glyphs[_ratio.m_FirstIndex].m_MaxY, m_Glyphs[_ratio.m_SecondIndex].m_MaxY, _ratio.m_Ratio);
}

//----------------------------------

void	Font::ComputeGlyph(U8 _char, const SGlyphSizeRatio& _ratio, SGlyph& _outGlyph)	const
{
	_outGlyph = LinearInterpolation(m_Glyphs[_ratio.m_FirstIndex].m_Chars[_char], m_Glyphs[_ratio.m_SecondIndex].m_Chars[_char], _ratio.m_Ratio);
}

//----------------------------------

void	Font::ComputeTextGeometry(const Str& _str, VertexBufferRef& _outVB, IndexBufferRef& _outIB, Float _size) const
{
	U16 length = (U16)_str.GetLength();
	_outVB->SetVertexCount(length*4);
	_outIB->m_Array.SetItemCount(length*6);

	SGlyphSizeRatio glyphRatio;
	ComputeGlyphSizeRatio(_size, glyphRatio);
	Float maxY = 0.f, minY = 0.f;
	ComputeGlyphSizeMinMaxY(glyphRatio, minY, maxY);

	Vec3f pos = Vec3f::YAxis * (maxY + minY) * -.5f;
	Vec3f p0,p1,p2,p3;
	Vec2f u0,u1,u2,u3;
	for(U16 i=0;i<length;i++)
	{
		Char c = _str.GetArray()[i];
		SGlyph glyph;
		ComputeGlyph(c, glyphRatio, glyph);
		Vec2f size = glyph.m_vSize;
		Vec2f bearing = glyph.m_vBearing;
		p0 = pos + Vec3f(bearing.x,-bearing.y,0.f);					// top left
		p1 = pos + Vec3f(bearing.x,size.y - bearing.y,0.f);			// bottom left
		p2 = pos + Vec3f(bearing.x+size.x,-bearing.y,0.f);			// top right
		p3 = pos + Vec3f(bearing.x+size.x,size.y - bearing.y,0.f);	// bottom right
		pos.x += glyph.m_fAdvance;

		u0 = Vec2f(glyph.m_vUVMin.x, glyph.m_vUVMin.y);
		u1 = Vec2f(glyph.m_vUVMin.x, glyph.m_vUVMax.y);
		u2 = Vec2f(glyph.m_vUVMax.x, glyph.m_vUVMin.y);
		u3 = Vec2f(glyph.m_vUVMax.x, glyph.m_vUVMax.y);

		U16 startvtx = i*4;
		_outVB->SetVertexAtIndex(startvtx,p0,u0,Vec3f::ZAxis);
		_outVB->SetVertexAtIndex(startvtx+1,p1,u1,Vec3f::ZAxis);
		_outVB->SetVertexAtIndex(startvtx+2,p2,u2,Vec3f::ZAxis);
		_outVB->SetVertexAtIndex(startvtx+3,p3,u3,Vec3f::ZAxis);

		U32 startidx = i*6;
		_outIB->m_Array[startidx] = startvtx;
		_outIB->m_Array[startidx+1] = startvtx+1;
		_outIB->m_Array[startidx+2] = startvtx+2;
		_outIB->m_Array[startidx+3] = startvtx+2;
		_outIB->m_Array[startidx+4] = startvtx+1;
		_outIB->m_Array[startidx+5] = startvtx+3;
	}
}

//----------------------------------

void	Font::ComputeTextBlock(const Str& _str, STextBlockParams& _params) const
{
	StrArray lines;
	_str.Split("\n", lines);
	SGlyphSizeRatio glyphRatio;
	ComputeGlyphSizeRatio(_params.m_Size,glyphRatio);
	SGlyph spaceGlyph;
	ComputeGlyph(' ', glyphRatio, spaceGlyph);
	ArrayOf<STextBlockLine> text;
	Vec2f blockSize = _params.m_BlockSize;
	if (blockSize.x < Float_Eps)
		blockSize.x = 1e7f;
	if (blockSize.y < Float_Eps)
		blockSize.y = 1e7f;
	for (U32 l = 0; l < lines.GetCount(); l++)
	{
		Float fLineLeft = blockSize.x;
		const Str& line = lines[l];
		StrArray words;
		line.Split(" ", words);
		STextBlockLine* lineToAdd = &(text.AddLastItem());
		for (U32 w = 0; w < words.GetCount(); w++)
		{
			const Str& word = words[w];

			// compute word size
			Float fWordSize = 0.f;
			for (U32 c = 0; c < word.GetLength(); c++)
			{
				SGlyph tmpGlyph;
				ComputeGlyph(word[c], glyphRatio, tmpGlyph);
				fWordSize += tmpGlyph.m_fAdvance;
			}

			// add word if need
			Bool bWordAdded = FALSE;
			if (fWordSize < fLineLeft)
			{
				// add space previously
				if (lineToAdd->m_Words.GetCount())
				{
					lineToAdd->m_fSize += spaceGlyph.m_fAdvance;
					fLineLeft -= spaceGlyph.m_fAdvance;
				}

				// add word
				lineToAdd->m_Words.AddLastItem(word);
				lineToAdd->m_fSize += fWordSize;
				fLineLeft -= fWordSize;
				
				bWordAdded = TRUE;
			}

			// new line ?
			if (fLineLeft <= 0.f || !bWordAdded)
			{
				if (!bWordAdded)
					lineToAdd->m_bIsComplete = TRUE; // current line is complete
				lineToAdd = &(text.AddLastItem()); // create new one
				fLineLeft = blockSize.x;
			}

			// add current word to next line if not added to current line
			if (!bWordAdded)
			{
				lineToAdd->m_Words.AddLastItem(word);
				lineToAdd->m_fSize += fWordSize;
				fLineLeft -= fWordSize;
			}
		}
	}

	AddTextToBlock(text, _params);
}

//----------------------------------

void	Font::AddTextToBlock(const ArrayOf<STextBlockLine>& _text, STextBlockParams& _params) const
{
	SGlyphSizeRatio glyphRatio;
	ComputeGlyphSizeRatio(_params.m_Size, glyphRatio);
	Float maxY = 0.f, minY = 0.f;
	ComputeGlyphSizeMinMaxY(glyphRatio, minY, maxY);
	SGlyph spaceGlyph;
	ComputeGlyph(' ', glyphRatio, spaceGlyph);
	Float fSpaceSize = spaceGlyph.m_fAdvance;
	Vec3f p0, p1, p2, p3;
	Vec2f u0, u1, u2, u3;
	Float startX = -_params.m_BlockSize.x * .5f;
	Float fLineSize = (maxY - minY);
	Float fVSize = _text.GetCount() * fLineSize;
	Vec3f pos;
	pos.x = startX;
	switch (_params.m_VAlign)
	{
	case VALIGN_TOP: 
	{
		pos.y = _params.m_BlockSize.y * -.5f - minY;
		break;
	}
	case VALIGN_CENTER:
	{
		pos.y = fVSize * -.5f - minY;
		break;
	}
	case VALIGN_BOTTOM:
	{
		pos.y = _params.m_BlockSize.y * .5f - fVSize - minY;
		break;
	}
	}
	pos.z = 0.f;
	S32 meshIdx = _params.m_outMesh->AddSubMesh();
	Primitive3DRef& primitive = _params.m_outMesh->GetPrimitive(meshIdx);
	VertexBufferRef outVB;
	IndexBufferRef outIB;
	U16 charIdx = 0;
	primitive->Lock(outVB, outIB);
	for (U32 l = 0; l < _text.GetCount(); l++)
	{
		const STextBlockLine& line = _text[l];

		// compute start pos X
		switch (_params.m_HAlign)
		{
		case HALIGN_LEFT: break; // do nothing
		case HALIGN_CENTER: pos.x += (_params.m_BlockSize.x - line.m_fSize) * .5f; break;
		case HALIGN_RIGHT: pos.x += (_params.m_BlockSize.x - line.m_fSize); break;
		case HALIGN_JUSTIFY:
		{
			if (line.m_bIsComplete)
			{
				Float fTotalSpaceSize = (_params.m_BlockSize.x - line.m_fSize);
				fSpaceSize = spaceGlyph.m_fAdvance + fTotalSpaceSize / Float(line.m_Words.GetCount() - 1);
			}
			else
			{
				fSpaceSize = spaceGlyph.m_fAdvance;
			}
			break;
		}
		}

		for (U32 w = 0; w < line.m_Words.GetCount(); w++)
		{
			const Str& word = line.m_Words[w];
			outVB->SetVertexCount(outVB->GetVertexCount() + U16(word.GetLength()) * 4);
			outIB->m_Array.SetItemCount(outIB->m_Array.GetCount() + word.GetLength() * 6);

			for (U32 c = 0; c < word.GetLength(); c++)
			{
				SGlyph tmpGlyph;
				ComputeGlyph(word[c], glyphRatio, tmpGlyph);
	
				Vec2f size = tmpGlyph.m_vSize;
				Vec2f bearing = tmpGlyph.m_vBearing;
				p0 = pos + Vec3f(bearing.x, -bearing.y, 0.f);					// top left
				p1 = pos + Vec3f(bearing.x, size.y - bearing.y, 0.f);			// bottom left
				p2 = pos + Vec3f(bearing.x + size.x, -bearing.y, 0.f);			// top right
				p3 = pos + Vec3f(bearing.x + size.x, size.y - bearing.y, 0.f);	// bottom right
				pos.x += tmpGlyph.m_fAdvance;

				u0 = Vec2f(tmpGlyph.m_vUVMin.x, tmpGlyph.m_vUVMin.y);
				u1 = Vec2f(tmpGlyph.m_vUVMin.x, tmpGlyph.m_vUVMax.y);
				u2 = Vec2f(tmpGlyph.m_vUVMax.x, tmpGlyph.m_vUVMin.y);
				u3 = Vec2f(tmpGlyph.m_vUVMax.x, tmpGlyph.m_vUVMax.y);

				U16 startvtx = charIdx * 4;
				outVB->SetVertexAtIndex(startvtx, p0, u0, Vec3f::ZAxis);
				outVB->SetVertexAtIndex(startvtx + 1, p1, u1, Vec3f::ZAxis);
				outVB->SetVertexAtIndex(startvtx + 2, p2, u2, Vec3f::ZAxis);
				outVB->SetVertexAtIndex(startvtx + 3, p3, u3, Vec3f::ZAxis);

				U32 startidx = charIdx * 6;
				outIB->m_Array[startidx] = startvtx;
				outIB->m_Array[startidx + 1] = startvtx + 1;
				outIB->m_Array[startidx + 2] = startvtx + 2;
				outIB->m_Array[startidx + 3] = startvtx + 2;
				outIB->m_Array[startidx + 4] = startvtx + 1;
				outIB->m_Array[startidx + 5] = startvtx + 3;

				charIdx++;
			}

			pos.x += fSpaceSize;
		}

		pos.x = startX;
		pos.y += fLineSize;
	}

	primitive->Unlock();

	MaterialRef& mat = _params.m_outMesh->GetMaterial(meshIdx);
	mat->SetTexture(GetTexture());
	mat->SetIsFont(TRUE);
}

//----------------------------------

SINGLETON_Define(FontManager);
BEGIN_SCRIPT_CLASS(FontManager)
	SCRIPT_STATIC_RESULT_METHOD(GetInstance,FontManager)
	SCRIPT_VOID_METHOD2(LoadFont,Name,Str)
END_SCRIPT_CLASS

//----------------------------------

FontManager::FontManager()
{
}

//----------------------------------

FontManager::~FontManager()
{
}

//----------------------------------

void FontManager::LoadFont(const Name& _name, const Str& _path)
{
	SFont& font = m_Fonts.AddLastItem(SFont());
	font.m_Path = _path;
	font.m_Name = _name;
}

//----------------------------------

void FontManager::LoadFontResources()
{
	for(U32 i=0; i<m_Fonts.GetCount(); i++)
	{
		SFont& font = m_Fonts[i];
		font.m_Font = ResourceManager::GetInstance().GetResourceFromFile<Font>(font.m_Path);
	}
}

//----------------------------------

const Font*	FontManager::GetFont(const Name& _name)
{
	for(U32 i=0; i<m_Fonts.GetCount(); i++)
		if( m_Fonts[i].m_Name == _name )
			return (Font*)m_Fonts[i].m_Font.GetPtr();

	DEBUG_Forbidden("Unknown font %s !!!",_name.GetStr().GetArray());
	return NULL;
}