#ifndef __RENDERER_H__
#define __RENDERER_H__

class Window3DScreen;
#include "3d\material.h"
class Texture;
class Font;
class ShaderGroup;
class Shader;
#include "3d\index_buffer.h"
#include "3d\vertex_buffer.h"
#include "3d\primitive_3D.h"
#include "shader_group.h"
#include "renderer_camera.h"
#include "framebuffer.h"
#include "renderer_datas.h"
#include "debug\profiler_pix.h"
#include "renderer_pass.h"
#include "renderer_shader.h"
#include "renderer_driver_data.h"
#include "shader_data.h"
class Renderer;
class RendererDriver;
enum GeometryType
{
	Geometry_TrianglesList,
	Geometry_TrianglesStrip,
	Geometry_Lines
};

enum class RendererPipeline
{
	None,
	Full,
	ImGui
};

// Profiler events
#ifdef USE_GPU_PROFILER
	#define GPUPROFILER_Begin(_x)				\
	{											\
		PROFILER_OPTICK_START(_x);			\
		PROFILER_PIX_Begin(Color::Blue,_x);	\
		RendererDriver::g_pProfilerDriver->ProfilerBegin(_x);	\
	}
	#define GPUPROFILER_End()		 \
	{								 \
		RendererDriver::g_pProfilerDriver->ProfilerEnd();	\
		PROFILER_PIX_End();			 \
		PROFILER_OPTICK_STOP();		\
	}
#else
	#define GPUPROFILER_Begin(_x)	{}
	#define GPUPROFILER_End()		{}
#endif

class Renderer
{
	RENDERERDRIVER_DATA
public:

	struct SDraw
	{
		Float					m_fDistToCam = Float_Max;
		Mat44					m_mModelMatrix = Mat44::Identity;
		Primitive3DPtr			m_Primitive;
		const Material*		m_pMaterial = NULL;
		Color					m_MainColor = Color::White;
		GeometryType			m_Type = Geometry_TrianglesList;
		Color					m_OutlineValue = Color::Black;
		U32						m_iIndiceCount = 0;
		U32						m_iIBOffset = 0;
		Bool					m_bClipped = FALSE;
		Rect					m_ClipRect;
#ifdef USE_EDITOR_V2
		Color					m_EntityColor = Color::Black;
#endif
	};

	struct SDrawPtr
	{
		U32		m_iIndex;
		ArrayOf<SDraw>* m_pArray;

		SDrawPtr() { m_iIndex = -1; }
		SDrawPtr(U32 _index, ArrayOf<SDraw>* _array) { m_iIndex = _index; m_pArray = _array; }
		INLINE SDraw* operator->() { return &(m_pArray->Get(m_iIndex)); }
		INLINE const SDraw* operator->(void) const { return &(m_pArray->Get(m_iIndex)); }
		INLINE const SDraw& operator*(void) const { return (m_pArray->Get(m_iIndex)); }
		INLINE SDraw& operator*(void) { return (m_pArray->Get(m_iIndex)); }
	};

	Renderer();
	~Renderer();

	void SetPipeline(RendererPipeline _pipeline) { m_Pipeline = _pipeline; }

	Bool Initialize(const Window3DScreen* _pScreen);
	Bool InitializeDriver(RendererDriver* _pDriver, const Window3DScreen* _pScreen);
	Bool ChangeViewport(RendererDriver* _pDriver, S32 _width, S32 _height);
	Bool FinalizeDriver(RendererDriver* _pDriver, const Window3DScreen* _pScreen);
	Bool Finalize();
	Bool IsInitialized() const { return m_bInitialized; }

	void ReloadParams(RendererDriver* _pDriver);
	void SetParamsToGlobal();

	void PrepareDraw(RendererDriver* _pDriver);
	void Draw(RendererDriver* _pDriver);
	void Swap(RendererDriver* _pDriver);

	void SetActiveMaterial(const MaterialRef& _pMaterial, const Color& _mainColor = Color::White);
	void SetActiveDrawMaterial(const Material* _pMaterial);
	void SetActiveOutlineValue(const Color& _value);
	SDrawPtr	CreateDraw();
	void		PushDraw(DrawBufferType _type, const SDrawPtr& _pDraw);
	void DrawGeometry(GeometryType _type, const Vec3f& _pos, const Quat& _rot, const Primitive3DRef& _primitive, const Vec3f& _scale = Vec3f::One, Bool _is2D = FALSE);
	void DrawGeometry(DrawBufferType _drawType, GeometryType _type, const Vec3f& _pos, const Quat& _rot, const Primitive3DRef& _primitive, const Vec3f& _scale = Vec3f::One, Bool _is2D =FALSE);
	void DrawOmni(const Vec3f& _pos, Float _fWorldRadius, const Color& _color, const RendererOmniDataRef& _pData);

	void ToggleWireframe() { m_bWireframe = !m_bWireframe; }
	void UseMetal()			{ m_bUseMetal = TRUE; }

	Vec2i			GetViewportSize() const			{ return Vec2i(m_iViewportWidth, m_iViewportHeight); }

	void SetCamera(const RendererCameraRef& _camera)	{ m_pCamera = _camera; }
	const RendererCamera*	GetCamera() const			{ return m_pCamera.GetPtr(); }
	void UpdateCameraMatrix();
	const Mat44& GetCameraInvViewMatrix() const { return m_mCameraInvView; }
	const Mat44& GetCameraViewProjMatrix() const { return m_mCameraViewProj; }
	const Mat44& GetCameraInvViewProjMatrix() const { return m_mCameraInvViewProj; }

