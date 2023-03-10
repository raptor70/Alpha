#include "renderer_camera.h"

RendererCamera::RendererCamera()
{
	m_vPos = Vec3f::Zero;
	m_vDir = -Vec3f::ZAxis;
	m_bIsOrtho = FALSE;
	m_fNear = 0.01f;
	m_fFar = 1000.f;
	m_fOrthoHeight = 20.f;
	m_fPerspectiveFov = MATH_PI_DIV_2;
}

//----------------------------------

RendererCamera::~RendererCamera()
{
}

//----------------------------------

void	RendererCamera::CopyFrom(const RendererCamera* _other)
{
	m_vPos = _other->m_vPos;
	m_vDir = _other->m_vDir;
	m_bIsOrtho = _other->m_bIsOrtho;
	m_fNear = _other->m_fNear;
	m_fFar = _other->m_fFar;
	m_fOrthoHeight = _other->m_fOrthoHeight;
	m_fPerspectiveFov = _other->m_fPerspectiveFov;
}

//----------------------------------

void	RendererCamera::ComputeProjMatrix(Float _width, Float _height, Mat44& _outProjMat)
{
	Float viewportRatio = _width/_height;
	if( m_bIsOrtho )
	{
		Float halfWidth = (m_fOrthoHeight * viewportRatio) * .5f;
		Float halfHeight = m_fOrthoHeight * .5f;
		_outProjMat.SetOrtho(-halfWidth,halfWidth,-halfHeight,halfHeight, m_fNear, m_fFar);
	}
	else
	{
		_outProjMat.SetPerspective(m_fPerspectiveFov,viewportRatio,m_fNear,m_fFar);
	}
}
	
//----------------------------------

void	RendererCamera::ComputeViewMatrix(Mat44& _outViewMat)
{
	_outViewMat.SetLookAt(m_vPos, m_vPos + m_vDir, Vec3f::YAxis);
}
	
//----------------------------------

Float	RendererCamera::ComputeDistFromCamera(const Vec3f& _pos)
{
	return (_pos - m_vPos) * m_vDir;
}