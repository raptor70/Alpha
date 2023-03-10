#include "debug_renderer.h"

#include "renderer/renderer.h"
#include "2d/font.h"
#include "resource/resource_manager.h"

#ifdef USE_DRAWDEBUG

SINGLETON_Define(DebugRenderer);

DebugRenderer::DebugRenderer()
{
	m_DebugFont = FontManager::GetInstance().GetFont("debug");
	m_iCurrentWriteBuffer = 0;
	m_iCurrentDrawBuffer = 0;
	m_LineMaterial = RendererDatas::GetInstance().CreateMaterial();
	m_LineMaterial->LoadTexture("debug/white.png");
	m_pPrimitive = Primitive3D::Create("draw_debug_lines", TRUE);
}

//----------------------------------

DebugRenderer::~DebugRenderer()
{
}

//----------------------------------

void	DebugRenderer::Reset()
{
	m_Lines[m_iCurrentWriteBuffer].Flush();
	m_Texts.Flush();
}

//----------------------------------

void	DebugRenderer::Draw2D(Renderer* _renderer)
{
	// todo
}

//----------------------------------

void	DebugRenderer::Draw3D(Renderer* _pRenderer)
{
	// set primitive
	VertexBufferRef vb;
	IndexBufferRef ib;
	m_pPrimitive->Lock(vb, ib);
	U16 nbLines = (U16)m_Lines[m_iCurrentDrawBuffer].GetCount();
	vb->SetVertexCount(nbLines * 2);
	ib->m_Array.SetItemCount(nbLines * 2);
	U32 idx = 0;
	for (U32 i = 0; i<m_Lines[m_iCurrentDrawBuffer].GetCount(); i++)
	{
		const Line& line = m_Lines[m_iCurrentDrawBuffer][i];
		
		vb->SetVertexAtIndex(
			idx,
			line.m_p0,
			Vec2f::Zero,
			Vec3f::Zero,
			line.m_color
		);
		ib->m_Array[idx] = (U16)idx;
		idx++;

		vb->SetVertexAtIndex(
			idx,
			line.m_p1,
			Vec2f::Zero,
			Vec3f::Zero,
			line.m_color
		);
		ib->m_Array[idx] = (U16)idx;
		idx++;
	}
	m_pPrimitive->Unlock();

	_pRenderer->SetActiveMaterial(m_LineMaterial);
	_pRenderer->DrawGeometry(
		DRAWBUFFER_DrawDebug,
		Geometry_Lines,
		Vec3f::Zero,
		Quat::Identity,
		m_pPrimitive);
	_pRenderer->SetActiveMaterial(NULL);
}

//----------------------------------

void	DebugRenderer::PrepareDraw(Renderer* _renderer)
{
	for(U32 i=0; i<m_Texts.GetCount(); i++)
	{
		Text& text = m_Texts[i];
		Vec3f pos = text.m_Pos;
		if (text.m_bIs3D)
		{
			Vec2i screenSize = _renderer->GetViewportSize();
			pos = _renderer->GetCameraViewProjMatrix() * pos;
			pos.x += 1.f;
			pos.x *= Float(screenSize.x)*.5f;
			pos.y *= -1.f;
			pos.y += 1.f;
			pos.y *= Float(screenSize.y)*.5f;
			pos.z = 1.f;
		}
		_renderer->SetActiveMaterial(text.m_Material);
		Float yDelta = 0.f;
		for (U32 i = 0; i < text.m_Primitives.GetCount(); i++)
		{
			_renderer->DrawGeometry(Geometry_TrianglesList, pos + Vec3f::YAxis * yDelta, Quat::Identity, text.m_Primitives[i], Vec3f(text.m_fSize), TRUE);
			yDelta += text.m_fSize;
		}
	}

	Swap(m_iCurrentWriteBuffer,m_iCurrentDrawBuffer);
	m_pPrimitive->PrepareDraw();
}

//----------------------------------

void	DebugRenderer::AddLine(const Vec3f& _a, const Vec3f& _b, const Color&  _color)
{
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(_a,_b,_color));
}

//----------------------------------

