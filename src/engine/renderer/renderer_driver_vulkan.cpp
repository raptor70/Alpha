#include "renderer_driver_vulkan.h"
#ifdef USE_VULKAN

//#include "platform/platform_win32.h"
#include "2d/font.h"
#include "renderer/shader.h"
#include "core/sort.h"
#include "window/window_3dscreen.h"
#include "math/spline.h"
#include "resource/resource_manager.h"

#include "3d/material.h"
#include "3d/vertex_buffer.h"
#include "3d/index_buffer.h"
#include "renderer/shader_group.h"
#include "file/base_file.h"

#if defined(USE_DEBUGVULKAN) || defined(USE_GPU_PROFILER)
#define VK_NEED_DEBUG_UTILS
#endif

//extensions
#define VK_EXT_EXTENSION(_func) PFN_##_func _func;
#include "vk_extension.h"
#undef VK_EXT_EXTENSION

#ifdef USE_DEBUGVULKAN
Bool VKCHECK(VkResult ret)
{
	if (ret != VK_SUCCESS)
	{
		const Char* pErr = "Unknown";
		switch (ret)
		{
#define VKERROR(_err)	case _err : pErr = #_err; break
			VKERROR(VK_NOT_READY);
			VKERROR(VK_TIMEOUT);
			VKERROR(VK_EVENT_SET);
			VKERROR(VK_EVENT_RESET);
			VKERROR(VK_INCOMPLETE);
			VKERROR(VK_ERROR_OUT_OF_HOST_MEMORY);
			VKERROR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
			VKERROR(VK_ERROR_INITIALIZATION_FAILED);
			VKERROR(VK_ERROR_DEVICE_LOST);
			VKERROR(VK_ERROR_MEMORY_MAP_FAILED);
			VKERROR(VK_ERROR_LAYER_NOT_PRESENT);
			VKERROR(VK_ERROR_EXTENSION_NOT_PRESENT);
			VKERROR(VK_ERROR_FEATURE_NOT_PRESENT);
			VKERROR(VK_ERROR_INCOMPATIBLE_DRIVER);
			VKERROR(VK_ERROR_TOO_MANY_OBJECTS);
			VKERROR(VK_ERROR_FORMAT_NOT_SUPPORTED);
			VKERROR(VK_ERROR_FRAGMENTED_POOL);
			VKERROR(VK_ERROR_UNKNOWN);
			VKERROR(VK_ERROR_OUT_OF_POOL_MEMORY);
			VKERROR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
			VKERROR(VK_ERROR_FRAGMENTATION);
			VKERROR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
			VKERROR(VK_PIPELINE_COMPILE_REQUIRED);
			VKERROR(VK_ERROR_SURFACE_LOST_KHR);
			VKERROR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
			VKERROR(VK_SUBOPTIMAL_KHR);
			VKERROR(VK_ERROR_OUT_OF_DATE_KHR);
			VKERROR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
			VKERROR(VK_ERROR_VALIDATION_FAILED_EXT);
			VKERROR(VK_ERROR_INVALID_SHADER_NV);
			VKERROR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
			VKERROR(VK_ERROR_NOT_PERMITTED_KHR);
			VKERROR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
			VKERROR(VK_THREAD_IDLE_KHR);
			VKERROR(VK_THREAD_DONE_KHR);
			VKERROR(VK_OPERATION_DEFERRED_KHR);
			VKERROR(VK_OPERATION_NOT_DEFERRED_KHR);
#undef VKERROR
		}


		LOGGER_LogError("Vulkan error : %s\n", pErr);
		DEBUG_Break();
		return FALSE;
	}
	return TRUE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL VK_DebugCall(VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity, VkDebugUtilsMessageTypeFlagsEXT _messageType, const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, void* _pUserData) 
{
	const Char* pType = "Unknown";
	switch(_messageType)
	{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: pType = "General"; break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: pType = "Validation"; break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: pType = "Performance"; break;
	}

	if (_messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		LOGGER_LogError("Vulkan %s error: %s\n", pType, _pCallbackData->pMessage);
		DEBUG_Break();
	}
	else if (_messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		LOGGER_LogWarning("Vulkan %s warning: %s\n", pType, _pCallbackData->pMessage);
	}
	else
	{
		LOGGER_Log("Vulkan %s info: %s\n", pType, _pCallbackData->pMessage);
	}

	//_pCallbackData->pObjects : handles

	return VK_FALSE;
}
#else
#define VKCHECK(_ret)	(_ret == VK_SUCCESS)
#endif

//----------------------------------

struct SDriverVKCommandBufferData
{
	Bool					m_bSingleTimeCommand = FALSE;
	VkCommandPool			m_CommandPool;
	VkCommandBuffer			m_CommandBuffer;
};

//----------------------------------

struct SDriverVKRendererData
{
	VkSurfaceKHR			m_Surface	= VK_NULL_HANDLE;
	VkExtent2D				m_Extent;
	VkFormat				m_Format;
	VkSwapchainKHR			m_SwapChain	= VK_NULL_HANDLE;
	ArrayOf<VkImageView>	m_SwapChainImageViews;
	ArrayOf<VkFramebuffer>	m_SwapChainImageFramebuffers;
	VkRenderPass			m_DefaultRenderPass;
	ArrayOf<VkDescriptorSetLayoutBinding> m_ShaderDataBindings;
	VkDescriptorPool		m_DescriptorPool;
	VkCommandPool			m_CommandPool;
	SDriverVKCommandBufferData	m_CommandBuffer;
	VkSemaphore				m_ImageAvailableSemaphore;
	VkSemaphore				m_RenderFinishedSemaphore;
	VkFence					m_InFlightFence;
	uint32_t				m_ImageIdx;

	VkFramebuffer			m_DefaultFramebuffer;
};

//----------------------------------

struct SDriverVKShaderData
{
	VkShaderModule			m_Shader = VK_NULL_HANDLE;
};

//----------------------------------

struct SDriverVKBufferData
{
	VkDeviceSize			m_Size = 0;
	VkBufferUsageFlags		m_Usage = 0;
	VkMemoryPropertyFlags	m_Properties = 0;
	VkBuffer				m_Buffer = VK_NULL_HANDLE;
	VkDeviceMemory			m_BufferMemory = VK_NULL_HANDLE;
};

//----------------------------------

struct SDriverVKImageData
{
	VkExtent3D				m_Extent;
	VkFormat				m_Format;
	VkImageLayout			m_Layout;
	VkImage					m_Image = VK_NULL_HANDLE;
	VkImageView				m_ImageView = VK_NULL_HANDLE;
	VkDeviceMemory			m_ImageMemory = VK_NULL_HANDLE;
};

//----------------------------------

struct SDriverVKTextureData
{
	SDriverVKImageData		m_Image;
	VkSampler				m_Sampler = VK_NULL_HANDLE;
};

//----------------------------------

struct SDriverVKPrimitiveData
{
	SDriverVKBufferData m_VertexBuffer;
	SDriverVKBufferData m_IndexBuffer;
};

//----------------------------------

struct SDriverVKShaderDataData
{
	SDriverVKBufferData		m_Buffer;
};

//----------------------------------

struct SDriverVKPassData
{
	VkViewport				m_Viewport;
	VkRenderPass			m_RenderPass = VK_NULL_HANDLE;
	ArrayOf<VkClearValue>	m_ClearValues;
	Bool					m_bHasDepthStencil = FALSE;
	Bool					m_bDefaultPass = FALSE;
};

//----------------------------------

struct SDriverVKFrameBufferData
{
	ArrayOf<VkAttachmentDescription>	m_AttachmentsDesc;
	VkExtent2D							m_Extent;
	VkFramebuffer						m_FrameBuffer = VK_NULL_HANDLE;
};

//----------------------------------

struct SDriverVKPipelineKey
{
	VkPrimitiveTopology					m_Topology;

	Bool operator==(const SDriverVKPipelineKey& _other) const { return (memcmp(this, &_other, sizeof(SDriverVKPipelineKey)) == 0); }
	Bool operator!=(const SDriverVKPipelineKey& _other) const { return !operator==(_other); }
};

//----------------------------------

struct SDriverVKPipelineInstance
{
	SDriverVKPipelineKey				m_Key = {};
	VkPipeline							m_Pipeline = VK_NULL_HANDLE;
};

//----------------------------------

struct SDriverVKDescriptorSetKey
{
	struct Slot
	{
		void*	m_pData;
		Bool	m_bTexture; // lese ShaderData
	};

	Slot	m_Slots[20];

	Bool operator==(const SDriverVKDescriptorSetKey& _other) const { return (memcmp(this, &_other, sizeof(SDriverVKDescriptorSetKey)) == 0); }
	Bool operator!=(const SDriverVKDescriptorSetKey& _other) const { return !operator==(_other); }
};

//----------------------------------

struct SDriverVKDescriptorSetInstance
{
	SDriverVKDescriptorSetKey		m_Key = {};
	VkDescriptorSet					m_DescriptorSet = VK_NULL_HANDLE;
};

//----------------------------------

struct SDriverVKSubpassData
{



	VkDescriptorSetLayout							m_DescriptorSetLayout = VK_NULL_HANDLE;
	SDriverVKDescriptorSetKey						m_DescriptorSetCurrentKey = {};
	SDriverVKDescriptorSetKey						m_DescriptorSetWantedKey = {};
	ArrayOf<SDriverVKDescriptorSetInstance>			m_DescriptorSetInstances;

	VkPipelineLayout								m_PipelineLayout = VK_NULL_HANDLE;
	VkPipelineShaderStageCreateInfo					m_PipelineShaderStages[2] = {};
	VkVertexInputBindingDescription					m_PipelineBindingDescription = {};
	ArrayOf<VkVertexInputAttributeDescription>		m_PipelineAttributeDescriptions;
	VkPipelineVertexInputStateCreateInfo			m_PipelineVertexInputInfo = {};
	VkPipelineInputAssemblyStateCreateInfo			m_PipelineInputAssembly = {};
	VkRect2D										m_PipelineScissor = {};
	VkPipelineViewportStateCreateInfo				m_PipelineViewportState = {};
	VkPipelineRasterizationStateCreateInfo			m_PipelineRasterizer = {};
	VkPipelineMultisampleStateCreateInfo			m_PipelineMultisampling = {};
	VkPipelineDepthStencilStateCreateInfo			m_PipelineDepthStencil = {};
	ArrayOf<VkPipelineColorBlendAttachmentState>	m_PipelineColorBlendAttachments;
	VkPipelineColorBlendStateCreateInfo				m_PipelineColorBlending = {};
	VkPipelineDynamicStateCreateInfo				m_PipelineDynamicState = {};
	VkGraphicsPipelineCreateInfo					m_PipelineInfo = {};
	SDriverVKPipelineKey							m_PipelineCurrentKey = {};
	SDriverVKPipelineKey							m_PipelineWantedKey = {};
	ArrayOf<SDriverVKPipelineInstance>				m_PipelineInstances;
	
};

//----------------------------------

#define VK_TEXTURE_BINDING_OFFSET	10

//----------------------------------

const Char* g_InstanceExtensions[] =
{
	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#ifdef VK_NEED_DEBUG_UTILS
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
};

//----------------------------------

const Char* g_DeviceExtensions[] =
{
	VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

//----------------------------------

VkDynamicState g_DynamicStates[] =
{
	VK_DYNAMIC_STATE_SCISSOR
};

//----------------------------------

#ifdef USE_DEBUGVULKAN
const Char* g_Layers[] =
{
	"VK_LAYER_KHRONOS_validation"
};
S32 g_LayerCount = _countof(g_Layers);
#else
const Char** g_Layers = NULL;
S32 g_LayerCount = 0;
#endif

PFN_vkCreateInstance vkCreateInstance = NULL;
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = NULL;

//----------------------------------

Bool RendererDriverVulkan::Initialize()
{
	if (!SUPER::Initialize())
		return FALSE;

	HMODULE vulkan_module = LoadLibrary("vulkan-1.dll");
	DEBUG_Require(vulkan_module);
	vkCreateInstance = (PFN_vkCreateInstance)GetProcAddress(vulkan_module, "vkCreateInstance");
	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkan_module, "vkGetInstanceProcAddr");

#ifdef USE_DEBUGVULKAN
	VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
	debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugInfo.pfnUserCallback = VK_DebugCall;
#endif

	S32 nbExtension = 0;
	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = NULL; // TODO : appinfo
	instanceInfo.enabledExtensionCount = _countof(g_InstanceExtensions);
	instanceInfo.ppEnabledExtensionNames = g_InstanceExtensions;
	instanceInfo.enabledLayerCount = g_LayerCount;
	instanceInfo.ppEnabledLayerNames = g_Layers;
#ifdef USE_DEBUGVULKAN
	instanceInfo.pNext = &debugInfo;
#else
	instanceInfo.pNext = NULL;
#endif
	if (!VKCHECK(vkCreateInstance(&instanceInfo, m_pAllocator, &m_Instance)))
		return FALSE;

#define VK_EXT_EXTENSION(_func) \
	_func = (PFN_##_func)vkGetInstanceProcAddr(m_Instance, #_func); \
	DEBUG_Require(_func);
#include "vk_extension.h"
#undef VK_EXT_EXTENSION

#ifdef USE_DEBUGVULKAN
	if (!VKCHECK(vkCreateDebugUtilsMessengerEXT(m_Instance, &debugInfo, m_pAllocator, &m_DebugMessenger)))
		return FALSE;
#endif

	if (!FindPhysicalDevice())
	{
		LOGGER_LogError("Unable to find valid Device");
		return FALSE;
	}

	if (!CreateDevice())
	{
		LOGGER_LogError("Unable to create Device");
		return FALSE;
	}
	
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::Finalize()
{
	if (!SUPER::Finalize())
		return FALSE;

	DestroyDevice();

	m_iQueueFamilyIndex = -1;
	m_PhysicalDevice = VK_NULL_HANDLE;

	// test last
#ifdef USE_DEBUGVULKAN
	vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, m_pAllocator);
#endif

	vkDestroyInstance(m_Instance, m_pAllocator);

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::FindPhysicalDevice()
{
	// physical device
	uint32_t iPhysicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &iPhysicalDeviceCount, nullptr);
	ArrayOf<VkPhysicalDevice> physicalDevices(iPhysicalDeviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &iPhysicalDeviceCount, physicalDevices.GetPointer());

	for (VkPhysicalDevice physicaldevice : physicalDevices)
	{
		// features
		//VkPhysicalDeviceFeatures deviceFeatures;
		//vkGetPhysicalDeviceFeatures(physicaldevice, &deviceFeatures);

		// check properties
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicaldevice, &deviceProperties);
		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			continue;

		// check needed extensions
		uint32_t iExtensionCount = 0;
		vkEnumerateDeviceExtensionProperties(physicaldevice, nullptr, &iExtensionCount, nullptr);
		ArrayOf<VkExtensionProperties> availableExtensions(iExtensionCount);
		vkEnumerateDeviceExtensionProperties(physicaldevice, nullptr, &iExtensionCount, availableExtensions.GetPointer());
		Bool bFoundAllExt = TRUE;
		for (const auto& neededExt : g_DeviceExtensions)
		{
			Bool bFound = FALSE;
			for (const auto& availableExt : availableExtensions)
			{
				if (strcmp(availableExt.extensionName, neededExt) == 0)
				{
					bFound = TRUE;
					break;
				}
			}

			if (!bFound)
			{
				bFoundAllExt = FALSE;
				break;
			}
		}
		if (!bFoundAllExt)
			continue;
		
		// check queues
		uint32_t iQueueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicaldevice, &iQueueFamilyCount, nullptr);
		ArrayOf<VkQueueFamilyProperties> queues(iQueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicaldevice, &iQueueFamilyCount, queues.GetPointer());
		m_iQueueFamilyIndex = -1;
		for (U32 q = 0; q < iQueueFamilyCount; q++)
		{
			VkQueueFamilyProperties queueFamily = queues[q];
			if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
				continue;

			m_iQueueFamilyIndex = q;
		}
		if (m_iQueueFamilyIndex == -1)
			continue;

		// Valid device
		m_PhysicalDevice = physicaldevice;

		LOGGER_Log("VULKAN %.d.%d.%d\n",
			VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
			VK_API_VERSION_MINOR(deviceProperties.apiVersion),
			VK_API_VERSION_PATCH(deviceProperties.apiVersion));
		LOGGER_Log("Graphic Card %s\n", deviceProperties.deviceName);
			
		return TRUE;
	}

	LOGGER_LogError("No physical graphic card found !\n");
	return FALSE;
}

