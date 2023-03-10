#ifndef __RENDERER_DRIVER_VULKAN_H__
#define __RENDERER_DRIVER_VULKAN_H__

#ifdef USE_VULKAN

#include "platform/platform_win32.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include INCLUDE_LIB(LIB_VULKAN, Include\vulkan\vulkan.h)
#pragma LINK_LIB(LIB_VULKAN, Lib/vulkan-1.lib)

#include "renderer_driver.h"

struct SDriverVKRendererData;
struct SDriverVKBufferData;
struct SDriverVKImageData;
struct SDriverVKCommandBufferData;

BEGIN_SUPER_CLASS(RendererDriverVulkan,RendererDriver)
public:
	virtual Bool Initialize() OVERRIDE;
	virtual Bool Finalize() OVERRIDE;

	virtual Bool CreateContext(const Window3DScreen* _pScreen, Renderer* _pRenderer) OVERRIDE;
	virtual Bool DestroyContext(const Window3DScreen* _pScreen, Renderer* _pRenderer) OVERRIDE;
	virtual Bool ChangeViewport(Renderer* _pRenderer, S32 _width, S32 _height) OVERRIDE;
	virtual void WaitPreviousFrame(Renderer* _pRenderer) OVERRIDE;

	virtual S32 GetMaxTextureSize() OVERRIDE;
	virtual Mat44& GetClipSpaceMatrix() const OVERRIDE { 
		static Mat44 m(
			1.f, 0.f, 0.f, 0.f,
			0.f, -1.f, 0.f, 0.f,
			0.f, 0.f, 0.5f, 0.5f,
			0.f, 0.f, 0.f, 1.f
		);
		return m;
	}
	virtual Mat44& GetFramebufferMatrix() const {
		static Mat44 m(
			0.5f, 0.f, 0.f, 0.5f,
			0.f, 0.5f, 0.f, 0.5f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		);
		return m;
	}
	virtual Bool UseReverseTexture() const { return TRUE; }

	virtual void BeginFrame(Renderer* _pRenderer) OVERRIDE;
	virtual void EndFrame() OVERRIDE;
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
	virtual void EndPass() OVERRIDE;
	virtual void RemovePass(void* _pDriverData) OVERRIDE;
	virtual void BeginSubPass(RendererSubpass* _pSubpass) OVERRIDE;
	virtual void EndSubPass() OVERRIDE;
	virtual void RemoveSubPass(void* _pDriverData) OVERRIDE;
	virtual void RemoveFrameBuffer(void* _pDriverData) OVERRIDE;

	virtual U32 ReadPixel(FrameBuffer* _pFrameBuffer, const Vec2i& _vPos) OVERRIDE;

#ifdef USE_GPU_PROFILER
	virtual void ProfilerBegin(const Char* _pEvent) OVERRIDE;
	virtual void ProfilerEnd() OVERRIDE;
#endif
private:
	Bool				FindPhysicalDevice();
	Bool				CreateDevice();
	Bool				DestroyDevice();
	Bool				CreateSwapchain(SDriverVKRendererData* _pRendererData, const Window3DScreen* _pScreen);
	Bool				DestroySwapchain(SDriverVKRendererData* _pRendererData);
	Bool				CreateDefaultRenderPass(SDriverVKRendererData* _pRendererData);
	Bool				DestroyDefaultRenderPass(SDriverVKRendererData* _pRendererData);
	Bool				CreateDefaultFramebuffers(SDriverVKRendererData* _pRendererData);
	Bool				DestroyDefaultFramebuffers(SDriverVKRendererData* _pRendererData);
	
	Bool				CreateDescriptorPool(SDriverVKRendererData* _pRendererData);
	Bool				DestroyDescriptorPool(SDriverVKRendererData* _pRendererData);
	Bool				CreateCommandPool(SDriverVKRendererData* _pRendererData);
	Bool				DestroyCommandPool(SDriverVKRendererData* _pRendererData);
	
	Bool				CreateSyncObjects(SDriverVKRendererData* _pRendererData);
	Bool				DestroySyncObjects(SDriverVKRendererData* _pRendererData);

	Bool				CreateCommandBuffer(SDriverVKCommandBufferData* _pBuffer);
	Bool				BeginCommandBuffer(SDriverVKCommandBufferData* _pBuffer);
	Bool				EndCommandBuffer(SDriverVKCommandBufferData* _pBuffer);
	Bool				DestroyCommandBuffer(SDriverVKCommandBufferData* _pBuffer);

	Bool				CreateBuffer(SDriverVKBufferData* _pBufferData);
	Bool				CopyToBuffer(const void* _pData, size_t _iDataSize, SDriverVKBufferData* _pBufferData);
	Bool				CopyBuffer(SDriverVKBufferData* _pFrom, SDriverVKBufferData* _pTo);
	Bool				DestroyBuffer(SDriverVKBufferData* _pBufferData);

	Bool				CopyImageData(const void* _pData, size_t _iDataSize, SDriverVKImageData* _pImage);
	Bool				CopyBufferToImage(SDriverVKBufferData* _pBuffer, SDriverVKImageData* _pImage);
	Bool				CopyImageToBuffer(SDriverVKImageData* _pImage, SDriverVKBufferData* _pBuffer);
	Bool				ChangeImageLayout(SDriverVKImageData* _pImage, VkImageLayout _from, VkImageLayout _to);

	Bool				CompileShader(Shader* _shader, Shader* _header);
	Bool				CreatePass(RendererPass* _pPass);
	Bool				DestroyPass(void* _pDriverData);
	Bool				CreateSubpass(RendererSubpass* _pSubpass);
	Bool				DestroySubpass(void* _pDriverData);
	Bool				PrepareFrameBuffer(FrameBuffer* _pFrameBuffer);
	Bool				CreateFrameBuffer(FrameBuffer* _pFrameBuffer);
	Bool				DestroyFrameBuffer(void* _pDriverData);

	void				BindDescriptorSet();
	void				BindPipeline();
#ifdef USE_GPU_PROFILER
	void SetObjectName(uint64_t _object, VkObjectType _objectType, const char* _pName);
#endif

	VkInstance			m_Instance			= VK_NULL_HANDLE;
	VkPhysicalDevice	m_PhysicalDevice	= VK_NULL_HANDLE;
	VkDevice			m_Device			= VK_NULL_HANDLE;
	VkQueue				m_Queue				= VK_NULL_HANDLE;
	U32					m_iQueueFamilyIndex = -1;

	Renderer*			m_pCurrentRenderer	= NULL;
	RendererPass*		m_pCurrentPass		= NULL;
	RendererSubpass*	m_pCurrentSubpass	= NULL;
	VkCommandBuffer		m_FrameCommandBuffer = VK_NULL_HANDLE;

	const VkAllocationCallbacks* m_pAllocator	= NULL;
#ifdef USE_DEBUGVULKAN
	VkDebugUtilsMessengerEXT m_DebugMessenger	= VK_NULL_HANDLE;
#endif
END_SUPER_CLASS
#endif

#endif