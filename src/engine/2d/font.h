#ifndef __FONT_H__
#define __FONT_H__

#include "resource/resource.h"
#include "3d/mesh.h"

// -----------------------------------------

BEGIN_SUPER_CLASS(Font,Resource)
public:
	struct SGlyph // ADD IT TO LINEAR INTERPOLATION
	{
		Vec2f	m_vUVMin;
		Vec2f	m_vUVMax;
		Float	m_fAdvance;
		Vec2f	m_vBearing;
		Vec2f	m_vSize;
	};

	struct SGlyphSize
	{
		SGlyphSize()
		{
			m_MaxY = -1e7f;
			m_MinY = 1e7f;
		}

		Float		m_Size;
		SGlyph		m_Chars[256];
		Float		m_MaxY;
		Float		m_MinY;
	};

	struct SGlyphSizeRatio
	{
		U32		m_FirstIndex;
		U32		m_SecondIndex;
		Float	m_Ratio;
	};

	enum EHAlignment
	{
		HALIGN_LEFT = 0,
		HALIGN_CENTER,
		HALIGN_RIGHT,
		HALIGN_JUSTIFY
	};

	enum EVAlignment
	{
		VALIGN_TOP = 0,
		VALIGN_CENTER,
		VALIGN_BOTTOM
	};

	Font();
	virtual ~Font();

	static  const Char* GetResourcePrefix() { return "font"; }

	virtual void	WriteCache();
	virtual Bool	LoadCache(const Str& _path);

	void				SetTexture(const ResourceRef&	_texture)	{ m_Texture = _texture; }
	const ResourceRef& 	GetTexture() const	{ return m_Texture; }

	SGlyphSize&		AddGlyphSize(Float _refSize) { SGlyphSize& newSize = m_Glyphs.AddLastItem(SGlyphSize()); newSize.m_Size = _refSize; return newSize;}
	void			ComputeGlyphSizeMinMaxY(const SGlyphSizeRatio& _ratio, Float& _outMinY, Float& _outMaxY) const;
	void			ComputeGlyphSizeRatio(Float _size, SGlyphSizeRatio& _ratio) const;
	void			AddGlyph(SGlyphSize& _glyphSize, U8 _char,const Vec2f& _uvmin,const Vec2f& _uvmax, Float _advance, const Vec2f& _bearing, const Vec2f& _size, Float _refSize);
	void			ComputeGlyph(U8 _char, const SGlyphSizeRatio& _ratio, SGlyph& _outGlyph)	const;
	
	void			ComputeTextGeometry(const Str& _str, VertexBufferRef& _outVB, IndexBufferRef& _outIB, Float _size) const;

	struct STextBlockParams
	{
		Vec2f		m_BlockSize;
		Float		m_Size;
		EHAlignment	m_HAlign;
		EVAlignment	m_VAlign;
		Mesh*		m_outMesh;
	};

	void			ComputeTextBlock(const Str& _str, STextBlockParams& _params) const ;

protected:
	struct STextBlockLine
	{
		STextBlockLine()
		{
			m_fSize = 0.f;
			m_bIsComplete = FALSE;
		}
		StrArray	m_Words;
		Float		m_fSize;
		Bool		m_bIsComplete;
	};

	void			AddTextToBlock(const ArrayOf<STextBlockLine>& _text, STextBlockParams& _params) const;

	ResourceRef		m_Texture;
	ArrayOf<SGlyphSize>	m_Glyphs;
END_SUPER_CLASS

// -----------------------------------------

Font::SGlyph	LinearInterpolation(const Font::SGlyph& _a, const Font::SGlyph& _b, Float _ratio);

// -----------------------------------------

class FontManager
{
public:
	SINGLETON_Declare(FontManager)
	DECLARE_SCRIPT_CLASS(FontManager)
	FontManager();
	virtual ~FontManager();

	void			LoadFont(const Name& _name, const Str& _path);
	const Font*	GetFont(const Name& _name);
	void			LoadFontResources();

protected:
	struct SFont
	{
		ResourceRef	m_Font;
		Str			m_Path;
		Name		m_Name;
	};
	ArrayOf<SFont>	m_Fonts;
};

#endif