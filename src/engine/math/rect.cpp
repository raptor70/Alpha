#include "rect.h"

BEGIN_SCRIPT_CLASS(Rect)
	SCRIPT_STATIC_CONSTRUCTOR2(Vec2f, Vec2f)
END_SCRIPT_CLASS

//..

Rect& Rect::operator=( const Rect& _other )
{
	m_vTopLeft = _other.m_vTopLeft;
	m_vSize = _other.m_vSize;

	return (*this);
}
