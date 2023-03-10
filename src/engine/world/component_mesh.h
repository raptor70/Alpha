#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "component.h"
#include "3d/mesh.h"

BEGIN_COMPONENT_CLASS(Mesh)
public:
	EntityComponentMesh();
	~EntityComponentMesh();

	virtual Bool IsUpdater() const OVERRIDE	{ return DEBUGFLAG(MeshNormal); }
	virtual Bool IsDrawer()	const OVERRIDE { return TRUE; }
	virtual Bool ReceiveEvent()	const OVERRIDE{ return FALSE; }

	virtual void	Initialize() OVERRIDE;
	virtual void	Update(Float _dTime) OVERRIDE;
	virtual void	Draw(Renderer*	_renderer) OVERRIDE;
	virtual void	OnParamChanged() OVERRIDE;

protected:
	ResourceRef		m_Mesh;
END_COMPONENT_CLASS

#endif
