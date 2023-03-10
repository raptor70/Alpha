#ifndef __SHADER_DATA_H__
#define __SHADER_DATA_H__

#include "resource/resource.h"

BEGIN_SUPER_CLASS(ShaderData, Resource)
public:
	static  const Char* GetResourcePrefix() { return "shader_data"; }
	
	virtual ~ShaderData();

	virtual void* GetDataPtr() = 0;
	virtual U32 GetDataSize() = 0;
END_SUPER_CLASS

template<class __DATASTRUCT__>
BEGIN_SUPER_CLASS(ShaderDataTpl, ShaderData)
public:
	const __DATASTRUCT__& GetData() const { return m_Data; }

	virtual void* GetDataPtr() { return &m_Data; }
	virtual U32 GetDataSize() { return sizeof(__DATASTRUCT__); }

	template<typename Visitor>
	void UpdateData(Visitor&& _visitor)
	{
		// todo : changed/dirty flag
		_visitor(m_Data);
	}
protected:
	__DATASTRUCT__ m_Data;
END_SUPER_CLASS

struct SRendererGlobal
{
	Mat44	m_maViewToShadow[3];
	Mat44	m_mClip;
	Mat44	m_mInvClip;
	Mat44	m_mFramebuffer;
	Mat44	m_mInvFramebuffer;
	Vec4f	m_vSunColor;
	Vec4f	m_vGlobalAmbient;
	Vec4f	m_vVignetteColor;
	Vec4f	m_vViewSunDir;
	Float	m_faShadowMapDistances[4];
	Vec2f	m_vResolution;
	Vec2f	m_vSSAOScreenRadius;
	Float	m_fAmbientFactor;
	Float	m_fDiffuseFactor;
	Float	m_fSpecularFactor;
	Float	m_fEmissiveFactor;
	Float	m_fShadowStrength;
	Float	m_fNoLightBoost;
	Float	m_fSSAORadius;
	Float	m_fSSAODotBias;
	Float	m_fSSAOMultiplier;
	Float	m_fBloomSize;
	S32		m_iOutlineSize;
	Float	m_fDOFMinNearPlane;
	Float	m_fDOFMaxFarPlane;
	Float	m_fDOFNearPlaneFromFocus;
	Float	m_fDOFFarPlaneFromFocus;
	Float	m_fVignetteRadius;
	Float	m_fVignetteSmoothness;
};

//-------------------------------------------------------------

struct SRendererView
{
	Mat44	m_mView = Mat44::Identity;	// world to view
	Mat44	m_mInvView = Mat44::Identity;	// view to world
	Mat44	m_mViewProj = Mat44::Identity;	// world to proj
	Mat44	m_mInvViewProj = Mat44::Identity;	// proj to world
	Mat44	m_mProj = Mat44::Identity;	// view to proj
	Mat44	m_mInvProj = Mat44::Identity;	// proj to view
	Float	m_fNear = 0.1f;
	Float	m_fFar = 1000.f;

	INLINE void	SetFromVP(const Mat44& _mView, const Mat44& _mProj)
	{
		m_mView = _mView;
		m_mInvView = _mView;
		m_mInvView.Inverse();
		m_mProj = _mProj;
		m_mInvProj = _mProj;
		m_mInvProj.Inverse();
		m_mViewProj = _mProj * _mView;
		m_mInvViewProj = m_mViewProj;
		m_mInvViewProj.Inverse();
	}
};

//---------------------------------------------------------------

struct SRendererMaterial
{
	Vec4f	m_vEmissiveColor;
	S32/*Bool*/	m_bIsFont;
};

//---------------------------------------------------------------

struct SRendererModel
{
	Mat44	m_mMVP = Mat44::Identity;	//	model to proj
	Mat44	m_mModelView = Mat44::Identity; // model to view
	Mat44	m_mModel = Mat44::Identity; // model to world
	Vec4f	m_vDiffuseColor;
	Vec4f	m_vOutlineColor;
};

//---------------------------------------------------------------

struct SRendererOmni
{
	Vec4f	m_vPos = Vec4f::Zero;
	Float	m_fIntensity = 1.f;
	Float	m_fRadius = 1.f;
};

//---------------------------------------------------------------

typedef RefTo<ShaderData>		ShaderDataRef;
typedef ArrayOf<ShaderDataRef>	ShaderDataRefArray;
typedef ShaderDataTpl<SRendererGlobal> RendererGlobalData;
typedef RefTo<RendererGlobalData> RendererGlobalDataRef;
typedef ShaderDataTpl<SRendererView> RendererViewData;
typedef RefTo<RendererViewData> RendererViewDataRef;
typedef ShaderDataTpl<SRendererMaterial> RendererMaterialData;
typedef RefTo<RendererMaterialData> RendererMaterialDataRef;
typedef ShaderDataTpl<SRendererOmni> RendererOmniData;
typedef RefTo<RendererOmniData> RendererOmniDataRef;
typedef ShaderDataTpl<SRendererModel> RendererModelData;
typedef RefTo<RendererModelData> RendererModelDataRef;

#endif