#pragma once
#ifndef __RENDERER_PASS_H__
#define __RENDERER_PASS_H__

#include "renderer_state.h"
#include "renderer_shader.h"

struct SRendererPassTextureSlot
{
	U32						m_iSlot;
	ShaderGroup::Params	m_ShaderParam;
	TexturePtr				m_pTexture;
};


BEGIN_SUPER_CLASS(RendererSubpass, Resource)
public:
	RendererSubpass()
	{
		for (U32& s : m_Slots)
			s = -1;
	}
	virtual ~RendererSubpass();

	static  const Char* GetResourcePrefix() { return "renderer_subpass"; }

	RenderState			m_RenderState = RenderState_None;
	RendererViewType	m_ViewType = RendererView_Game;
	ShaderGroupPtr		m_pShader = NULL;
	DrawBufferType		m_DrawBuffer = DRAWBUFFER_None;
	DrawBufferMode		m_DrawBufferMode = DRAWBUFFERMODE_None;

	void AddTextureSlot(U32 _iSlot, const ShaderGroup::Params& _param, const TexturePtr& _pTexture)
	{
		m_TextureSlots.AddLastItem({ _iSlot ,_param, _pTexture });
		m_Slots[_param] = _iSlot;
	}

	ArrayOf<SRendererPassTextureSlot>	m_TextureSlots;
	U32									m_Slots[ShaderGroup::PARAM_Count] = {};
END_SUPER_CLASS

typedef RefTo<RendererSubpass>		RendererSubpassRef;
typedef PtrTo<RendererSubpass>		RendererSubpassPtr;
typedef ArrayOf<RendererSubpassRef> RendererSubpassRefArray;

BEGIN_SUPER_CLASS(RendererPass, Resource)
public:
	virtual ~RendererPass();
	static  const Char* GetResourcePrefix() { return "renderer_pass"; }

	RendererSubpassRef	AddSubpass(const Char* _pName = "");

	FrameBufferRef				m_pFrameBuffer;
	RendererSubpassRefArray		m_Subpasses;

	ClearFlag				m_ClearFlag = Clear_None;
	Color					m_ClearColor = Color::Black;
	Float					m_ClearDepth = 1.f;
END_SUPER_CLASS

typedef RefTo<RendererPass>		RendererPassRef;
typedef PtrTo<RendererPass>		RendererPassPtr;
typedef ArrayOf<RendererPassRef>	RendererPassRefArray;
typedef ArrayOf<RendererPassPtr>	RendererPassPtrArray;

#endif
