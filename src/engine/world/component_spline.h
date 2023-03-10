#ifndef __COMPONENT_SPLINE_H__
#define __COMPONENT_SPLINE_H__

#include "component.h"
#include "math/spline.h"

BEGIN_COMPONENT_CLASS(Spline)
	DECLARE_SCRIPT_CLASS(EntityComponentSpline)
public:
	EntityComponentSpline();
	~EntityComponentSpline();

	virtual Bool IsUpdater() const	{ return FALSE; }
	virtual Bool IsDrawer()	const	{ return TRUE; }
	virtual Bool ReceiveEvent()	const { return FALSE; }

	virtual void	Draw(Renderer*	_renderer);

	void		SetFromPoints(S32 _type, const Vec3fArray& _points);
	const CSpline& GetSpline() const {return m_Spline;}

	void		SetThickness(Float _thickness)	{ m_fThickness = _thickness; }
	void		SetTexture(const Str& _path)	{ m_Material->LoadTexture(_path); }
	void		SetTextureLength(Float _length)	{ m_fTextureLength = _length; }

protected:
	MaterialRef	m_Material;
	Primitive3DRef	m_Primitive;
	CSpline	m_Spline;
	Float	m_fThickness;
	Float	m_fTextureLength;
END_COMPONENT_CLASS

#endif
