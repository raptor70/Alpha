#include "editor_gizmo.h"
#ifdef USE_EDITOR

#include "input\input_manager.h"

#define TRANSLATION_SIZE	0.5f
#define TRANSLATION_CAM_OFFSET 0.15f
#define TRANSLATION_DUO_OFFSET 0.4f
#define ROTATION_NB_SEGMENT	30
#define ROTATION_RADIUS	0.5f
#define ROTATION_CAMERA_RADIUS 0.525f
#define SCALE_SIZE	0.5f;
#define SCALE_CAM_OFFSET 0.15f
#define SCALE_DUO_OFFSET 0.4f

EditorGizmo::EditorGizmo()
{
	m_bIsEnabled = FALSE;
	m_Mode = MODE_Translation;
	m_bMustDraw = FALSE;
	m_LineMaterial = RendererDatas::GetInstance().CreateMaterial();
	m_LineMaterial->LoadTexture("debug/white.png");
	m_fScreenScale = 1.f;
	m_Gizmos[MODE_Translation] = NEW CGizmoTranslation;
	m_Gizmos[MODE_Rotation] = NEW CGizmoRotation;
	m_Gizmos[MODE_Scale] = NEW CGizmoScale;
}

//..

void EditorGizmo::Update(Float _dTime)
{
	m_bMustDraw = FALSE;
	if (m_bIsEnabled)
	{
		if (m_pEntity)
		{
			m_Gizmos[m_Mode]->Update(_dTime,this);

			m_bMustDraw = TRUE;
			m_mEntityWorldMat = m_pEntity->GetWorldMatrix();
			m_qEntityWorldRot = m_pEntity->GetWorldRot();
			m_vEntityWorldPos = m_pEntity->GetWorldPos();
		}
	}
	else
	{
		m_Mode = MODE_Translation;
	}
}

//..

void EditorGizmo::Draw(Renderer*_renderer)
{
	if (m_bMustDraw)
	{
		m_CameraDir = _renderer->GetCamera()->GetDir();
		m_CameraPos = _renderer->GetCamera()->GetPos();
		Float fDist = (m_vEntityWorldPos - _renderer->GetCamera()->GetPos()).GetLength();
		m_fScreenScale = fDist / 8.f;

		_renderer->SetActiveDrawMaterial(m_LineMaterial);
		m_Gizmos[m_Mode]->Draw(_renderer,this);
	}
}

//..

Bool	EditorGizmo::IsMoving() const 
{ 
	return  (m_Mode != MODE_None && m_Gizmos[m_Mode]->IsMoving());
}

//..

CGizmoBase::CGizmoBase()
{
	m_UseAxis = AXIS_None;
	m_OverAxis = AXIS_None;
	m_bIsMoving = FALSE;
}

//--

void CGizmoBase::Update(Float _dTime, EditorGizmo* _editorGizmo)
{
	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	Vec2i curPos = mouse.GetPos();
	if (m_UseAxis == AXIS_None)
	{
		m_bIsMoving = FALSE;
	}
	else
	{
		if (curPos != m_iPreviousMousePos)
			m_bIsMoving = TRUE;
	}
	m_iPreviousMousePos = curPos;
}

//--

void CGizmoTranslation::Update(Float _dTime, EditorGizmo* _editorGizmo)
{
	SUPER::Update(_dTime, _editorGizmo);

	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
	Float fScreenScale = _editorGizmo->m_fScreenScale * TRANSLATION_SIZE;
	if (m_UseAxis == AXIS_None)
	{
		// reinit over axis
		m_OverAxis = AXIS_None;

		// mouse ray test
		Segment seg = mouse.GetWorldSegment();
		seg.m_fLength = 1e7f;
		Quat worldrot = _editorGizmo->m_pEntity->GetWorldRot();

		// TEST CAMERA AXIS
		Vec3f entityPos = _editorGizmo->m_pEntity->GetWorldPos();
		m_TranslationPlane.m_vPoint = entityPos;
		m_TranslationExcludeAxis = Vec3f::Zero;
		if (seg.GetDistanceToPos(entityPos)<fScreenScale*TRANSLATION_CAM_OFFSET*.5f)
		{
			m_OverAxis = AXIS_XYZ;
			m_TranslationPlane.m_vNormal = -_editorGizmo->m_CameraDir;
			m_TranslationPlane.m_vNormal.Normalize();
		}

		// axis
		Vec3f entityXAxis = worldrot * Vec3f::XAxis;
		Vec3f entityYAxis = worldrot * Vec3f::YAxis;
		Vec3f entityZAxis = worldrot * Vec3f::ZAxis;

		// TEST DUO AXIS
		if (m_OverAxis == AXIS_None)
		{
			// SEG TO LOCAL AXIS
			struct
			{
				Plane m_Plane;
				Axis m_Axis;
				Vec3f m_AxisTest1;
				Vec3f m_AxisTest2;
			} planes[3] =
			{
				Plane(entityPos, entityZAxis), AXIS_XY,entityXAxis,entityYAxis,
				Plane(entityPos, entityXAxis), AXIS_YZ,entityYAxis,entityZAxis,
				Plane(entityPos, entityYAxis), AXIS_XZ,entityXAxis,entityZAxis
			};
			Vec3f inter = Vec3f::Zero;
			Float fMinDist = 1e7;
			for (auto plane : planes)
			{
				if (plane.m_Plane.IntersectSegment(seg, inter))
				{
					Vec3f tointer = inter - plane.m_Plane.m_vPoint;
					Float fOnAxisValue = tointer * plane.m_AxisTest1;
					if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale*TRANSLATION_DUO_OFFSET)
					{
						fOnAxisValue = tointer * plane.m_AxisTest2;
						if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale*TRANSLATION_DUO_OFFSET)
						{
							Float fDist = (inter - seg.m_vOrigin).GetLength();
							if (fDist < fMinDist)
							{
								fMinDist = fDist;
								m_OverAxis = plane.m_Axis;
								m_TranslationPlane.m_vNormal = plane.m_Plane.m_vNormal;
							}
						}
					}
				}
			}
		}

		// TEST SOLO AXIS
		if (m_OverAxis == AXIS_None)
		{
			struct
			{
				Plane m_Plane;
				Axis m_Axis;
				Vec3f m_AxisTest;
				Vec3f m_DistanceTest;
				Vec3f m_AxisToExclude;
			} planes[] =
			{
				Plane(entityPos, entityYAxis), AXIS_X,entityXAxis,entityZAxis,entityZAxis,
				Plane(entityPos, entityZAxis), AXIS_X,entityXAxis,entityYAxis,entityYAxis,
				Plane(entityPos, entityZAxis), AXIS_Y,entityYAxis,entityXAxis,entityXAxis,
				Plane(entityPos, entityXAxis), AXIS_Y,entityYAxis,entityZAxis,entityZAxis,
				Plane(entityPos, entityXAxis), AXIS_Z,entityZAxis,entityYAxis,entityYAxis,
				Plane(entityPos, entityYAxis), AXIS_Z,entityZAxis,entityXAxis,entityXAxis
			};
			Vec3f inter = Vec3f::Zero;
			Float fMinDist = 1e7;
			for (auto plane : planes)
			{
				if (plane.m_Plane.IntersectSegment(seg, inter))
				{
					Vec3f tointer = inter - plane.m_Plane.m_vPoint;
					Float fOnAxisValue = tointer * plane.m_AxisTest;
					if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale)
					{
						Float fDist = Abs(tointer * plane.m_DistanceTest);
						if (fDist < fScreenScale*0.1f && fDist < fMinDist)
						{
							fMinDist = fDist;
							m_OverAxis = plane.m_Axis;
							m_TranslationPlane.m_vNormal = plane.m_Plane.m_vNormal;
							m_TranslationExcludeAxis = plane.m_AxisToExclude;
						}
					}
				}
			}
		}

		if (mouse.IsJustPressed(MOUSE_Left))
		{
			m_UseAxis = m_OverAxis;

			// compute delta
			Segment seg = mouse.GetWorldSegment();
			Vec3f inter = Vec3f::Zero;
			if (m_TranslationPlane.IntersectSegment(seg, inter))
			{
				m_DeltaPosAtStart = (m_TranslationPlane.m_vPoint - inter);
			}
		}
	}
	else
	{
		m_OverAxis = m_UseAxis;

		// new pos
		Segment seg = mouse.GetWorldSegment();
		Vec3f inter = Vec3f::Zero;
		Bool posValid = FALSE;
		if (m_TranslationPlane.IntersectSegment(seg, inter))
		{
			inter += m_DeltaPosAtStart;
			inter -= ((inter - m_TranslationPlane.m_vPoint) * m_TranslationExcludeAxis)*m_TranslationExcludeAxis;
			_editorGizmo->m_pEntity->SetWorldPos(inter);
			posValid = TRUE;
		}

		// exit translation
		if (!mouse.IsPressed(MOUSE_Left))
		{
			m_UseAxis = AXIS_None;
			if (posValid)
				emit _editorGizmo->OnEndTranslation(m_TranslationPlane.m_vPoint, inter);
			else
				_editorGizmo->m_pEntity->SetWorldPos(m_TranslationPlane.m_vPoint);
		}

		if (keyboard.IsJustPressed(KEY_Escape) || mouse.IsJustPressed(MOUSE_Right))
		{
			m_UseAxis = AXIS_None;
			_editorGizmo->m_pEntity->SetWorldPos(m_TranslationPlane.m_vPoint);
		}
	}
}

