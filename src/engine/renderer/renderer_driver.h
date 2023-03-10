#ifndef __RENDERER_DRIVER_H__
#define __RENDERER_DRIVER_H__

enum GeometryType;
enum RenderState;
enum ClearFlag;
class Window3DScreen;
class Renderer;
class ShaderData;
class RendererPass;
class RendererSubpass;

class RendererDriver
{
public:
	RendererDriver()	{ m_pCurrentShader = NULL; }

	virtual Bool Initialize() { return TRUE; }
	virtual Bool Finalize() { return TRUE; }

	virtual Bool CreateContext(const Window3DScreen* _pScreen, Renderer* _pRenderer) { return TRUE; }
	virtual Bool DestroyContext(const Window3DScreen* _pScreen, Renderer* _pRenderer) { return TRUE; }
	virtual Bool ChangeViewport(Renderer* _pRenderer, S32 _width, S32 _height) { return TRUE; }
	virtual void WaitPreviousFrame(Renderer* _pRenderer) {}

	virtual S32 GetMaxTextureSize() = 0;
	virtual Mat44& GetClipSpaceMatrix() const { return Mat44::Identity; }
	virtual Mat44& GetFramebufferMatrix() const { 
		static Mat44 m(
			0.5f, 0.f, 0.f, 0.5f,
			0.f, 0.5f, 0.f, 0.5f,
			0.f, 0.f, 0.5f, 0.5f,
			0.f, 0.f, 0.f, 1.f
		); 
		return m;
	}

	virtual void BeginFrame(Renderer* _pRenderer) {}
	virtual void EndFrame() {}
	virtual void SetWireframe(Bool _bWireframe) = 0;
	virtual void EnableAdditive() =0;
	virtual void DisableAdditive() = 0;
	virtual void EnableClipRect(Renderer* _pRenderer, const Rect& _rect) = 0;
	virtual void DisableClipRect() = 0;
	virtual void AddTexture(Texture* _pTexture) = 0;
	virtual void RemoveTexture(Texture* _pTexture) = 0;
	virtual void UpdatePrimitive(Primitive3D* _pPrimitive) = 0;
	virtual void RemovePrimitive(void* _pDriverData) = 0;
	virtual void Swap(Renderer* _pRenderer) = 0;

	virtual void CompileShaderGroup(ShaderGroup* _shader) = 0;
	virtual void RemoveShaderGroup(ShaderGroup* _shader) = 0;
	virtual void UseShaderGroup(const ShaderGroup* _shaderGroup) { m_pCurrentShader = _shaderGroup; }
	virtual S32 GetShaderAttribute(ShaderGroup* _shaderGroup, const Char* _name) = 0;
	virtual S32 GetShaderUniform(ShaderGroup* _shaderGroup, const Char* _name) = 0;
	virtual S32 GetShaderUniformData(ShaderGroup* _shaderGroup, const Char* _name) = 0;
	virtual void ActiveTexture(U32 _slot, const ShaderGroup::Params& _params, Texture* _pTexture) = 0;
	virtual void PushShaderData(ShaderData* _pData) = 0;
	virtual void UseShaderData(const ShaderGroup::Params& _params, ShaderData* _pData, U32 _iSlot) = 0;
	virtual void PushDrawShaderData(const ShaderGroup::Params& _params, ShaderData* _pData) = 0;
	virtual void RemoveShaderData(void* _pDriverData) = 0;

	virtual void BeginPass(Renderer* _pRenderer, RendererPass* _pPass) {}
	virtual void EndPass() {}
	virtual void RemovePass(void* _pDriverData) {}
	virtual void BeginSubPass(RendererSubpass* _pSubpass) {}
	virtual void EndSubPass() {}
	virtual void RemoveSubPass(void* _pDriverData) {}
	virtual void RemoveFrameBuffer(void* _pDriverData) {}

	virtual U32 ReadPixel(FrameBuffer* _pFrameBuffer, const Vec2i& _vPos) = 0;

	INLINE void Draw(GeometryType _type, const Primitive3D* _pPrimitive)
	{
		Draw(_type, _pPrimitive, _pPrimitive->GetIB()->m_Array.GetCount(), 0);
	}
	virtual void Draw(GeometryType _type, const Primitive3D* _pPrimitive, U32 _indiceCount, U32 _ibOffset) = 0;
	
#ifdef USE_GPU_PROFILER
	virtual void ProfilerBegin(const Char* _pEvent) = 0;
	virtual void ProfilerEnd() = 0;
	static RendererDriver* g_pProfilerDriver;
#endif

	const ShaderGroup* m_pCurrentShader;
};
#endif