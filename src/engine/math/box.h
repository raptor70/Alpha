#ifndef __BOX_H__
#define __BOX_H__

class Box
{
	DECLARE_SCRIPT_CLASS_FINAL(Box)
public :    
	Box(): m_vCenter(Vec3f::Zero), m_vSize(Vec3f::One),m_qRot(Quat::Identity) { }
	Box( const Vec3f& _vSize, const Vec3f& _vCenter = Vec3f::Zero, const Quat& _qRot = Quat::Identity ): m_vCenter(_vCenter), m_vSize(_vSize),m_qRot(_qRot) { }
	Box( const Box & _other ): m_vCenter(_other.m_vCenter), m_vSize(_other.m_vSize),m_qRot(_other.m_qRot) { }
	~Box() { } 

	Bool	IntersectSegment(const Segment& _seg, Vec3f& _inter) const;

	Box& operator=( const Box& _other );

	Vec3f	m_vCenter;
	Vec3f	m_vSize;
	Quat	m_qRot;
};

#endif