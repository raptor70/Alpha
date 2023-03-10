#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "component.h"
#include "renderer/renderer_camera.h"

BEGIN_COMPONENT_CLASS(Camera)
	DECLARE_SCRIPT_CLASS(EntityComponentCamera)
public:
	EntityComponentCamera();
	~EntityComponentCamera();

	virtual Bool IsUpdater() const	{ return TRUE; }
	virtual Bool IsDrawer()	const	{ return FALSE; }
	virtual Bool ReceiveEvent()	const { return FALSE; }

	virtual void	Initialize() OVERRIDE;
	virtual void	OnParamChanged() OVERRIDE;
	virtual void	Update(Float _dTime) OVERRIDE;

	const RendererCameraRef& GetRendererCamera() const { return m_pRendererCamera; }

protected:
	RendererCameraRef	m_pRendererCamera;
END_COMPONENT_CLASS

#endif
