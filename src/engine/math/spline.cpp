#include "spline.h"

CSpline::CSpline()
{
	m_fLength = 0.f;
	m_Type = TYPE_Hermite;
}

//-------------------------------------

CSpline::~CSpline()
{
}

//-------------------------------------

void	CSpline::SetFromPoints(const Vec3fArray& _points)
{
	if( _points.GetCount() > 1 )
	{
		// first 
		Vec3f pos = _points[0];
		Vec3f tangent = (_points[1] - _points[0]) * .5f;
		//tangent.Normalize();
		AddPoint(pos,tangent);

		for(U32 i=1; i<_points.GetCount()-1; i++)
		{
			pos = _points[i];
			tangent = (_points[i+1] - _points[i-1]) * .5f;
			//tangent.Normalize();
			AddPoint(pos,tangent);
		}

		// last
		if( _points.GetCount() >= 3 )
		{
			U32 i = _points.GetCount() - 1;
			pos = _points[i];
			tangent = (_points[i] - _points[i-1]) * .5f;
			//tangent.Normalize();
			AddPoint(pos,tangent);
		}
	}

	UpdateLength();
}

//-------------------------------------

Vec3f	CSpline::GetPos(Float _ratio, Vec3f* _tangent /*=NULL*/) const
{
	DEBUG_Require(_ratio>=0.f && _ratio<=1.f);

	Float cumulateLength = 0.f;
	Float targetLength = _ratio * m_fLength;
	for(U32 s=0; s<m_Segments.GetCount();s++)
	{
		const Segment& seg = m_Segments[s];
		cumulateLength += m_Segments[s].m_fLength;
		if( cumulateLength >= targetLength )
		{
			Float ratio = 1.f - (cumulateLength - targetLength) / m_Segments[s].m_fLength;

			if( _tangent )
				*_tangent = GetTangentOnSegment(seg,ratio);

			return GetPosOnSegment(seg,ratio);
		}
	}

	if( m_PosArray.GetCount() )
		return m_PosArray[0];

	return Vec3f::Zero;
}

//-------------------------------------

Float	CSpline::GetDistToSpline(const Vec3f& _pos, Float _maxDist, Vec3f& _outNearestPos, Vec3f& _outNearestTangent, Float& _outRatio) const
{
	Float maxDist = _maxDist;
	if( maxDist < MATH_Eps ) maxDist = 1e7f;
	Float minDist = maxDist;
	Vec3f pos = Vec3f::Zero;
	Vec3f tgt = Vec3f::Zero;
	Float cumulateLength = 0.f;
	Float bestCumulateLength = 0.f;
	for(U32 s=0; s<m_Segments.GetCount(); s++)
	{
		const Segment& seg = m_Segments[s];

		Float nb = 100.f; //FLOOR(seg.m_fLength / maxDist) + 1.f;
		Float deltaRatio = 1.f / nb ;
		for(S32 i=0; i<nb; i++)
		{
			Vec3f newPos = GetPosOnSegment(seg,i*deltaRatio);
			Vec3f newTgt = GetTangentOnSegment(seg,i*deltaRatio);
			Float dist = (newPos - _pos).GetLength();
			if( dist < minDist )
			{
				minDist = dist;
				pos = newPos;
				tgt = newTgt;
				bestCumulateLength = cumulateLength + ((seg.m_fLength / nb) * (Float)i);
			}
		}

		cumulateLength += seg.m_fLength;
	}

	_outNearestPos = pos;
	_outNearestTangent = tgt;
	_outNearestTangent.Normalize();
	_outRatio = bestCumulateLength / m_fLength;

	return minDist;
}

//-------------------------------------

void	CSpline::AddPoint(const Vec3f& _pos, const Vec3f& _tangent)
{
	m_PosArray.AddLastItem(_pos);
	m_TangentArray.AddLastItem(_tangent);

	if( m_PosArray.GetCount() > 1 )
	{
		Segment newSeg;
		newSeg.m_Pos[0] = m_PosArray.GetCount() - 2;
		newSeg.m_Pos[1] = newSeg.m_Pos[0] + 1;
		newSeg.m_fLength = 0.f;
		
		// hermite coeff
		Vec3f& p0 = m_PosArray[newSeg.m_Pos[0]];
		Vec3f& t0 = m_TangentArray[newSeg.m_Pos[0]];
		Vec3f& p1 = m_PosArray[newSeg.m_Pos[1]];
		Vec3f& t1 = m_TangentArray[newSeg.m_Pos[1]];
		switch(m_Type)
		{
		case TYPE_Linear:
			{
				newSeg.a =  newSeg.b = 0.f;
				newSeg.c = (p1 - p0);
				newSeg.d = p0;
				break;
			}
		case TYPE_Hermite:
			{
				newSeg.a = 2.f * p0 - 2.f * p1 + t0 + t1;
				newSeg.b = - 3.f * p0 + 3.f * p1 - 2.f * t0 - t1;
				newSeg.c = t0;
				newSeg.d = p0;
				break;
			}
		}

		m_Segments.AddLastItem(newSeg);
	}
}

//-------------------------------------

void	CSpline::UpdateLength()
{
	m_fLength = 0.f;

	for(U32 s=0; s<m_Segments.GetCount(); s++)
	{
		Segment& seg = m_Segments[s];

		// compute segment length
		seg.m_fLength = 0.f;
		Vec3f previous = m_PosArray[seg.m_Pos[0]];
		Float deltaRatio = 1.f / 8.f ;
		for(U32 i=1; i<=8; i++)
		{
			Vec3f newPos = GetPosOnSegment(seg,i*deltaRatio);
			seg.m_fLength += (newPos - previous).GetLength();
			previous = newPos;
		}

		m_fLength += seg.m_fLength;
	}
}

//-------------------------------------

Vec3f	CSpline::GetPosOnSegment(const Segment& _segment, Float _ratio) const
{
	DEBUG_Require(_ratio>=0.f && _ratio<=1.f);

	// at3 + bt2 + ct + d
	return	_ratio * ( _ratio * ( _ratio * _segment.a + _segment.b ) + _segment.c ) +	_segment.d ;
}

//-------------------------------------

Vec3f	CSpline::GetTangentOnSegment(const Segment& _segment, Float _ratio) const
{
	DEBUG_Require(_ratio>=0.f && _ratio<=1.f);

	// 3at2 + 2bt + c
	return	( _ratio * ( _ratio * _segment.a * 3.f + _segment.b * 2.f ) + _segment.c );
}
