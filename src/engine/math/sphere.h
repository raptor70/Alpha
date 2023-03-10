#ifndef __SPHERE_H__
#define __SPHERE_H__

class Sphere
{
	DECLARE_SCRIPT_CLASS_FINAL(Sphere)
public :    
	Sphere(): m_vCenter(Vec3f::Zero), m_fRadius(0.f) { }
	Sphere( const Vec3f& _vCenter, Float _fRadius = 0.f ): m_vCenter(_vCenter), m_fRadius(_fRadius) { }
	Sphere( const Sphere & _other ): m_vCenter(_other.m_vCenter), m_fRadius(_other.m_fRadius) { }
	~Sphere() { }

	Sphere& operator=( const Sphere& _other );

	Vec3f	m_vCenter;
	Float	m_fRadius;
};

#endif