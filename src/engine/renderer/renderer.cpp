#include "renderer.h"

#include "2d/font.h"
#include "renderer/shader.h"
#include "core/sort.h"
#include "window/window_3dscreen.h"
#include "math/spline.h"
#include "resource/resource_manager.h"
#include "3d/texture_manager.h"

#include "3d/material.h"
#include "3d/vertex_buffer.h"
#include "3d/index_buffer.h"
#include "shader_group.h"

#include "renderer_driver.h"

#include "editor\editor.h"

#define SHADOWMAP_SIZE 2048

Renderer::Renderer()
{
	m_bWireframe = FALSE;

	m_Pipeline = RendererPipeline::None;
	m_iMaxTextureSize = 4096;
	m_iViewportWidth = 1280;
	m_iViewportHeight = 720;
	m_bInitialized = FALSE;

	m_NextOutline = Color::Black;
#ifdef USE_EDITOR_V2
	m_bEditorOutline = FALSE;
	m_iEditorEntityID = 0;
#endif

	m_pUpdateDrawList =	NULL;
	m_pRenderDrawList =	NULL;
	
	m_pFrameDriver = NULL;
	m_pDriverData = NULL;
}

Bool Renderer::m_bUseMetal = FALSE;

//----------------------------------

Renderer::~Renderer()
{
}

//----------------------------------

Bool Renderer::Initialize(const Window3DScreen* _pScreen)
{
	// draw lists
	m_pUpdateDrawList = NEW Renderer::DrawList;
	m_pRenderDrawList = NEW Renderer::DrawList;

	// add fullscreen quad
	if (m_FullScreenQuad.IsNull())
	{
		m_FullScreenQuad = Primitive3D::Create("fullscreenQuad", FALSE);
		VertexBufferRef vb;
		IndexBufferRef ib;
		m_FullScreenQuad->Lock(vb, ib);
		ib->m_Array.SetEmpty();
		vb->SetVertexCount(0);
		ib->m_Array.AddLastItem(vb->AddLastVertex(Vec3f(-1.f, -1.f, 0.f), Vec2f(0.f, 1.f), Vec3f::ZAxis));
		ib->m_Array.AddLastItem(vb->AddLastVertex(Vec3f(1.f, -1.f, 0.f), Vec2f(1.f, 1.f), Vec3f::ZAxis));
		ib->m_Array.AddLastItem(vb->AddLastVertex(Vec3f(-1.f, 1.f, 0.f), Vec2f(0.f, 0.f), Vec3f::ZAxis));
		ib->m_Array.AddLastItem(vb->AddLastVertex(Vec3f(1.f, 1.f, 0.f), Vec2f(1.f, 0.f), Vec3f::ZAxis));
		m_FullScreenQuad->Unlock();
	}

	return TRUE;
}

//----------------------------------

Bool Renderer::InitializeDriver(RendererDriver* _pDriver, const Window3DScreen* _pScreen )
{
	ReloadParams(NULL);
		
	if( !_pDriver->CreateContext(_pScreen, this) )
		return FALSE;

	ChangeViewport(_pDriver, _pScreen->GetWidth(),_pScreen->GetHeight());

	m_bInitialized = TRUE;

	return TRUE;
}

//----------------------------------

Bool Renderer::ChangeViewport(RendererDriver* _pDriver, S32 _width, S32 _height)
{
	if( !_pDriver->ChangeViewport(this, _width,_height) )
		return FALSE;

	m_iViewportWidth = _width;
	m_iViewportHeight = _height;
	
	m_mCameraView = Mat44::Identity;
	m_mCameraInvView = Mat44::Identity;
	m_mCameraProj = Mat44::Identity;
	m_mCameraInvProj = Mat44::Identity;
	m_mCameraViewProj = Mat44::Identity;
	m_mCameraInvViewProj = Mat44::Identity;
	
	m_pUpdateDrawList->m_MainColor = Color::White;
	m_pRenderDrawList->m_MainColor = Color::White;
	m_NextOutline = Color::Black;

	// create framebuffers
	DestroyFrameBuffers();
	CreateFrameBuffers(_width,_height);

	return TRUE;
}

//----------------------------------

void Renderer::UpdateCameraMatrix()
{
	// no cam = default
	if (m_pCamera.IsNull())
	{
		m_pCamera = NEW RendererCamera;
		m_pCamera->SetPosAndDir(Vec3f::Zero, Vec3f::ZAxis);
		m_pCamera->SetNearAndFar(0.01f, 10000.f);
		m_pCamera->SetPerspective(MATH_PI_DIV_2);
	}

	// view & proj
	Float fWidth = (Float)m_iViewportWidth;
	Float fHeight = (Float)m_iViewportHeight;
	m_pCamera->ComputeProjMatrix(fWidth, fHeight, m_mCameraProj);
	m_pCamera->ComputeViewMatrix(m_mCameraView);

	// viewproj
	m_mCameraViewProj = m_mCameraProj * m_mCameraView;
	
	// inverses
	m_mCameraInvView = m_mCameraView;
	m_mCameraInvView.Inverse();
	m_mCameraInvProj = m_mCameraProj;
	m_mCameraInvProj.Inverse();
	m_mCameraInvViewProj = m_mCameraViewProj;
	m_mCameraInvViewProj.Inverse();
}

//----------------------------------

void Renderer::UpdateViewProjMatrix()
{
	// WORLD : YUP
	// VIEW : X = Right, Y = Up, Z = Back
	// CLIP(engine) : X = Right, Y = Up, Z = Front (-1 -1 -1) to (1 1 1)
	// CLIP(driver)
	//	- OGL = Identity (same as clip engine)
	//  - VK = transform to X = Right, Y = Down, Z = Front (-1 -1 0) to (1 1 1)
	// FRAMEBUFFER (driver)
	//	- OGL = transform to (0,0) lower left to (1,1) upper right
	//  - VK = transform to (0,0) top left to (1,1) lower right
	m_Views[RendererView_Game]->UpdateData([&](auto& _data)
		{
			_data.SetFromVP(m_mCameraView, m_mCameraProj);
			_data.m_fNear = m_pCamera->GetNear();
			_data.m_fFar = m_pCamera->GetFar();
		});
	
	// 2D View
	// (0,0,0) -> (-1,1,1)
	// (w/2,h/2,0) -> (0,0,1)
	// (w,h,0) -> (1,-1,1)
	Float fNear = -100.f;
	Float fFar = 100.f;
	Float fWidth = (Float)m_iViewportWidth;
	Float fHeight = (Float)m_iViewportHeight;
	Mat44 mProj, mView;
	mProj.SetOrtho(0.f,fWidth,fHeight,0.f, fNear, fFar);
	mView.SetLookAt(Vec3f::Zero, -Vec3f::ZAxis, Vec3f::YAxis);
	m_Views[RendererView_2D]->UpdateData([&](auto& _data)
		{
			_data.SetFromVP(mView, mProj);
			_data.m_fNear = fNear;
			_data.m_fFar = fFar;
		});
}

//----------------------------------

Float Renderer::ComputeDistFromCamera(const Vec3f& _pos)
{
	if( m_pCamera.IsValid() )
	{
		return m_pCamera->ComputeDistFromCamera(_pos);
	}

	return _pos * (-Vec3f::ZAxis);
}

//----------------------------------

Bool Renderer::FinalizeDriver(RendererDriver* _pDriver, const Window3DScreen* _pScreen)
{
	_pDriver->WaitPreviousFrame(this);
	RendererDatas::GetInstance().ManageDatas(_pDriver);
	_pDriver->DestroyContext(_pScreen, this);

	return TRUE;
}

//----------------------------------

Bool Renderer::Finalize()
{
	m_SkyboxTexture.SetNull();
	m_BlueNoise.SetNull();
	DestroyFrameBuffers();

	DELETE m_pUpdateDrawList;
	DELETE m_pRenderDrawList;

	return TRUE;
}

//----------------------------------

