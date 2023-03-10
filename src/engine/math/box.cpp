#include "box.h"

BEGIN_SCRIPT_CLASS(Box)
	SCRIPT_STATIC_CONSTRUCTOR3(Vec3f,Vec3f,Quat)
END_SCRIPT_CLASS

//..

Box& Box::operator=( const Box& _other )
{
	m_vCenter = _other.m_vCenter;
	m_vSize = _other.m_vSize;
	m_qRot = _other.m_qRot;

	return (*this);
}

Bool	Box::IntersectSegment(const Segment& _seg, Vec3f& _inter) const
{
	Quat invRot = m_qRot.GetInverse();
	Vec3f boxSegOrg = invRot *_seg.m_vOrigin; 
	Vec3f boxSegDir = invRot *_seg.m_vDirection; 
	
	Float xmin = m_vCenter.x - m_vSize.x*.5f;
	Float xmax = m_vCenter.x + m_vSize.x*.5f;
	Float ymin = m_vCenter.y - m_vSize.y*.5f;
	Float ymax = m_vCenter.y + m_vSize.y*.5f;
	Float zmin = m_vCenter.z - m_vSize.z*.5f;
	Float zmax = m_vCenter.z + m_vSize.z*.5f;

	Float factor = 0.f;
	Vec3f inter = Vec3f::Zero;

	// x axis min
	if( boxSegDir.x > 0.f )
	{
		factor = (xmin - boxSegOrg.x) / boxSegDir.x;
		if( factor < _seg.m_fLength)
		{
			inter = boxSegOrg + boxSegDir * factor;
			if( inter.y > ymin && inter.y < ymax && inter.z > zmin && inter.z < zmax )
			{
				_inter = m_qRot * inter;
				return TRUE;
			}
		}
	}

	// x axis max
	if( boxSegDir.x < 0.f )
	{
		factor = (xmax - boxSegOrg.x) / boxSegDir.x;
		if( factor < _seg.m_fLength)
		{
			inter = boxSegOrg + boxSegDir * factor;
			if( inter.y > ymin && inter.y < ymax && inter.z > zmin && inter.z < zmax )
			{
				_inter = m_qRot * inter;
				return TRUE;
			}
		}
	}

	// y axis min
	if( boxSegDir.y > 0.f )
	{
		factor = (ymin - boxSegOrg.y) / boxSegDir.y;
		if( factor < _seg.m_fLength)
		{
			inter = boxSegOrg + boxSegDir * factor;
			if( inter.x > xmin && inter.x < xmax && inter.z > zmin && inter.z < zmax )
			{
				_inter = m_qRot * inter;
				return TRUE;
			}
		}
	}

	// y axis max
	if( boxSegDir.y < 0.f )
	{
		factor = (ymax - boxSegOrg.y) / boxSegDir.y;
		if( factor < _seg.m_fLength)
		{
			inter = boxSegOrg + boxSegDir * factor;
			if( inter.x > xmin && inter.x < xmax && inter.z > zmin && inter.z < zmax )
			{
				_inter = m_qRot * inter;
				return TRUE;
			}
		}
	}

	// z axis min
	if( boxSegDir.z > 0.f )
	{
		factor = (zmin - boxSegOrg.z) / boxSegDir.z;
		if( factor < _seg.m_fLength)
		{
			inter = boxSegOrg + boxSegDir * factor;
			if( inter.x > xmin && inter.x < xmax && inter.y > ymin && inter.y < ymax )
			{
				_inter = m_qRot * inter;
				return TRUE;
			}
		}
	}

	// z axis max
	if( boxSegDir.z < 0.f )
	{
		factor = (zmax - boxSegOrg.z) / boxSegDir.z;
		if( factor < _seg.m_fLength)
		{
			inter = boxSegOrg + boxSegDir * factor;
			if( inter.x > xmin && inter.x < xmax && inter.y > ymin && inter.y < ymax )
			{
				_inter = m_qRot * inter;
				return TRUE;
			}
		}
	}

	return FALSE;
}