//--

void CGizmoTranslation::Draw(Renderer*_renderer, EditorGizmo* _editorGizmo)
{
	Quat worldRot = _editorGizmo->m_qEntityWorldRot;
	Vec3f worldPos = _editorGizmo->m_vEntityWorldPos;
	Float fScreenScale = _editorGizmo->m_fScreenScale * TRANSLATION_SIZE;
	Float fAxisSize = fScreenScale;
	Float fSideOffset = fScreenScale*TRANSLATION_DUO_OFFSET;
	Vec3f xaxis = worldRot*Vec3f::XAxis;
	Vec3f yaxis = worldRot*Vec3f::YAxis;
	Vec3f zaxis = worldRot*Vec3f::ZAxis;
	Vec3f pos = worldPos;
	Vec3f posx = pos + xaxis*fSideOffset;
	Vec3f posy = pos + yaxis*fSideOffset;
	Vec3f posz = pos + zaxis*fSideOffset;

	// compute CAM axis
	Vec3f cameraDir = _renderer->GetCamera()->GetDir();
	Vec3f leftCamera = (Vec3f::YAxis ^ cameraDir);
	leftCamera.Normalize();
	Vec3f upCamera = (cameraDir ^ leftCamera);
	upCamera.Normalize();
	Float fCameraOffset = fScreenScale*TRANSLATION_CAM_OFFSET;
	Float fHalfCameraOffset = 0.5f * fCameraOffset;
	Vec3f startCameraLines = pos - leftCamera * fHalfCameraOffset - upCamera * fHalfCameraOffset;

	struct Lines
	{
		Vec3f a;
		Vec3f b;
		Color c;
		Bool selected;
	} lines[] =
	{
		pos, pos + xaxis*fAxisSize, Color::Red, FALSE,
		posx, posx + yaxis*fSideOffset, Color::Red, FALSE,
		posx, posx + zaxis*fSideOffset, Color::Red, FALSE,
		pos, pos + yaxis*fAxisSize, Color::Green, FALSE,
		posy, posy + xaxis*fSideOffset, Color::Green, FALSE,
		posy, posy + zaxis*fSideOffset, Color::Green, FALSE,
		pos, pos + zaxis*fAxisSize, Color::Blue, FALSE,
		posz, posz + xaxis*fSideOffset, Color::Blue, FALSE,
		posz, posz + yaxis*fSideOffset, Color::Blue, FALSE,
		startCameraLines,startCameraLines + upCamera*fCameraOffset,Color::Grey, FALSE,
		startCameraLines,startCameraLines + leftCamera*fCameraOffset,Color::Grey, FALSE,
		startCameraLines + leftCamera*fCameraOffset, startCameraLines + (leftCamera + upCamera) * fCameraOffset,Color::Grey, FALSE,
		startCameraLines + upCamera*fCameraOffset, startCameraLines + (leftCamera + upCamera) * fCameraOffset,Color::Grey, FALSE,
	};

	switch (m_OverAxis)
	{
	case AXIS_X:
		lines[0].selected = TRUE;
		break;
	case AXIS_Y:
		lines[3].selected = TRUE;
		break;
	case AXIS_Z:
		lines[6].selected = TRUE;
		break;
	case AXIS_XY:
		lines[0].selected = TRUE;
		lines[1].selected = TRUE;
		lines[3].selected = TRUE;
		lines[4].selected = TRUE;
		break;
	case AXIS_YZ:
		lines[3].selected = TRUE;
		lines[5].selected = TRUE;
		lines[6].selected = TRUE;
		lines[8].selected = TRUE;
		break;
	case AXIS_XZ:
		lines[0].selected = TRUE;
		lines[2].selected = TRUE;
		lines[6].selected = TRUE;
		lines[7].selected = TRUE;
		break;
	case AXIS_XYZ:
		lines[0].selected = TRUE;
		lines[1].selected = TRUE;
		lines[2].selected = TRUE;
		lines[3].selected = TRUE;
		lines[4].selected = TRUE;
		lines[5].selected = TRUE;
		lines[6].selected = TRUE;
		lines[7].selected = TRUE;
		lines[8].selected = TRUE;
		break;
	}

	for (U32 l = 0; l<_countof(lines); l++)
	{
		Lines& line = lines[l];
		_editorGizmo->m_LineMaterial->SetDiffuseColor(line.selected ? Color::Yellow : line.c);
		_renderer->DrawLine(line.a, line.b);
	}
}

//--

void CGizmoRotation::Update(Float _dTime, EditorGizmo* _editorGizmo)
{
	SUPER::Update(_dTime, _editorGizmo);

	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
	if (m_UseAxis == AXIS_None)
	{
		// reinit over axis
		m_OverAxis = AXIS_None;

		// planes
		Segment seg = mouse.GetWorldSegment();
		seg.m_fLength = 1e7f;
		Vec3f entityPos = _editorGizmo->m_pEntity->GetWorldPos();
		Quat worldrot = _editorGizmo->m_pEntity->GetWorldRot();
		Vec3f entityXAxis = worldrot * Vec3f::XAxis;
		Vec3f entityYAxis = worldrot * Vec3f::YAxis;
		Vec3f entityZAxis = worldrot * Vec3f::ZAxis;
		Vec3f cameraDir = (_editorGizmo->m_CameraDir);
		Vec3f cameraPos = (_editorGizmo->m_CameraPos);
		Float fScreenScale = _editorGizmo->m_fScreenScale;
		Float fRadius = ROTATION_RADIUS*fScreenScale;
		Float fCameraRadius = ROTATION_CAMERA_RADIUS*fScreenScale;

		struct
		{
			Plane m_Plane;
			Float m_fRadius;
			Axis m_Axis;
		} planes[4] =
		{
			Plane(entityPos,entityXAxis),fRadius,AXIS_X,
			Plane(entityPos,entityYAxis),fRadius,AXIS_Y,
			Plane(entityPos,entityZAxis),fRadius,AXIS_Z,
			Plane(entityPos,cameraDir),fCameraRadius,AXIS_XYZ
		};

		Float fBestDist = 1e7f;
		for (auto& plane : planes)
		{
			Vec3f inter = Vec3f::Zero;
			if (plane.m_Plane.IntersectSegment(seg, inter))
			{
				Vec3f fromEntity = (inter - entityPos);
				Float distToEntity = fromEntity.GetLength();
				if (Abs(distToEntity - plane.m_fRadius) < _editorGizmo->m_fScreenScale*0.1f)
				{
					Float distToCamera = (inter- cameraPos).GetLength();
					if (distToCamera < fBestDist)
					{
						fBestDist = distToCamera;
						m_OverAxis = plane.m_Axis;
						m_RotationPlane = plane.m_Plane;
						m_RotationDirRef = fromEntity;
					}
				}
			}
		}

		if (mouse.IsJustPressed(MOUSE_Left))
		{
			m_UseAxis = m_OverAxis;
			m_qRotAtStart = _editorGizmo->m_pEntity->GetRot();
		}
	}
	else
	{
		m_OverAxis = m_UseAxis;

		// new pos
		Segment seg = mouse.GetWorldSegment();
		seg.m_fLength = 1e7f;
		Vec3f inter = Vec3f::Zero;
		Bool posValid = FALSE;
		Quat deltaRot = Quat::Identity;
		if (m_RotationPlane.IntersectSegment(seg, inter))
		{
			Vec3f curDir = inter - _editorGizmo->m_pEntity->GetWorldPos();
			deltaRot = Quat(m_RotationDirRef, curDir);
			_editorGizmo->m_pEntity->SetRot(deltaRot*m_qRotAtStart);
			posValid = TRUE;
		}

		// exit translation
		if (!mouse.IsPressed(MOUSE_Left))
		{
			m_UseAxis = AXIS_None;
			if (posValid)
				emit _editorGizmo->OnEndRotation(m_qRotAtStart, deltaRot*m_qRotAtStart);
			else
				_editorGizmo->m_pEntity->SetRot(m_qRotAtStart);
		}

		if (keyboard.IsJustPressed(KEY_Escape) || mouse.IsJustPressed(MOUSE_Right))
		{
			m_UseAxis = AXIS_None;
			_editorGizmo->m_pEntity->SetRot(m_qRotAtStart);
		}
	}
}