void Renderer::ReloadParams(RendererDriver* _pDriver)
{
	m_ParamsScript = ResourceManager::GetInstance().GetResourceFromFile<Script>("script/renderer.lua");
	if (!m_ParamsScript.IsValid())
	{
		DEBUG_Forbidden("Unable to load renderer.lua\n");
		return;
	}

	m_ParamsScript.GetCastPtr<Script>()->Call("FillParamsWithTable", m_Params);

	Vec2f uv; // useless
	TextureManager::GetInstance().GetTexture(m_Params.GetValue("SkyboxTexture")->GetStr(), m_SkyboxTexture, uv, uv);
	m_SkyboxTexture.GetCastPtr<Texture>()->SetWrapping(Texture::TEXTUREWRAP_Clamp);

	// blue noise
	TextureManager::GetInstance().GetTexture(m_Params.GetValue("BlueNoiseTexture")->GetStr(), m_BlueNoise, uv, uv);
	m_BlueNoise.GetCastPtr<Texture>()->SetWrapping(Texture::TEXTUREWRAP_Repeat);

	// force update all
	if(_pDriver)
		ChangeViewport(_pDriver, m_iViewportWidth,m_iViewportHeight);
}

//----------------------------------

void Renderer::PrepareDraw(RendererDriver* _pDriver)
{
	// auto reload script
	if (m_ParamsScript.IsValid() && m_ParamsScript->IsDirty())
	{
		ReloadParams(_pDriver);
	}

	::Swap(m_pUpdateDrawList, m_pRenderDrawList);
	for(S32 i=0; i<DRAWBUFFER_Count; i++)
		m_pUpdateDrawList->m_daDrawBuffers[i].SetEmpty();
	m_pUpdateDrawList->m_daAllDraws.SetEmpty();

	DrawGeometry(DRAWBUFFER_FullScreenQuad, Geometry_TrianglesStrip, Vec3f::Zero, Quat::Identity, m_FullScreenQuad);

	UpdateCameraMatrix();
}

//----------------------------------

void Renderer::Draw(RendererDriver* _pDriver)
{
	_pDriver->BeginFrame(this); // at start to setup context

	GPUPROFILER_Begin("FullFrame");
	// PREPARE
	m_pFrameDriver = _pDriver;
	m_pCurrentSubpass = NULL;
	GPUPROFILER_Begin("Prepare");

	GPUPROFILER_Begin("RendererData");
	RendererDatas::GetInstance().ManageDatas(_pDriver);
	GPUPROFILER_End();

	m_bWireframe = DEBUGFLAG(Wireframe);

	SortDesc(m_pRenderDrawList->m_daDrawBuffers[DRAWBUFFER_Transparent]);
	SortAsc(m_pRenderDrawList->m_daDrawBuffers[DRAWBUFFER_Opaque]);
	SortDesc(m_pRenderDrawList->m_daDrawBuffers[DRAWBUFFER_2D]);

	UpdateViewProjMatrix();
	ComputeShadowMatrices();

	// convert normal to view space
	m_pGlobalData->UpdateData([&](auto& _data)
		{
			Mat33 mat(m_Views[RendererView_Game]->GetData().m_mView);
			//mat.Inverse();
			//mat.Transpose();
			Vec3f sunDir = m_Params.GetValue("SunDir")->GetVec3f();
			sunDir = mat * sunDir;
			sunDir.Normalize();
			_data.m_vViewSunDir = Vec4f(sunDir,0.f);

			_data.m_mClip = _pDriver->GetClipSpaceMatrix();
			_data.m_mInvClip = _data.m_mClip;
			_data.m_mInvClip.Inverse();
			_data.m_mFramebuffer = _pDriver->GetFramebufferMatrix();
			_data.m_mInvFramebuffer = _data.m_mFramebuffer;
			_data.m_mInvFramebuffer.Inverse();
		});
	
	_pDriver->PushShaderData(m_pGlobalData);
	for (auto& v : m_Views)
		_pDriver->PushShaderData(v);

	GPUPROFILER_End();

	GPUPROFILER_Begin("Passes");
	for (RendererPass* pPass : m_Passes)
		DoPass(pPass);
	GPUPROFILER_End();

	// post rendering
	if (m_vEditorPickingRequest != Vec2i::Zero)
	{
		Vec2i screenPos = m_vEditorPickingRequest;
		screenPos.y = m_iViewportHeight - screenPos.y;
		m_iEditorPickingResult = m_pFrameDriver->ReadPixel(m_EditorPickerPass->m_pFrameBuffer, screenPos);
		m_vEditorPickingRequest = Vec2i::Zero;
	}
	
	GPUPROFILER_Begin("EndFrame");
	_pDriver->EndFrame();
	GPUPROFILER_End();

	m_pFrameDriver = NULL;
	GPUPROFILER_End();
}

//----------------------------------

void Renderer::Swap(RendererDriver* _pDriver)
{
	GPUPROFILER_Begin("Swap");
	_pDriver->Swap(this);
	GPUPROFILER_End();
}

//----------------------------------

void Renderer::DoPass(RendererPass* _pPass)
{
	GPUPROFILER_Begin(_pPass->GetBaseName().GetStr().GetArray());
	m_pFrameDriver->BeginPass(this, _pPass);
	for (RendererSubpass* subpass : _pPass->m_Subpasses)
	{
		GPUPROFILER_Begin(subpass->GetBaseName().GetStr().GetArray());
		m_pFrameDriver->BeginSubPass(subpass);
		m_pCurrentSubpass = subpass;

		for (auto& slot : subpass->m_TextureSlots)
		{
			if(slot.m_pTexture.IsValid())
				m_pFrameDriver->ActiveTexture(slot.m_iSlot, slot.m_ShaderParam, slot.m_pTexture);
		}
		
		m_CurrentType = subpass->m_ViewType;
		m_pFrameDriver->UseShaderData(ShaderGroup::UNIFORMDATA_GlobalData, m_pGlobalData, 0);
		m_pFrameDriver->UseShaderData(ShaderGroup::UNIFORMDATA_ViewData, m_Views[subpass->m_ViewType], 1);

		if (subpass->m_DrawBuffer != DRAWBUFFER_None)
		{
			switch (subpass->m_DrawBufferMode)
			{
			case DRAWBUFFERMODE_NoMaterial: DrawBufferNoMaterial(subpass->m_DrawBuffer); break;
			case DRAWBUFFERMODE_Picking :	DrawBufferPicking(subpass->m_DrawBuffer); break;
			default:						DrawBuffer(subpass->m_DrawBuffer); break;
			}
		}

		m_pCurrentSubpass = NULL;
		m_pFrameDriver->EndSubPass();
		GPUPROFILER_End();
	}
	m_pFrameDriver->EndPass();
	GPUPROFILER_End();
}

//----------------------------------

void Renderer::SetActiveMaterial(const MaterialRef& _pMaterial, const Color& _mainColor)
{
	m_pUpdateDrawList->m_pMaterial = _pMaterial.GetPtr();
	m_pUpdateDrawList->m_MainColor = _mainColor;
}

//----------------------------------

void Renderer::SetActiveDrawMaterial(const Material* _pMaterial)
{
	m_pRenderDrawList->m_pMaterial = _pMaterial;
}

//----------------------------------

void Renderer::SetActiveOutlineValue(const Color& _value)
{
	m_NextOutline = _value;
}

//----------------------------------

void Renderer::BeginActiveMaterial()
{
	const Material* pMaterial = m_pRenderDrawList->m_pMaterial;
	if (pMaterial)
	{
		Texture* pEmissiveTexture = pMaterial->GetEmissiveTexture();
		if (!pEmissiveTexture)
			pEmissiveTexture = m_DefaultEmissive.GetCastPtr<Texture>();

		if (pMaterial->IsAdditive())
			m_pFrameDriver->EnableAdditive();
		else
			m_pFrameDriver->DisableAdditive();

		// todo : update only when changed
		((Material*)pMaterial)->UpdateShaderData();
		m_pFrameDriver->PushShaderData(pMaterial->GetShaderData());
		m_pFrameDriver->UseShaderData(ShaderGroup::UNIFORMDATA_MaterialData, pMaterial->GetShaderData(), 3);

		DEBUG_Require(m_pCurrentSubpass->m_Slots[ShaderGroup::UNIFORM_sTexture] != -1);
		DEBUG_Require(m_pCurrentSubpass->m_Slots[ShaderGroup::UNIFORM_sEmissiveTexture] != -1);
		m_pFrameDriver->ActiveTexture(m_pCurrentSubpass->m_Slots[ShaderGroup::UNIFORM_sTexture], ShaderGroup::UNIFORM_sTexture, pMaterial->GetTexture());
		m_pFrameDriver->ActiveTexture(m_pCurrentSubpass->m_Slots[ShaderGroup::UNIFORM_sEmissiveTexture], ShaderGroup::UNIFORM_sEmissiveTexture, pEmissiveTexture);
	}
}

