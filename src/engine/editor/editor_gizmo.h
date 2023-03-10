#ifndef __EDITOR_GIZMO_H__
#define __EDITOR_GIZMO_H__
#include "editor_include.h"
#ifdef USE_EDITOR

class Entity;
class CGizmoTranslation;
class CGizmoRotation;
class CGizmoScale;
class CGizmoBase;

BEGIN_EDITOR_CLASS(EditorGizmo,QObject)
public:
	EditorGizmo();

	enum Modes
	{
		MODE_None = -1,
		MODE_Translation,
		MODE_Rotation,
		MODE_Scale,
		MODE_Count
	};

	void Update(Float _dTime);
	void Draw(Renderer*_renderer);

	void	SetEnabled(Bool _enabled) { m_bIsEnabled = _enabled; }
	Bool	IsEnabled() const { return m_bIsEnabled; }
	Bool	IsMoving() const;

	void SetMode(const Modes& _mode) { m_Mode = _mode; }
	const Modes& GetMode() const { return m_Mode; }

	void SetEntity(Entity* _pEntity) { m_pEntity = _pEntity; }

signals:
	void OnEndTranslation(const Vec3f& _vSourcePos, const Vec3f& _vTargetPos);
	void OnEndRotation(const Quat& _vSourceRot, const Quat& _vTargetRot);
	void OnEndScale(const Vec3f& _vSourceScale, const Vec3f& _vTargetScale);

protected:
	Modes	m_Mode;
	Entity* m_pEntity;
	Vec3f m_CameraDir;
	Vec3f m_CameraPos;
	Mat44 m_mEntityWorldMat;
	Vec3f m_vEntityWorldPos;
	Quat m_qEntityWorldRot;
	Float	m_fScreenScale;
	Bool	m_bIsEnabled;
	Bool	m_bMustDraw;
	MaterialRef		m_LineMaterial;
	PtrTo<CGizmoBase>	m_Gizmos[MODE_Count];

	friend class CGizmoBase;
	friend class CGizmoTranslation;
	friend class CGizmoRotation;
	friend class CGizmoScale;
END_EDITOR_CLASS

//--

class CGizmoBase
{
public:
	CGizmoBase();
	virtual void Update(Float _dTime, EditorGizmo* _editorGizmo);
	virtual void Draw(Renderer*_renderer, EditorGizmo* _editorGizmo) = 0;
	Bool	IsMoving() const { return (m_bIsMoving); }

protected:
	enum Axis
	{
		AXIS_None = -1,
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		AXIS_XY,
		AXIS_YZ,
		AXIS_XZ,
		AXIS_XYZ
	};

	Axis	m_UseAxis;
	Axis	m_OverAxis;
	Bool	m_bIsMoving;
	Vec2i	m_iPreviousMousePos;
};

//--

BEGIN_SUPER_CLASS(CGizmoTranslation, CGizmoBase)
public:
	virtual void Update(Float _dTime, EditorGizmo* _editorGizmo) OVERRIDE;
	virtual void Draw(Renderer*_renderer, EditorGizmo* _editorGizmo) OVERRIDE;

protected:
	Plane m_TranslationPlane;
	Vec3f m_TranslationExcludeAxis;
	Vec3f m_DeltaPosAtStart;
END_SUPER_CLASS

//--

BEGIN_SUPER_CLASS(CGizmoRotation, CGizmoBase)
public:
	virtual void Update(Float _dTime, EditorGizmo* _editorGizmo) OVERRIDE;
	virtual void Draw(Renderer*_renderer, EditorGizmo* _editorGizmo) OVERRIDE;

protected:
	Plane	m_RotationPlane;
	Vec3f	m_RotationDirRef;
	Quat	m_qRotAtStart;
END_SUPER_CLASS

//--

BEGIN_SUPER_CLASS(CGizmoScale, CGizmoBase)
public:
	virtual void Update(Float _dTime, EditorGizmo* _editorGizmo) OVERRIDE;
	virtual void Draw(Renderer*_renderer, EditorGizmo* _editorGizmo) OVERRIDE;