//--

void CGizmoRotation::Draw(Renderer*_renderer, EditorGizmo* _editorGizmo)
{
	Vec3f pos = _editorGizmo->m_vEntityWorldPos;
	Quat worldRot = _editorGizmo->m_qEntityWorldRot;
	Vec3f xaxis = worldRot*Vec3f::XAxis;
	Vec3f yaxis = worldRot*Vec3f::YAxis;
	Vec3f zaxis = worldRot*Vec3f::ZAxis;
	Float deltaAngle = MATH_2_PI / ROTATION_NB_SEGMENT;

	// compute CAM axis
	Vec3f cameraDir = _renderer->GetCamera()->GetDir();
	Vec3f leftCamera = (Vec3f::YAxis ^ cameraDir);
	leftCamera.Normalize();
	Vec3f upCamera = (cameraDir ^ leftCamera);
	upCamera.Normalize();

	Float fScreenScale = _editorGizmo->m_fScreenScale;
	Float fRadius = ROTATION_RADIUS*fScreenScale;
	Float fCameraRadius = ROTATION_CAMERA_RADIUS*fScreenScale;

	struct Circles
	{
		Vec3f m_RotateAxis;
		Vec3f m_ZeroAxis;
		Color m_Color;
		Bool m_Selected;

	} circles[] =
	{
		xaxis,zaxis*fRadius,Color::Red,FALSE,
		yaxis,xaxis*fRadius,Color::Green,FALSE,
		zaxis,yaxis*fRadius,Color::Blue,FALSE,
		cameraDir,upCamera*fCameraRadius,Color::Grey,FALSE
	};

	switch (m_OverAxis)
	{
	case AXIS_X:
		circles[0].m_Selected = TRUE;
		break;
	case AXIS_Y:
		circles[1].m_Selected = TRUE;
		break;
	case AXIS_Z:
		circles[2].m_Selected = TRUE;
		break;
	case AXIS_XYZ:
		circles[3].m_Selected = TRUE;
		break;
	}

	for (U32 c = 0; c < _countof(circles); c++)
	{
		Circles& circle = circles[c];
		for (S32 i = 0; i < ROTATION_NB_SEGMENT; i++)
		{
			Quat rota(i*deltaAngle, circle.m_RotateAxis);
			Vec3f a = pos + rota*circle.m_ZeroAxis;
			Quat rotb((i + 1)*deltaAngle, circle.m_RotateAxis);
			Vec3f b = pos + rotb*circle.m_ZeroAxis;
			_editorGizmo->m_LineMaterial->SetDiffuseColor(circle.m_Selected ? Color::Yellow : circle.m_Color);
			_renderer->DrawLine(a, b);
		}
	}
}

//--

void CGizmoScale::Update(Float _dTime, EditorGizmo* _editorGizmo)
{
	SUPER::Update(_dTime, _editorGizmo);

	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
	Float fScreenScale = _editorGizmo->m_fScreenScale * SCALE_SIZE;
	if (m_UseAxis == AXIS_None)
	{
		// reinit over axis
		m_OverAxis = AXIS_None;

		// mouse ray test
		Segment seg = mouse.GetWorldSegment();
		Quat worldrot = _editorGizmo->m_pEntity->GetWorldRot();

		// TEST CAMERA AXIS
		Vec3f entityPos = _editorGizmo->m_pEntity->GetWorldPos();
		m_ScaleSegment.m_vOrigin = entityPos;
		m_ScaleSegment.m_fLength = fScreenScale;
		m_ScalePlane.m_vPoint = entityPos;
		if (seg.GetDistanceToPos(entityPos)<fScreenScale*SCALE_CAM_OFFSET*.5f)
		{
			m_OverAxis = AXIS_XYZ;
			m_vScaleAxis = Vec3f::One;

			// compute segment
			Vec3f normal = (-_editorGizmo->m_CameraDir);
			Vec3f rightDir = Vec3f::YAxis ^ normal;
			rightDir.Normalize();
			Vec3f upDir = (normal ^ rightDir);
			upDir.Normalize();
			m_ScaleSegment.m_vDirection = (rightDir + upDir);
			m_ScaleSegment.m_vDirection.Normalize();
			m_ScalePlane.m_vNormal = normal;
		}

		// TEST DUO AXIS
		Vec3f entityXAxis = worldrot * Vec3f::XAxis;
		Vec3f entityYAxis = worldrot * Vec3f::YAxis;
		Vec3f entityZAxis = worldrot * Vec3f::ZAxis;
		if (m_OverAxis == AXIS_None)
		{
			// SEG TO LOCAL AXIS
			seg.m_fLength = 1e7f;

			struct
			{
				Plane m_Plane;
				Axis m_Axis;
				Vec3f m_AxisTest1;
				Vec3f m_AxisTest2;
				Vec3f m_ScaleAxis;
			} planes[3] =
			{
				Plane(entityPos, entityZAxis), AXIS_XY,entityXAxis,entityYAxis,Vec3f(1.f,1.f,0.f),
				Plane(entityPos, entityXAxis), AXIS_YZ,entityYAxis,entityZAxis,Vec3f(0.f,1.f,1.f),
				Plane(entityPos, entityYAxis), AXIS_XZ,entityXAxis,entityZAxis,Vec3f(1.f,0.f,1.f),
			};
			Vec3f inter = Vec3f::Zero;
			Float fMinDist = 1e7;
			for (auto plane : planes)
			{
				if (plane.m_Plane.IntersectSegment(seg, inter))
				{
					Vec3f tointer = inter - plane.m_Plane.m_vPoint;
					Float fOnAxisValue = tointer* plane.m_AxisTest1;
					if (fOnAxisValue > 0.f && fOnAxisValue < fScreenScale*SCALE_DUO_OFFSET)
					{
						fOnAxisValue += tointer * plane.m_AxisTest2;
						if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale*SCALE_DUO_OFFSET)
						{
							Float fDist = (inter - seg.m_vOrigin).GetLength();
							if (fDist < fMinDist)
							{
								fMinDist = fDist;
								m_OverAxis = plane.m_Axis;
								m_vScaleAxis = plane.m_ScaleAxis;
								m_ScaleSegment.m_vDirection = _editorGizmo->m_pEntity->GetWorldRot()*m_vScaleAxis;
								m_ScalePlane.m_vNormal = _editorGizmo->m_pEntity->GetWorldRot()*plane.m_Plane.m_vNormal;
							}
						}
					}
				}
			}
		}

		// TEST SOLO AXIS
		if (m_OverAxis == AXIS_None)
		{
			struct
			{
				Plane m_Plane;
				Axis m_Axis;
				Vec3f m_AxisTest;
				Vec3f m_DistanceTest;
				Vec3f m_AxisToExclude;
				Vec3f m_ScaleAxis;
			} planes[] =
			{
				Plane(entityPos, entityYAxis), AXIS_X,entityXAxis,entityZAxis,Vec3f::ZAxis,Vec3f::XAxis,
				Plane(entityPos, entityZAxis), AXIS_X,entityXAxis,entityYAxis,Vec3f::YAxis,Vec3f::XAxis,
				Plane(entityPos, entityZAxis), AXIS_Y,entityYAxis,entityXAxis,Vec3f::XAxis,Vec3f::YAxis,
				Plane(entityPos, entityXAxis), AXIS_Y,entityYAxis,entityZAxis,Vec3f::ZAxis,Vec3f::YAxis,
				Plane(entityPos, entityXAxis), AXIS_Z,entityZAxis,entityYAxis,Vec3f::YAxis,Vec3f::ZAxis,
				Plane(entityPos, entityYAxis), AXIS_Z,entityZAxis,entityXAxis,Vec3f::XAxis,Vec3f::ZAxis,
			};
			Vec3f inter = Vec3f::Zero;
			Float fMinDist = 1e7;
			for (auto plane : planes)
			{
				if (plane.m_Plane.IntersectSegment(seg, inter))
				{
					Vec3f tointer = inter - plane.m_Plane.m_vPoint;
					Float fOnAxisValue = tointer*plane.m_AxisTest;
					if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale)
					{
						Float fDist = Abs(tointer*plane.m_DistanceTest);
						if (fDist < fScreenScale*0.1f && fDist < fMinDist)
						{
							fMinDist = fDist;
							m_OverAxis = plane.m_Axis;
							m_vScaleAxis = plane.m_ScaleAxis;
							m_ScaleSegment.m_vDirection = _editorGizmo->m_pEntity->GetWorldRot()*m_vScaleAxis;
							m_ScalePlane.m_vNormal = _editorGizmo->m_pEntity->GetWorldRot()*plane.m_Plane.m_vNormal;
						}
					}
				}
			}
		}

		if (mouse.IsJustPressed(MOUSE_Left))
		{
			m_UseAxis = m_OverAxis;

			// compute delta
			Segment seg = mouse.GetWorldSegment();
			Vec3f inter = Vec3f::Zero;
			if (m_ScalePlane.IntersectSegment(seg, inter))
			{
				m_DeltaPosAtStart = (m_ScalePlane.m_vPoint - inter);
			}

			m_vScaleAtStart = _editorGizmo->m_pEntity->GetScale();
		}
	}
	else
	{
		m_OverAxis = m_UseAxis;

		// new pos
		Segment seg = mouse.GetWorldSegment();
		Vec3f inter = Vec3f::Zero;
		Bool posValid = FALSE;
		Vec3f newScale = m_vScaleAtStart;;
		if (m_ScalePlane.IntersectSegment(seg, inter))
		{
			inter += m_DeltaPosAtStart;
			Float dot1 = (inter - m_ScaleSegment.m_vOrigin)*m_ScaleSegment.m_vDirection;
			Float dot2 = m_ScaleSegment.m_vDirection*m_ScaleSegment.m_vDirection;
			Float fDist = dot1 / dot2;
			fDist /= m_ScaleSegment.m_fLength;
			newScale += newScale.Product(m_vScaleAxis*fDist);
			_editorGizmo->m_pEntity->SetScale(newScale);
			posValid = TRUE;
		}

		// exit translation
		if (!mouse.IsPressed(MOUSE_Left))
		{
			m_UseAxis = AXIS_None;
			if (posValid)
				emit _editorGizmo->OnEndScale(m_vScaleAtStart, newScale);
			else
				_editorGizmo->m_pEntity->SetScale(m_vScaleAtStart);
		}

		if (keyboard.IsJustPressed(KEY_Escape) || mouse.IsJustPressed(MOUSE_Right))
		{
			m_UseAxis = AXIS_None;
			_editorGizmo->m_pEntity->SetScale(m_vScaleAtStart);
		}
	}
}

