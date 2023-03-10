#ifndef __SPLINE_H__
#define __SPLINE_H__

class CSpline
{
public:
	CSpline();
	~CSpline();

	void	SetFromPoints(const Vec3fArray& _points);
	Float	GetLength() const		{ return m_fLength; }
	Vec3f	GetPos(Float _ratio, Vec3f* _tangent =NULL) const;
	Float	GetDistToSpline(const Vec3f& _pos, Float _maxDist, Vec3f& _outNearestPos, Vec3f& _outNearestTangent, Float& _outRatio) const;
	const Vec3fArray& GetPosArray() const	{ return m_PosArray; }

	enum Type
	{
		TYPE_Linear = 0,
		TYPE_Hermite
	};

	void	SetType(Type _type)		{ m_Type = _type; }

protected:
	void	AddPoint(const Vec3f& _pos, const Vec3f& _tangent);
	void	UpdateLength();

	struct Segment;
	Vec3f	GetPosOnSegment(const Segment& _segment, Float _ratio) const;
	Vec3f	GetTangentOnSegment(const Segment& _segment, Float _ratio) const;

	struct Segment
	{
		S32		m_Pos[2];
		Float	m_fLength;
		Vec3f	a,b,c,d;		// Hermite coeff

		Segment()
		{
			m_Pos[0] = m_Pos[1] = -1;
			m_fLength = 0.f;
			a = b = c = d = Vec3f::Zero;
		}
	};

	ArrayOf<Segment>	m_Segments;
	Vec3fArray			m_PosArray;
	Vec3fArray			m_TangentArray;
	Float				m_fLength;
	Type				m_Type;
};

#endif