//----------------------------------

Bool RendererDriverVulkan::CreateDevice()
{
	Float fQueuePriority = 1.0f;
	VkDeviceQueueCreateInfo queueInfos = {};
	queueInfos.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfos.queueFamilyIndex = m_iQueueFamilyIndex;
	queueInfos.queueCount = 1;
	queueInfos.pQueuePriorities = &fQueuePriority;

	VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamicStateExt{};
	dynamicStateExt.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
	dynamicStateExt.extendedDynamicState = true;

	VkDeviceCreateInfo deviceInfo{};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = &dynamicStateExt;
	deviceInfo.pQueueCreateInfos = &queueInfos;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.enabledExtensionCount = _countof(g_DeviceExtensions);
	deviceInfo.ppEnabledExtensionNames = g_DeviceExtensions;
	deviceInfo.enabledLayerCount = g_LayerCount;
	deviceInfo.ppEnabledLayerNames = g_Layers;


	if (!VKCHECK(vkCreateDevice(m_PhysicalDevice, &deviceInfo, m_pAllocator, &m_Device)))
		return FALSE;

	// access to queue
	vkGetDeviceQueue(m_Device, m_iQueueFamilyIndex, 0, &m_Queue);

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroyDevice()
{
	vkDestroyDevice(m_Device, m_pAllocator);
	m_Device = VK_NULL_HANDLE;
	m_Queue = VK_NULL_HANDLE;
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CreateContext(const Window3DScreen* _pScreen, Renderer* _pRenderer)
{
	if (!SUPER::CreateContext(_pScreen, _pRenderer))
		return FALSE;

	SDriverVKRendererData* pRendererData = NEW SDriverVKRendererData;
	_pRenderer->m_pDriverData = pRendererData;

	// surface
#ifdef WIN32
	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.hinstance = ::GetModuleHandle(NULL);
	surfaceInfo.hwnd = _pScreen->GetPlatformData().GetDynamicCastPtr<WIN32ComponentPlatformData>()->m_hWnd;
	if (!VKCHECK(vkCreateWin32SurfaceKHR(m_Instance, &surfaceInfo, m_pAllocator, &pRendererData->m_Surface)))
		return FALSE;
#else
	#error not supported!
#endif

	// present support (check in physical device list with fake surface ?)
	VkBool32 presentSupport = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_iQueueFamilyIndex, pRendererData->m_Surface, &presentSupport);
	if (!presentSupport)
	{
		LOGGER_LogError("Graphic card doesn't support Present\n");
		return FALSE;
	}

	if (!CreateSwapchain(pRendererData, _pScreen))
	{
		LOGGER_LogError("Unable to create Swapchain");
		return FALSE;
	}

	if (!CreateDefaultRenderPass(pRendererData))// tmp
	{
		LOGGER_LogError("Unable to create RenderPass");
		return FALSE;
	}

	if (!CreateDefaultFramebuffers(pRendererData)) // per context for FB screen
	{
		LOGGER_LogError("Unable to create Framebuffer");
		return FALSE;
	}

	// descriptors set layout
	S32 nbUniformBuffer = 4;
	pRendererData->m_ShaderDataBindings.SetEmpty();
	for (S32 i = 0; i < nbUniformBuffer; i++)
	{
		VkDescriptorSetLayoutBinding& binding = pRendererData->m_ShaderDataBindings.AddLastItem({ 0 });
		binding.binding = i;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	}

	if (!CreateDescriptorPool(pRendererData)) // per context
	{
		LOGGER_LogError("Unable to create DescriptorPool");
		return FALSE;
	}

	if (!CreateCommandPool(pRendererData)) // per context
	{
		LOGGER_LogError("Unable to create CommandPool");
		return FALSE;
	}

	pRendererData->m_CommandBuffer.m_CommandPool = pRendererData->m_CommandPool;
	if (!CreateCommandBuffer(&pRendererData->m_CommandBuffer))
	{
		LOGGER_LogError("Unable to create CommandBuffer");
		return FALSE;
	}

	if (!CreateSyncObjects(pRendererData))
	{
		LOGGER_LogError("Unable to create SyncObjects");
		return FALSE;
	}

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroyContext(const Window3DScreen* _pScreen, Renderer* _pRenderer)
{
	if (!SUPER::DestroyContext(_pScreen, _pRenderer))
		return FALSE;

	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)_pRenderer->m_pDriverData;

	DestroySyncObjects(pRendererData);
	DestroyCommandBuffer(&pRendererData->m_CommandBuffer);
	DestroyCommandPool(pRendererData);
	DestroyDescriptorPool(pRendererData);
	DestroyDefaultFramebuffers(pRendererData);
	DestroyDefaultRenderPass(pRendererData);
	DestroySwapchain(pRendererData);
	
	vkDestroySurfaceKHR(m_Instance, pRendererData->m_Surface, m_pAllocator);

	DELETE pRendererData;
	_pRenderer->m_pDriverData = NULL;

	return TRUE;
}

//----------------------------------

Bool	RendererDriverVulkan::CreateSwapchain(SDriverVKRendererData* _pRendererData, const Window3DScreen* _pScreen)
{
	// surface formats
	uint32_t iFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, _pRendererData->m_Surface, &iFormatCount, nullptr);
	ArrayOf<VkSurfaceFormatKHR> formats(iFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, _pRendererData->m_Surface, &iFormatCount, formats.GetPointer());
	VkSurfaceFormatKHR* pSurfaceFormat = &formats[0];
	for (U32 f = 0; f < iFormatCount; f++)
	{
		VkSurfaceFormatKHR& format = formats[f];
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			pSurfaceFormat = &format;
			break;
		}
	}

	// present modes
	uint32_t iPresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, _pRendererData->m_Surface, &iPresentModeCount, nullptr);
	ArrayOf<VkPresentModeKHR> presentModes(iPresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, _pRendererData->m_Surface, &iPresentModeCount, presentModes.GetPointer());
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (auto& mode : presentModes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			presentMode = mode;
			break;
		}
	}

	// capabilities
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, _pRendererData->m_Surface, &surfaceCapabilities);
	_pRendererData->m_Extent = surfaceCapabilities.currentExtent;
	if (_pRendererData->m_Extent.width == (U32)-1)
	{
		_pRendererData->m_Extent.width = Clamp((uint32_t)_pScreen->GetWidth(), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		_pRendererData->m_Extent.height = Clamp((uint32_t)_pScreen->GetHeight(), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
	}
	_pRendererData->m_Format = pSurfaceFormat->format;

	// create swapchain
	VkSwapchainCreateInfoKHR swapChainInfo{};
	swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainInfo.surface = _pRendererData->m_Surface;
	swapChainInfo.minImageCount = Min(surfaceCapabilities.minImageCount + 1, surfaceCapabilities.maxImageCount);
	swapChainInfo.imageFormat = pSurfaceFormat->format;
	swapChainInfo.imageColorSpace = pSurfaceFormat->colorSpace;
	swapChainInfo.imageExtent = _pRendererData->m_Extent;
	swapChainInfo.imageArrayLayers = 1;
	swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapChainInfo.queueFamilyIndexCount = 0; // Optional
	swapChainInfo.pQueueFamilyIndices = nullptr; // Optional
	swapChainInfo.preTransform = surfaceCapabilities.currentTransform;
	swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainInfo.presentMode = presentMode;
	swapChainInfo.clipped = VK_TRUE;
	swapChainInfo.oldSwapchain = VK_NULL_HANDLE;
	if (!VKCHECK(vkCreateSwapchainKHR(m_Device, &swapChainInfo, m_pAllocator, &_pRendererData->m_SwapChain)))
		return FALSE;

	// images
	uint32_t iImageCount = 0;
	vkGetSwapchainImagesKHR(m_Device, _pRendererData->m_SwapChain, &iImageCount, nullptr);
	ArrayOf<VkImage> images(iImageCount);
	vkGetSwapchainImagesKHR(m_Device, _pRendererData->m_SwapChain, &iImageCount, images.GetPointer());
	_pRendererData->m_SwapChainImageViews.SetItemCount(iImageCount);
	for (U32 i = 0; i < iImageCount; i++)
	{
		VkImageViewCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageInfo.image = images[i];
		imageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageInfo.format = pSurfaceFormat->format;
		imageInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageInfo.subresourceRange.baseMipLevel = 0;
		imageInfo.subresourceRange.levelCount = 1;
		imageInfo.subresourceRange.baseArrayLayer = 0;
		imageInfo.subresourceRange.layerCount = 1;
		if (!VKCHECK(vkCreateImageView(m_Device, &imageInfo, m_pAllocator, &_pRendererData->m_SwapChainImageViews[i])))
			return FALSE;
	}

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroySwapchain(SDriverVKRendererData* _pRendererData)
{
	for (auto& v : _pRendererData->m_SwapChainImageViews)
	{
		vkDestroyImageView(m_Device, v, m_pAllocator);
	}
	_pRendererData->m_SwapChainImageViews.SetEmpty();

	vkDestroySwapchainKHR(m_Device, _pRendererData->m_SwapChain, m_pAllocator);
	_pRendererData->m_SwapChain = VK_NULL_HANDLE;

	return TRUE;

}

//----------------------------------

Bool RendererDriverVulkan::CreateDefaultRenderPass(SDriverVKRendererData* _pRendererData)
{
	// attachement desc
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = _pRendererData->m_Format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// attachement ref
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// sub pass
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	// sub pass dependency
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// render pass
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	if (!VKCHECK(vkCreateRenderPass(m_Device, &renderPassInfo, m_pAllocator, &_pRendererData->m_DefaultRenderPass)))
		return FALSE;

	LOGGER_Log("[ADD]%x\n", _pRendererData->m_DefaultRenderPass);

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroyDefaultRenderPass(SDriverVKRendererData* _pRendererData)
{
	LOGGER_Log("[REM]%x\n", _pRendererData->m_DefaultRenderPass);
	vkDestroyRenderPass(m_Device, _pRendererData->m_DefaultRenderPass, m_pAllocator);
	_pRendererData->m_DefaultRenderPass = VK_NULL_HANDLE;
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CreateDefaultFramebuffers(SDriverVKRendererData* _pRendererData)
{
	for (U32 i = 0; i < _pRendererData->m_SwapChainImageViews.GetCount(); i++) 
	{
		VkImageView attachments[] = 
		{
			_pRendererData->m_SwapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _pRendererData->m_DefaultRenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = _pRendererData->m_Extent.width;
		framebufferInfo.height = _pRendererData->m_Extent.height;
		framebufferInfo.layers = 1;
		VkFramebuffer& framebuffer = _pRendererData->m_SwapChainImageFramebuffers.AddLastItem();
		if (!VKCHECK(vkCreateFramebuffer(m_Device, &framebufferInfo, m_pAllocator, &framebuffer)))
			return FALSE;
	}

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroyDefaultFramebuffers(SDriverVKRendererData* _pRendererData)
{
	for (auto& fb : _pRendererData->m_SwapChainImageFramebuffers)
	{
		vkDestroyFramebuffer(m_Device, fb, m_pAllocator);
	}
	_pRendererData->m_SwapChainImageFramebuffers.SetEmpty();
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CreateDescriptorPool(SDriverVKRendererData* _pRendererData)
{
	VkDescriptorPoolSize poolSize[2] = {};
	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = 1;
	poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[1].descriptorCount = 1;
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = _countof(poolSize);
	poolInfo.pPoolSizes = poolSize;
	poolInfo.maxSets = 128;
	if (!VKCHECK(vkCreateDescriptorPool(m_Device, &poolInfo, m_pAllocator, &_pRendererData->m_DescriptorPool)))
		return FALSE;

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroyDescriptorPool(SDriverVKRendererData* _pRendererData)
{
	vkDestroyDescriptorPool(m_Device, _pRendererData->m_DescriptorPool, m_pAllocator);
	_pRendererData->m_DescriptorPool = VK_NULL_HANDLE;
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CreateCommandPool(SDriverVKRendererData* _pRendererData)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = m_iQueueFamilyIndex;
	if (!VKCHECK(vkCreateCommandPool(m_Device, &poolInfo, m_pAllocator, &_pRendererData->m_CommandPool)))
		return FALSE;

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroyCommandPool(SDriverVKRendererData* _pRendererData)
{
	vkDestroyCommandPool(m_Device, _pRendererData->m_CommandPool, m_pAllocator);
	_pRendererData->m_CommandPool = VK_NULL_HANDLE;
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CreateSyncObjects(SDriverVKRendererData* _pRendererData)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (!VKCHECK(vkCreateSemaphore(m_Device, &semaphoreInfo, m_pAllocator, &_pRendererData->m_ImageAvailableSemaphore)))
		return FALSE;
	if (!VKCHECK(vkCreateSemaphore(m_Device, &semaphoreInfo, m_pAllocator, &_pRendererData->m_RenderFinishedSemaphore)))
		return FALSE;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	if (!VKCHECK(vkCreateFence(m_Device, &fenceInfo, m_pAllocator, &_pRendererData->m_InFlightFence)))
		return FALSE;

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroySyncObjects(SDriverVKRendererData* _pRendererData)
{
	vkWaitForFences(m_Device, 1, &_pRendererData->m_InFlightFence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(m_Device, _pRendererData->m_InFlightFence, m_pAllocator);
	vkDestroySemaphore(m_Device, _pRendererData->m_RenderFinishedSemaphore, m_pAllocator);
	vkDestroySemaphore(m_Device, _pRendererData->m_ImageAvailableSemaphore, m_pAllocator);

	_pRendererData->m_InFlightFence = VK_NULL_HANDLE;
	_pRendererData->m_RenderFinishedSemaphore = VK_NULL_HANDLE;
	_pRendererData->m_ImageAvailableSemaphore = VK_NULL_HANDLE;

	return TRUE;
}

//----------------------------------

void RendererDriverVulkan::WaitPreviousFrame(Renderer* _pRenderer)
{
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)_pRenderer->m_pDriverData;
	vkWaitForFences(m_Device, 1, &pRendererData->m_InFlightFence, VK_TRUE, UINT64_MAX);
}

//----------------------------------

Bool RendererDriverVulkan::CreateCommandBuffer(SDriverVKCommandBufferData* _pBuffer)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = _pBuffer->m_CommandPool;
	allocInfo.commandBufferCount = 1;

	if (!VKCHECK(vkAllocateCommandBuffers(m_Device, &allocInfo, &_pBuffer->m_CommandBuffer)))
		return FALSE;

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::BeginCommandBuffer(SDriverVKCommandBufferData* _pBuffer)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (_pBuffer->m_bSingleTimeCommand)
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	else
		vkResetCommandBuffer(_pBuffer->m_CommandBuffer, 0); // re-use
	
	if (!VKCHECK(vkBeginCommandBuffer(_pBuffer->m_CommandBuffer, &beginInfo)))
		return FALSE;
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::EndCommandBuffer(SDriverVKCommandBufferData* _pBuffer)
{
	if (!VKCHECK(vkEndCommandBuffer(_pBuffer->m_CommandBuffer)))
		return FALSE;

	if (_pBuffer->m_bSingleTimeCommand)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_pBuffer->m_CommandBuffer;
		vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_Queue);
	}

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroyCommandBuffer(SDriverVKCommandBufferData* _pBuffer)
{
	vkFreeCommandBuffers(m_Device, _pBuffer->m_CommandPool, 1, &_pBuffer->m_CommandBuffer);
	_pBuffer->m_CommandBuffer = VK_NULL_HANDLE;

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CreateBuffer(SDriverVKBufferData* _pBufferData)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = _pBufferData->m_Size;
	bufferInfo.usage = _pBufferData->m_Usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (!VKCHECK(vkCreateBuffer(m_Device, &bufferInfo, m_pAllocator, &_pBufferData->m_Buffer)))
		return FALSE;

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_Device, _pBufferData->m_Buffer, &memRequirements);
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	uint32_t memoryTypeIdx = -1;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _pBufferData->m_Properties) == _pBufferData->m_Properties)
		{
			memoryTypeIdx = i;
			break;
		}
	}
	if (memoryTypeIdx == -1)
	{
		LOGGER_LogError("Unable to find Buffer memory Type!");
		return FALSE;
	}

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIdx;
	if (!VKCHECK(vkAllocateMemory(m_Device, &allocInfo, m_pAllocator, &_pBufferData->m_BufferMemory)))
		return FALSE;

	vkBindBufferMemory(m_Device, _pBufferData->m_Buffer, _pBufferData->m_BufferMemory, 0);

	return TRUE;
}


Bool RendererDriverVulkan::CopyToBuffer(const void* _pData, size_t _iDataSize, SDriverVKBufferData* _pBufferData)
{
	DEBUG_Require(_pData && _iDataSize > 0);
	DEBUG_Require(_pBufferData->m_Usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	// create temp buffer with CPU Access HOST_VISIBLE
	SDriverVKBufferData tempBuffer;
	tempBuffer.m_Size = _iDataSize;
	tempBuffer.m_Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	tempBuffer.m_Properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	CreateBuffer(&tempBuffer);

	// upload vb to temp buffer
	void* data;
	vkMapMemory(m_Device, tempBuffer.m_BufferMemory, 0, tempBuffer.m_Size, 0, &data);
	memcpy(data, _pData, (size_t)_iDataSize);
	vkUnmapMemory(m_Device, tempBuffer.m_BufferMemory);

	// copy temp to vb
	CopyBuffer(&tempBuffer, _pBufferData);

	// destroy temp
	DestroyBuffer(&tempBuffer);

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CopyBuffer(SDriverVKBufferData* _pFrom, SDriverVKBufferData* _pTo)
{
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;
	
	SDriverVKCommandBufferData commandBufferData;
	commandBufferData.m_CommandPool = pRendererData->m_CommandPool;
	commandBufferData.m_bSingleTimeCommand = TRUE;
	CreateCommandBuffer(&commandBufferData);
	BeginCommandBuffer(&commandBufferData);

	VkBufferCopy copyRegion{};
	copyRegion.size = _pFrom->m_Size;
	vkCmdCopyBuffer(commandBufferData.m_CommandBuffer, _pFrom->m_Buffer, _pTo->m_Buffer, 1, &copyRegion);

	EndCommandBuffer(&commandBufferData);
	DestroyCommandBuffer(&commandBufferData);

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroyBuffer(SDriverVKBufferData* _pBufferData)
{
	vkDestroyBuffer(m_Device, _pBufferData->m_Buffer, nullptr);
	vkFreeMemory(m_Device, _pBufferData->m_BufferMemory, nullptr);
	_pBufferData->m_Buffer = VK_NULL_HANDLE;
	_pBufferData->m_BufferMemory = VK_NULL_HANDLE;
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CopyImageData(const void* _pData, size_t _iDataSize, SDriverVKImageData* _pImage)
{
	SDriverVKBufferData tempBuffer;
	tempBuffer.m_Size = _iDataSize;
	tempBuffer.m_Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	tempBuffer.m_Properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	CreateBuffer(&tempBuffer);

	void* data;
	vkMapMemory(m_Device, tempBuffer.m_BufferMemory, 0, _iDataSize, 0, &data);
	memcpy(data, _pData, _iDataSize);
	vkUnmapMemory(m_Device, tempBuffer.m_BufferMemory);

	CopyBufferToImage(&tempBuffer, _pImage);

	DestroyBuffer(&tempBuffer);
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CopyBufferToImage(SDriverVKBufferData* _pBuffer, SDriverVKImageData* _pImage)
{
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;

	ChangeImageLayout(_pImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	
	SDriverVKCommandBufferData commandBufferData;
	commandBufferData.m_CommandPool = pRendererData->m_CommandPool;
	commandBufferData.m_bSingleTimeCommand = TRUE;
	CreateCommandBuffer(&commandBufferData);
	BeginCommandBuffer(&commandBufferData);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = _pImage->m_Extent;
	vkCmdCopyBufferToImage(commandBufferData.m_CommandBuffer, _pBuffer->m_Buffer, _pImage->m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	EndCommandBuffer(&commandBufferData);
	DestroyCommandBuffer(&commandBufferData);

	ChangeImageLayout(_pImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CopyImageToBuffer(SDriverVKImageData* _pImage, SDriverVKBufferData* _pBuffer)
{
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;

	ChangeImageLayout(_pImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	SDriverVKCommandBufferData commandBufferData;
	commandBufferData.m_CommandPool = pRendererData->m_CommandPool;
	commandBufferData.m_bSingleTimeCommand = TRUE;
	CreateCommandBuffer(&commandBufferData);
	BeginCommandBuffer(&commandBufferData);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = _pImage->m_Extent;
	vkCmdCopyImageToBuffer(commandBufferData.m_CommandBuffer, _pImage->m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _pBuffer->m_Buffer, 1, &region);

	EndCommandBuffer(&commandBufferData);
	DestroyCommandBuffer(&commandBufferData);

	ChangeImageLayout(_pImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::ChangeImageLayout(SDriverVKImageData* _pImage, VkImageLayout _from, VkImageLayout _to)
{
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;
	SDriverVKCommandBufferData commandBufferData;
	commandBufferData.m_CommandPool = pRendererData->m_CommandPool;
	commandBufferData.m_bSingleTimeCommand = TRUE;
	CreateCommandBuffer(&commandBufferData);
	BeginCommandBuffer(&commandBufferData);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = _from;
	barrier.newLayout = _to;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = _pImage->m_Image;
	barrier.subresourceRange.aspectMask = (_pImage->m_Format == VK_FORMAT_D24_UNORM_S8_UINT) ? (VK_IMAGE_ASPECT_DEPTH_BIT| VK_IMAGE_ASPECT_STENCIL_BIT ) : VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	switch (_from)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED:
		{
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		default:
		{
			DEBUG_Forbidden("unsupported FROM layout transition!");
		}
	}

	switch (_to)
	{
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		}
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			break;
		}
		default:
		{
			DEBUG_Forbidden("unsupported TO layout transition!");
		}
	}

	vkCmdPipelineBarrier(
		commandBufferData.m_CommandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	EndCommandBuffer(&commandBufferData);
	DestroyCommandBuffer(&commandBufferData);

	_pImage->m_Layout = _to;

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::ChangeViewport(Renderer* _pRenderer, S32 _width, S32 _height)
{
	if (!SUPER::ChangeViewport(_pRenderer, _width, _height))
		return FALSE;

	return TRUE;
}

//----------------------------------

S32 RendererDriverVulkan::GetMaxTextureSize()
{
	return 0;
}

//----------------------------------

void RendererDriverVulkan::BeginFrame(Renderer* _pRenderer)
{
	m_pCurrentRenderer = _pRenderer;

	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)_pRenderer->m_pDriverData;

	//Wait previous frame
	vkWaitForFences(m_Device, 1, &pRendererData->m_InFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_Device, 1, &pRendererData->m_InFlightFence);

	// get image from swap
	vkAcquireNextImageKHR(m_Device, pRendererData->m_SwapChain, UINT64_MAX, pRendererData->m_ImageAvailableSemaphore, VK_NULL_HANDLE, &pRendererData->m_ImageIdx);

	//vkResetDescriptorPool(m_Device, pRendererData->m_DescriptorPool,0);

	// buffer recording
	if (!BeginCommandBuffer(&pRendererData->m_CommandBuffer))
		return;

	m_FrameCommandBuffer = pRendererData->m_CommandBuffer.m_CommandBuffer;
}

//----------------------------------

void RendererDriverVulkan::EndFrame()
{
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;

	// end command buffer
	m_FrameCommandBuffer = VK_NULL_HANDLE;
	if (!EndCommandBuffer(&pRendererData->m_CommandBuffer))
		return;
	
	// submit command buffer
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { pRendererData->m_ImageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { pRendererData->m_RenderFinishedSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &pRendererData->m_CommandBuffer.m_CommandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	if (!VKCHECK(vkQueueSubmit(m_Queue, 1, &submitInfo, pRendererData->m_InFlightFence)))
		return;

	// present
	VkSwapchainKHR swapChains[] = { pRendererData->m_SwapChain };
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &pRendererData->m_ImageIdx;
	vkQueuePresentKHR(m_Queue, &presentInfo);

	m_pCurrentRenderer = NULL;
}

//----------------------------------

void RendererDriverVulkan::SetWireframe(Bool _bWireframe)
{
}

//----------------------------------

void RendererDriverVulkan::EnableAdditive()
{
}

//----------------------------------

void RendererDriverVulkan::DisableAdditive()
{
}

//----------------------------------

void RendererDriverVulkan::EnableClipRect(Renderer* _pRenderer, const Rect& _rect)
{
	VkRect2D rect;
	rect.extent.width = (uint32_t)_rect.m_vSize.x;
	rect.extent.height = (uint32_t)_rect.m_vSize.y;
	rect.offset.x = (uint32_t)_rect.m_vTopLeft.x;
	rect.offset.y = (uint32_t)_rect.m_vTopLeft.y;
	vkCmdSetScissor(m_FrameCommandBuffer, 0, 1, &rect);
}

//----------------------------------

void RendererDriverVulkan::DisableClipRect()
{
	SDriverVKSubpassData* pSubpassData = (SDriverVKSubpassData*)m_pCurrentSubpass->m_pDriverData;
	vkCmdSetScissor(m_FrameCommandBuffer, 0, 1, &pSubpassData->m_PipelineScissor);
}

//----------------------------------

void RendererDriverVulkan::Swap(Renderer* _pRenderer)
{
}

//----------------------------------

void RendererDriverVulkan::Draw(GeometryType _type, const Primitive3D* _pPrimitive, U32 _indiceCount, U32 _ibOffset)
{
	UpdatePrimitive((Primitive3D*)_pPrimitive);
	
	SDriverVKSubpassData* pSubpassData = (SDriverVKSubpassData*)m_pCurrentSubpass->m_pDriverData;
	
	// topology
	switch (_type)
	{
	case Geometry_TrianglesList:	pSubpassData->m_PipelineWantedKey.m_Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
	case Geometry_TrianglesStrip:	pSubpassData->m_PipelineWantedKey.m_Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
	case Geometry_Lines:			pSubpassData->m_PipelineWantedKey.m_Topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
	default:	DEBUG_Forbidden("Not supported"); break;
	}

	BindPipeline();
	BindDescriptorSet();	

	SDriverVKPrimitiveData* pData = (SDriverVKPrimitiveData*)_pPrimitive->m_pDriverData;
	VkBuffer vertexBuffers[] = { pData->m_VertexBuffer.m_Buffer };
	VkDeviceSize offsets[] = { 0 };
	//vkCmdSetPrimitiveTopologyEXT(m_FrameCommandBuffer, topology);
	vkCmdBindVertexBuffers(m_FrameCommandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(m_FrameCommandBuffer, pData->m_IndexBuffer.m_Buffer, _ibOffset * sizeof(U16), VK_INDEX_TYPE_UINT16);
	
	vkCmdDrawIndexed(m_FrameCommandBuffer, static_cast<uint32_t>(_indiceCount), 1, 0, 0, 0);
}

//----------------------------------

void RendererDriverVulkan::BindDescriptorSet()
{
	// need new descriptor set ?
	SDriverVKSubpassData* pSubpassData = (SDriverVKSubpassData*)m_pCurrentSubpass->m_pDriverData;
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;
	if (pSubpassData->m_DescriptorSetWantedKey != pSubpassData->m_DescriptorSetCurrentKey)
	{
		//
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

		// try find new pipeline in list from key
		for (SDriverVKDescriptorSetInstance& inst : pSubpassData->m_DescriptorSetInstances)
		{
			if (inst.m_Key == pSubpassData->m_DescriptorSetWantedKey)
			{
				descriptorSet = inst.m_DescriptorSet;
				break;
			}
		}

		// create new one
		if (descriptorSet == VK_NULL_HANDLE)
		{
			// new one
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = pRendererData->m_DescriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &pSubpassData->m_DescriptorSetLayout;
			if (!VKCHECK(vkAllocateDescriptorSets(m_Device, &allocInfo, &descriptorSet)))
				return;

			// update it
			ArrayOf<VkWriteDescriptorSet> descriptorWrites;
			ArrayOf<VkDescriptorBufferInfo> bufferInfos;
			ArrayOf<VkDescriptorImageInfo> imageInfos;
			bufferInfos.ReserveCount(32);
			imageInfos.ReserveCount(32);
			for (U32 s = 0; s <_countof(pSubpassData->m_DescriptorSetWantedKey.m_Slots); s++)
			{
				SDriverVKDescriptorSetKey::Slot& slot = pSubpassData->m_DescriptorSetWantedKey.m_Slots[s];
				if (!slot.m_pData)
					continue;

				VkWriteDescriptorSet& descriptorWrite = descriptorWrites.AddLastItem();
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = descriptorSet;
				descriptorWrite.dstBinding = s;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorCount = 1;
				if (slot.m_bTexture)
				{
					SDriverVKTextureData* pTextureData = (SDriverVKTextureData*)slot.m_pData;
					VkDescriptorImageInfo& imageInfo = imageInfos.AddLastItem();
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView = pTextureData->m_Image.m_ImageView;
					imageInfo.sampler = pTextureData->m_Sampler;
					descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorWrite.pImageInfo = &imageInfo;
				}
				else
				{
					SDriverVKShaderDataData* pShaderDataData = (SDriverVKShaderDataData*)slot.m_pData;
					VkDescriptorBufferInfo& bufferInfo = bufferInfos.AddLastItem();
					bufferInfo.buffer = pShaderDataData->m_Buffer.m_Buffer;
					bufferInfo.offset = 0;
					bufferInfo.range = pShaderDataData->m_Buffer.m_Size;
					descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrite.pBufferInfo = &bufferInfo;
				}
			}

			// really update it
			vkUpdateDescriptorSets(m_Device, descriptorWrites.GetCount(), descriptorWrites.GetPointer(), 0, nullptr);
			
			// create new instance
			SDriverVKDescriptorSetInstance& newInstance = pSubpassData->m_DescriptorSetInstances.AddLastItem();
			newInstance.m_Key = pSubpassData->m_DescriptorSetWantedKey;
			newInstance.m_DescriptorSet = descriptorSet;
		}

		// bind it
		vkCmdBindDescriptorSets(m_FrameCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pSubpassData->m_PipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		pSubpassData->m_DescriptorSetCurrentKey = pSubpassData->m_DescriptorSetWantedKey;
	}
}

//----------------------------------

void RendererDriverVulkan::BindPipeline()
{
	SDriverVKSubpassData* pSubpassData = (SDriverVKSubpassData*)m_pCurrentSubpass->m_pDriverData;
	if (pSubpassData->m_PipelineWantedKey != pSubpassData->m_PipelineCurrentKey)
	{
		VkPipeline pipeline = VK_NULL_HANDLE;

		// try find new pipeline in list from key
		for (SDriverVKPipelineInstance& inst : pSubpassData->m_PipelineInstances)
		{
			if (inst.m_Key == pSubpassData->m_PipelineWantedKey)
			{
				pipeline = inst.m_Pipeline;
				break;
			}
		}

		// create new one
		if (pipeline == VK_NULL_HANDLE)
		{
			// update setup from key
			pSubpassData->m_PipelineInputAssembly.topology = pSubpassData->m_PipelineWantedKey.m_Topology;

			// create pipeline
			if (!VKCHECK(vkCreateGraphicsPipelines(m_Device, /*pipeline_cache*/VK_NULL_HANDLE, 1, &pSubpassData->m_PipelineInfo, m_pAllocator, &pipeline)))
				return;

			// create new instance
			SDriverVKPipelineInstance& newInstance = pSubpassData->m_PipelineInstances.AddLastItem();
			newInstance.m_Key = pSubpassData->m_PipelineWantedKey;
			newInstance.m_Pipeline = pipeline;
		}

		// bind
		vkCmdBindPipeline(m_FrameCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		pSubpassData->m_PipelineCurrentKey = pSubpassData->m_PipelineWantedKey;

		pSubpassData->m_DescriptorSetCurrentKey = {};
	}
}

//----------------------------------

void RendererDriverVulkan::AddTexture(Texture* _texture)
{
	if (_texture->GetNbLevels() < 1)
		return;

	SDriverVKTextureData* pData = NEW SDriverVKTextureData;

	// format
	switch (_texture->GetFormat())
	{
		case Texture::TEXTUREFORMAT_R8:		pData->m_Image.m_Format = VK_FORMAT_R8_UNORM;				break;
		case Texture::TEXTUREFORMAT_RG8:		pData->m_Image.m_Format = VK_FORMAT_R8G8_UNORM;				break;
		case Texture::TEXTUREFORMAT_RGB8:		pData->m_Image.m_Format = VK_FORMAT_R8G8B8_UNORM;			break;
		case Texture::TEXTUREFORMAT_RGBA8:		pData->m_Image.m_Format = VK_FORMAT_R8G8B8A8_UNORM;			break;
		case Texture::TEXTUREFORMAT_R16F:		pData->m_Image.m_Format = VK_FORMAT_R16_SFLOAT;				break;
		case Texture::TEXTUREFORMAT_RG16F:		pData->m_Image.m_Format = VK_FORMAT_R16G16_SFLOAT;			break;
		case Texture::TEXTUREFORMAT_RGB16F:	pData->m_Image.m_Format = VK_FORMAT_R16G16B16_SFLOAT;		break;
		case Texture::TEXTUREFORMAT_RGBA16F:	pData->m_Image.m_Format = VK_FORMAT_R16G16B16A16_SFLOAT;	break;
		case Texture::TEXTUREFORMAT_R32F:		pData->m_Image.m_Format = VK_FORMAT_R32_SFLOAT;				break;
		case Texture::TEXTUREFORMAT_RGB32F:	pData->m_Image.m_Format = VK_FORMAT_R32G32B32_SFLOAT;		break;
		case Texture::TEXTUREFORMAT_RGBA32F:	pData->m_Image.m_Format = VK_FORMAT_R32G32B32A32_SFLOAT;	break;
		case Texture::TEXTUREFORMAT_DepthStencil: 
		{
			pData->m_Image.m_Format = VK_FORMAT_D24_UNORM_S8_UINT;
			break;
		}
		default: DEBUG_Forbidden("Unknown format"); break;
	};

	// type
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
	switch (_texture->GetType())
	{
		case Texture::TEXTURETYPE_2D: viewType = VK_IMAGE_VIEW_TYPE_2D; break;
		default: DEBUG_Forbidden("Unknown type"); break;
	}

	// usage
	Bool bIsRenderTarget = (_texture->GetData() == NULL); // todo : enum usage
	VkImageUsageFlags usage = 0;
	VkImageAspectFlags aspect = 0;
	if (bIsRenderTarget)
	{
		if (_texture->GetFormat() == Texture::TEXTUREFORMAT_DepthStencil) // depth
		{
			usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		else
		{
			usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		}
	}
	else
	{
		usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	// extent (todo from type)
	pData->m_Image.m_Extent.width = static_cast<uint32_t>(_texture->GetSizeX());
	pData->m_Image.m_Extent.height = static_cast<uint32_t>(_texture->GetSizeY());
	pData->m_Image.m_Extent.depth = 1;
	pData->m_Image.m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;

	// wrapping
	VkSamplerAddressMode wrapping = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	switch (_texture->GetWrapping())
	{
	case Texture::TEXTUREWRAP_Repeat: wrapping = VK_SAMPLER_ADDRESS_MODE_REPEAT; break;
	case Texture::TEXTUREWRAP_Clamp : wrapping = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; break;
	default: DEBUG_Forbidden("Unknown wrapping"); break;
	}
			 
	VkImageType type = VK_IMAGE_TYPE_2D;
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageFormatProperties formatproperties;
	VKCHECK(vkGetPhysicalDeviceImageFormatProperties(m_PhysicalDevice, pData->m_Image.m_Format, type, tiling, usage, 0, &formatproperties));

	// image
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = type;
	imageInfo.extent = pData->m_Image.m_Extent;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = pData->m_Image.m_Format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = pData->m_Image.m_Layout;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	if (!VKCHECK(vkCreateImage(m_Device, &imageInfo, m_pAllocator, &pData->m_Image.m_Image)))
		return;

#ifdef USE_GPU_PROFILER
	SetObjectName((uint64_t)pData->m_Image.m_Image, VK_OBJECT_TYPE_IMAGE, _texture->GetName().GetStr().GetArray());
#endif

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_Device, pData->m_Image.m_Image, &memRequirements);
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);
	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	uint32_t memoryTypeIdx = -1;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			memoryTypeIdx = i;
			break;
		}
	}
	if (memoryTypeIdx == -1)
	{
		LOGGER_LogError("Unable to find Buffer memory Type!");
		return;
	}

	// memory
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = memoryTypeIdx;
	if (!VKCHECK(vkAllocateMemory(m_Device, &allocInfo, m_pAllocator, &pData->m_Image.m_ImageMemory)))
		return;

	vkBindImageMemory(m_Device, pData->m_Image.m_Image, pData->m_Image.m_ImageMemory, 0);

	// copy data
	if (!bIsRenderTarget)
	{
		CopyImageData(_texture->GetData(), _texture->GetDataSize(), &pData->m_Image);
	}

	// imageview
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = pData->m_Image.m_Image;
	viewInfo.viewType = viewType;
	viewInfo.format = pData->m_Image.m_Format;
	viewInfo.subresourceRange.aspectMask = aspect;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	if (!VKCHECK(vkCreateImageView(m_Device, &viewInfo, m_pAllocator, &pData->m_Image.m_ImageView)))
		return;

	// sampler
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = wrapping;
	samplerInfo.addressModeV = wrapping;
	samplerInfo.addressModeW = wrapping;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 0/*properties.limits.maxSamplerAnisotropy*/;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	if (!VKCHECK(vkCreateSampler(m_Device, &samplerInfo, m_pAllocator, &pData->m_Sampler)))
		return;

	_texture->m_pDriverData = pData;
}

//----------------------------------

void RendererDriverVulkan::RemoveTexture(Texture* _pTexture)
{
	SDriverVKTextureData* pData = (SDriverVKTextureData*)_pTexture->m_pDriverData;

	vkDestroySampler(m_Device, pData->m_Sampler, m_pAllocator);
	pData->m_Sampler = VK_NULL_HANDLE;
	vkDestroyImageView(m_Device, pData->m_Image.m_ImageView, m_pAllocator);
	pData->m_Image.m_ImageView = VK_NULL_HANDLE;
	vkFreeMemory(m_Device, pData->m_Image.m_ImageMemory, m_pAllocator);
	pData->m_Image.m_ImageMemory = VK_NULL_HANDLE;
	vkDestroyImage(m_Device, pData->m_Image.m_Image, m_pAllocator);
	pData->m_Image.m_Image = VK_NULL_HANDLE;
	
	DELETE pData;
	_pTexture->m_pDriverData = NULL;
}

//----------------------------------

void RendererDriverVulkan::UpdatePrimitive(Primitive3D* _pPrimitive)
{
	if (!_pPrimitive->m_pDriverData)
	{
		VertexBuffer* pVB = (VertexBuffer*)_pPrimitive->GetVB().GetPtr();
		IndexBuffer* pIB = (IndexBuffer*)_pPrimitive->GetIB().GetPtr();
		SDriverVKPrimitiveData* pData = NEW SDriverVKPrimitiveData;		

		// VB
		pData->m_VertexBuffer.m_Size = Max<VkDeviceSize>(1, pVB->GetArraySize());
		pData->m_VertexBuffer.m_Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		pData->m_VertexBuffer.m_Properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		CreateBuffer(&pData->m_VertexBuffer);
		if(pVB->GetArraySize())
			CopyToBuffer(pVB->GetArray(), (size_t)pVB->GetArraySize(), &pData->m_VertexBuffer);

		// IB
		pData->m_IndexBuffer.m_Size = Max<VkDeviceSize>(1, pIB->m_Array.GetItemSize());
		pData->m_IndexBuffer.m_Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		pData->m_IndexBuffer.m_Properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		CreateBuffer(&pData->m_IndexBuffer);
		if(pIB->m_Array.GetItemSize())
			CopyToBuffer(pIB->m_Array.GetPointer(), (size_t)pIB->m_Array.GetItemSize(), &pData->m_IndexBuffer);
		
#ifdef USE_GPU_PROFILER
		SetObjectName((uint64_t)pData->m_VertexBuffer.m_Buffer, VK_OBJECT_TYPE_BUFFER, pVB->GetName().GetStr().GetArray());
		SetObjectName((uint64_t)pData->m_IndexBuffer.m_Buffer, VK_OBJECT_TYPE_BUFFER, pIB->GetName().GetStr().GetArray());
#endif

		_pPrimitive->m_pDriverData = pData;
	}
	else if (!_pPrimitive->IsStatic())// update data
	{
		SDriverVKPrimitiveData* pData = (SDriverVKPrimitiveData*)_pPrimitive->m_pDriverData;

		VertexBuffer* pVB = (VertexBuffer*)_pPrimitive->GetVB().GetPtr();
		if (pVB->GetArraySize() == 0)
			return;

		if (pVB->GetArraySize() > pData->m_VertexBuffer.m_Size) // vb increase size
		{
			DestroyBuffer(&pData->m_VertexBuffer);
			pData->m_VertexBuffer.m_Size = pVB->GetArraySize();
			CreateBuffer(&pData->m_VertexBuffer);
#ifdef USE_GPU_PROFILER
			SetObjectName((uint64_t)pData->m_VertexBuffer.m_Buffer, VK_OBJECT_TYPE_BUFFER, pVB->GetName().GetStr().GetArray());
#endif
		}

		IndexBuffer* pIB = (IndexBuffer*)_pPrimitive->GetIB().GetPtr();
		if (pIB->m_Array.GetItemSize() > pData->m_IndexBuffer.m_Size) // vb increase size
		{
			DestroyBuffer(&pData->m_IndexBuffer);
			pData->m_IndexBuffer.m_Size = pIB->m_Array.GetItemSize();
			CreateBuffer(&pData->m_IndexBuffer);
#ifdef USE_GPU_PROFILER
			SetObjectName((uint64_t)pData->m_IndexBuffer.m_Buffer, VK_OBJECT_TYPE_BUFFER, pIB->GetName().GetStr().GetArray());
#endif
		}

		CopyToBuffer(pVB->GetArray(), (size_t)pVB->GetArraySize(), &pData->m_VertexBuffer);
		CopyToBuffer(pIB->m_Array.GetPointer(), (size_t)pIB->m_Array.GetItemSize(), &pData->m_IndexBuffer);
	}
}

//----------------------------------

void RendererDriverVulkan::RemovePrimitive(void* _pDriverData)
{
	SDriverVKPrimitiveData* pData = (SDriverVKPrimitiveData*)_pDriverData;
	DestroyBuffer(&pData->m_IndexBuffer);
	DestroyBuffer(&pData->m_VertexBuffer);
	DELETE pData;
}

//----------------------------------

void RendererDriverVulkan::CompileShaderGroup(ShaderGroup* _shaderGroup)
{
	// already compiled
	if (_shaderGroup->m_pDriverData)
	{
		return;
	}

	// compiles each shaders
	Shader* vertex = (Shader*)_shaderGroup->m_Shaders[SHADER_Vertex].GetPtr();
	Shader* pixel = (Shader*)_shaderGroup->m_Shaders[SHADER_Pixel].GetPtr();
	Shader* header = (Shader*)_shaderGroup->m_Shaders[SHADER_Header].GetPtr();
	CompileShader(vertex, header);
	CompileShader(pixel, header);
}

//----------------------------------

Bool RendererDriverVulkan::CompileShader(Shader* _shader, Shader* _header)
{
	// already compile
	if (_shader->m_pDriverData)
	{
		return TRUE;
	}

	ShaderSource* pShaderSource = _shader->GetSource().GetCastPtr<ShaderSource>();
	ShaderSource* pHeaderSource = NULL;
	if (_header)
		pHeaderSource = _header->GetSource().GetCastPtr<ShaderSource>();

	Str sourceType = "";
	const Char* pShaderStage = NULL;
	switch (pShaderSource->GetType())
	{
		case SHADER_Vertex: sourceType = "#define VFX\n"; pShaderStage = "vertex"; break;
		case SHADER_Pixel: sourceType = "#define PFX\n"; pShaderStage = "fragment"; break;
		default:
		{
			DEBUG_Forbidden("Unknown shader type !!\n");
			return FALSE;
		}
	}

	// source
	Str codeHeader = "#version 450\n";
	codeHeader += sourceType;
	for (const Str& f : _shader->GetFlags())
		codeHeader += Str("#define %s 1\n", f.GetArray());
	Str sourceStr = codeHeader;
	sourceStr += "\n";
	if (pHeaderSource)
	{
		sourceStr += pHeaderSource->GetSource().GetArray();
		sourceStr += "\n";
	}
	sourceStr += pShaderSource->GetSource().GetArray();

	size_t iShaderByteCodeSize = 0;
	const uint32_t* pShaderByteCode = NULL;

	OldFile srcfile("tmp.fx", OldFile::USE_TYPE(OldFile::USE_TYPE_Write | OldFile::USE_TYPE_Temp));
	OldFile bytefile("tmp.bin", OldFile::USE_TYPE(OldFile::USE_TYPE_ReadBuffer | OldFile::USE_TYPE_Temp));
	OldFile logfile("tmp.log", OldFile::USE_TYPE(OldFile::USE_TYPE_ReadBuffer | OldFile::USE_TYPE_Temp));

	// flush files
	bytefile.Delete();
	logfile.Delete();

	// copy source to tmp file	
	srcfile.Open(TRUE);
	srcfile.WriteByte(sourceStr.GetArray(), sourceStr.GetLength());
	srcfile.Close();

	// launch compile
	switch (pShaderSource->GetType())
	{
		case SHADER_Vertex: pShaderStage = "vertex"; break;
		case SHADER_Pixel: pShaderStage = "fragment"; break;
	}
	Str cmd("\"\"../src/lib/vulkan-1.3.204/Bin/glslc.exe\" -fshader-stage=%s %s -o %s >> %s\"", pShaderStage, srcfile.GetPath().GetArray(), bytefile.GetPath().GetArray(), logfile.GetPath().GetArray());
	system(cmd.GetArray());

	// read tmp file result
	
	if (!bytefile.Exists())
	{
		LOGGER_LogError("Could not compile shader %s\n", _shader->GetPath().GetArray());
		logfile.Open();
		LOGGER_LogError(logfile.GetBuffer());
		logfile.Close();
		return FALSE;
	}

	bytefile.Open();
	iShaderByteCodeSize = bytefile.GetBufferSize();
	pShaderByteCode = (uint32_t*)bytefile.GetBuffer();

	SDriverVKShaderData* pShaderData = NEW SDriverVKShaderData;	
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = iShaderByteCodeSize;
	createInfo.pCode = pShaderByteCode;
	if (!VKCHECK(vkCreateShaderModule(m_Device, &createInfo, m_pAllocator, &pShaderData->m_Shader)))
		return FALSE;

	bytefile.Close();

#ifdef USE_GPU_PROFILER
	SetObjectName((uint64_t)pShaderData->m_Shader, VK_OBJECT_TYPE_SHADER_MODULE, _shader->GetName().GetStr().GetArray());
#endif

	_shader->m_pDriverData = pShaderData;
	return TRUE;
}

//----------------------------------

void RendererDriverVulkan::RemoveShaderGroup(ShaderGroup* _shaderGroup) 
{
	Shader* vertex = (Shader*)_shaderGroup->m_Shaders[SHADER_Vertex].GetPtr();	
	SDriverVKShaderData* pData = (SDriverVKShaderData*)vertex->m_pDriverData;
	if (pData) // tmp
	{
		vkDestroyShaderModule(m_Device, pData->m_Shader, m_pAllocator);
		DELETE pData;
		vertex->m_pDriverData = NULL;
	}

	Shader* pixel = (Shader*)_shaderGroup->m_Shaders[SHADER_Pixel].GetPtr();
	pData = (SDriverVKShaderData*)pixel->m_pDriverData;
	if (pData) // tmp
	{
		vkDestroyShaderModule(m_Device, pData->m_Shader, m_pAllocator);
		DELETE pData;
		pixel->m_pDriverData = NULL;
	}
}

//----------------------------------

void RendererDriverVulkan::UseShaderGroup(const ShaderGroup* _shaderGroup) 
{
	SUPER::UseShaderGroup(_shaderGroup);
}

//----------------------------------

S32 RendererDriverVulkan::GetShaderAttribute(ShaderGroup* _shaderGroup, const Char* _name)
{
	return -1;
}

//----------------------------------
	
S32 RendererDriverVulkan::GetShaderUniform(ShaderGroup* _shaderGroup, const Char* _name)
{
	return -1;
}

//----------------------------------

S32 RendererDriverVulkan::GetShaderUniformData(ShaderGroup* _shaderGroup, const Char* _name)
{
	return -1;
}

//----------------------------------

void RendererDriverVulkan::ActiveTexture(U32 _iSlot, const ShaderGroup::Params& _params, Texture* _pTexture)
{
	SDriverVKTextureData* pData = (SDriverVKTextureData*)_pTexture->m_pDriverData;
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;
	SDriverVKSubpassData* pSubpassData = (SDriverVKSubpassData*)m_pCurrentSubpass->m_pDriverData;
	
	pSubpassData->m_DescriptorSetWantedKey.m_Slots[VK_TEXTURE_BINDING_OFFSET + _iSlot].m_pData = pData;
	pSubpassData->m_DescriptorSetWantedKey.m_Slots[VK_TEXTURE_BINDING_OFFSET + _iSlot].m_bTexture = TRUE;
}

//----------------------------------

void RendererDriverVulkan::PushShaderData(ShaderData* _pData)
{
	SDriverVKShaderDataData* pData = (SDriverVKShaderDataData*)_pData->m_pDriverData;
	if (!pData)
	{
		pData = NEW SDriverVKShaderDataData;

		pData->m_Buffer.m_Size = _pData->GetDataSize();
		pData->m_Buffer.m_Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		pData->m_Buffer.m_Properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		CreateBuffer(&pData->m_Buffer);
#ifdef USE_GPU_PROFILER
		SetObjectName((uint64_t)pData->m_Buffer.m_Buffer, VK_OBJECT_TYPE_BUFFER, _pData->GetName().GetStr().GetArray());
#endif

		_pData->m_pDriverData = pData;
	}

	// update data
	void* data;
	vkMapMemory(m_Device, pData->m_Buffer.m_BufferMemory, 0, _pData->GetDataSize(), 0, &data);
	memcpy(data, _pData->GetDataPtr(), _pData->GetDataSize());
	vkUnmapMemory(m_Device, pData->m_Buffer.m_BufferMemory);
}

//----------------------------------

void RendererDriverVulkan::RemoveShaderData(void* _pDriverData)
{
	SDriverVKShaderDataData* pData = (SDriverVKShaderDataData*)_pDriverData;
	DestroyBuffer(&pData->m_Buffer);

	DELETE pData;
}

//----------------------------------

void RendererDriverVulkan::UseShaderData(const ShaderGroup::Params& _params, ShaderData* _pData, U32 _iSlot)
{
	SDriverVKShaderDataData* pData = (SDriverVKShaderDataData*)_pData->m_pDriverData;
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;
	SDriverVKSubpassData* pSubpassData = (SDriverVKSubpassData*)m_pCurrentSubpass->m_pDriverData;

	pSubpassData->m_DescriptorSetWantedKey.m_Slots[_iSlot].m_pData = pData;
	pSubpassData->m_DescriptorSetWantedKey.m_Slots[_iSlot].m_bTexture = FALSE;
}

//----------------------------------

void RendererDriverVulkan::PushDrawShaderData(const ShaderGroup::Params& _params, ShaderData* _pData)
{
	SDriverVKSubpassData* pSubpassData = (SDriverVKSubpassData*)m_pCurrentSubpass->m_pDriverData;
	vkCmdPushConstants(m_FrameCommandBuffer, pSubpassData->m_PipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, 0, _pData->GetDataSize(), _pData->GetDataPtr());
}

//----------------------------------

void RendererDriverVulkan::BeginPass(Renderer* _pRenderer, RendererPass* _pPass)
{
	m_pCurrentPass = _pPass;

	if (!_pPass->m_pDriverData)
		CreatePass(_pPass);
	
	SDriverVKPassData* pData = (SDriverVKPassData*)_pPass->m_pDriverData;
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;

	// start render pass
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	if (_pPass->m_pFrameBuffer)
	{
		SDriverVKFrameBufferData* pFramebufferData = (SDriverVKFrameBufferData*)_pPass->m_pFrameBuffer->m_pDriverData;
		renderPassInfo.renderPass = pData->m_RenderPass;
		renderPassInfo.framebuffer = pFramebufferData->m_FrameBuffer;
		renderPassInfo.renderArea.extent.width = (uint32_t)pData->m_Viewport.width;
		renderPassInfo.renderArea.extent.height = (uint32_t)pData->m_Viewport.height;
	}
	else
	{
		renderPassInfo.renderPass = pRendererData->m_DefaultRenderPass;
		renderPassInfo.framebuffer = pRendererData->m_SwapChainImageFramebuffers[pRendererData->m_ImageIdx];
		renderPassInfo.renderArea.extent = pRendererData->m_Extent;
	}
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.clearValueCount = pData->m_ClearValues.GetCount();
	renderPassInfo.pClearValues = pData->m_ClearValues.GetPointer();
	vkCmdBeginRenderPass(m_FrameCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

//----------------------------------

void RendererDriverVulkan::EndPass()
{
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;
	vkCmdEndRenderPass(m_FrameCommandBuffer);

	m_pCurrentPass = NULL;
}

//----------------------------------

void RendererDriverVulkan::RemovePass(void* _pDriverData)
{
	DestroyPass(_pDriverData);
}

//----------------------------------

void RendererDriverVulkan::BeginSubPass(RendererSubpass* _pSubpass)
{
	m_pCurrentSubpass = _pSubpass;

	if (!_pSubpass->m_pDriverData)
		CreateSubpass(_pSubpass);

	SDriverVKSubpassData* pData = (SDriverVKSubpassData*)_pSubpass->m_pDriverData;
	
	// force reupdate
	pData->m_DescriptorSetCurrentKey = {};
	pData->m_PipelineCurrentKey = {}; 

	vkCmdSetScissor(m_FrameCommandBuffer, 0, 1, &pData->m_PipelineScissor);
}

//----------------------------------

void RendererDriverVulkan::EndSubPass()
{
	m_pCurrentSubpass = NULL;
}

//----------------------------------

void RendererDriverVulkan::RemoveSubPass(void* _pDriverData)
{
	DestroySubpass(_pDriverData);
}

//----------------------------------

void RendererDriverVulkan::RemoveFrameBuffer(void* _pDriverData)
{
	DestroyFrameBuffer(_pDriverData);
}

//----------------------------------

Bool RendererDriverVulkan::CreatePass(RendererPass* _pPass)
{
	DEBUG_Require(!_pPass->m_pDriverData);
	SDriverVKPassData* pData = NEW SDriverVKPassData;
	_pPass->m_pDriverData = pData;

	// viewport
	pData->m_Viewport = {};
	pData->m_Viewport.x = 0.0f;
	pData->m_Viewport.y = 0.0f;
	pData->m_Viewport.minDepth = 0.0f;
	pData->m_Viewport.maxDepth = 1.0f;

	// atachements
	ArrayOf<VkAttachmentDescription> attachmentDescs;
	ArrayOf<VkAttachmentReference> attachmentColorRefs;
	VkAttachmentReference attachmentDepthRef;
	VkAttachmentReference* pAttachmentDepth = nullptr;
	FrameBuffer* pFrameBuffer = _pPass->m_pFrameBuffer;	
	if (pFrameBuffer)
	{
		if (!pFrameBuffer->m_pDriverData)
			PrepareFrameBuffer(pFrameBuffer);

		SDriverVKFrameBufferData* pFrameBufferData = (SDriverVKFrameBufferData*)pFrameBuffer->m_pDriverData;

		pData->m_Viewport.width = (float)pFrameBufferData->m_Extent.width;
		pData->m_Viewport.height = (float)pFrameBufferData->m_Extent.height;
		pData->m_ClearValues.SetEmpty();

		attachmentDescs = pFrameBufferData->m_AttachmentsDesc;
		for (U32 i = 0; i < attachmentDescs.GetCount(); i++)
		{
			VkAttachmentReference* pRef = nullptr;
			VkAttachmentDescription& desc = attachmentDescs[i];
			if (desc.finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				pAttachmentDepth = &attachmentDepthRef;
				pRef = pAttachmentDepth;

				if (_pPass->m_ClearFlag & Clear_Depth)
				{
					desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					VkClearValue& v = pData->m_ClearValues.AddLastItem();
					v.depthStencil.depth = _pPass->m_ClearDepth;
				}
			}
			else
			{
				pRef = &(attachmentColorRefs.AddLastItem()); 
				
				if (_pPass->m_ClearFlag & Clear_Color)
				{
					desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					VkClearValue& v = pData->m_ClearValues.AddLastItem();
					v.color = { _pPass->m_ClearColor.r, _pPass->m_ClearColor.g, _pPass->m_ClearColor.b, _pPass->m_ClearColor.a };
				}
			}

			pRef->attachment = i;
			pRef->layout = desc.finalLayout;
			desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = attachmentColorRefs.GetCount();
		subpass.pColorAttachments = attachmentColorRefs.GetPointer();
		subpass.pDepthStencilAttachment = pAttachmentDepth;
		pData->m_bHasDepthStencil = (pAttachmentDepth != nullptr);

		VkSubpassDependency dependencies[2] {};
		
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachmentDescs.GetCount();
		renderPassInfo.pAttachments = attachmentDescs.GetPointer();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 2;
		renderPassInfo.pDependencies = dependencies;
		if (!VKCHECK(vkCreateRenderPass(m_Device, &renderPassInfo, m_pAllocator, &pData->m_RenderPass)))
			return FALSE;
		
		CreateFrameBuffer(pFrameBuffer);
	}
	else
	{
		SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;
		pData->m_Viewport.width = (float)pRendererData->m_Extent.width;
		pData->m_Viewport.height = (float)pRendererData->m_Extent.height;
		pData->m_RenderPass = pRendererData->m_DefaultRenderPass;
		pData->m_bDefaultPass = TRUE;
	}

	_pPass->m_pDriverData = pData;


	return TRUE;
}

//----------------------------------

Bool	RendererDriverVulkan::DestroyPass(void* _pDriverData)
{
	SDriverVKPassData* pPassData = (SDriverVKPassData*)_pDriverData;
	LOGGER_Log("[REM]%x\n", pPassData->m_RenderPass);
	if(!pPassData->m_bDefaultPass)
		vkDestroyRenderPass(m_Device, pPassData->m_RenderPass, m_pAllocator);
	DELETE pPassData;
	return TRUE;
}

//----------------------------------

Bool	RendererDriverVulkan::PrepareFrameBuffer(FrameBuffer* _pFrameBuffer)
{
	DEBUG_Require(!_pFrameBuffer->m_pDriverData);
	SDriverVKFrameBufferData* pData = NEW SDriverVKFrameBufferData;

	pData->m_Extent.width = _pFrameBuffer->GetTexture(0)->GetSizeX();
	pData->m_Extent.height = _pFrameBuffer->GetTexture(0)->GetSizeY();

	for (U32 i = 0; i < _pFrameBuffer->GetTextureCount(); i++)
	{
		Texture* pTexture = _pFrameBuffer->GetTexture(i);
		SDriverVKTextureData* pTextureData = (SDriverVKTextureData*)pTexture->m_pDriverData;

		VkAttachmentDescription& desc = pData->m_AttachmentsDesc.AddLastItem();
		desc.samples = VK_SAMPLE_COUNT_1_BIT;
		desc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		desc.format = pTextureData->m_Image.m_Format;
		
#if 0
		desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		desc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		if (pTexture->GetFormat() == Texture::TEXTUREFORMAT_DepthStencil)
		{
			desc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
#else
		desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		if(pTexture->GetFormat() == Texture::TEXTUREFORMAT_DepthStencil)
			desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
#endif
		//desc.finalLayout = desc.initialLayout/*VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/;
	}

	_pFrameBuffer->m_pDriverData = pData;

	return TRUE;
}

//----------------------------------

Bool	 RendererDriverVulkan::CreateFrameBuffer(FrameBuffer* _pFrameBuffer)
{
	DEBUG_Require(_pFrameBuffer);
	DEBUG_Require(_pFrameBuffer->m_pDriverData);
	SDriverVKFrameBufferData* pData = (SDriverVKFrameBufferData*)_pFrameBuffer->m_pDriverData;
	if (pData->m_FrameBuffer == VK_NULL_HANDLE)
	{
		// views
		ArrayOf<VkImageView> views;
		for (U32 i = 0; i < _pFrameBuffer->GetTextureCount(); i++)
		{
			Texture* pTexture = _pFrameBuffer->GetTexture(i);
			SDriverVKTextureData* pTextureData = (SDriverVKTextureData*)pTexture->m_pDriverData;
			views.AddLastItem(pTextureData->m_Image.m_ImageView);
		}

		// create framebuffer
		SDriverVKPassData* pPassData = (SDriverVKPassData*)m_pCurrentPass->m_pDriverData;
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = pPassData->m_RenderPass;
		framebufferInfo.layers = 1;
		framebufferInfo.attachmentCount = views.GetCount();
		framebufferInfo.pAttachments = views.GetPointer();
		framebufferInfo.width = pData->m_Extent.width;
		framebufferInfo.height = pData->m_Extent.height;
		if (!VKCHECK(vkCreateFramebuffer(m_Device, &framebufferInfo, m_pAllocator, &pData->m_FrameBuffer)))
			return FALSE;
	}
	
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroyFrameBuffer(void* _pDriverData)
{
	SDriverVKFrameBufferData* pData = (SDriverVKFrameBufferData*)_pDriverData;
	vkDestroyFramebuffer(m_Device, pData->m_FrameBuffer, m_pAllocator);
	DELETE pData;
	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::CreateSubpass(RendererSubpass* _pSubpass)
{
	DEBUG_Require(!_pSubpass->m_pDriverData);
	SDriverVKSubpassData* pData = NEW SDriverVKSubpassData;
	SDriverVKPassData* pPassData = (SDriverVKPassData*)m_pCurrentPass->m_pDriverData;
	SDriverVKRendererData* pRendererData = (SDriverVKRendererData*)m_pCurrentRenderer->m_pDriverData;

	// descriptors set layout
	ArrayOf<VkDescriptorSetLayoutBinding> bindings = pRendererData->m_ShaderDataBindings;
	for (U32 i = 0; i < _pSubpass->m_TextureSlots.GetCount(); i++)
	{
		VkDescriptorSetLayoutBinding& binding = bindings.AddLastItem({ 0 });
		binding.binding = VK_TEXTURE_BINDING_OFFSET + _pSubpass->m_TextureSlots[i].m_iSlot;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	}
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindings.GetCount();
	layoutInfo.pBindings = bindings.GetPointer();
	if (!VKCHECK(vkCreateDescriptorSetLayout(m_Device, &layoutInfo, m_pAllocator, &pData->m_DescriptorSetLayout)))
		return FALSE;

	// push constant
	VkPushConstantRange push_constant{};
	push_constant.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	push_constant.offset = 0;
	push_constant.size = 256;

	// create pipeline
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &pData->m_DescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &push_constant;
	if (!VKCHECK(vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, m_pAllocator, &pData->m_PipelineLayout)))
		return FALSE;

	// shader stages
	VkPipelineShaderStageCreateInfo& vertShaderStageInfo = pData->m_PipelineShaderStages[0];
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.pName = "main";
	VkPipelineShaderStageCreateInfo& fragShaderStageInfo = pData->m_PipelineShaderStages[1];
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.pName = "main";
	if (_pSubpass->m_pShader)
	{
		Shader* vertex = (Shader*)_pSubpass->m_pShader->m_Shaders[SHADER_Vertex].GetPtr();
		SDriverVKShaderData* pShaderData = (SDriverVKShaderData*)vertex->m_pDriverData;
		vertShaderStageInfo.module = pShaderData->m_Shader;

		Shader * pixel = (Shader*)_pSubpass->m_pShader->m_Shaders[SHADER_Pixel].GetPtr();
		pShaderData = (SDriverVKShaderData*)pixel->m_pDriverData;
		fragShaderStageInfo.module = pShaderData->m_Shader;
	}
	
	// vertex input
	pData->m_PipelineBindingDescription.binding = 0;
	pData->m_PipelineBindingDescription.stride = VB_ValueCountPerVertex * sizeof(Float);
	pData->m_PipelineBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	pData->m_PipelineAttributeDescriptions.SetItemCount(4);
	pData->m_PipelineAttributeDescriptions[0].binding = 0; // position
	pData->m_PipelineAttributeDescriptions[0].location = 0;
	pData->m_PipelineAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	pData->m_PipelineAttributeDescriptions[0].offset = 0;
	pData->m_PipelineAttributeDescriptions[1].binding = 0; // normal
	pData->m_PipelineAttributeDescriptions[1].location = 1;
	pData->m_PipelineAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	pData->m_PipelineAttributeDescriptions[1].offset = 5 * sizeof(Float);
	pData->m_PipelineAttributeDescriptions[2].binding = 0; // uv
	pData->m_PipelineAttributeDescriptions[2].location = 2;
	pData->m_PipelineAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	pData->m_PipelineAttributeDescriptions[2].offset = 3 * sizeof(Float);
	pData->m_PipelineAttributeDescriptions[3].binding = 0; // color
	pData->m_PipelineAttributeDescriptions[3].location = 3;
	pData->m_PipelineAttributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	pData->m_PipelineAttributeDescriptions[3].offset = 8 * sizeof(Float);
	pData->m_PipelineVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pData->m_PipelineVertexInputInfo.vertexBindingDescriptionCount = 1;
	pData->m_PipelineVertexInputInfo.pVertexBindingDescriptions = &pData->m_PipelineBindingDescription;
	pData->m_PipelineVertexInputInfo.vertexAttributeDescriptionCount = pData->m_PipelineAttributeDescriptions.GetCount();
	pData->m_PipelineVertexInputInfo.pVertexAttributeDescriptions = pData->m_PipelineAttributeDescriptions.GetPointer();
	
	// input assembly
	pData->m_PipelineInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pData->m_PipelineInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pData->m_PipelineInputAssembly.primitiveRestartEnable = VK_FALSE;

	// viewport
	pData->m_PipelineScissor.offset = { 0, 0 };
	pData->m_PipelineScissor.extent.width = (uint32_t)ROUNDINT(pPassData->m_Viewport.width);
	pData->m_PipelineScissor.extent.height = (uint32_t)ROUNDINT(pPassData->m_Viewport.height);	
	pData->m_PipelineViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pData->m_PipelineViewportState.viewportCount = 1;
	pData->m_PipelineViewportState.pViewports = &pPassData->m_Viewport;
	pData->m_PipelineViewportState.scissorCount = 1;
	pData->m_PipelineViewportState.pScissors = &pData->m_PipelineScissor;

	// rasterizer
	pData->m_PipelineRasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pData->m_PipelineRasterizer.depthClampEnable = VK_FALSE;
	pData->m_PipelineRasterizer.rasterizerDiscardEnable = VK_FALSE;
	pData->m_PipelineRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	pData->m_PipelineRasterizer.lineWidth = 1.0f;
	if (_pSubpass->m_RenderState & RenderState_FaceCulling)
	{
		if(_pSubpass->m_RenderState & RenderState_InvertCulling)
			pData->m_PipelineRasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		else
			pData->m_PipelineRasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	}
	else
		pData->m_PipelineRasterizer.cullMode = VK_CULL_MODE_NONE;
	pData->m_PipelineRasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	pData->m_PipelineRasterizer.depthBiasEnable = VK_FALSE;
	pData->m_PipelineRasterizer.depthBiasConstantFactor = 0.0f; // Optional
	pData->m_PipelineRasterizer.depthBiasClamp = 0.0f; // Optional
	pData->m_PipelineRasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	// multisampling (disabled)
	pData->m_PipelineMultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pData->m_PipelineMultisampling.sampleShadingEnable = VK_FALSE;
	pData->m_PipelineMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pData->m_PipelineMultisampling.minSampleShading = 1.0f; // Optional
	pData->m_PipelineMultisampling.pSampleMask = nullptr; // Optional
	pData->m_PipelineMultisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	pData->m_PipelineMultisampling.alphaToOneEnable = VK_FALSE; // Optional

	// deph/stencil
	pData->m_PipelineDepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pData->m_PipelineDepthStencil.depthTestEnable = (_pSubpass->m_RenderState & RenderState_DepthTest) ? VK_TRUE : VK_FALSE;
	pData->m_PipelineDepthStencil.depthWriteEnable = (_pSubpass->m_RenderState & RenderState_DepthWrite) ? VK_TRUE : VK_FALSE;
	pData->m_PipelineDepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	pData->m_PipelineDepthStencil.depthBoundsTestEnable = VK_FALSE;
	pData->m_PipelineDepthStencil.minDepthBounds = 0.f;
	pData->m_PipelineDepthStencil.maxDepthBounds = 1.f;
	//pData->m_PipelineDepthStencil.flags;
	/*pData->m_PipelineDepthStencil.stencilTestEnable;
	pData->m_PipelineDepthStencil.front;
	pData->m_PipelineDepthStencil.back;*/
	
	// color attachements
	pData->m_PipelineColorBlendAttachments.SetEmpty();
	auto addAttachement = [&]()
	{
		VkPipelineColorBlendAttachmentState& colorBlendAttachment = pData->m_PipelineColorBlendAttachments.AddLastItem();
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		if (_pSubpass->m_RenderState & RenderState_AlphaBlend)
		{
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		}
		else if (_pSubpass->m_RenderState & RenderState_Additive)
		{
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		}
	};
	if (m_pCurrentPass->m_pFrameBuffer)
	{
		SDriverVKFrameBufferData* pFramebufferData = (SDriverVKFrameBufferData*)m_pCurrentPass->m_pFrameBuffer->m_pDriverData;
		for (auto& desc : pFramebufferData->m_AttachmentsDesc)
		{
			if (desc.finalLayout != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				addAttachement();
			}
		}
	}
	else
		addAttachement();
	
	// color blending
	pData->m_PipelineColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pData->m_PipelineColorBlending.logicOpEnable = VK_FALSE;
	pData->m_PipelineColorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	pData->m_PipelineColorBlending.attachmentCount = pData->m_PipelineColorBlendAttachments.GetCount();
	pData->m_PipelineColorBlending.pAttachments = pData->m_PipelineColorBlendAttachments.GetPointer();
	pData->m_PipelineColorBlending.blendConstants[0] = 0.0f; // Optional
	pData->m_PipelineColorBlending.blendConstants[1] = 0.0f; // Optional
	pData->m_PipelineColorBlending.blendConstants[2] = 0.0f; // Optional
	pData->m_PipelineColorBlending.blendConstants[3] = 0.0f; // Optional

	// dynamic state
	pData->m_PipelineDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pData->m_PipelineDynamicState.dynamicStateCount = _countof(g_DynamicStates);
	pData->m_PipelineDynamicState.pDynamicStates = g_DynamicStates;

	// pipeline info
	pData->m_PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pData->m_PipelineInfo.stageCount = _countof(pData->m_PipelineShaderStages);
	pData->m_PipelineInfo.pStages = pData->m_PipelineShaderStages;
	pData->m_PipelineInfo.pVertexInputState = &pData->m_PipelineVertexInputInfo;
	pData->m_PipelineInfo.pInputAssemblyState = &pData->m_PipelineInputAssembly;
	pData->m_PipelineInfo.pViewportState = &pData->m_PipelineViewportState;
	pData->m_PipelineInfo.pRasterizationState = &pData->m_PipelineRasterizer;
	pData->m_PipelineInfo.pMultisampleState = &pData->m_PipelineMultisampling;
	pData->m_PipelineInfo.pDepthStencilState = pPassData->m_bHasDepthStencil ? &pData->m_PipelineDepthStencil : NULL;
	pData->m_PipelineInfo.pColorBlendState = &pData->m_PipelineColorBlending;
	pData->m_PipelineInfo.pDynamicState = &pData->m_PipelineDynamicState;
	pData->m_PipelineInfo.layout = pData->m_PipelineLayout;
	pData->m_PipelineInfo.renderPass = pPassData->m_RenderPass;
	pData->m_PipelineInfo.subpass = 0;
	pData->m_PipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pData->m_PipelineInfo.basePipelineIndex = -1; // Optional

	// setup first key
	pData->m_PipelineWantedKey.m_Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	_pSubpass->m_pDriverData = pData;

	return TRUE;
}

//----------------------------------

Bool RendererDriverVulkan::DestroySubpass(void* _pDriverData)
{
	SDriverVKSubpassData* pData = (SDriverVKSubpassData*)_pDriverData;
	for (auto& pipeline : pData->m_PipelineInstances)
		vkDestroyPipeline(m_Device, pipeline.m_Pipeline, m_pAllocator);
	vkDestroyPipelineLayout(m_Device, pData->m_PipelineLayout, m_pAllocator);
	vkDestroyDescriptorSetLayout(m_Device, pData->m_DescriptorSetLayout, m_pAllocator);
	DELETE pData;
	return TRUE;
}

//----------------------------------

U32 RendererDriverVulkan::ReadPixel(FrameBuffer* _pFrameBuffer, const Vec2i& _vPos)
{
	Texture* pTexture = _pFrameBuffer->GetTexture(0);
	DEBUG_Require(pTexture->m_pDriverData);
	DEBUG_Require(pTexture->GetFormat() == Texture::TEXTUREFORMAT_RGBA8);
	SDriverVKTextureData* pData = (SDriverVKTextureData*)_pFrameBuffer->GetTexture(0)->m_pDriverData;

	// create temp buffer with CPU Access HOST_VISIBLE
	SDriverVKBufferData tempBuffer;
	tempBuffer.m_Size = pTexture->GetDataSize();
	tempBuffer.m_Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	tempBuffer.m_Properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	CreateBuffer(&tempBuffer);

	CopyImageToBuffer(&pData->m_Image, &tempBuffer);

	U32* data = NULL;
	vkMapMemory(m_Device, tempBuffer.m_BufferMemory, 0, pTexture->GetDataSize(), 0, (void**)&data);
	Vec2i vkPos = _vPos;
	vkPos.y = pTexture->GetSizeY() - vkPos.y;
	U32 res = *(data + vkPos.x + vkPos.y * pTexture->GetSizeX());
	vkUnmapMemory(m_Device, tempBuffer.m_BufferMemory);	

	// destroy temp
	DestroyBuffer(&tempBuffer);	

	return res;
}

//----------------------------------

#ifdef USE_GPU_PROFILER
void RendererDriverVulkan::ProfilerBegin(const Char* _pEvent)
{
	if (m_FrameCommandBuffer != VK_NULL_HANDLE)
	{
		VkDebugUtilsLabelEXT info = {};
		info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		info.pLabelName = _pEvent;
		/*info.color[0] = 1.f;
		info.color[1] = 0.f;
		info.color[2] = 0.f;
		info.color[3] = 1.f;*/
		vkCmdBeginDebugUtilsLabelEXT(m_FrameCommandBuffer, &info);
	}
}

//----------------------------------

void RendererDriverVulkan::ProfilerEnd()
{
	if (m_FrameCommandBuffer != VK_NULL_HANDLE)
		vkCmdEndDebugUtilsLabelEXT(m_FrameCommandBuffer);
}

//----------------------------------

void RendererDriverVulkan::SetObjectName(uint64_t _object, VkObjectType _objectType, const char* _pName)
{
	VkDebugUtilsObjectNameInfoEXT nameInfo = {};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = _objectType;
	nameInfo.objectHandle = _object;
	nameInfo.pObjectName = _pName;
	vkSetDebugUtilsObjectNameEXT(m_Device, &nameInfo);
}
#endif

#endif // USE_VULKAN

