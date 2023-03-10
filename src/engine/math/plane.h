#ifndef __PLANE_H__
#define __PLANE_H__

class Plane
{
	DECLARE_SCRIPT_CLASS_FINAL(Plane)
public:
	Plane() : m_vPoint(Vec3f::Zero), m_vNormal(Vec3f::YAxis) { }
	Plane(const Vec3f& _vPoint, const Vec3f& _vNormal) : m_vPoint(_vPoint), m_vNormal(_vNormal) { }
	Plane(const Plane & _other) : m_vPoint(_other.m_vPoint), m_vNormal(_other.m_vNormal) { }
	~Plane() { }

	Bool	IntersectSegment(const Segment& _seg, Vec3f& _inter) const;

	Plane& operator=(const Plane& _other);

	Vec3f	m_vPoint;
	Vec3f	m_vNormal;
};

#endif