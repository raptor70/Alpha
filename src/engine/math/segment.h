#ifndef __SEGMENT_H__
#define __SEGMENT_H__

class Segment
{
	DECLARE_SCRIPT_CLASS_FINAL(Segment)
public :    
	Segment(): m_vOrigin(Vec3f::Zero), m_vDirection(Vec3f::ZAxis),m_fLength(1e7f) { }
	Segment( const Vec3f& _vOrigin, const Vec3f&_vDirection, Float _fLength ): m_vOrigin(_vOrigin), m_vDirection(_vDirection),m_fLength(_fLength) { }
	Segment( const Vec3f& _a, const Vec3f& _b);
	Segment( const Segment & _other ): m_vOrigin(_other.m_vOrigin), m_vDirection(_other.m_vDirection),m_fLength(_other.m_fLength) { }
	~Segment() { } 

	Float GetDistanceToPos(const Vec3f& _pos);

	Segment& operator=( const Segment& _other );

	Vec3f	m_vOrigin;
	Vec3f	m_vDirection;
	Float	m_fLength;
};

#endif