//----------------------------------

void Renderer::EndActiveMaterial()
{
}

//----------------------------------

Renderer::SDrawPtr Renderer::CreateDraw()
{
	ArrayOf<SDraw>* pAllDrawArray = &(m_pUpdateDrawList->m_daAllDraws);
	U32 index = pAllDrawArray->GetCount();
	pAllDrawArray->AddLastItem();
	return SDrawPtr(index, pAllDrawArray);
}

//----------------------------------

void	Renderer::PushDraw(DrawBufferType _type, const Renderer::SDrawPtr& _pDraw)
{
	m_pUpdateDrawList->m_daDrawBuffers[_type].AddLastItem(_pDraw);
}

//----------------------------------

void Renderer::DrawGeometry(GeometryType _type, const Vec3f& _pos, const Quat& _rot, const Primitive3DRef& _primitive, const Vec3f& _scale /*= Vec3f::One*/, Bool _is2D /*=FALSE*/)
{
	DrawBufferType bufferType = DRAWBUFFER_Opaque;
	if (_is2D)
		bufferType = DRAWBUFFER_2D;
	else if (m_pUpdateDrawList->m_pMaterial && m_pUpdateDrawList->m_pMaterial->IsTransparent())
		bufferType = DRAWBUFFER_Transparent;

	DrawGeometry(bufferType, _type, _pos, _rot, _primitive, _scale, _is2D);
}

//----------------------------------

void Renderer::DrawGeometry(DrawBufferType _drawType, GeometryType _type, const Vec3f& _pos, const Quat& _rot, const Primitive3DRef& _primitive, const Vec3f& _scale /*= Vec3f::One*/, Bool _is2D /*=FALSE*/)
{

	DEBUG_Require(_primitive.IsValid());
	SDrawPtr drawPtr = CreateDraw();
	SDraw& draw = *drawPtr;
	draw.m_mModelMatrix.SetTRS(_pos,_rot,_scale);
	draw.m_Primitive = _primitive;
	draw.m_pMaterial = m_pUpdateDrawList->m_pMaterial;
	draw.m_MainColor = m_pUpdateDrawList->m_MainColor;
	draw.m_fDistToCam = ComputeDistFromCamera(_pos);
	draw.m_Type = _type;
#ifdef USE_EDITOR_V2
	draw.m_EntityColor = Color(m_iEditorEntityID);
#endif

	// 2d
	if (_is2D)
		draw.m_fDistToCam = _pos.z;
	
	// outline
	Bool bHasOutline = (m_NextOutline != Color::Black);
	draw.m_OutlineValue = m_NextOutline;
		
	PushDraw(_drawType,drawPtr);

	// outline
	if (bHasOutline)
	{
		PushDraw(DRAWBUFFER_Outline, drawPtr);
	}

#ifdef USE_EDITOR_V2
	// editor outline
	if (m_bEditorOutline)
	{
		SDrawPtr otherdrawPtr = CreateDraw();
		SDraw& otherDraw = *otherdrawPtr;
		otherDraw = draw;
		otherDraw.m_OutlineValue = Color::Yellow;
		if(_is2D)
			PushDraw(DRAWBUFFER_EditorOutline2D, otherdrawPtr);
		else
			PushDraw(DRAWBUFFER_EditorOutline, otherdrawPtr);
	}
#endif
}

//----------------------------------

void Renderer::DrawOmni(const Vec3f& _worldPos, Float _fWorldRadius, const Color& _color, const RendererOmniDataRef& _pData)
{
	SDrawPtr drawPtr = CreateDraw();
	SDraw& draw = *drawPtr;

	draw.m_mModelMatrix.SetTRS(_worldPos, Quat::Identity, Vec3f(_fWorldRadius));
	draw.m_Primitive = m_FullScreenQuad;
	draw.m_MainColor = _color;
	draw.m_Type = Geometry_TrianglesStrip;
	PushDraw(DRAWBUFFER_Omni, drawPtr);
}

//----------------------------------

void Renderer::DrawBuffer(const DrawBufferType& _buffer)
{
	m_pFrameDriver->SetWireframe(m_bWireframe);
	SetActiveDrawMaterial(NULL);
	BeginActiveMaterial();
	for (U32 i = 0; i < m_pRenderDrawList->m_daDrawBuffers[_buffer].GetCount(); i++)
	{
		SDraw& draw = *(m_pRenderDrawList->m_daDrawBuffers[_buffer][i]);
		if (m_pRenderDrawList->m_pMaterial != draw.m_pMaterial)
		{
			EndActiveMaterial();
			SetActiveDrawMaterial(draw.m_pMaterial);
			BeginActiveMaterial();
		}

		if (draw.m_bClipped)
			m_pFrameDriver->EnableClipRect(this,draw.m_ClipRect);

		PushDrawModelData(draw, draw.m_MainColor);
		
		U32 indiceCount = draw.m_Primitive->GetIB()->m_Array.GetCount();
		if (draw.m_iIndiceCount > 0)
			indiceCount = draw.m_iIndiceCount;

		m_pFrameDriver->Draw(draw.m_Type,
			draw.m_Primitive,
			indiceCount,
			draw.m_iIBOffset);

		if (draw.m_bClipped)
			m_pFrameDriver->DisableClipRect();
	}

	SetActiveDrawMaterial(NULL);
	m_pFrameDriver->SetWireframe(FALSE);
}

//----------------------------------

void Renderer::DrawBufferNoMaterial(const DrawBufferType& _buffer)
{
	m_pFrameDriver->SetWireframe(m_bWireframe);
	SetActiveDrawMaterial(NULL);
	for (U32 i = 0; i < m_pRenderDrawList->m_daDrawBuffers[_buffer].GetCount(); i++)
	{
		SDraw& draw = *(m_pRenderDrawList->m_daDrawBuffers[_buffer][i]);
		PushDrawModelData(draw);
		m_pFrameDriver->Draw(draw.m_Type, draw.m_Primitive);
	}

	m_pFrameDriver->SetWireframe(FALSE);
}

//----------------------------------

#ifdef USE_EDITOR_V2
void Renderer::DrawBufferPicking(const DrawBufferType& _buffer)
{
	m_pFrameDriver->SetWireframe(m_bWireframe);
	SetActiveDrawMaterial(NULL);
	for (U32 i = 0; i < m_pRenderDrawList->m_daDrawBuffers[_buffer].GetCount(); i++)
	{
		SDraw& draw = *(m_pRenderDrawList->m_daDrawBuffers[_buffer][i]);
		PushDrawModelData(draw, draw.m_EntityColor);
		m_pFrameDriver->Draw(draw.m_Type, draw.m_Primitive);
	}

	m_pFrameDriver->SetWireframe(FALSE);
}
#endif

//----------------------------------

void Renderer::PushDrawModelData(const SDraw& _draw, const Color& _drawColor /*= Color::White*/)
{
	// Model data
	const SRendererView& view = m_Views[m_CurrentType]->GetData();
	
	m_pModelData->UpdateData([&](auto& _data)
		{
			Color col = _drawColor;
			if (m_pRenderDrawList->m_pMaterial)
				col *= m_pRenderDrawList->m_pMaterial->GetDiffuseColor();
			_data.m_vDiffuseColor = Vec4f(col.r, col.g, col.b, col.a);
			_data.m_vOutlineColor = Vec4f(_draw.m_OutlineValue.r, _draw.m_OutlineValue.g, _draw.m_OutlineValue.b, _draw.m_OutlineValue.a);
			_data.m_mMVP = view.m_mViewProj * _draw.m_mModelMatrix;
			_data.m_mModelView = view.m_mView * _draw.m_mModelMatrix;
			_data.m_mModel = _draw.m_mModelMatrix;
		});
	/*m_pFrameDriver->PushShaderData(m_pModelData);
	m_pFrameDriver->UseShaderData(ShaderGroup::UNIFORMDATA_ModelData, m_pModelData, 2);*/
	m_pFrameDriver->PushDrawShaderData(ShaderGroup::UNIFORMDATA_ModelData, m_pModelData);
}