//--

void CGizmoScale::Draw(Renderer*_renderer, EditorGizmo* _editorGizmo)
{
	Quat worldRot = _editorGizmo->m_qEntityWorldRot;
	Float fScreenScale = _editorGizmo->m_fScreenScale * SCALE_SIZE;
	Float fAxisSize = fScreenScale;
	Float fSideOffset = fScreenScale*SCALE_DUO_OFFSET;
	Vec3f xaxis = worldRot*Vec3f::XAxis;
	Vec3f yaxis = worldRot*Vec3f::YAxis;
	Vec3f zaxis = worldRot*Vec3f::ZAxis;
	Vec3f pos = _editorGizmo->m_vEntityWorldPos;
	Vec3f posx = pos + xaxis*fSideOffset;
	Vec3f posy = pos + yaxis*fSideOffset;
	Vec3f posz = pos + zaxis*fSideOffset;

	// compute CAM axis
	Vec3f cameraDir = _renderer->GetCamera()->GetDir();
	Vec3f leftCamera = (Vec3f::YAxis ^ cameraDir);
	leftCamera.Normalize();
	Vec3f upCamera = (cameraDir ^ leftCamera);
	upCamera.Normalize();
	Float fCameraOffset = fScreenScale*SCALE_CAM_OFFSET;
	Float fHalfCameraOffset = 0.5f * fCameraOffset;
	Vec3f startCameraLines = pos - leftCamera * fHalfCameraOffset - upCamera * fHalfCameraOffset;

	struct Lines
	{
		Vec3f a;
		Vec3f b;
		Color c;
		Bool selected;
	} lines[] =
	{
		pos, pos + xaxis*fAxisSize, Color::Red, FALSE,
		posx, posx + 0.5f*(yaxis*fSideOffset - xaxis*fSideOffset), Color::Red, FALSE,
		posx, posx + 0.5f*(zaxis*fSideOffset - xaxis*fSideOffset), Color::Red, FALSE,
		pos, pos + yaxis*fAxisSize, Color::Green, FALSE,
		posy, posy + 0.5f*(xaxis*fSideOffset - yaxis*fSideOffset), Color::Green, FALSE,
		posy, posy + 0.5f*(zaxis*fSideOffset - yaxis*fSideOffset), Color::Green, FALSE,
		pos, pos + zaxis*fAxisSize, Color::Blue, FALSE,
		posz, posz + 0.5f*(xaxis*fSideOffset - zaxis*fSideOffset), Color::Blue, FALSE,
		posz, posz + 0.5f*(yaxis*fSideOffset - zaxis*fSideOffset), Color::Blue, FALSE,
		startCameraLines,startCameraLines + leftCamera*fCameraOffset,Color::Grey, FALSE,
		startCameraLines,startCameraLines + upCamera*fCameraOffset + leftCamera*fCameraOffset*.5f,Color::Grey, FALSE,
		startCameraLines + leftCamera*fCameraOffset, startCameraLines + upCamera*fCameraOffset + leftCamera*fCameraOffset*.5f,Color::Grey, FALSE
	};

	switch (m_OverAxis)
	{
	case AXIS_X:
		lines[0].selected = TRUE;
		break;
	case AXIS_Y:
		lines[3].selected = TRUE;
		break;
	case AXIS_Z:
		lines[6].selected = TRUE;
		break;
	case AXIS_XY:
		lines[0].selected = TRUE;
		lines[1].selected = TRUE;
		lines[3].selected = TRUE;
		lines[4].selected = TRUE;
		break;
	case AXIS_YZ:
		lines[3].selected = TRUE;
		lines[5].selected = TRUE;
		lines[6].selected = TRUE;
		lines[8].selected = TRUE;
		break;
	case AXIS_XZ:
		lines[0].selected = TRUE;
		lines[2].selected = TRUE;
		lines[6].selected = TRUE;
		lines[7].selected = TRUE;
		break;
	case AXIS_XYZ:
		lines[0].selected = TRUE;
		lines[1].selected = TRUE;
		lines[2].selected = TRUE;
		lines[3].selected = TRUE;
		lines[4].selected = TRUE;
		lines[5].selected = TRUE;
		lines[6].selected = TRUE;
		lines[7].selected = TRUE;
		lines[8].selected = TRUE;
		break;
	}

	for (U32 l = 0; l<_countof(lines); l++)
	{
		Lines& line = lines[l];
		_editorGizmo->m_LineMaterial->SetDiffuseColor(line.selected ? Color::Yellow : line.c);
		_renderer->DrawLine(line.a, line.b);
	}
}

#endif

#ifdef USE_EDITOR_V2

#include "input\input_manager.h"

#define TRANSLATION_SIZE	0.5f
#define TRANSLATION_CAM_OFFSET 0.15f
#define TRANSLATION_DUO_OFFSET 0.4f
#define ROTATION_NB_SEGMENT	30
#define ROTATION_RADIUS	0.5f
#define ROTATION_CAMERA_RADIUS 0.525f
#define SCALE_SIZE	0.5f;
#define SCALE_CAM_OFFSET 0.15f
#define SCALE_DUO_OFFSET 0.4f

EditorGizmo::EditorGizmo()
{
	m_bIsEnabled = FALSE;
	m_Mode = MODE_Translation;
	m_bMustDraw = FALSE;
	m_LineMaterial = RendererDatas::GetInstance().CreateMaterial();
	m_LineMaterial->LoadTexture("debug/white.png");
	m_fScreenScale = 1.f;
	m_Gizmos[MODE_Translation] = NEW CGizmoTranslation;
	m_Gizmos[MODE_Rotation] = NEW CGizmoRotation;
	m_Gizmos[MODE_Scale] = NEW CGizmoScale;
	m_pPrimitive = Primitive3D::Create("editor_gizmo", TRUE);
}

//..

