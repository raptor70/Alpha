#ifndef __COMPONENT_QUAD_H__
#define __COMPONENT_QUAD_H__

#include "world/component.h"
#include "3d/vertex_buffer.h"
#include "3d/index_buffer.h"

BEGIN_COMPONENT_CLASS(Quad)
	DECLARE_SCRIPT_CLASS(EntityComponentQuad)
public:
	EntityComponentQuad();
	~EntityComponentQuad();

	virtual Bool IsUpdater() const	{ return FALSE; }
	virtual Bool IsDrawer()	const	{ return TRUE; }
	virtual Bool ReceiveEvent()	const { return FALSE; }

	virtual void	Initialize();
	virtual void	Draw(Renderer*	_renderer);
	virtual void	OnParamChanged() OVERRIDE;

	void	SetColor(const Color& _color);
	void	SetIsAdditive(Bool _isAdditive);

	Vec2f GetSize() const { return Vec2f(GET_ENTITY_PARAMS(Float,QuadSizeX),GET_ENTITY_PARAMS(Float,QuadSizeY)); }

protected:
	virtual const MaterialRef& GetCurrentMaterial() const { return m_Material; }
	virtual const Primitive3DRef& GetPrimitive() const { return m_Primitive; }
	virtual void CreateGeometry();

	MaterialRef	m_Material;
	Primitive3DRef	m_Primitive;
END_COMPONENT_CLASS

BEGIN_COMPONENT_CLASS_INHERITED(MaskedQuad,Quad)
	DECLARE_SCRIPT_CLASS(EntityComponentMaskedQuad)
public:
	EntityComponentMaskedQuad();
	~EntityComponentMaskedQuad(){}

	virtual void	Initialize();

protected:
	virtual void CreateGeometry();

END_COMPONENT_CLASS

#endif