//----------------------------------

void Renderer::SetParamsToGlobal()
{
	// todo : param dirty check + multithread
	m_pGlobalData->UpdateData([&](auto& _data)
		{
			Color sunColor = m_Params.GetValue("SunColor")->GetColor();
			_data.m_vSunColor = Vec4f(sunColor.r, sunColor.g, sunColor.b, sunColor.a);
			_data.m_fAmbientFactor = m_Params.GetValue("AmbientFactor")->GetFloat();
			_data.m_fDiffuseFactor = m_Params.GetValue("DiffuseFactor")->GetFloat();
			_data.m_fSpecularFactor = m_Params.GetValue("SpecularFactor")->GetFloat();
			_data.m_fEmissiveFactor = m_Params.GetValue("EmissiveFactor")->GetFloat();
			Color cGlobalAmbient = m_Params.GetValue("GlobalAmbient")->GetColor();
			_data.m_vGlobalAmbient = Vec4f(cGlobalAmbient.r, cGlobalAmbient.g, cGlobalAmbient.b, cGlobalAmbient.a);
			_data.m_fShadowStrength = m_Params.GetValue("ShadowStrength")->GetFloat();
			_data.m_fNoLightBoost = m_Params.GetValue("NoLightBoost")->GetFloat();
			_data.m_fSSAORadius = m_Params.GetValue("SSAORadius")->GetFloat();
			_data.m_vSSAOScreenRadius.x = m_Params.GetValue("SSAOScreenRadiusMin")->GetFloat();
			_data.m_vSSAOScreenRadius.y = m_Params.GetValue("SSAOScreenRadiusMax")->GetFloat();
			_data.m_fSSAODotBias = m_Params.GetValue("SSAODotBias")->GetFloat();
			_data.m_fSSAOMultiplier = m_Params.GetValue("SSAOMultiplier")->GetFloat();
			_data.m_fBloomSize = m_Params.GetValue("BloomSize")->GetFloat();
			_data.m_iOutlineSize = m_Params.GetValue("OutlineSize")->GetS32();

			_data.m_fDOFMinNearPlane = m_Params.GetValue("DOFMinNearPlane")->GetFloat();
			_data.m_fDOFMaxFarPlane = m_Params.GetValue("DOFMaxFarPlane")->GetFloat();
			_data.m_fDOFNearPlaneFromFocus = m_Params.GetValue("DOFNearPlaneFromFocus")->GetFloat();
			_data.m_fDOFFarPlaneFromFocus = m_Params.GetValue("DOFFarPlaneFromFocus")->GetFloat();

			Color vignetteColor = m_Params.GetValue("VignetteColor")->GetColor();
			_data.m_vVignetteColor = Vec4f(vignetteColor.r, vignetteColor.g, vignetteColor.b, vignetteColor.a);
			_data.m_fVignetteRadius = m_Params.GetValue("VignetteRadius")->GetFloat();
			_data.m_fVignetteSmoothness = m_Params.GetValue("VignetteSmoothness")->GetFloat();

			_data.m_faShadowMapDistances[0] = m_Params.GetValue("ShadowSliceDist1")->GetFloat();
			_data.m_faShadowMapDistances[1] = m_Params.GetValue("ShadowSliceDist2")->GetFloat();
			_data.m_faShadowMapDistances[2] = m_Params.GetValue("ShadowSliceDist3")->GetFloat();
		});
}

//----------------------------------
#ifdef USE_EDITOR_V2
Bool Renderer::IsPickingReady(U32& _id) 
{
	if (m_iEditorPickingResult != -1)
	{
		_id = m_iEditorPickingResult;
		m_iEditorPickingResult = -1;
		m_vEditorPickingRequest = Vec2i::Zero;
		return TRUE;
	}
	return FALSE;
}
#endif

//----------------------------------

void Renderer::CreateFrameBuffers(S32 _width, S32 _height)
{
	// global data
	Str name("GlobalData_%x", this);
	m_pGlobalData = ResourceManager::GetInstance().GetVirtualResource<RendererGlobalData>(name, TRUE);
	m_pGlobalData->UpdateData([&](auto& _data)
		{
			_data.m_vResolution = Vec2f(Float(m_iViewportWidth), Float(m_iViewportHeight));
		});
	SetParamsToGlobal();

	// view datas
	for(U32 v = 0; v < RendererView_Count; v++)
	{
		Str name("ViewData_%d_%x", v, this);
		m_Views[v] = ResourceManager::GetInstance().GetVirtualResource<RendererViewData>(name, TRUE);
	}

	// model data
	name.SetFrom("ModelData_%x", this);
	m_pModelData = ResourceManager::GetInstance().GetVirtualResource<RendererModelData>(name, TRUE);

	m_Passes.SetEmpty();

	TextureRef pGameTexture;
	TextureRef pDepthStencil = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("DepthStencil_%x",this), TRUE);
	pDepthStencil->Init(_width, _height, Texture::TEXTUREFORMAT_DepthStencil);
	pDepthStencil->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	
	// defautl emissive
	m_DefaultEmissive = ResourceManager::GetInstance().GetVirtualResource<Texture>("DefaultEmissive");
	if (m_DefaultEmissive->GetNbLevels() == 0)
	{
		U8 white[4 * 4];
		memset(white, 255, sizeof(white));
		m_DefaultEmissive->Init(2, 2, Texture::TEXTUREFORMAT_RGBA8, white);
	}

	if(m_Pipeline == RendererPipeline::Full)
	{
		// Passes
		RendererPassRef	pGBufferPass = CreateGBufferPass(_width, _height, pDepthStencil);
		CreateShadowMapPass();
		RendererPassRef	pSSAOPass = CreateSSAOPass(_width, _height, pGBufferPass->m_pFrameBuffer->GetTexture(0), pDepthStencil);
		RendererPassRef pBloomPass = CreateBloomPass(_width / 2, _height / 2, pGBufferPass->m_pFrameBuffer->GetTexture(2));
		RendererPassRef pOutlinePass = CreateOutlinePass(_width, _height);

		ForwardParam forwardParams;
		forwardParams.m_pNormalTexture = pGBufferPass->m_pFrameBuffer->GetTexture(0);
		forwardParams.m_pAlbedoTexture = pGBufferPass->m_pFrameBuffer->GetTexture(1);
		forwardParams.m_pEmissiveTexture = pGBufferPass->m_pFrameBuffer->GetTexture(2);
		forwardParams.m_pSSAOTexture = pSSAOPass->m_pFrameBuffer->GetTexture();
		forwardParams.m_ShadowMaps[0] = m_ShadowPasses[0]->m_pFrameBuffer->GetTexture();
		forwardParams.m_ShadowMaps[1] = m_ShadowPasses[1]->m_pFrameBuffer->GetTexture();
		forwardParams.m_ShadowMaps[2] = m_ShadowPasses[2]->m_pFrameBuffer->GetTexture();
		forwardParams.m_pOutlineColor = pOutlinePass->m_pFrameBuffer->GetTexture(0);
		forwardParams.m_pOutlinePos = pOutlinePass->m_pFrameBuffer->GetTexture(1);
		RendererPassRef	pForwardPass = CreateForwardPass(_width, _height, pDepthStencil, forwardParams);
		
		RendererPassRef pDOFPass = CreateDOFPass(_width, _height, pDepthStencil, pForwardPass->m_pFrameBuffer->GetTexture(0));
		RendererPassRef pToneMapping = CreateToneMappingPass(_width, _height, pDOFPass->m_pFrameBuffer->GetTexture(0));
		
#ifdef USE_EDITOR_V2
		CreateEditorPass(_width, _height);
#endif

		pGameTexture = pToneMapping->m_pFrameBuffer->GetTexture(0);
	}

	RendererPassRef pFinalPass = CreatureFinalPass(_width, _height, pDepthStencil, pGameTexture);
}

//----------------------------------

