#include "segment.h"

BEGIN_SCRIPT_CLASS(Segment)
	SCRIPT_STATIC_CONSTRUCTOR3(Vec3f,Vec3f,Float)
END_SCRIPT_CLASS

//..

Segment::Segment(const Vec3f& _a, const Vec3f& _b)
{
	m_vOrigin = _a;
	m_vDirection = _a - _b;
	m_fLength = m_vDirection.GetLength();
	if (m_fLength > Float_Eps)
	{
		m_vDirection /= m_fLength;
	}
}

//..

Segment& Segment::operator=( const Segment& _other )
{
	m_vOrigin = _other.m_vOrigin;
	m_vDirection = _other.m_vDirection;
	m_fLength = _other.m_fLength;

	return (*this);
}

//..

Float Segment::GetDistanceToPos(const Vec3f& _pos)
{
	Vec3f orignToPos = (_pos - m_vOrigin);
	Float dot = orignToPos * m_vDirection;
	if (dot < 0.f)
		return orignToPos.GetLength();

	if (dot > m_fLength)
	{
		Vec3f ending = m_vOrigin + m_vDirection * m_fLength;
		return (ending - _pos).GetLength();
	}

	Vec3f inter = m_vOrigin + m_vDirection * dot;
	return (inter - _pos).GetLength();
}