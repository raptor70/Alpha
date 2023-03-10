#include "renderer_datas.h"

SINGLETON_Define(RendererDatas)

//----------------------------------

RendererDatas::RendererDatas()
{
	m_iCurTextureMemory = 0;
	m_iMaxTextureMemory = 0;

#define SHADER(_name,_vfx,_pfx, _combinations)	\
{	\
	SShaderDesc& desc = m_ShaderDescs.AddLastItem(); \
		desc.m_Name = #_name; \
		desc.m_VS = #_vfx; \
		desc.m_PS = #_pfx; \
		Str(_combinations).Split("|",desc.m_Combinations); \
}
#include "renderer_shader_list.h"
#undef SHADER
}

//----------------------------------

RendererDatas::~RendererDatas()
{
}

//----------------------------------

void	RendererDatas::Initialize()
{
	m_Shaders.SetItemCount(RDRSHADER_Count);
	for (S32 i = 0; i < RDRSHADER_Count; i++)
	{
		S32 nbCombinations = m_ShaderDescs[i].m_Combinations.GetCount();
		m_Shaders[i].SetItemCount(1 << nbCombinations);
	}
}

//----------------------------------

void	RendererDatas::Finalize()
{
	DestroyShaders();
}

//----------------------------------

void	RendererDatas::RegisterTexture(Texture* _texture)
{
	if( !_texture->m_pDriverData)
		m_daTextureToAdd.AddLastItem(_texture);
}

//----------------------------------

void	RendererDatas::UnregisterTexture(Texture* _texture)
{
	if (_texture->m_pDriverData)
	{
		TextureRef newTextureTmp = NEW Texture(FALSE);
		newTextureTmp->m_pDriverData = _texture->m_pDriverData;
		m_daTextureToRemove.AddLastItem(newTextureTmp);
	}
	else 
	{
		U32 idx = 0;
		if( m_daTextureToAdd.Find(_texture,idx))
			m_daTextureToAdd.RemoveItems(idx,1);
	}
}

//----------------------------------

void	RendererDatas::UnregisterShaderData(ShaderData* _data)
{
	if(_data->m_pDriverData)
		m_daShaderDataToRemove.AddLastItem(_data->m_pDriverData);
}

//----------------------------------

void	RendererDatas::UnregisterPrimitve(Primitive3D* _primitive)
{
	if (_primitive->m_pDriverData)
		m_daPrimitiveToRemove.AddLastItem(_primitive->m_pDriverData);
}

//----------------------------------

void	RendererDatas::UnregisterPass(RendererPass* _pass)
{
	if (_pass->m_pDriverData)
		m_daPassToRemove.AddLastItem(_pass->m_pDriverData);

}

//----------------------------------

void	RendererDatas::UnregisterSubpass(RendererSubpass* _subpass)
{
	if (_subpass->m_pDriverData)
		m_daSubpassToRemove.AddLastItem(_subpass->m_pDriverData);
}

//----------------------------------

void	RendererDatas::UnregisterFrameBuffer(FrameBuffer* _frameBuffer)
{
	if (_frameBuffer->m_pDriverData)
		m_daFrameBufferToRemove.AddLastItem(_frameBuffer->m_pDriverData);
}

//----------------------------------

ShaderGroupPtr RendererDatas::CreateShaderGroup(SHADERS _shader, const Str& _combinations)
{
	const SShaderDesc& desc = m_ShaderDescs[_shader];
	StrArray flags;
	_combinations.Split("|", flags);

	// compute crc
	U32 flagId = 0;
	for (const Str& f : flags)
	{
		U32 curId = 0;
		for (U32 c = 0; c < desc.m_Combinations.GetCount(); c++)
		{
			if (desc.m_Combinations[c] == f)
			{
				curId = (1 << c);
				break;
			}
		}

		DEBUG_RequireMessage(curId,"Unknown combination %s for shader group %s", f.GetArray(), desc.m_Name.GetArray());

		flagId |= curId;
		
	}

	ShaderGroupPtr pShader = m_Shaders[_shader][flagId];
	if (pShader.IsNull()) // create it
	{
		Str nName("%s_%d", desc.m_Name.GetArray(), flagId);
		pShader = ResourceManager::GetInstance().GetVirtualResource<ShaderGroup>(nName);
		pShader->AddShader(SHADER_Vertex, Str("shader/%s.vfx", desc.m_VS.GetArray()), flags);
		pShader->AddShader(SHADER_Pixel, Str("shader/%s.pfx", desc.m_PS.GetArray()), flags);
		pShader->AddShader(SHADER_Header, Str("shader/%s.hfx","header"));
		RegisterShader(pShader);
		m_Shaders[_shader][flagId] = pShader;
	}

	return pShader;
}

//----------------------------------