RendererPassRef Renderer::AddRendererPass(const Char* _pName, Bool _bCreateFB /*= TRUE*/)
{
	RendererPassRef pPass = ResourceManager::GetInstance().GetVirtualResource<RendererPass>(_pName, TRUE);
	m_Passes.AddLastItem(pPass);

	if (_bCreateFB)
	{
		pPass->m_pFrameBuffer = ResourceManager::GetInstance().GetVirtualResource<FrameBuffer>(_pName, TRUE);
	}

	return pPass;
}

//----------------------------------

RendererPassRef	Renderer::CreateGBufferPass(S32 _width, S32 _height, TextureRef _pDepthTexture)
{
	const Char* pName = "GBuffer";
	RendererPassRef pPass = AddRendererPass(pName);

	// textures
	TextureRef newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_Normal",pName), TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RG16F);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_AlbedoSpec", pName), TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_EmissiveGlow", pName), TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	pPass->m_pFrameBuffer->AddTexture(_pDepthTexture);
	pPass->m_ClearFlag = Clear_Color | Clear_Depth;

	// skybox subpass
	RendererSubpassRef pSubpass = pPass->AddSubpass("Skybox");
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_Skybox);
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, m_SkyboxTexture.GetCastedPtrTo<Texture>());
	pSubpass->AddTextureSlot(1, ShaderGroup::UNIFORM_sEmissiveTexture, NULL);
	pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

	// geometry subpass
	pSubpass = pPass->AddSubpass("Geometry");
	pSubpass->m_RenderState = RenderState_DepthWrite | RenderState_DepthTest | RenderState_FaceCulling;
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_DeferredGeometry);
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, NULL);
	pSubpass->AddTextureSlot(1, ShaderGroup::UNIFORM_sEmissiveTexture, NULL);
	pSubpass->AddTextureSlot(2, ShaderGroup::UNIFORM_sBlueNoise, m_BlueNoise.GetCastedPtrTo<Texture>());
	pSubpass->m_DrawBuffer = DRAWBUFFER_Opaque;

	return pPass;
}

//----------------------------------

void Renderer::CreateShadowMapPass()
{
	// Create ShadowBuffer
	m_ShadowPasses.SetEmpty();
	TextureRef pDepthStencilSM = ResourceManager::GetInstance().GetVirtualResource<Texture>("Shadow_DepthStencil", TRUE);
	pDepthStencilSM->Init(SHADOWMAP_SIZE, SHADOWMAP_SIZE, Texture::TEXTUREFORMAT_DepthStencil);
	pDepthStencilSM->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	for (U32 i = 0; i < 3; i++)
	{
		Str name("Shadow%d", i);
		RendererPassRef pPass = AddRendererPass(name);
		
		//textures
		TextureRef newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_PositionDepth", name.GetArray()), TRUE); // position/depth
		newTexture->Init(SHADOWMAP_SIZE, SHADOWMAP_SIZE, Texture::TEXTUREFORMAT_R32F);
		newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
		pPass->m_pFrameBuffer->AddTexture(newTexture);
		pPass->m_pFrameBuffer->AddTexture(pDepthStencilSM);
		pPass->m_ClearFlag = Clear_Color | Clear_Depth;
		pPass->m_ClearColor = Color::White;
		
		// subpass
		RendererSubpassRef pSubpass = pPass->AddSubpass();
		pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_Shadow);
		pSubpass->m_RenderState = (RenderState_DepthWrite | RenderState_DepthTest /*| RenderState_InvertCulling*/);
		pSubpass->m_ViewType = (RendererViewType)(RendererView_ShadowMap0 + i);
		pSubpass->m_DrawBuffer = DRAWBUFFER_Opaque;
		pSubpass->m_DrawBufferMode = DRAWBUFFERMODE_NoMaterial;

		m_ShadowPasses.AddLastItem(pPass);
	}
}

//----------------------------------

RendererPassRef Renderer::CreateSSAOPass(S32 _width, S32 _height, TextureRef _NormalTexture, TextureRef _pDepthTexture)
{
	// ssao noise texture	
	m_SSAONoise = ResourceManager::GetInstance().GetVirtualResource<Texture>("SSAONoise", TRUE);
	FloatArray SSAONoise;
	for (S32 i = 0; i < 16; i++)
	{
		Vec2f dir;
		dir.x = (Float(rand()) / Float(RAND_MAX)) * 2.f - 1.f;
		dir.y = (Float(rand()) / Float(RAND_MAX)) * 2.f - 1.f;
		dir.Normalize();
		SSAONoise.AddLastItem(dir.x);
		SSAONoise.AddLastItem(dir.y);
		SSAONoise.AddLastItem((Float(rand()) / Float(RAND_MAX)));
		SSAONoise.AddLastItem(1.f);
	}
	m_SSAONoise->Init(4, 4, Texture::TEXTUREFORMAT_RGBA32F, (U8*)&SSAONoise[0]);
	m_SSAONoise->SetWrapping(Texture::TEXTUREWRAP_Repeat);

	// Create SSAO
	const Char* pName = "SSAO";
	RendererPassRef pPass = AddRendererPass(pName);

	// textures	
	TextureRef newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(pName, TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	pPass->m_ClearFlag = Clear_Color;
	
	// ssao subpass
	RendererSubpassRef pSubpass = pPass->AddSubpass();
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_SSAO);
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot(2, ShaderGroup::UNIFORM_sDeferredNormal, _NormalTexture );
	pSubpass->AddTextureSlot(3, ShaderGroup::UNIFORM_sSSAONoise, m_SSAONoise );
	pSubpass->AddTextureSlot(4, ShaderGroup::UNIFORM_sDepth, _pDepthTexture );
	pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

	return pPass;
}

//----------------------------------

RendererPassRef Renderer::CreateBloomPass(S32 _width, S32 _height, TextureRef _pEmissiveTexture)
{
	S32 nbBlurPass = 5;
	const Char* pName = "Bloom";
	Bool bFirst = TRUE;
	RendererPassRef passes[2];
	for (S32 i = 0; i < nbBlurPass; i++)
	{
		// pass 0 framebuffer
		RendererPassRef pPassA = AddRendererPass(Str("%s%dA", pName, i), bFirst);
		RendererPassRef pPassB = AddRendererPass(Str("%s%dB", pName, i), bFirst);
		if (bFirst)
		{
			TextureRef newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%sA",pName), TRUE);
			newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
			newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
			pPassA->m_pFrameBuffer->AddTexture(newTexture);
			
			newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%sB", pName), TRUE);
			newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
			newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
			pPassB->m_pFrameBuffer->AddTexture(newTexture);
		}
		else
		{
			pPassA->m_pFrameBuffer = passes[0]->m_pFrameBuffer;
			pPassB->m_pFrameBuffer = passes[1]->m_pFrameBuffer;
		}

		// pass 0 subpass
		RendererSubpassRef pSubpass = pPassA->AddSubpass();
		pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_Blur, "HORIZONTAL");
		pSubpass->AddTextureSlot( 0, ShaderGroup::UNIFORM_sTexture, bFirst ? _pEmissiveTexture : passes[1]->m_pFrameBuffer->GetTexture(0) );
		pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

		// pass 1 subpass
		pSubpass = pPassB->AddSubpass();
		pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_Blur);
		pSubpass->AddTextureSlot( 0, ShaderGroup::UNIFORM_sTexture, pPassA->m_pFrameBuffer->GetTexture(0) );
		pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

		passes[0] = pPassA;
		passes[1] = pPassB;
		bFirst = FALSE;
	}

	return passes[1];
}

//----------------------------------

