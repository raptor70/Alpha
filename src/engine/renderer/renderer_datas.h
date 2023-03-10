#ifndef __RENDERER_DATAS_H__
#define __RENDERER_DATAS_H__

#include "renderer_driver.h"
#include "shader_group.h"
#include "shader_desc.h"
#include "renderer_shader.h"

class RendererDatas
{
	SINGLETON_Declare(RendererDatas);
public:

	RendererDatas();
	~RendererDatas();

	U32				GetCurTextureMemory() const		{ return m_iCurTextureMemory; }
	U32				GetMaxTextureMemory() const		{ return m_iMaxTextureMemory; }

	void	Initialize();
	void	Finalize();

	void	RegisterTexture(Texture* _texture);
	void	UnregisterTexture(Texture* _texture);

	ShaderGroupPtr	CreateShaderGroup(SHADERS _shader, const Str& _combinations ="");
	void	DestroyShaders();
	void	RegisterShader(const ShaderGroupRef& _shader);
	void	UnregisterShader(const ShaderGroupRef& _shader);

	void	UnregisterShaderData(ShaderData* _data);
	void	UnregisterPrimitve(Primitive3D* _primitive);
	void	UnregisterPass(RendererPass* _pass);
	void	UnregisterSubpass(RendererSubpass* _subpass);
	void	UnregisterFrameBuffer(FrameBuffer* _frameBuffer);

	//Primitive3DRef&	CreatePrimitive(Bool _bDynamic = FALSE);
	MaterialRef&	CreateMaterial();

	void	PrepareDraw();
	void	ManageDatas(RendererDriver* _pDriver);
	void	ManageMaterials();

protected:
	void	ManageTextures(RendererDriver* _pDriver);
	void	ManageShaders(RendererDriver* _pDriver);

	void	UpdateDirtyShaders(RendererDriver* _pDriver);

	U32		m_iCurTextureMemory;
	U32		m_iMaxTextureMemory;

	// Shaders
	ShaderDescArray					m_ShaderDescs;
	ArrayOf<ShaderGroupRefArray>	m_Shaders;
	ArrayOf<ShaderGroupRef>			m_daShaderGroupToAdd;
	ArrayOf<ShaderGroupRef>			m_daShaderGroupToRemove;

	ArrayOf<Texture*>				m_daTextureToAdd;
	TextureRefArray					m_daTextureToRemove;
	MaterialRefArray				m_daMaterials;
	ArrayOf<void* >					m_daShaderDataToRemove;
	ArrayOf<void* >					m_daPrimitiveToRemove;
	ArrayOf<void* >					m_daPassToRemove;
	ArrayOf<void* >					m_daSubpassToRemove;
	ArrayOf<void* >					m_daFrameBufferToRemove;
};

#endif