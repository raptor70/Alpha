#ifndef __RENDERER_CAMERA_H__
#define __RENDERER_CAMERA_H__

class RendererCamera
{
public:
	RendererCamera();
	virtual ~RendererCamera();

	void	SetPosAndDir(const Vec3f& _pos, const Vec3f& _dir)	{ m_vPos = _pos; m_vDir = _dir; }
	void	SetPos(const Vec3f& _pos)					{ m_vPos = _pos;}
	void	SetDir(const Vec3f& _dir)					{ m_vDir = _dir; }
	void	SetOrtho(Float _height)						{ m_bIsOrtho = TRUE; m_fOrthoHeight = _height; }
	void	SetPerspective(Float _fov)					{ m_bIsOrtho = FALSE; m_fPerspectiveFov = _fov; }
	void	SetNearAndFar(Float _near, Float _far)	{ m_fNear = _near; m_fFar = _far; }

	void	ComputeProjMatrix(Float _width, Float _height, Mat44& _outProjMat);
	void	ComputeViewMatrix(Mat44& _outViewMat);
	Float	ComputeDistFromCamera(const Vec3f& _pos);

	const Vec3f& GetPos() const { return m_vPos; }
	const Vec3f& GetDir() const { return m_vDir; }
	Float	GetNear() const		{ return m_fNear; }
	Float	GetFar() const		{ return m_fFar; }

	void	CopyFrom(const RendererCamera* _other);

protected:
	Vec3f m_vPos;
	Vec3f m_vDir;
	Bool m_bIsOrtho;	// if false, perspective
	Float m_fNear;
	Float m_fFar;
	Float m_fOrthoHeight;
	Float m_fPerspectiveFov;
};

typedef RefTo<RendererCamera>	RendererCameraRef;

#endif