RendererPassRef Renderer::CreateOutlinePass(S32 _width, S32 _height)
{
	// Outline frame buffer
	const Char* pName = "Outline";
	RendererPassRef pPass = AddRendererPass(pName);

	// textures
	TextureRef newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_Color", pName), TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_Pos", pName), TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_R32F);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	pPass->m_ClearFlag = Clear_Color | Clear_Depth;
	
	// outline subpass
	RendererSubpassRef pSubpass = pPass->AddSubpass();
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_Outline);
	pSubpass->m_DrawBuffer = DRAWBUFFER_Outline;

	// Outline horizontal frame buffer
	pName = "OutlineHorizontal";
	RendererPassRef pPassH = AddRendererPass(pName);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_Color", pName), TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPassH->m_pFrameBuffer->AddTexture(newTexture);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_Pos", pName), TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_R32F);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPassH->m_pFrameBuffer->AddTexture(newTexture);

	// outline horizontal subpass
	pSubpass = pPassH->AddSubpass();
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_OutlineHorizontal);
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot( 0, ShaderGroup::UNIFORM_sTexture, pPass->m_pFrameBuffer->GetTexture(0) );
	pSubpass->AddTextureSlot( 1, ShaderGroup::UNIFORM_sOutlinePos, pPass->m_pFrameBuffer->GetTexture(1) );
	pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

	return pPassH;
}

//----------------------------------

RendererPassRef	Renderer::CreateForwardPass(S32 _width, S32 _height, TextureRef _pDepth, const ForwardParam& _params)
{
	// Main frame buffer
	const Char* pName = "Forward";
	RendererPassRef pPass = AddRendererPass(pName);
	
	// textures
	TextureRef newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(pName, TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA16F);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	//pPass->m_pFrameBuffer->AddTexture(_pDepth);
	pPass->m_ClearFlag = Clear_Color;

	// ambient/emissive + sun
	RendererSubpassRef pSubpass = pPass->AddSubpass("BaseLighting");
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_DeferredLighting);
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot( 2, ShaderGroup::UNIFORM_sDeferredNormal, _params.m_pNormalTexture );
	pSubpass->AddTextureSlot( 3, ShaderGroup::UNIFORM_sDeferredAlbedoSpec, _params.m_pAlbedoTexture );
	pSubpass->AddTextureSlot( 4, ShaderGroup::UNIFORM_sSSAO, _params.m_pSSAOTexture );
	pSubpass->AddTextureSlot( 5, ShaderGroup::UNIFORM_sShadowMap0, _params.m_ShadowMaps[0] );
	pSubpass->AddTextureSlot( 6, ShaderGroup::UNIFORM_sShadowMap1, _params.m_ShadowMaps[1] );
	pSubpass->AddTextureSlot( 7, ShaderGroup::UNIFORM_sShadowMap2, _params.m_ShadowMaps[2] );
	pSubpass->AddTextureSlot( 8, ShaderGroup::UNIFORM_sDeferredEmissiveGlow, _params.m_pEmissiveTexture );
	pSubpass->AddTextureSlot( 9, ShaderGroup::UNIFORM_sDepth, _pDepth );
	pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

	// omnis
	pSubpass = pPass->AddSubpass("Omnis");
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_DeferredOmni);
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot(2, ShaderGroup::UNIFORM_sDeferredNormal, _params.m_pNormalTexture);
	pSubpass->AddTextureSlot(3, ShaderGroup::UNIFORM_sDeferredAlbedoSpec, _params.m_pAlbedoTexture);	
	pSubpass->AddTextureSlot(4, ShaderGroup::UNIFORM_sDepth, _pDepth);
	pSubpass->m_RenderState = RenderState_Additive;
	pSubpass->m_DrawBuffer = DRAWBUFFER_Omni;

	// transparent
	//pSubpass = pPass->AddSubpass("Transparent");
	//pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_Transparent);
	//pSubpass->m_ViewType = RendererView_Game;
	//pSubpass->m_RenderState = RenderState_DepthWrite | RenderState_DepthTest | RenderState_AlphaBlend | RenderState_FaceCulling;
	//pSubpass->m_TextureSlots.SetEmpty();
	//pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, NULL);
	//pSubpass->AddTextureSlot(1, ShaderGroup::UNIFORM_sEmissiveTexture, NULL);
	//pSubpass->m_DrawBuffer = DRAWBUFFER_Transparent;

	// draw outline
	pSubpass = pPass->AddSubpass("DrawOutline");
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_OutlineVertical);
	pSubpass->m_ViewType = RendererView_Game;
	pSubpass->m_RenderState = RenderState_AlphaBlend;
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot( 0, ShaderGroup::UNIFORM_sTexture, _params.m_pOutlineColor );
	pSubpass->AddTextureSlot( 2, ShaderGroup::UNIFORM_sOutlinePos, _params.m_pOutlinePos );
	pSubpass->AddTextureSlot( 3, ShaderGroup::UNIFORM_sDepth, _pDepth );
	pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

	return pPass;
}

//----------------------------------

RendererPassRef Renderer::CreateDOFPass(S32 _width, S32 _height, TextureRef _pDepth, TextureRef _pInputTexture)
{
	// White, BarreBrisebois- Rendering in BF3 (Siggraph 2011 Advances in Real-Time Rendering Course)
	const Char* pName = "DOF";

	// p0
	Str name("%s0", pName);
	RendererPassRef pPass0 = AddRendererPass(name);
	TextureRef newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(name, TRUE);
	newTexture->Init(_width / 2, _height / 2, Texture::TEXTUREFORMAT_RGBA16F);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass0->m_pFrameBuffer->AddTexture(newTexture);

	// sb0
	RendererSubpassRef pSubpass = pPass0->AddSubpass();
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_DOF, "COC");
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot( 0, ShaderGroup::UNIFORM_sTexture, _pInputTexture); // useless
	pSubpass->AddTextureSlot( 1, ShaderGroup::UNIFORM_sMainFrame, _pInputTexture );
	pSubpass->AddTextureSlot( 2, ShaderGroup::UNIFORM_sDeferredAlbedoSpec, _pInputTexture); // useless
	pSubpass->AddTextureSlot( 3, ShaderGroup::UNIFORM_sDepth, _pDepth );
	pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

	// p1
	name.SetFrom("%s1", pName);
	RendererPassRef pPass1 = AddRendererPass(name);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%sA", name.GetArray()), TRUE);
	newTexture->Init(_width / 2, _height / 2, Texture::TEXTUREFORMAT_RGBA16F);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass1->m_pFrameBuffer->AddTexture(newTexture);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%sB", name.GetArray()), TRUE);
	newTexture->Init(_width / 2, _height / 2, Texture::TEXTUREFORMAT_RGBA16F);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass1->m_pFrameBuffer->AddTexture(newTexture);

	// sb1
	pSubpass = pPass1->AddSubpass();
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_DOF, "FIRST");
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot( 0, ShaderGroup::UNIFORM_sTexture, _pInputTexture); // useless
	pSubpass->AddTextureSlot( 1, ShaderGroup::UNIFORM_sMainFrame, pPass0->m_pFrameBuffer->GetTexture(0) );
	pSubpass->AddTextureSlot( 2, ShaderGroup::UNIFORM_sDeferredAlbedoSpec, _pInputTexture); // useless
	pSubpass->AddTextureSlot( 3, ShaderGroup::UNIFORM_sDepth, _pDepth); // useless
	pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

	// p2
	name.SetFrom("%s2", pName);
	RendererPassRef pPass2 = AddRendererPass(name);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(name, TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA16F);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass2->m_pFrameBuffer->AddTexture(newTexture);

	// sb2
	pSubpass = pPass2->AddSubpass();
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_DOF);
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot( 0, ShaderGroup::UNIFORM_sTexture, pPass1->m_pFrameBuffer->GetTexture(0) );
	pSubpass->AddTextureSlot( 1, ShaderGroup::UNIFORM_sMainFrame, _pInputTexture);
	pSubpass->AddTextureSlot( 2, ShaderGroup::UNIFORM_sDeferredAlbedoSpec, pPass1->m_pFrameBuffer->GetTexture(1) );
	pSubpass->AddTextureSlot( 3, ShaderGroup::UNIFORM_sDepth, _pDepth); // useless
	pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

	return pPass2;
}

//----------------------------------

RendererPassRef Renderer::CreateToneMappingPass(S32 _width, S32 _height, TextureRef _pInputTexture)
{
	// ToneMapping pass
	const Char* pName = "ToneMapping";
	RendererPassRef pPass = AddRendererPass(pName);

	// texture
	TextureRef newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(pName, TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	
	// ToneMapping subpass
	RendererSubpassRef pSubpass = pPass->AddSubpass();
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_ToneMapping);
	pSubpass->m_TextureSlots.SetEmpty();
	pSubpass->AddTextureSlot( 0, ShaderGroup::UNIFORM_sMainFrame, _pInputTexture );
	pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

	return pPass;
}