	void CreateFrameBuffers(S32 _width, S32 _height);
	void DestroyFrameBuffers();

#ifdef USE_EDITOR_V2
	void SetForceEditorOutline(Bool _enable) { m_bEditorOutline = _enable; }
	void SetEntityID(U32 _id) { m_iEditorEntityID = _id; }
	void RequestPicking(const Vec2i& _pos) { m_vEditorPickingRequest = _pos; }
	Bool IsPickingReady(U32& _id);

#endif

protected:
	void BeginActiveMaterial();
	void EndActiveMaterial();

	void DrawBuffer(const DrawBufferType& _buffer);
	void DrawBufferNoMaterial(const DrawBufferType& _buffer);

#ifdef USE_EDITOR_V2
	void DrawBufferPicking(const DrawBufferType& _buffer);
#endif

	RendererPassRef AddRendererPass(const Char* _pName, Bool _bCreateFB=TRUE);
	RendererPassRef	CreateGBufferPass(S32 _width, S32 _height, TextureRef _pDepthTexture);
	void	CreateShadowMapPass();
	RendererPassRef CreateSSAOPass(S32 _width, S32 _height, TextureRef _NormalTexture, TextureRef _pDepthTexture);
	RendererPassRef CreateBloomPass(S32 _width, S32 _height, TextureRef _pEmissiveTexture);
	RendererPassRef CreateOutlinePass(S32 _width, S32 _height);

	struct ForwardParam
	{
		TextureRef	m_pAlbedoTexture;
		TextureRef	m_pEmissiveTexture;
		TextureRef	m_pNormalTexture;
		TextureRef	m_pSSAOTexture;
		TextureRef	m_ShadowMaps[3];
		TextureRef	m_pOutlineColor;
		TextureRef	m_pOutlinePos;
	};
	RendererPassRef	CreateForwardPass(S32 _width, S32 _height, TextureRef _pDepth, const ForwardParam& _params);
	RendererPassRef CreateDOFPass(S32 _width, S32 _height, TextureRef _pDepth, TextureRef _pInputTexture);
	RendererPassRef CreateToneMappingPass(S32 _width, S32 _height, TextureRef _pInputTexture);
#ifdef USE_EDITOR_V2
	void CreateEditorPass(S32 _width, S32 _height);
#endif
	RendererPassRef CreatureFinalPass(S32 _width, S32 _height, TextureRef _pDepth, TextureRef _pInputTexture);

	void DoPass(RendererPass* _pass);
	void UpdateViewProjMatrix();
	Float ComputeDistFromCamera(const Vec3f& _pos);
	void PushDrawModelData(const SDraw& _draw, const Color& _drawColor = Color::White);

	// shadow matrix
	void ComputeShadowMatrices();
	void ComputeShadowBSphere(const Mat44& _invview, const Mat44& _invviewproj, Float _fNear, Float _fFar, Sphere& _outBSphere);

	RendererPipeline	m_Pipeline;
	RendererDriver* m_pFrameDriver;
	RendererSubpass* m_pCurrentSubpass;
	RendererCameraRef	m_pCamera;
	Mat44	m_mCameraView;
	Mat44	m_mCameraInvView;
	Mat44	m_mCameraProj;
	Mat44	m_mCameraInvProj;
	Mat44	m_mCameraViewProj;
	Mat44	m_mCameraInvViewProj;
	Bool	m_bWireframe;
	Color	m_NextOutline;

	RendererViewDataRef m_Views[RendererView_Count];
	RendererViewType	m_CurrentType;
	
	U32		m_iMaxTextureSize;
	S32		m_iViewportWidth;
	S32		m_iViewportHeight;
	Bool	m_bInitialized;
#ifdef USE_EDITOR_V2
	Bool	m_bEditorOutline;
	U32		m_iEditorEntityID;
	RendererPassPtr	m_EditorPickerPass;
	RendererPassPtr	m_EditorOutlinePass;
	RendererPassPtr	m_EditorPass;
#endif

	// Draw lists (double buffer to be thread safe)
	struct DrawList
	{
		ArrayOf<SDraw>		m_daAllDraws;
		ArrayOf<SDrawPtr>	m_daDrawBuffers[DRAWBUFFER_Count];
		const Material*	m_pMaterial = NULL;
		Color				m_MainColor = Color::White;
	};
	DrawList*			m_pUpdateDrawList;
	DrawList*			m_pRenderDrawList;

	Primitive3DRef	m_FullScreenQuad;

	// FrameBuffers
	RendererPassRefArray	m_Passes;
	RendererGlobalDataRef	m_pGlobalData;
	RendererModelDataRef	m_pModelData;
	TextureRef		m_SSAONoise;
	RendererPassPtrArray	m_ShadowPasses;
#ifdef USE_EDITOR_V2
	Vec2i			m_vEditorPickingRequest = Vec2i::Zero;
	U32				m_iEditorPickingResult = -1;
#endif

	// sun dir (to change to class like camera)
	ParamsTable	m_Params;
	ResourceRef		m_ParamsScript;

	TextureRef		m_DefaultEmissive;
	ResourceRef		m_BlueNoise;

	// skybox
	ResourceRef		m_SkyboxTexture;

public:
	static Bool	m_bUseMetal;
};

Bool	operator>(const Renderer::SDraw& _a,const Renderer::SDraw& _b);
Bool	operator<(const Renderer::SDraw& _a,const Renderer::SDraw& _b);
Bool	operator>(const Renderer::SDrawPtr& _a, const Renderer::SDrawPtr& _b);
Bool	operator<(const Renderer::SDrawPtr& _a, const Renderer::SDrawPtr& _b);

typedef RefTo<Renderer> RendererRef;

#endif