void EditorGizmo::Update(Float _dTime)
{
	m_bMustDraw = FALSE;
	if (m_bIsEnabled)
	{
		if (m_pEntity)
		{
			m_Gizmos[m_Mode]->Update(_dTime, this);

			m_bMustDraw = TRUE;
			m_mEntityWorldMat = m_pEntity->GetWorldMatrix();
			m_qEntityWorldRot = m_pEntity->GetWorldRot();
			m_vEntityWorldPos = m_pEntity->GetWorldPos();
		}
	}
	else
	{
		m_Mode = MODE_Translation;
	}
}

//..

void EditorGizmo::Draw(Renderer* _pRenderer)
{
	if (m_bMustDraw)
	{
		m_CameraDir = _pRenderer->GetCamera()->GetDir();
		m_CameraPos = _pRenderer->GetCamera()->GetPos();
		Float fDist = (m_vEntityWorldPos - _pRenderer->GetCamera()->GetPos()).GetLength();
		m_fScreenScale = fDist / 8.f;

		m_Gizmos[m_Mode]->Draw(_pRenderer, this);

		_pRenderer->SetActiveMaterial(m_LineMaterial);
		_pRenderer->DrawGeometry(
			DRAWBUFFER_Gizmo,
			Geometry_Lines,
			Vec3f::Zero,
			Quat::Identity,
			m_pPrimitive);
		_pRenderer->SetActiveMaterial(NULL);
	}
}

//..

void EditorGizmo::PrepareDraw(RendererDriver* _pDriver)
{
	m_pPrimitive->PrepareDraw();
}

//..

Bool	EditorGizmo::IsMoving() const
{
	return  (m_Mode != MODE_None && m_Gizmos[m_Mode]->IsMoving());
}

//..

CGizmoBase::CGizmoBase()
{
	m_UseAxis = AXIS_None;
	m_OverAxis = AXIS_None;
	m_bIsMoving = FALSE;
}

//--

void CGizmoBase::Update(Float _dTime, EditorGizmo* _editorGizmo)
{
	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	Vec2i curPos = mouse.GetPos();
	if (m_UseAxis == AXIS_None)
	{
		m_bIsMoving = FALSE;
	}
	else
	{
		if (curPos != m_iPreviousMousePos)
			m_bIsMoving = TRUE;
	}
	m_iPreviousMousePos = curPos;
}

//--

void CGizmoTranslation::Update(Float _dTime, EditorGizmo* _editorGizmo)
{
	SUPER::Update(_dTime, _editorGizmo);

	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
	Float fScreenScale = _editorGizmo->m_fScreenScale * TRANSLATION_SIZE;
	if (m_UseAxis == AXIS_None)
	{
		// reinit over axis
		m_OverAxis = AXIS_None;

		// mouse ray test
		Segment seg = mouse.GetWorldSegment();
		seg.m_fLength = 1e7f;
		Quat worldrot = _editorGizmo->m_pEntity->GetWorldRot();

		// TEST CAMERA AXIS
		Vec3f entityPos = _editorGizmo->m_pEntity->GetWorldPos();
		m_TranslationPlane.m_vPoint = entityPos;
		m_TranslationExcludeAxis = Vec3f::Zero;
		if (seg.GetDistanceToPos(entityPos) < fScreenScale * TRANSLATION_CAM_OFFSET * .5f)
		{
			m_OverAxis = AXIS_XYZ;
			m_TranslationPlane.m_vNormal = -_editorGizmo->m_CameraDir;
			m_TranslationPlane.m_vNormal.Normalize();
		}

		// axis
		Vec3f entityXAxis = worldrot * Vec3f::XAxis;
		Vec3f entityYAxis = worldrot * Vec3f::YAxis;
		Vec3f entityZAxis = worldrot * Vec3f::ZAxis;

		// TEST DUO AXIS
		if (m_OverAxis == AXIS_None)
		{
			// SEG TO LOCAL AXIS
			struct
			{
				Plane m_Plane;
				Axis m_Axis;
				Vec3f m_AxisTest1;
				Vec3f m_AxisTest2;
			} planes[3] =
			{
				Plane(entityPos, entityZAxis), AXIS_XY,entityXAxis,entityYAxis,
				Plane(entityPos, entityXAxis), AXIS_YZ,entityYAxis,entityZAxis,
				Plane(entityPos, entityYAxis), AXIS_XZ,entityXAxis,entityZAxis
			};
			Vec3f inter = Vec3f::Zero;
			Float fMinDist = 1e7;
			for (auto plane : planes)
			{
				if (plane.m_Plane.IntersectSegment(seg, inter))
				{
					Vec3f tointer = inter - plane.m_Plane.m_vPoint;
					Float fOnAxisValue = tointer * plane.m_AxisTest1;
					if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale * TRANSLATION_DUO_OFFSET)
					{
						fOnAxisValue = tointer * plane.m_AxisTest2;
						if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale * TRANSLATION_DUO_OFFSET)
						{
							Float fDist = (inter - seg.m_vOrigin).GetLength();
							if (fDist < fMinDist)
							{
								fMinDist = fDist;
								m_OverAxis = plane.m_Axis;
								m_TranslationPlane.m_vNormal = plane.m_Plane.m_vNormal;
							}
						}
					}
				}
			}
		}

		// TEST SOLO AXIS
		if (m_OverAxis == AXIS_None)
		{
			struct
			{
				Plane m_Plane;
				Axis m_Axis;
				Vec3f m_AxisTest;
				Vec3f m_DistanceTest;
				Vec3f m_AxisToExclude;
			} planes[] =
			{
				Plane(entityPos, entityYAxis), AXIS_X,entityXAxis,entityZAxis,entityZAxis,
				Plane(entityPos, entityZAxis), AXIS_X,entityXAxis,entityYAxis,entityYAxis,
				Plane(entityPos, entityZAxis), AXIS_Y,entityYAxis,entityXAxis,entityXAxis,
				Plane(entityPos, entityXAxis), AXIS_Y,entityYAxis,entityZAxis,entityZAxis,
				Plane(entityPos, entityXAxis), AXIS_Z,entityZAxis,entityYAxis,entityYAxis,
				Plane(entityPos, entityYAxis), AXIS_Z,entityZAxis,entityXAxis,entityXAxis
			};
			Vec3f inter = Vec3f::Zero;
			Float fMinDist = 1e7;
			for (auto plane : planes)
			{
				if (plane.m_Plane.IntersectSegment(seg, inter))
				{
					Vec3f tointer = inter - plane.m_Plane.m_vPoint;
					Float fOnAxisValue = tointer * plane.m_AxisTest;
					if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale)
					{
						Float fDist = Abs(tointer * plane.m_DistanceTest);
						if (fDist < fScreenScale * 0.1f && fDist < fMinDist)
						{
							fMinDist = fDist;
							m_OverAxis = plane.m_Axis;
							m_TranslationPlane.m_vNormal = plane.m_Plane.m_vNormal;
							m_TranslationExcludeAxis = plane.m_AxisToExclude;
						}
					}
				}
			}
		}

		if (mouse.IsJustPressed(MOUSE_Left))
		{
			m_UseAxis = m_OverAxis;

			// compute delta
			Segment seg = mouse.GetWorldSegment();
			Vec3f inter = Vec3f::Zero;
			if (m_TranslationPlane.IntersectSegment(seg, inter))
			{
				m_DeltaPosAtStart = (m_TranslationPlane.m_vPoint - inter);
			}
		}
	}
	else
	{
		m_OverAxis = m_UseAxis;

		// new pos
		Segment seg = mouse.GetWorldSegment();
		Vec3f inter = Vec3f::Zero;
		Bool posValid = FALSE;
		if (m_TranslationPlane.IntersectSegment(seg, inter))
		{
			inter += m_DeltaPosAtStart;
			inter -= ((inter - m_TranslationPlane.m_vPoint) * m_TranslationExcludeAxis) * m_TranslationExcludeAxis;
			_editorGizmo->m_pEntity->SetWorldPos(inter);
			posValid = TRUE;
		}

		// exit translation
		if (!mouse.IsPressed(MOUSE_Left))
		{
			m_UseAxis = AXIS_None;
			if (posValid)
			{
				//_editorGizmo->OnEndTranslation(m_TranslationPlane.m_vPoint, inter);
			}
			else
				_editorGizmo->m_pEntity->SetWorldPos(m_TranslationPlane.m_vPoint);
		}

		if (keyboard.IsJustPressed(KEY_Escape) || mouse.IsJustPressed(MOUSE_Right))
		{
			m_UseAxis = AXIS_None;
			_editorGizmo->m_pEntity->SetWorldPos(m_TranslationPlane.m_vPoint);
		}
	}
}