//----------------------------------

#ifdef USE_EDITOR_V2
void Renderer::CreateEditorPass(S32 _width, S32 _height)
{
	// picker
	const Char* pName = "EditorPicker";
	RendererPassRef pPass = AddRendererPass(pName);
	TextureRef newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(pName, TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	TextureRef pPickerDepth = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_DepthStencil", pName), TRUE);
	pPickerDepth->Init(_width, _height, Texture::TEXTUREFORMAT_DepthStencil);
	pPickerDepth->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	pPass->m_pFrameBuffer->AddTexture(pPickerDepth);
	pPass->m_ClearFlag = Clear_Color | Clear_Depth;
	RendererSubpassRef pSubpass = pPass->AddSubpass("Opaque");
	pSubpass->m_RenderState = RenderState_DepthTest | RenderState_DepthWrite;
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_EditorPicking);
	pSubpass->m_DrawBuffer = DRAWBUFFER_Opaque;
	pSubpass->m_DrawBufferMode = DRAWBUFFERMODE_Picking;
	pSubpass = pPass->AddSubpass("Transparent");
	pSubpass->m_RenderState = RenderState_DepthTest | RenderState_DepthWrite;
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_EditorPicking);
	pSubpass->m_DrawBuffer = DRAWBUFFER_Transparent;
	pSubpass->m_DrawBufferMode = DRAWBUFFERMODE_Picking;
	m_EditorPickerPass = pPass;

	// 
	pName = "EditorOutline";
	pPass = AddRendererPass(pName);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(pName, TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	pPass->m_ClearFlag = Clear_Color;
	pSubpass = pPass->AddSubpass("EditorOutline");
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_EditorOutlineDraw);
	pSubpass->m_ViewType = RendererView_Game;
	pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, NULL);
	pSubpass->AddTextureSlot(1, ShaderGroup::UNIFORM_sEmissiveTexture, NULL);
	pSubpass->m_DrawBuffer = DRAWBUFFER_EditorOutline;
	pSubpass = pPass->AddSubpass("EditorOutline2D");
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_EditorOutlineDraw2D);
	pSubpass->m_ViewType = RendererView_2D;
	pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, NULL);
	pSubpass->AddTextureSlot(1, ShaderGroup::UNIFORM_sEmissiveTexture, NULL);
	pSubpass->m_DrawBuffer = DRAWBUFFER_EditorOutline2D;
	m_EditorOutlinePass = pPass;

	// 
	pName = "Editor";
	pPass = AddRendererPass(pName);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(pName, TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_RGBA8);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	newTexture = ResourceManager::GetInstance().GetVirtualResource<Texture>(Str("%s_DepthStencil", pName), TRUE);
	newTexture->Init(_width, _height, Texture::TEXTUREFORMAT_DepthStencil);
	newTexture->SetWrapping(Texture::TEXTUREWRAP_Clamp);
	pPass->m_pFrameBuffer->AddTexture(newTexture);
	pPass->m_ClearFlag = Clear_Color | Clear_Depth;
	pPass->m_ClearColor.a = 0.f;
	pSubpass = pPass->AddSubpass();
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_Debug);
	pSubpass->m_ViewType = RendererView_Game;
	pSubpass->m_RenderState = RenderState_DepthWrite | RenderState_DepthTest;
	pSubpass->m_DrawBuffer = DRAWBUFFER_Gizmo;
	pSubpass->m_DrawBufferMode = DRAWBUFFERMODE_NoMaterial;
	m_EditorPass = pPass;
}
#endif

//----------------------------------

RendererPassRef Renderer::CreatureFinalPass(S32 _width, S32 _height, TextureRef _pDepth, TextureRef _pInputTexture)
{
	const Char* pName = "Final";
	RendererPassRef pPass = AddRendererPass(Str("%s_%x",pName), FALSE);

	RendererSubpassRef pSubpass;
	if (m_Pipeline == RendererPipeline::Full)
	{
		pPass->m_ClearFlag = Clear_Color;

		// AA
		pSubpass = pPass->AddSubpass("AA");
		pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_AA);
		pSubpass->m_TextureSlots.SetEmpty();
		pSubpass->AddTextureSlot( 0, ShaderGroup::UNIFORM_sDepth, _pDepth );
		pSubpass->AddTextureSlot( 1, ShaderGroup::UNIFORM_sMainFrame, _pInputTexture );
		pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

#ifdef USE_DRAWDEBUG
		// Debug
		pSubpass = pPass->AddSubpass("Debug");
		pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_Debug);
		pSubpass->m_ViewType = RendererView_Game;
		pSubpass->m_RenderState = RenderState_DepthWrite | RenderState_DepthTest;
		pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, NULL);
		pSubpass->AddTextureSlot(1, ShaderGroup::UNIFORM_sEmissiveTexture, NULL);
		pSubpass->m_DrawBuffer = DRAWBUFFER_DrawDebug;
#endif

#ifdef USE_EDITOR_V2
		// Editor Outline
		pSubpass = pPass->AddSubpass("EditorOutlineFinal");
		pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_EditorOutlineExtend);
		pSubpass->m_ViewType = RendererView_Game;
		pSubpass->m_TextureSlots.SetEmpty();
		pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, m_EditorOutlinePass->m_pFrameBuffer->GetTexture(0) );
		pSubpass->m_RenderState = RenderState_AlphaBlend;
		pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;

		// Editor shapes
		pSubpass = pPass->AddSubpass("Editor");
		pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_DisplayTexture);
		pSubpass->m_ViewType = RendererView_Game;
		pSubpass->m_TextureSlots.SetEmpty();
		pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, m_EditorPass->m_pFrameBuffer->GetTexture(0));
		pSubpass->m_RenderState = RenderState_AlphaBlend;
		pSubpass->m_DrawBuffer = DRAWBUFFER_FullScreenQuad;
#endif

		pSubpass = pPass->AddSubpass("2D");
		pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_2D);
		pSubpass->m_ViewType = RendererView_2D;
		pSubpass->m_TextureSlots.SetEmpty();
		pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, NULL);
		pSubpass->m_RenderState = RenderState_AlphaBlend;
		pSubpass->m_DrawBuffer = DRAWBUFFER_2D;
	}

#ifdef USE_IMGUI
	pSubpass = pPass->AddSubpass("ImGui");
	pSubpass->m_pShader = RendererDatas::GetInstance().CreateShaderGroup(SHADER_2D);
	pSubpass->m_ViewType = RendererView_2D;
	pSubpass->m_RenderState = RenderState_AlphaBlend;
	pSubpass->AddTextureSlot(0, ShaderGroup::UNIFORM_sTexture, NULL);
	pSubpass->AddTextureSlot(1, ShaderGroup::UNIFORM_sEmissiveTexture, NULL);
	pSubpass->m_DrawBuffer = DRAWBUFFER_ImGui;
#endif

	return pPass;
}

//----------------------------------

void Renderer::DestroyFrameBuffers()
{
	RendererDatas& datas = RendererDatas::GetInstance();

	m_pGlobalData.SetNull();
	for (auto& v : m_Views)
		v.SetNull();
	m_pModelData.SetNull();

	m_Passes.SetEmpty();
		
	m_SSAONoise.SetNull();
	m_DefaultEmissive.SetNull();

	ResourceManager::GetInstance().CleanResources();
}

//----------------------------------