void	DebugRenderer::AddCross(const Vec3f& _pos, const Float _size, const Color&  _color)
{
	Float halfSize = .5f * _size;
	Vec3f delta(halfSize,0.f,0.f);
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(_pos-delta,_pos+delta,_color));
	delta.Set(0.f,halfSize,0.f);
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(_pos-delta,_pos+delta,_color));
	delta.Set(0.f,0.f,halfSize);
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(_pos-delta,_pos+delta,_color));
}

//----------------------------------

void	DebugRenderer::AddBox(const Vec3f& _pos, const Quat& _rot, const Vec3f& _size, const Color&  _color)
{
	Vec3f halfSize = .5f * _size;
	
	Vec3f p0 = _pos + _rot * Vec3f(-halfSize.x,-halfSize.y,-halfSize.z);
	Vec3f p1 = _pos + _rot * Vec3f(-halfSize.x,-halfSize.y,halfSize.z);
	Vec3f p2 = _pos + _rot * Vec3f(-halfSize.x,halfSize.y,-halfSize.z);
	Vec3f p3 = _pos + _rot * Vec3f(-halfSize.x,halfSize.y,halfSize.z);

	Vec3f p4 = _pos + _rot * Vec3f(halfSize.x,-halfSize.y,-halfSize.z);
	Vec3f p5 = _pos + _rot * Vec3f(halfSize.x,-halfSize.y,halfSize.z);
	Vec3f p6 = _pos + _rot * Vec3f(halfSize.x,halfSize.y,-halfSize.z);
	Vec3f p7 = _pos + _rot * Vec3f(halfSize.x,halfSize.y,halfSize.z);

	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p0,p1,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p0,p2,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p0,p4,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p1,p3,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p1,p5,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p2,p3,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p2,p6,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p3,p7,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p4,p5,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p4,p6,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p5,p7,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p6,p7,_color));
}

//----------------------------------

void	DebugRenderer::AddQuad(const Vec3f& _min, const Vec3f& _max, const Color&  _color)
{
	Vec3f p0 = _min;
	Vec3f p1 = p0;
	p1.x = _max.x;
	Vec3f p2 = p0;
	p2.y = _max.y;
	Vec3f p3 = _max;
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p0,p1,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p0,p2,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p1,p3,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p2,p3,_color));
}

//----------------------------------

void	DebugRenderer::AddSphere(const Vec3f& _pos, const Float _radius, const Color&  _color)
{
	// xy
	Vec3f p0 = _pos + Vec3f(1.f,0.f,0.f)*_radius;
	Vec3f p1 = _pos + Vec3f(0.86f,0.5f,0.f)*_radius;
	Vec3f p2 = _pos + Vec3f(0.5f,0.86f,0.f)*_radius;
	Vec3f p3 = _pos + Vec3f(0.f,1.f,0.f)*_radius;
	Vec3f p4 = _pos + Vec3f(-0.5f,0.86f,0.f)*_radius;
	Vec3f p5 = _pos + Vec3f(-0.86f,0.5f,0.f)*_radius;
	Vec3f p6 = _pos + Vec3f(-1.0f,0.f,0.f)*_radius;
	Vec3f p7 = _pos + Vec3f(-0.86f,-0.5f,0.f)*_radius;
	Vec3f p8 = _pos + Vec3f(-0.5f,-0.86f,0.f)*_radius;
	Vec3f p9 = _pos + Vec3f(0.f,-1.f,0.f)*_radius;
	Vec3f p10 = _pos + Vec3f(0.5f,-.86f,0.f)*_radius;
	Vec3f p11 = _pos + Vec3f(0.86f,-.5f,0.f)*_radius;

	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p0,p1,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p1,p2,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p2,p3,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p3,p4,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p4,p5,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p5,p6,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p6,p7,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p7,p8,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p8,p9,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p9,p10,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p10,p11,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p11,p0,_color));

	// yz
	p0 = _pos + Vec3f(0.f,1.f,0.f)*_radius;
	p1 = _pos + Vec3f(0.f,0.86f,0.5f)*_radius;
	p2 = _pos + Vec3f(0.f,0.5f,0.86f)*_radius;
	p3 = _pos + Vec3f(0.f,0.f,1.f)*_radius;
	p4 = _pos + Vec3f(0.f,-0.5f,0.86f)*_radius;
	p5 = _pos + Vec3f(0.f,-0.86f,0.5f)*_radius;
	p6 = _pos + Vec3f(0.f,-1.0f,0.f)*_radius;
	p7 = _pos + Vec3f(0.f,-0.86f,-0.5f)*_radius;
	p8 = _pos + Vec3f(0.f,-0.5f,-0.86f)*_radius;
	p9 = _pos + Vec3f(0.f,0.f,-1.f)*_radius;
	p10 = _pos + Vec3f(0.f,0.5f,-.86f)*_radius;
	p11 = _pos + Vec3f(0.f,0.86f,-.5f)*_radius;

	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p0,p1,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p1,p2,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p2,p3,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p3,p4,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p4,p5,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p5,p6,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p6,p7,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p7,p8,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p8,p9,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p9,p10,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p10,p11,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p11,p0,_color));

	// xz
	p0 = _pos + Vec3f(1.f,0.f,0.f)*_radius;
	p1 = _pos + Vec3f(0.86f,0.f,0.5f)*_radius;
	p2 = _pos + Vec3f(0.5f,0.f,0.86f)*_radius;
	p3 = _pos + Vec3f(0.f,0.f,1.f)*_radius;
	p4 = _pos + Vec3f(-0.5f,0.f,0.86f)*_radius;
	p5 = _pos + Vec3f(-0.86f,0.f,0.5f)*_radius;
	p6 = _pos + Vec3f(-1.0f,0.f,0.f)*_radius;
	p7 = _pos + Vec3f(-0.86f,0.f,-0.5f)*_radius;
	p8 = _pos + Vec3f(-0.5f,0.f,-0.86f)*_radius;
	p9 = _pos + Vec3f(0.f,0.f,-1.f)*_radius;
	p10 = _pos + Vec3f(0.5f,0.f,-.86f)*_radius;
	p11 = _pos + Vec3f(0.86f,0.f,-.5f)*_radius;

	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p0,p1,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p1,p2,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p2,p3,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p3,p4,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p4,p5,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p5,p6,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p6,p7,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p7,p8,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p8,p9,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p9,p10,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p10,p11,_color));
	m_Lines[m_iCurrentWriteBuffer].AddLastItem(Line(p11,p0,_color));
}