protected:
	Plane m_ScalePlane;
	Vec3f m_vScaleAtStart;
	Segment m_ScaleSegment;
	Vec3f m_vScaleAxis;
	Vec3f m_DeltaPosAtStart;
END_SUPER_CLASS

//--

#endif // USE_EDITOR

#ifdef USE_EDITOR_V2

class Entity;
class CGizmoTranslation;
class CGizmoRotation;
class CGizmoScale;
class CGizmoBase;

class EditorGizmo
{
public:
	EditorGizmo();

	enum Modes
	{
		MODE_None = -1,
		MODE_Translation,
		MODE_Rotation,
		MODE_Scale,
		MODE_Count
	};

	void Update(Float _dTime);
	void Draw(Renderer* _renderer);
	void PrepareDraw(RendererDriver* _pDriver);

	void	SetEnabled(Bool _enabled) { m_bIsEnabled = _enabled; }
	Bool	IsEnabled() const { return m_bIsEnabled; }
	Bool	IsMoving() const;

	void SetMode(const Modes& _mode) { m_Mode = _mode; }
	const Modes& GetMode() const { return m_Mode; }

	void SetEntity(EntityPtr _pEntity) { m_pEntity = _pEntity; }

protected:
	Modes	m_Mode;
	EntityPtr m_pEntity;
	Vec3f m_CameraDir;
	Vec3f m_CameraPos;
	Mat44 m_mEntityWorldMat;
	Vec3f m_vEntityWorldPos;
	Quat m_qEntityWorldRot;
	Float	m_fScreenScale;
	Bool	m_bIsEnabled;
	Bool	m_bMustDraw;
	Primitive3DRef	m_pPrimitive;
	MaterialRef		m_LineMaterial;
	RefTo<CGizmoBase>	m_Gizmos[MODE_Count];

	friend class CGizmoBase;
	friend class CGizmoTranslation;
	friend class CGizmoRotation;
	friend class CGizmoScale;
};

//--

class CGizmoBase
{
public:
	CGizmoBase();
	virtual void Update(Float _dTime, EditorGizmo* _editorGizmo);
	virtual void Draw(Renderer* _renderer, EditorGizmo* _editorGizmo) = 0;
	Bool	IsMoving() const { return (m_bIsMoving); }

protected:
	enum Axis
	{
		AXIS_None = -1,
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		AXIS_XY,
		AXIS_YZ,
		AXIS_XZ,
		AXIS_XYZ
	};

	Axis	m_UseAxis;
	Axis	m_OverAxis;
	Bool	m_bIsMoving;
	Vec2i	m_iPreviousMousePos;
};

//--

BEGIN_SUPER_CLASS(CGizmoTranslation, CGizmoBase)
public:
	virtual void Update(Float _dTime, EditorGizmo* _editorGizmo) OVERRIDE;
	virtual void Draw(Renderer* _renderer, EditorGizmo* _editorGizmo) OVERRIDE;

protected:
	Plane m_TranslationPlane;
	Vec3f m_TranslationExcludeAxis;
	Vec3f m_DeltaPosAtStart;
END_SUPER_CLASS

//--

BEGIN_SUPER_CLASS(CGizmoRotation, CGizmoBase)
public:
	virtual void Update(Float _dTime, EditorGizmo* _editorGizmo) OVERRIDE;
	virtual void Draw(Renderer* _renderer, EditorGizmo* _editorGizmo) OVERRIDE;

protected:
	Plane	m_RotationPlane;
	Vec3f	m_RotationDirRef;
	Quat	m_qRotAtStart;
END_SUPER_CLASS

//--

BEGIN_SUPER_CLASS(CGizmoScale, CGizmoBase)
public:
	virtual void Update(Float _dTime, EditorGizmo* _editorGizmo) OVERRIDE;
	virtual void Draw(Renderer* _renderer, EditorGizmo* _editorGizmo) OVERRIDE;

protected:
	Plane m_ScalePlane;
	Vec3f m_vScaleAtStart;
	Segment m_ScaleSegment;
	Vec3f m_vScaleAxis;
	Vec3f m_DeltaPosAtStart;
END_SUPER_CLASS

#endif // USE_EDITOR_V2

#endif // __EDITOR_CAMERA_H__