void Renderer::ComputeShadowMatrices()
{
	if (m_ShadowPasses.GetCount() == 0)
		return;

	// compute matrices
	Vec3f sunDir = m_Params.GetValue("SunDir")->GetVec3f();
	sunDir.Normalize();
	for (U32 m = 0; m < 3; m++)
	{
		// compute bsphere
		Float fNear = (m > 0) ? m_pGlobalData->GetData().m_faShadowMapDistances[m - 1] : 0.f;
		Float fFar = m_pGlobalData->GetData().m_faShadowMapDistances[m];

		static Bool bFreezed = FALSE;
		static Sphere bsphere[3];
		if (!bFreezed)
		{
			ComputeShadowBSphere(m_mCameraInvView, m_mCameraInvViewProj, fNear, fFar, bsphere[m]);

			// snap radius to 0.5
			bsphere[m].m_fRadius /= 0.5f;
			bsphere[m].m_fRadius = ROUND(bsphere[m].m_fRadius);
			bsphere[m].m_fRadius *= 0.5f;
		}
		//else
		//{
		//	DEBUG_DrawSphere(bsphere[m].m_vCenter, bsphere[m].m_fRadius, Color::Red);
		//}
		//DEBUG_DrawSphere(bsphere[m].m_vCenter, 0.2f, Color::Red);

		// compute matrices from bsphere
		static Float zSize = 20.f;
		Vec3f rightaxis = (sunDir ^ Vec3f::YAxis);
		rightaxis.Normalize();
		Vec3f upaxis = rightaxis ^ sunDir;
		upaxis.Normalize();

		Mat44 mProj, mView;
		Vec3f startPos = Vec3f::Zero;
		mView.SetLookAt(startPos, startPos + sunDir, upaxis);

		// snap sphere to texel size
		Float bSphereDiameter = 2.f * bsphere[m].m_fRadius;
		Float  texelSize((2.f * bsphere[m].m_fRadius) / Float(SHADOWMAP_SIZE));
		Vec3f viewCenter = mView * bsphere[m].m_vCenter;
		viewCenter.x /= texelSize;
		viewCenter.x = ROUND(viewCenter.x);
		viewCenter.x *= texelSize;
		viewCenter.y /= texelSize;
		viewCenter.y = ROUND(viewCenter.y);
		viewCenter.y *= texelSize;
		Mat44 invProj = mView;
		invProj.Inverse();
		bsphere[m].m_vCenter = invProj * viewCenter;

		startPos = bsphere[m].m_vCenter - sunDir * (bsphere[m].m_fRadius + zSize);
		mView.SetLookAt(startPos, startPos + sunDir, upaxis);

		Float fShadowNear = 0.f;
		Float fShadowFar = zSize + bSphereDiameter;
		mProj.SetOrtho(-bsphere[m].m_fRadius, bsphere[m].m_fRadius, -bsphere[m].m_fRadius, bsphere[m].m_fRadius, fShadowNear, fShadowFar);
		auto& pShadowView = m_Views[m_ShadowPasses[m]->m_Subpasses[0]->m_ViewType];
		pShadowView->UpdateData([&](auto& _data)
			{
				_data.SetFromVP(mView, mProj);
				_data.m_fNear = fShadowNear;
				_data.m_fFar = fShadowFar;
			});

		//static 	Mat44 biasMat(
		//	0.5f, 0.0f, 0.0f, 0.5f,
		//	0.0f, 0.5f, 0.0f, 0.5f,
		//	0.0f, 0.0f, 0.5f, 0.5f,
		//	0.0f, 0.0f, 0.0f, 1.0f
		//);

		/*static 	Mat44 biasMat(
			0.5f, 0.0f, 0.0f, 0.5f,
			0.0f, 0.5f, 0.0f, 0.5f,
			0.0f, 0.0f, 1.f, 0.f,
			0.0f, 0.0f, 0.0f, 1.0f
		);*/

		m_pGlobalData->UpdateData([&](auto& _data)
			{
				_data.m_maViewToShadow[m] = pShadowView->GetData().m_mViewProj * m_mCameraInvView;
			});

		// debug
		if (bFreezed)
		{
			Mat44 invViewProj = mProj * mView;
			invViewProj.Inverse();
			DEBUG_DrawLine(invViewProj * Vec3f(-1.f, -1.f, -1.f), invViewProj * Vec3f(-1.f, -1.f, 1.f), Color::Red);
			DEBUG_DrawLine(invViewProj * Vec3f(-1.f, 1.f, -1.f), invViewProj * Vec3f(-1.f, 1.f, 1.f), Color::Red);
			DEBUG_DrawLine(invViewProj * Vec3f(1.f, -1.f, -1.f), invViewProj * Vec3f(1.f, -1.f, 1.f), Color::Red);
			DEBUG_DrawLine(invViewProj * Vec3f(1.f, 1.f, -1.f), invViewProj * Vec3f(1.f, 1.f, 1.f), Color::Red);

			DEBUG_DrawLine(invViewProj * Vec3f(-1.f, -1.f, -1.f), invViewProj * Vec3f(1.f, -1.f, -1.f), Color::Red);
			DEBUG_DrawLine(invViewProj * Vec3f(-1.f, -1.f, -1.f), invViewProj * Vec3f(-1.f, 1.f, -1.f), Color::Red);
			DEBUG_DrawLine(invViewProj * Vec3f(1.f, 1.f, -1.f), invViewProj * Vec3f(1.f, -1.f, -1.f), Color::Red);
			DEBUG_DrawLine(invViewProj * Vec3f(1.f, 1.f, -1.f), invViewProj * Vec3f(-1.f, 1.f, -1.f), Color::Red);

			DEBUG_DrawLine(invViewProj * Vec3f(-1.f, -1.f, 1.f), invViewProj * Vec3f(1.f, -1.f, 1.f), Color::Red);
			DEBUG_DrawLine(invViewProj * Vec3f(-1.f, -1.f, 1.f), invViewProj * Vec3f(-1.f, 1.f, 1.f), Color::Red);
			DEBUG_DrawLine(invViewProj * Vec3f(1.f, 1.f, 1.f), invViewProj * Vec3f(1.f, -1.f, 1.f), Color::Red);
			DEBUG_DrawLine(invViewProj * Vec3f(1.f, 1.f, 1.f), invViewProj * Vec3f(-1.f, 1.f, 1.f), Color::Red);
		}
	}
}

//----------------------------------

void Renderer::ComputeShadowBSphere(const Mat44& _invview, const Mat44& _invviewproj, Float _fNear, Float _fFar, Sphere& _outBSphere)
{
	// ref pos
	Vec3f camPos = _invview * Vec3f::Zero;
	Vec3f screenPos[] = {
		_invviewproj*Vec3f(-1.0f,-1.0f,-1.f),
		_invviewproj*Vec3f(-1.0f,1.0f,-1.f),
		_invviewproj*Vec3f(1.0f,-1.0f,-1.f),
		_invviewproj*Vec3f(1.0f,1.0f,-1.f)
	};

	// center
	Vec3f dir = (_invviewproj *Vec3f(0.0f, 0.0f, -1.f) - camPos);
	dir.Normalize();
	_outBSphere.m_vCenter = camPos + dir * ((_fFar + _fNear) * 0.5f);
	
	// compute radius
	_outBSphere.m_fRadius = 0.f;
	for (U32 i = 0; i < _countof(screenPos); i++)
	{
		dir = screenPos[i] - camPos;
		dir.Normalize();

		// near pos
		Vec3f nearPos = screenPos[i] + dir * _fNear;
		_outBSphere.m_fRadius = Max(_outBSphere.m_fRadius, (nearPos - _outBSphere.m_vCenter).GetLength());

		// far pos
		Vec3f farPos = screenPos[i] + dir * _fFar;
		_outBSphere.m_fRadius = Max(_outBSphere.m_fRadius, (farPos - _outBSphere.m_vCenter).GetLength());
	}
}

//----------------------------------

Bool	operator>(const Renderer::SDraw& _a,const Renderer::SDraw& _b)
{ 
	if( _a.m_fDistToCam == _b.m_fDistToCam )
		return _a.m_pMaterial > _b.m_pMaterial;

	return _a.m_fDistToCam > _b.m_fDistToCam;
}

//----------------------------------

Bool	operator<(const Renderer::SDraw& _a,const Renderer::SDraw& _b)
{ 
	if( _a.m_fDistToCam == _b.m_fDistToCam )
		return _a.m_pMaterial < _b.m_pMaterial;

	return _a.m_fDistToCam < _b.m_fDistToCam;
}

//----------------------------------

Bool	operator>(const Renderer::SDrawPtr& _a, const Renderer::SDrawPtr& _b)
{
	return *_a > *_b;
}

//----------------------------------

Bool	operator<(const Renderer::SDrawPtr& _a, const Renderer::SDrawPtr& _b)
{
	return *_a < *_b;
}