#ifndef __DEBUG_RENDERER_H__
#define __DEBUG_RENDERER_H__

#ifdef USE_DRAWDEBUG

class DebugRenderer
{
	SINGLETON_Declare(DebugRenderer);
protected:
	struct Line
	{
		Color	m_color;
		Vec3f	m_p0;
		Vec3f	m_p1;

		Line(const Vec3f& _a, const Vec3f& _b, const Color& _c):m_p0(_a),m_p1(_b),m_color(_c) {}
		Line(){}
	};

	struct Text
	{
		StrArray	m_TextLines;
		Vec3f		m_Pos;
		Float		m_fSize;
		MaterialRef	m_Material;
		Primitive3DRefArray m_Primitives;
		Bool		m_bIs3D;

		Text(const Char* _str, const Vec3f& _pos):m_Pos(_pos), m_fSize(1.f),m_bIs3D(FALSE)
		{
			Str(_str).Split("\n", m_TextLines);
		}
		Text(){}
	};

public:
	DebugRenderer();
	virtual ~DebugRenderer();

	void	Draw2D(Renderer* _renderer);
	void	Draw3D(Renderer* _renderer);
	void	PrepareDraw(Renderer* _renderer);
	void	Reset();

	void	AddLine(const Vec3f& _a, const Vec3f& _b, const Color&  _color);
	void	AddCross(const Vec3f& _pos, const Float _size, const Color&  _color);
	void	AddBox(const Vec3f& _pos, const Quat& _rot, const Vec3f& _size, const Color&  _color);
	void	AddQuad(const Vec3f& _min, const Vec3f& _max, const Color&  _color);
	void	AddSphere(const Vec3f& _pos, const Float _radius, const Color&  _color);
	void	AddText(const Char* _str, const S32 _posX, S32 _posY,const Color& _color, Float _size = 20.f);
	void	AddText3D(const Char* _str, const Vec3f& _pos,const Color& _color, Float _size = 20.f);

protected:
	U8				m_iCurrentDrawBuffer;
	U8				m_iCurrentWriteBuffer;
	ArrayOf<Line>	m_Lines[2];
	ArrayOf<Text>	m_Texts;
	const Font*	m_DebugFont;
	Primitive3DRef	m_pPrimitive;
	MaterialRef		m_LineMaterial;
END_SUPER_CLASS

#define DEBUG_DrawLine(_a,_b,_color)							DebugRenderer::GetInstance().AddLine(_a,_b,_color)
#define DEBUG_DrawCross(_pos,_size,_color)						DebugRenderer::GetInstance().AddCross(_pos,_size,_color)
#define DEBUG_DrawBox(_center,_rot,_size,_color)				DebugRenderer::GetInstance().AddBox(_center,_rot,_size,_color)
#define DEBUG_DrawQuad(_min,_max,_color)						DebugRenderer::GetInstance().AddQuad(_min,_max,_color)
#define DEBUG_DrawSphere(_center,_radius,_color)				DebugRenderer::GetInstance().AddSphere(_center,_radius,_color)
#define DEBUG_DrawText(_str,_posX,_posY,_color)					DebugRenderer::GetInstance().AddText(_str,_posX,_posY,_color, 16.f)
#define DEBUG_DrawTextSized(_str,_posX,_posY,_color, _size)		DebugRenderer::GetInstance().AddText(_str,_posX,_posY,_color, _size)
#define DEBUG_DrawText3D(_str,_pos,_color)						DebugRenderer::GetInstance().AddText3D(_str,_pos,_color, 20.f)
#define DEBUG_DrawText3DSized(_str,_pos,_color, _size)			DebugRenderer::GetInstance().AddText3D(_str,_pos,_color, _size)

#else
#define DEBUG_DrawLine(_a,_b,_color)							{}
#define DEBUG_DrawCross(_pos,_size,_color)						{}
#define DEBUG_DrawBox(_center,_rot,_size,_color)				{}
#define DEBUG_DrawQuad(_min,_max,_color)						{}
#define DEBUG_DrawSphere(_center,_radius,_color)				{}
#define DEBUG_DrawText(_str,_posX,_posY,_color)					{}
#define DEBUG_DrawTextSized(_str,_posX,_posY,_color, _size)		{}
#define DEBUG_DrawText3D(_str,_pos,_color)						{}
#define DEBUG_DrawText3DSized(_str,_pos,_color, _size)			{}
#endif

#endif