#include "plane.h"

BEGIN_SCRIPT_CLASS(Plane)
SCRIPT_STATIC_CONSTRUCTOR2(Vec3f, Vec3f)
END_SCRIPT_CLASS

//..

Plane& Plane::operator=(const Plane& _other)
{
	m_vPoint = _other.m_vPoint;
	m_vNormal = _other.m_vNormal;

	return (*this);
}

Bool	Plane::IntersectSegment(const Segment& _seg, Vec3f& _inter) const
{
	Float dot2 = _seg.m_vDirection * m_vNormal;
	if (Abs(dot2) > Float_Eps)
	{
		Float dot1 = (m_vPoint - _seg.m_vOrigin) * m_vNormal;
		Float dist = dot1 / dot2;
		if (dist >= 0.f && dist <= _seg.m_fLength)
		{
			_inter = _seg.m_vOrigin + _seg.m_vDirection * dist;
			return TRUE;
		}
	}

	return FALSE;
}