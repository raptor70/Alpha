#include "editor_camera.h"
#ifdef USE_EDITOR_V2

#include "input\input_manager.h"

EditorCamera::EditorCamera()
{
	m_pRendererCamera = NEW RendererCamera;
	m_bIsEnabled = FALSE;
	m_bIsMoving = FALSE;
	m_Mode = MODE_None;
}

//..

void EditorCamera::Update(Float _dTime)
{
	if (m_bIsEnabled)
	{
		DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
		DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
		Vec2i currentPos = mouse.GetPos();
		switch (m_Mode)
		{
			case MODE_None:
			{
				m_bIsMoving = FALSE;
				if (m_bCanMove)
				{
					if (mouse.IsPressed(MOUSE_Middle))
					{
						m_Mode = MODE_Pan;
					}
					else
					{
						if (keyboard.IsPressed(KEY_Alt))
						{
							if (mouse.IsPressed(MOUSE_Left))
							{
								m_Mode = MODE_RotateAroundTarget;
							}
							else if (mouse.IsPressed(MOUSE_Right))
							{
								m_Mode = MODE_Zoom;
							}
						}
						else
						{
							if (mouse.IsPressed(MOUSE_Right))
							{
								m_Mode = MODE_Rotate;
							}
						}
					}
				}
				break;
			}

			case MODE_Pan:
			{
				Float fSpeedPerPixel = 0.05f;
				Vec3f upDelta = Vec3f::YAxis * fSpeedPerPixel * Float(currentPos.y - m_PreviousPos.y);
				Vec3f leftDir = Vec3f::YAxis ^ m_pRendererCamera->GetDir();
				Vec3f leftDelta = leftDir * fSpeedPerPixel * Float(currentPos.x - m_PreviousPos.x);
				m_pRendererCamera->SetPos(m_pRendererCamera->GetPos() + upDelta + leftDelta);

				if (!m_bCanMove || !mouse.IsPressed(MOUSE_Middle))
				{
					m_Mode = MODE_None;
				}

				if (currentPos != m_PreviousPos)
					m_bIsMoving = TRUE;

				break;
			}

			case MODE_Rotate:
			{
				Float fRotSpeedPerPixel = MATH_DEG_TO_RAD(0.2f);
				Quat deltaRotX;
				deltaRotX.SetFromAxis(fRotSpeedPerPixel*Float(currentPos.x - m_PreviousPos.x), -Vec3f::YAxis);
				Vec3f leftDir = Vec3f::YAxis ^ m_pRendererCamera->GetDir();
				Quat deltaRotY;
				deltaRotY.SetFromAxis(fRotSpeedPerPixel*Float(currentPos.y - m_PreviousPos.y), leftDir);
				m_pRendererCamera->SetDir(deltaRotX*deltaRotY*m_pRendererCamera->GetDir());

				if (!m_bCanMove || !mouse.IsPressed(MOUSE_Right))
				{
					m_Mode = MODE_None;
				}

				if (currentPos != m_PreviousPos)
					m_bIsMoving = TRUE;

				break;
			}

			case MODE_RotateAroundTarget:
			{
				Float fRotSpeedPerPixel = MATH_DEG_TO_RAD(0.2f);
				Float fTargetDist = 10.f;
				Quat deltaRotX;
				deltaRotX.SetFromAxis(fRotSpeedPerPixel*Float(currentPos.x - m_PreviousPos.x), -Vec3f::YAxis);
				Vec3f leftDir = Vec3f::YAxis ^ m_pRendererCamera->GetDir();
				Quat deltaRotY;
				deltaRotY.SetFromAxis(fRotSpeedPerPixel*Float(currentPos.y - m_PreviousPos.y), leftDir);
				m_pRendererCamera->SetPos(m_pRendererCamera->GetPos() + m_pRendererCamera->GetDir()*fTargetDist);
				m_pRendererCamera->SetDir(deltaRotX*deltaRotY*m_pRendererCamera->GetDir());
				m_pRendererCamera->SetPos(m_pRendererCamera->GetPos() - m_pRendererCamera->GetDir()*fTargetDist);

				if (!m_bCanMove || !mouse.IsPressed(MOUSE_Left))
				{
					m_Mode = MODE_None;
				}

				if (currentPos != m_PreviousPos)
					m_bIsMoving = TRUE;

				break;
			}

			case MODE_Zoom:
			{
				Float fScalePerPixel = 0.05f;
				Float fDelta = fScalePerPixel * Float(currentPos.x - m_PreviousPos.x);
				fDelta += fScalePerPixel * Float(currentPos.y - m_PreviousPos.y);
				m_pRendererCamera->SetPos(m_pRendererCamera->GetPos() + m_pRendererCamera->GetDir()*fDelta);

				if (!m_bCanMove || !mouse.IsPressed(MOUSE_Right))
				{
					m_Mode = MODE_None;
				}

				if (currentPos != m_PreviousPos)
					m_bIsMoving = TRUE;

				break;
			}
		}

		// zoom from wheel
		Float fZoomSpeed = 0.5f;
		m_pRendererCamera->SetPos(m_pRendererCamera->GetPos() + m_pRendererCamera->GetDir()*fZoomSpeed*mouse.GetWheelDelta());

		m_PreviousPos = currentPos;
	}
	else
	{
		m_Mode = MODE_None;
	}
}

#endif