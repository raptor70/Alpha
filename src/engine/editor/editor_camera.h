#ifndef __EDITOR_CAMERA_H__
#define __EDITOR_CAMERA_H__
#include "editor_include.h"
#ifdef USE_EDITOR_V2

#include "renderer/renderer_camera.h"

class EditorCamera
{
public:
	EditorCamera();

	void Update(Float _dTime);

	void	SetEnabled(Bool _enabled) { m_bIsEnabled = _enabled; }
	Bool	IsEnabled() const { return m_bIsEnabled; }

	void	SetCanMove(Bool _canMove) { m_bCanMove = _canMove; }
	Bool	IsMoving() const { return m_bIsMoving; }

	const RendererCameraRef& GetRendererCamera() const { return m_pRendererCamera; }
	void CopyFromCamera(const RendererCamera* _other) { m_pRendererCamera->CopyFrom(_other); }

protected:
	enum Modes
	{
		MODE_None = -1,
		MODE_Pan,
		MODE_Zoom,
		MODE_Rotate,
		MODE_RotateAroundTarget
	};

	Modes	m_Mode;
	Vec2i	m_PreviousPos;

	RendererCameraRef	m_pRendererCamera;
	Bool	m_bIsEnabled;
	Bool	m_bCanMove;
	Bool	m_bIsMoving;
};

#endif // USE_EDITOR

#endif // __EDITOR_CAMERA_H__