//----------------------------------

void	DebugRenderer::AddText(const Char* _str, const S32 _posX, S32 _posY,const Color& _color, Float _size)
{
	if(!m_DebugFont )
		return;

	Text& newText = m_Texts.AddLastItem(Text(_str, Vec3f((Float)_posX, (Float)_posY, 1.f)));
	newText.m_Material = RendererDatas::GetInstance().CreateMaterial();
	newText.m_Material->SetTexture(m_DebugFont->GetTexture());
	newText.m_Material->SetDiffuseColor(_color);
	newText.m_Material->SetIsFont(TRUE);

	for (U32 i = 0; i < newText.m_TextLines.GetCount(); i++)
	{
		Primitive3DRef& primitive = newText.m_Primitives.AddLastItem();
		primitive = Primitive3D::Create(Str("debugtext_%d", i), FALSE);
		VertexBufferRef vb;
		IndexBufferRef ib;
		primitive->Lock(vb, ib);
		m_DebugFont->ComputeTextGeometry(newText.m_TextLines[i], vb, ib, _size);
		primitive->Unlock();
	}
	newText.m_fSize = _size;
}

//----------------------------------

void	DebugRenderer::AddText3D(const Char* _str, const Vec3f& _pos, const Color& _color, Float _size /*= 20.f*/)
{
	if (!m_DebugFont)
		return;

	Text& newText = m_Texts.AddLastItem(Text(_str, _pos));
	newText.m_Material = RendererDatas::GetInstance().CreateMaterial();
	newText.m_Material->SetTexture(m_DebugFont->GetTexture());
	newText.m_Material->SetDiffuseColor(_color);
	newText.m_Material->SetIsFont(TRUE);

	for (U32 i = 0; i < newText.m_TextLines.GetCount(); i++)
	{
		Primitive3DRef& primitive = newText.m_Primitives.AddLastItem();
		primitive = Primitive3D::Create(Str("debugtext3D_%d",i),FALSE);
		VertexBufferRef vb;
		IndexBufferRef ib;
		primitive->Lock(vb, ib);
		m_DebugFont->ComputeTextGeometry(newText.m_TextLines[i], vb, ib, _size);
		primitive->Unlock();
	}
	newText.m_fSize = _size;

	newText.m_bIs3D = TRUE;
}

#endif