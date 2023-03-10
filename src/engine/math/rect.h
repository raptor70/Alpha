#ifndef __RECT_H__
#define __RECT_H__

class Rect
{
	DECLARE_SCRIPT_CLASS_FINAL(Rect)
public :    
	Rect(): m_vTopLeft(Vec2f::Zero), m_vSize(Vec2f::One) { }
	Rect( const Vec2f& _vTopLeft, const Vec2f& _vSize ): m_vTopLeft(_vTopLeft), m_vSize(_vSize) { }
	Rect( const Rect& _other ): m_vTopLeft(_other.m_vTopLeft), m_vSize(_other.m_vSize) { }
	~Rect() { }

	Rect& operator=( const Rect& _other );

	Vec2f	m_vTopLeft;
	Vec2f	m_vSize;
};

#endif