#include "sphere.h"

BEGIN_SCRIPT_CLASS(Sphere)
	SCRIPT_STATIC_CONSTRUCTOR2(Vec3f,Float)
END_SCRIPT_CLASS

//..

Sphere& Sphere::operator=( const Sphere& _other )
{
	m_vCenter = _other.m_vCenter;
	m_fRadius = _other.m_fRadius;

	return (*this);
}