void RendererDatas::DestroyShaders()
{
#define SHADER(_name,_vfx,_pfx, _combinations)	\
	for(U32 i=0; i<m_Shaders[SHADER_##_name].GetCount();i++)	\
		if(m_Shaders[SHADER_##_name][i].IsValid())	\
			UnregisterShader(m_Shaders[SHADER_##_name][i]);
#include "renderer_shader_list.h"
#undef SHADER
	m_Shaders.SetEmpty();
}

//----------------------------------

void	RendererDatas::RegisterShader(const ShaderGroupRef& _shader)
{
	m_daShaderGroupToAdd.AddLastItem(_shader);
}
	
//----------------------------------

void	RendererDatas::UnregisterShader(const ShaderGroupRef& _shader)
{
	m_daShaderGroupToRemove.AddLastItem(_shader);
}

//----------------------------------

//Primitive3DRef&	RendererDatas::CreatePrimitive(Bool _bDynamic /*= FALSE*/)
//{
//	Primitive3DRef newPrimitive;
//	if( _bDynamic )
//		newPrimitive = NEW CDynamiPrimitive3D;
//	else
//		newPrimitive = NEW CStatiPrimitive3D;
//
//	return m_daPrimitives.AddLastItem(newPrimitive);
//}
	
//----------------------------------

MaterialRef&	RendererDatas::CreateMaterial()
{
	MaterialRef newMaterial = NEW Material;
	return m_daMaterials.AddLastItem(newMaterial);
}
	
//----------------------------------

void	RendererDatas::PrepareDraw()
{
	/*for(S32 p=m_daPrimitives.GetCount()-1; p>=0; p--)
	{
		Primitive3DRef& curPrim = m_daPrimitives[p];
		if(curPrim.GetRefCount() == 1)
		{
			m_daPrimitives.RemoveItems(p,1);
		}
		else
		{
			curPrim->PrepareDraw();
		}
	}*/
}

//----------------------------------

void	RendererDatas::ManageMaterials()
{
	for(S32 m=m_daMaterials.GetCount()-1; m>=0; m--)
	{
		MaterialRef& curMat = m_daMaterials[m];
		if(curMat.GetRefCount() == 1)
		{
			m_daMaterials.RemoveItems(m,1);
		}
	}
}

//----------------------------------

void	RendererDatas::ManageDatas(RendererDriver* _pDriver)
{
	ManageTextures(_pDriver);
	ManageShaders(_pDriver);
	UpdateDirtyShaders(_pDriver);

	ManageMaterials();
	for (void* pData : m_daShaderDataToRemove)
		_pDriver->RemoveShaderData(pData);
	m_daShaderDataToRemove.SetEmpty();
	for (void* pData : m_daPrimitiveToRemove)
		_pDriver->RemovePrimitive(pData);
	m_daPrimitiveToRemove.SetEmpty();
	for (void* pData : m_daPassToRemove)
		_pDriver->RemovePass(pData);
	m_daPassToRemove.SetEmpty();
	for (void* pData : m_daSubpassToRemove)
		_pDriver->RemoveSubPass(pData);
	m_daSubpassToRemove.SetEmpty();
	for (void* pData : m_daFrameBufferToRemove)
		_pDriver->RemoveFrameBuffer(pData);
	m_daFrameBufferToRemove.SetEmpty();
}
	
//----------------------------------

void	RendererDatas::ManageTextures(RendererDriver* _pDriver)
{
	// remove texture
	for (U32 i = 0;i<m_daTextureToRemove.GetCount();i++)
	{
		Texture* pTexture = m_daTextureToRemove[i];
		_pDriver->RemoveTexture(pTexture);

#ifndef MASTER
		for (S32 l = 0; l<pTexture->GetNbLevels(); l++)
		{
			m_iCurTextureMemory -= pTexture->GetDataSize(l);
		}
#endif
	}
	m_daTextureToRemove.Flush();

	// add textures
	for (U32 i = 0;i<m_daTextureToAdd.GetCount();i++)
	{
		Texture* pTexture = m_daTextureToAdd[i];
		_pDriver->AddTexture(pTexture);
		if (pTexture->m_pDriverData)
		{
#ifndef MASTER
			for (S32 l = 0; l<pTexture->GetNbLevels(); l++)
			{
				m_iCurTextureMemory += pTexture->GetDataSize(l);
				m_iMaxTextureMemory = Max(m_iMaxTextureMemory, m_iCurTextureMemory);
			}
#endif
		}
	}
	m_daTextureToAdd.Flush();
}

//----------------------------------

void	RendererDatas::ManageShaders(RendererDriver* _pDriver)
{
	// add shader
	for (U32 i = 0;i < m_daShaderGroupToAdd.GetCount();i++)
	{
		_pDriver->CompileShaderGroup(m_daShaderGroupToAdd[i]);
		m_daShaderGroupToAdd[i]->InitParams(_pDriver);
	}
	m_daShaderGroupToAdd.Flush();

	// remove shader
	for (U32 i = 0;i < m_daShaderGroupToRemove.GetCount();i++)
	{
		_pDriver->RemoveShaderGroup(m_daShaderGroupToRemove[i]);
	}
	m_daShaderGroupToRemove.Flush();
}

//----------------------------------

void	RendererDatas::UpdateDirtyShaders(RendererDriver* _pDriver)
{
	for (auto& grouptype : m_Shaders)
		for (auto& group : grouptype)
		{
			if (group.IsValid() && group->IsDirty())
			{
				_pDriver->RemoveShaderGroup(group);
				group->Reload();
				_pDriver->CompileShaderGroup(group);
				group->InitParams(_pDriver);
			}
		}
}