//--

void CGizmoTranslation::Draw(Renderer* _renderer, EditorGizmo* _editorGizmo)
{
	Quat worldRot = _editorGizmo->m_qEntityWorldRot;
	Vec3f worldPos = _editorGizmo->m_vEntityWorldPos;
	Float fScreenScale = _editorGizmo->m_fScreenScale * TRANSLATION_SIZE;
	Float fAxisSize = fScreenScale;
	Float fSideOffset = fScreenScale * TRANSLATION_DUO_OFFSET;
	Vec3f xaxis = worldRot * Vec3f::XAxis;
	Vec3f yaxis = worldRot * Vec3f::YAxis;
	Vec3f zaxis = worldRot * Vec3f::ZAxis;
	Vec3f pos = worldPos;
	Vec3f posx = pos + xaxis * fSideOffset;
	Vec3f posy = pos + yaxis * fSideOffset;
	Vec3f posz = pos + zaxis * fSideOffset;

	// compute CAM axis
	Vec3f cameraDir = _renderer->GetCamera()->GetDir();
	Vec3f leftCamera = (Vec3f::YAxis ^ cameraDir);
	leftCamera.Normalize();
	Vec3f upCamera = (cameraDir ^ leftCamera);
	upCamera.Normalize();
	Float fCameraOffset = fScreenScale * TRANSLATION_CAM_OFFSET;
	Float fHalfCameraOffset = 0.5f * fCameraOffset;
	Vec3f startCameraLines = pos - leftCamera * fHalfCameraOffset - upCamera * fHalfCameraOffset;

	struct Lines
	{
		Vec3f a;
		Vec3f b;
		Color c;
		Bool selected;
	} lines[] =
	{
		pos, pos + xaxis * fAxisSize, Color::Red, FALSE,
		posx, posx + yaxis * fSideOffset, Color::Red, FALSE,
		posx, posx + zaxis * fSideOffset, Color::Red, FALSE,
		pos, pos + yaxis * fAxisSize, Color::Green, FALSE,
		posy, posy + xaxis * fSideOffset, Color::Green, FALSE,
		posy, posy + zaxis * fSideOffset, Color::Green, FALSE,
		pos, pos + zaxis * fAxisSize, Color::Blue, FALSE,
		posz, posz + xaxis * fSideOffset, Color::Blue, FALSE,
		posz, posz + yaxis * fSideOffset, Color::Blue, FALSE,
		startCameraLines,startCameraLines + upCamera * fCameraOffset,Color::Grey, FALSE,
		startCameraLines,startCameraLines + leftCamera * fCameraOffset,Color::Grey, FALSE,
		startCameraLines + leftCamera * fCameraOffset, startCameraLines + (leftCamera + upCamera) * fCameraOffset,Color::Grey, FALSE,
		startCameraLines + upCamera * fCameraOffset, startCameraLines + (leftCamera + upCamera) * fCameraOffset,Color::Grey, FALSE,
	};

	switch (m_OverAxis)
	{
	case AXIS_X:
		lines[0].selected = TRUE;
		break;
	case AXIS_Y:
		lines[3].selected = TRUE;
		break;
	case AXIS_Z:
		lines[6].selected = TRUE;
		break;
	case AXIS_XY:
		lines[0].selected = TRUE;
		lines[1].selected = TRUE;
		lines[3].selected = TRUE;
		lines[4].selected = TRUE;
		break;
	case AXIS_YZ:
		lines[3].selected = TRUE;
		lines[5].selected = TRUE;
		lines[6].selected = TRUE;
		lines[8].selected = TRUE;
		break;
	case AXIS_XZ:
		lines[0].selected = TRUE;
		lines[2].selected = TRUE;
		lines[6].selected = TRUE;
		lines[7].selected = TRUE;
		break;
	case AXIS_XYZ:
		lines[0].selected = TRUE;
		lines[1].selected = TRUE;
		lines[2].selected = TRUE;
		lines[3].selected = TRUE;
		lines[4].selected = TRUE;
		lines[5].selected = TRUE;
		lines[6].selected = TRUE;
		lines[7].selected = TRUE;
		lines[8].selected = TRUE;
		break;
	}
	
	// set primitive
	VertexBufferRef vb;
	IndexBufferRef ib;
	_editorGizmo->m_pPrimitive->Lock(vb, ib);
	vb->SetVertexCount(_countof(lines)*2);
	ib->m_Array.SetItemCount(_countof(lines) * 2);
	U16 idx = 0;
	for (U32 l = 0; l < _countof(lines); l++)
	{
		Lines& line = lines[l];
		Color col = line.selected ? Color::Yellow : line.c;
		vb->SetVertexAtIndex(
			idx,
			line.a,
			Vec2f::Zero,
			Vec3f::Zero,
			col
		);
		ib->m_Array[idx] = idx;
		idx++;

		vb->SetVertexAtIndex(
			idx,
			line.b,
			Vec2f::Zero,
			Vec3f::Zero,
			col
		);
		ib->m_Array[idx] = idx;
		idx++;
	}
	_editorGizmo->m_pPrimitive->Unlock();
}

//--

void CGizmoRotation::Update(Float _dTime, EditorGizmo* _editorGizmo)
{
	SUPER::Update(_dTime, _editorGizmo);

	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
	if (m_UseAxis == AXIS_None)
	{
		// reinit over axis
		m_OverAxis = AXIS_None;

		// planes
		Segment seg = mouse.GetWorldSegment();
		seg.m_fLength = 1e7f;
		Vec3f entityPos = _editorGizmo->m_pEntity->GetWorldPos();
		Quat worldrot = _editorGizmo->m_pEntity->GetWorldRot();
		Vec3f entityXAxis = worldrot * Vec3f::XAxis;
		Vec3f entityYAxis = worldrot * Vec3f::YAxis;
		Vec3f entityZAxis = worldrot * Vec3f::ZAxis;
		Vec3f cameraDir = (_editorGizmo->m_CameraDir);
		Vec3f cameraPos = (_editorGizmo->m_CameraPos);
		Float fScreenScale = _editorGizmo->m_fScreenScale;
		Float fRadius = ROTATION_RADIUS * fScreenScale;
		Float fCameraRadius = ROTATION_CAMERA_RADIUS * fScreenScale;

		struct
		{
			Plane m_Plane;
			Float m_fRadius;
			Axis m_Axis;
		} planes[4] =
		{
			Plane(entityPos,entityXAxis),fRadius,AXIS_X,
			Plane(entityPos,entityYAxis),fRadius,AXIS_Y,
			Plane(entityPos,entityZAxis),fRadius,AXIS_Z,
			Plane(entityPos,cameraDir),fCameraRadius,AXIS_XYZ
		};

		Float fBestDist = 1e7f;
		for (auto& plane : planes)
		{
			Vec3f inter = Vec3f::Zero;
			if (plane.m_Plane.IntersectSegment(seg, inter))
			{
				Vec3f fromEntity = (inter - entityPos);
				Float distToEntity = fromEntity.GetLength();
				if (Abs(distToEntity - plane.m_fRadius) < _editorGizmo->m_fScreenScale * 0.1f)
				{
					Float distToCamera = (inter - cameraPos).GetLength();
					if (distToCamera < fBestDist)
					{
						fBestDist = distToCamera;
						m_OverAxis = plane.m_Axis;
						m_RotationPlane = plane.m_Plane;
						m_RotationDirRef = fromEntity;
					}
				}
			}
		}

		if (mouse.IsJustPressed(MOUSE_Left))
		{
			m_UseAxis = m_OverAxis;
			m_qRotAtStart = _editorGizmo->m_pEntity->GetRot();
		}
	}
	else
	{
		m_OverAxis = m_UseAxis;

		// new pos
		Segment seg = mouse.GetWorldSegment();
		seg.m_fLength = 1e7f;
		Vec3f inter = Vec3f::Zero;
		Bool posValid = FALSE;
		Quat deltaRot = Quat::Identity;
		if (m_RotationPlane.IntersectSegment(seg, inter))
		{
			Vec3f curDir = inter - _editorGizmo->m_pEntity->GetWorldPos();
			deltaRot = Quat(m_RotationDirRef, curDir);
			_editorGizmo->m_pEntity->SetRot(deltaRot * m_qRotAtStart);
			posValid = TRUE;
		}

		// exit translation
		if (!mouse.IsPressed(MOUSE_Left))
		{
			m_UseAxis = AXIS_None;
			if (posValid)
			{
				// _editorGizmo->OnEndRotation(m_qRotAtStart, deltaRot * m_qRotAtStart);
			}
			else
				_editorGizmo->m_pEntity->SetRot(m_qRotAtStart);
		}

		if (keyboard.IsJustPressed(KEY_Escape) || mouse.IsJustPressed(MOUSE_Right))
		{
			m_UseAxis = AXIS_None;
			_editorGizmo->m_pEntity->SetRot(m_qRotAtStart);
		}
	}
}

