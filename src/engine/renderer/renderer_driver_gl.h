#ifndef __RENDERER_DRIVER_GL_H__
#define __RENDERER_DRIVER_GL_H__

#include "renderer_driver.h"
#include "gl_include.h"

// Error management
#ifdef USE_DEBUGOPENGL
class CGLCheckError
{
public:
	CGLCheckError(const Char * _name, const Char* _file, S64 _line);
	~CGLCheckError();
protected:
	const Char*	m_Name;
	const Char* m_File;
	S64 m_Line;
};
#endif

BEGIN_SUPER_CLASS(RendererDriverGL,RendererDriver)
public:
	RendererDriverGL();
	virtual Bool Initialize() OVERRIDE;
	virtual Bool Finalize() OVERRIDE;

	virtual Bool CreateContext(const Window3DScreen* _pScreen, Renderer* _pRenderer) OVERRIDE;
	virtual Bool DestroyContext(const Window3DScreen* _pScreen, Renderer* _pRenderer) OVERRIDE;
	virtual Bool ChangeViewport(Renderer* _pRenderer, S32 _width, S32 _height) OVERRIDE;

	virtual S32 GetMaxTextureSize() OVERRIDE;

	virtual void BeginFrame(Renderer* _pRenderer) OVERRIDE;
	virtual void SetWireframe(Bool _bWireframe) OVERRIDE;
	virtual void EnableAdditive() OVERRIDE;
	virtual void DisableAdditive() OVERRIDE;
	virtual void EnableClipRect(Renderer* _pRenderer, const Rect& _rect) OVERRIDE;
	virtual void DisableClipRect() OVERRIDE;
	virtual void AddTexture(Texture* _pTexture) OVERRIDE;
	virtual void RemoveTexture(Texture* _pTexture) OVERRIDE;

	virtual void UpdatePrimitive(Primitive3D* _pPrimitive) OVERRIDE;
	virtual void RemovePrimitive(void* _pDriverData) OVERRIDE;
	
	virtual void Swap(Renderer* _pRenderer) OVERRIDE;

	virtual void Draw(GeometryType _type, const Primitive3D* _pPrimitive, U32 _indiceCount, U32 _ibOffset) OVERRIDE;

	virtual void CompileShaderGroup(ShaderGroup* _shader) OVERRIDE;
	virtual void RemoveShaderGroup(ShaderGroup* _shader) OVERRIDE;
	virtual void UseShaderGroup(const ShaderGroup* _shaderGroup) OVERRIDE;
	virtual S32 GetShaderAttribute(ShaderGroup* _shaderGroup, const Char* _name) OVERRIDE;
	virtual S32 GetShaderUniform(ShaderGroup* _shaderGroup, const Char* _name) OVERRIDE;
	virtual S32 GetShaderUniformData(ShaderGroup* _shaderGroup, const Char* _name) OVERRIDE;
	virtual void ActiveTexture(U32 _slot, const ShaderGroup::Params& _params, Texture* _pTexture) OVERRIDE;
	virtual void PushShaderData(ShaderData* _pData) OVERRIDE;
	virtual void RemoveShaderData(void* _pDriverData) OVERRIDE;
	virtual void UseShaderData(const ShaderGroup::Params& _params, ShaderData* _pData, U32 _iSlot) OVERRIDE;
	virtual void PushDrawShaderData(const ShaderGroup::Params& _params, ShaderData* _pData) OVERRIDE;

	virtual void BeginPass(Renderer* _pRenderer, RendererPass* _pPass) OVERRIDE;
	virtual void BeginSubPass(RendererSubpass* _pSubpass) OVERRIDE;

	virtual U32 ReadPixel(FrameBuffer* _pFrameBuffer, const Vec2i& _vPos) OVERRIDE;

#ifdef USE_GPU_PROFILER
	virtual void ProfilerBegin(const Char* _pEvent) OVERRIDE;
	virtual void ProfilerEnd() OVERRIDE;
#endif
protected:
	void Clear(ClearFlag  _flag);
	void SetRenderState(const RenderState& _newstate);

	void AddFrameBuffer(FrameBuffer* _pFrameBuffer);
	void RemoveFrameBuffer(FrameBuffer* _pFrameBuffer);
	
	void	SetRenderStateValue(const RenderState& _flag, Bool _bEnable);
	void	CompileShader(Shader* _shader, Shader* _header);
	void	DisplayGLError(const Str& _error, const Str& _codeHeader, const Shader* _header, const Shader* _shader, const LogType& _logType);

	RenderState	m_CurrentRenderState;
	GLuint	m_iCurrentFBO;

	Bool m_bFirstFrame;

	HGLRC	m_SharedRC;
END_SUPER_CLASS
#endif