//--

void CGizmoRotation::Draw(Renderer* _renderer, EditorGizmo* _editorGizmo)
{
	Vec3f pos = _editorGizmo->m_vEntityWorldPos;
	Quat worldRot = _editorGizmo->m_qEntityWorldRot;
	Vec3f xaxis = worldRot * Vec3f::XAxis;
	Vec3f yaxis = worldRot * Vec3f::YAxis;
	Vec3f zaxis = worldRot * Vec3f::ZAxis;
	Float deltaAngle = MATH_2_PI / ROTATION_NB_SEGMENT;

	// compute CAM axis
	Vec3f cameraDir = _renderer->GetCamera()->GetDir();
	Vec3f leftCamera = (Vec3f::YAxis ^ cameraDir);
	leftCamera.Normalize();
	Vec3f upCamera = (cameraDir ^ leftCamera);
	upCamera.Normalize();

	Float fScreenScale = _editorGizmo->m_fScreenScale;
	Float fRadius = ROTATION_RADIUS * fScreenScale;
	Float fCameraRadius = ROTATION_CAMERA_RADIUS * fScreenScale;

	struct Circles
	{
		Vec3f m_RotateAxis;
		Vec3f m_ZeroAxis;
		Color m_Color;
		Bool m_Selected;

	} circles[] =
	{
		xaxis,zaxis * fRadius,Color::Red,FALSE,
		yaxis,xaxis * fRadius,Color::Green,FALSE,
		zaxis,yaxis * fRadius,Color::Blue,FALSE,
		cameraDir,upCamera * fCameraRadius,Color::Grey,FALSE
	};

	switch (m_OverAxis)
	{
	case AXIS_X:
		circles[0].m_Selected = TRUE;
		break;
	case AXIS_Y:
		circles[1].m_Selected = TRUE;
		break;
	case AXIS_Z:
		circles[2].m_Selected = TRUE;
		break;
	case AXIS_XYZ:
		circles[3].m_Selected = TRUE;
		break;
	}

	// set primitive
	VertexBufferRef vb;
	IndexBufferRef ib;
	_editorGizmo->m_pPrimitive->Lock(vb, ib);
	U16 nbLines = _countof(circles) * ROTATION_NB_SEGMENT;
	vb->SetVertexCount(nbLines * 2);
	ib->m_Array.SetItemCount(nbLines * 2);
	U16 idx = 0;
	for (U32 c = 0; c < _countof(circles); c++)
	{
		Circles& circle = circles[c];
		for (S32 i = 0; i < ROTATION_NB_SEGMENT; i++)
		{
			Quat rota(i * deltaAngle, circle.m_RotateAxis);
			Vec3f a = pos + rota * circle.m_ZeroAxis;
			Quat rotb((i + 1) * deltaAngle, circle.m_RotateAxis);
			Vec3f b = pos + rotb * circle.m_ZeroAxis;
			
			Color col = circle.m_Selected ? Color::Yellow : circle.m_Color;
			vb->SetVertexAtIndex(
				idx,
				a,
				Vec2f::Zero,
				Vec3f::Zero,
				col
			);
			ib->m_Array[idx] = idx;
			idx++;

			vb->SetVertexAtIndex(
				idx,
				b,
				Vec2f::Zero,
				Vec3f::Zero,
				col
			);
			ib->m_Array[idx] = idx;
			idx++;
		}
	}
	_editorGizmo->m_pPrimitive->Unlock();
}

//--

void CGizmoScale::Update(Float _dTime, EditorGizmo* _editorGizmo)
{
	SUPER::Update(_dTime, _editorGizmo);

	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
	Float fScreenScale = _editorGizmo->m_fScreenScale * SCALE_SIZE;
	if (m_UseAxis == AXIS_None)
	{
		// reinit over axis
		m_OverAxis = AXIS_None;

		// mouse ray test
		Segment seg = mouse.GetWorldSegment();
		Quat worldrot = _editorGizmo->m_pEntity->GetWorldRot();

		// TEST CAMERA AXIS
		Vec3f entityPos = _editorGizmo->m_pEntity->GetWorldPos();
		m_ScaleSegment.m_vOrigin = entityPos;
		m_ScaleSegment.m_fLength = fScreenScale;
		m_ScalePlane.m_vPoint = entityPos;
		if (seg.GetDistanceToPos(entityPos) < fScreenScale * SCALE_CAM_OFFSET * .5f)
		{
			m_OverAxis = AXIS_XYZ;
			m_vScaleAxis = Vec3f::One;

			// compute segment
			Vec3f normal = (-_editorGizmo->m_CameraDir);
			Vec3f rightDir = Vec3f::YAxis ^ normal;
			rightDir.Normalize();
			Vec3f upDir = (normal ^ rightDir);
			upDir.Normalize();
			m_ScaleSegment.m_vDirection = (rightDir + upDir);
			m_ScaleSegment.m_vDirection.Normalize();
			m_ScalePlane.m_vNormal = normal;
		}

		// TEST DUO AXIS
		Vec3f entityXAxis = worldrot * Vec3f::XAxis;
		Vec3f entityYAxis = worldrot * Vec3f::YAxis;
		Vec3f entityZAxis = worldrot * Vec3f::ZAxis;
		if (m_OverAxis == AXIS_None)
		{
			// SEG TO LOCAL AXIS
			seg.m_fLength = 1e7f;

			struct
			{
				Plane m_Plane;
				Axis m_Axis;
				Vec3f m_AxisTest1;
				Vec3f m_AxisTest2;
				Vec3f m_ScaleAxis;
			} planes[3] =
			{
				Plane(entityPos, entityZAxis), AXIS_XY,entityXAxis,entityYAxis,Vec3f(1.f,1.f,0.f),
				Plane(entityPos, entityXAxis), AXIS_YZ,entityYAxis,entityZAxis,Vec3f(0.f,1.f,1.f),
				Plane(entityPos, entityYAxis), AXIS_XZ,entityXAxis,entityZAxis,Vec3f(1.f,0.f,1.f),
			};
			Vec3f inter = Vec3f::Zero;
			Float fMinDist = 1e7;
			for (auto plane : planes)
			{
				if (plane.m_Plane.IntersectSegment(seg, inter))
				{
					Vec3f tointer = inter - plane.m_Plane.m_vPoint;
					Float fOnAxisValue = tointer * plane.m_AxisTest1;
					if (fOnAxisValue > 0.f && fOnAxisValue < fScreenScale * SCALE_DUO_OFFSET)
					{
						fOnAxisValue += tointer * plane.m_AxisTest2;
						if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale * SCALE_DUO_OFFSET)
						{
							Float fDist = (inter - seg.m_vOrigin).GetLength();
							if (fDist < fMinDist)
							{
								fMinDist = fDist;
								m_OverAxis = plane.m_Axis;
								m_vScaleAxis = plane.m_ScaleAxis;
								m_ScaleSegment.m_vDirection = _editorGizmo->m_pEntity->GetWorldRot() * m_vScaleAxis;
								m_ScalePlane.m_vNormal = _editorGizmo->m_pEntity->GetWorldRot() * plane.m_Plane.m_vNormal;
							}
						}
					}
				}
			}
		}

		// TEST SOLO AXIS
		if (m_OverAxis == AXIS_None)
		{
			struct
			{
				Plane m_Plane;
				Axis m_Axis;
				Vec3f m_AxisTest;
				Vec3f m_DistanceTest;
				Vec3f m_AxisToExclude;
				Vec3f m_ScaleAxis;
			} planes[] =
			{
				Plane(entityPos, entityYAxis), AXIS_X,entityXAxis,entityZAxis,Vec3f::ZAxis,Vec3f::XAxis,
				Plane(entityPos, entityZAxis), AXIS_X,entityXAxis,entityYAxis,Vec3f::YAxis,Vec3f::XAxis,
				Plane(entityPos, entityZAxis), AXIS_Y,entityYAxis,entityXAxis,Vec3f::XAxis,Vec3f::YAxis,
				Plane(entityPos, entityXAxis), AXIS_Y,entityYAxis,entityZAxis,Vec3f::ZAxis,Vec3f::YAxis,
				Plane(entityPos, entityXAxis), AXIS_Z,entityZAxis,entityYAxis,Vec3f::YAxis,Vec3f::ZAxis,
				Plane(entityPos, entityYAxis), AXIS_Z,entityZAxis,entityXAxis,Vec3f::XAxis,Vec3f::ZAxis,
			};
			Vec3f inter = Vec3f::Zero;
			Float fMinDist = 1e7;
			for (auto plane : planes)
			{
				if (plane.m_Plane.IntersectSegment(seg, inter))
				{
					Vec3f tointer = inter - plane.m_Plane.m_vPoint;
					Float fOnAxisValue = tointer * plane.m_AxisTest;
					if (fOnAxisValue >= 0.f && fOnAxisValue <= fScreenScale)
					{
						Float fDist = Abs(tointer * plane.m_DistanceTest);
						if (fDist < fScreenScale * 0.1f && fDist < fMinDist)
						{
							fMinDist = fDist;
							m_OverAxis = plane.m_Axis;
							m_vScaleAxis = plane.m_ScaleAxis;
							m_ScaleSegment.m_vDirection = _editorGizmo->m_pEntity->GetWorldRot() * m_vScaleAxis;
							m_ScalePlane.m_vNormal = _editorGizmo->m_pEntity->GetWorldRot() * plane.m_Plane.m_vNormal;
						}
					}
				}
			}
		}

		if (mouse.IsJustPressed(MOUSE_Left))
		{
			m_UseAxis = m_OverAxis;

			// compute delta
			Segment seg = mouse.GetWorldSegment();
			Vec3f inter = Vec3f::Zero;
			if (m_ScalePlane.IntersectSegment(seg, inter))
			{
				m_DeltaPosAtStart = (m_ScalePlane.m_vPoint - inter);
			}

			m_vScaleAtStart = _editorGizmo->m_pEntity->GetScale();
		}
	}
	else
	{
		m_OverAxis = m_UseAxis;

		// new pos
		Segment seg = mouse.GetWorldSegment();
		Vec3f inter = Vec3f::Zero;
		Bool posValid = FALSE;
		Vec3f newScale = m_vScaleAtStart;;
		if (m_ScalePlane.IntersectSegment(seg, inter))
		{
			inter += m_DeltaPosAtStart;
			Float dot1 = (inter - m_ScaleSegment.m_vOrigin) * m_ScaleSegment.m_vDirection;
			Float dot2 = m_ScaleSegment.m_vDirection * m_ScaleSegment.m_vDirection;
			Float fDist = dot1 / dot2;
			fDist /= m_ScaleSegment.m_fLength;
			newScale += newScale.Product(m_vScaleAxis * fDist);
			_editorGizmo->m_pEntity->SetScale(newScale);
			posValid = TRUE;
		}

		// exit translation
		if (!mouse.IsPressed(MOUSE_Left))
		{
			m_UseAxis = AXIS_None;
			if (posValid)
			{
				// _editorGizmo->OnEndScale(m_vScaleAtStart, newScale);
			}
			else
				_editorGizmo->m_pEntity->SetScale(m_vScaleAtStart);
		}

		if (keyboard.IsJustPressed(KEY_Escape) || mouse.IsJustPressed(MOUSE_Right))
		{
			m_UseAxis = AXIS_None;
			_editorGizmo->m_pEntity->SetScale(m_vScaleAtStart);
		}
	}
}

//--

void CGizmoScale::Draw(Renderer* _renderer, EditorGizmo* _editorGizmo)
{
	Quat worldRot = _editorGizmo->m_qEntityWorldRot;
	Float fScreenScale = _editorGizmo->m_fScreenScale * SCALE_SIZE;
	Float fAxisSize = fScreenScale;
	Float fSideOffset = fScreenScale * SCALE_DUO_OFFSET;
	Vec3f xaxis = worldRot * Vec3f::XAxis;
	Vec3f yaxis = worldRot * Vec3f::YAxis;
	Vec3f zaxis = worldRot * Vec3f::ZAxis;
	Vec3f pos = _editorGizmo->m_vEntityWorldPos;
	Vec3f posx = pos + xaxis * fSideOffset;
	Vec3f posy = pos + yaxis * fSideOffset;
	Vec3f posz = pos + zaxis * fSideOffset;

	// compute CAM axis
	Vec3f cameraDir = _renderer->GetCamera()->GetDir();
	Vec3f leftCamera = (Vec3f::YAxis ^ cameraDir);
	leftCamera.Normalize();
	Vec3f upCamera = (cameraDir ^ leftCamera);
	upCamera.Normalize();
	Float fCameraOffset = fScreenScale * SCALE_CAM_OFFSET;
	Float fHalfCameraOffset = 0.5f * fCameraOffset;
	Vec3f startCameraLines = pos - leftCamera * fHalfCameraOffset - upCamera * fHalfCameraOffset;

	struct Lines
	{
		Vec3f a;
		Vec3f b;
		Color c;
		Bool selected;
	} lines[] =
	{
		pos, pos + xaxis * fAxisSize, Color::Red, FALSE,
		posx, posx + 0.5f * (yaxis * fSideOffset - xaxis * fSideOffset), Color::Red, FALSE,
		posx, posx + 0.5f * (zaxis * fSideOffset - xaxis * fSideOffset), Color::Red, FALSE,
		pos, pos + yaxis * fAxisSize, Color::Green, FALSE,
		posy, posy + 0.5f * (xaxis * fSideOffset - yaxis * fSideOffset), Color::Green, FALSE,
		posy, posy + 0.5f * (zaxis * fSideOffset - yaxis * fSideOffset), Color::Green, FALSE,
		pos, pos + zaxis * fAxisSize, Color::Blue, FALSE,
		posz, posz + 0.5f * (xaxis * fSideOffset - zaxis * fSideOffset), Color::Blue, FALSE,
		posz, posz + 0.5f * (yaxis * fSideOffset - zaxis * fSideOffset), Color::Blue, FALSE,
		startCameraLines,startCameraLines + leftCamera * fCameraOffset,Color::Grey, FALSE,
		startCameraLines,startCameraLines + upCamera * fCameraOffset + leftCamera * fCameraOffset * .5f,Color::Grey, FALSE,
		startCameraLines + leftCamera * fCameraOffset, startCameraLines + upCamera * fCameraOffset + leftCamera * fCameraOffset * .5f,Color::Grey, FALSE
	};

	switch (m_OverAxis)
	{
	case AXIS_X:
		lines[0].selected = TRUE;
		break;
	case AXIS_Y:
		lines[3].selected = TRUE;
		break;
	case AXIS_Z:
		lines[6].selected = TRUE;
		break;
	case AXIS_XY:
		lines[0].selected = TRUE;
		lines[1].selected = TRUE;
		lines[3].selected = TRUE;
		lines[4].selected = TRUE;
		break;
	case AXIS_YZ:
		lines[3].selected = TRUE;
		lines[5].selected = TRUE;
		lines[6].selected = TRUE;
		lines[8].selected = TRUE;
		break;
	case AXIS_XZ:
		lines[0].selected = TRUE;
		lines[2].selected = TRUE;
		lines[6].selected = TRUE;
		lines[7].selected = TRUE;
		break;
	case AXIS_XYZ:
		lines[0].selected = TRUE;
		lines[1].selected = TRUE;
		lines[2].selected = TRUE;
		lines[3].selected = TRUE;
		lines[4].selected = TRUE;
		lines[5].selected = TRUE;
		lines[6].selected = TRUE;
		lines[7].selected = TRUE;
		lines[8].selected = TRUE;
		break;
	}

	// set primitive
	VertexBufferRef vb;
	IndexBufferRef ib;
	_editorGizmo->m_pPrimitive->Lock(vb, ib);
	U16 nbLines = _countof(lines);
	vb->SetVertexCount(nbLines * 2);
	ib->m_Array.SetItemCount(nbLines * 2);
	U16 idx = 0;
	for (U32 l = 0; l < _countof(lines); l++)
	{
		Lines& line = lines[l];
		Color col = line.selected ? Color::Yellow : line.c;
		vb->SetVertexAtIndex(
			idx,
			line.a,
			Vec2f::Zero,
			Vec3f::Zero,
			col
		);
		ib->m_Array[idx] = idx;
		idx++;

		vb->SetVertexAtIndex(
			idx,
			line.b,
			Vec2f::Zero,
			Vec3f::Zero,
			col
		);
		ib->m_Array[idx] = idx;
		idx++;
	}
	_editorGizmo->m_pPrimitive->Unlock();
}

#endif