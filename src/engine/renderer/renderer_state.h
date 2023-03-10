#pragma once
#ifndef __RENDERER_STATE_H__
#define __RENDERER_STATE_H__

enum RenderState
{
	RenderState_None = 0,
	RenderState_DepthTest		= (1 << 0),
	RenderState_DepthWrite		= (1 << 1),
	RenderState_AlphaBlend		= (1 << 2),
	RenderState_FaceCulling		= (1 << 3),		
	RenderState_InvertCulling	= (1 << 4),
	RenderState_Additive		= (1 << 5)
};
DECLARE_FLAG_OPERATOR(RenderState)

enum ClearFlag
{
	Clear_None = 0,
	Clear_Color = (1 << 0),
	Clear_Depth = (1 << 1),
};
DECLARE_FLAG_OPERATOR(ClearFlag)

enum RendererViewType
{
	RendererView_Game = 0,
	RendererView_ShadowMap0,
	RendererView_ShadowMap1,
	RendererView_ShadowMap2,
	RendererView_2D,
	RendererView_Count
};

enum DrawBufferType
{
	DRAWBUFFER_None = -1,
	DRAWBUFFER_Opaque = 0,
	DRAWBUFFER_Transparent,
	DRAWBUFFER_FullScreenQuad,
	DRAWBUFFER_Omni,
	DRAWBUFFER_2D,
	DRAWBUFFER_Outline,
#ifdef USE_EDITOR_V2
	DRAWBUFFER_EditorOutline,
	DRAWBUFFER_EditorOutline2D,
	DRAWBUFFER_Gizmo,
#endif
#ifdef USE_DRAWDEBUG
	DRAWBUFFER_DrawDebug,
#endif
#ifdef USE_IMGUI
	DRAWBUFFER_ImGui,
#endif
	DRAWBUFFER_Count
};

enum DrawBufferMode
{
	DRAWBUFFERMODE_None = -1,
	DRAWBUFFERMODE_NoMaterial = 0,
	DRAWBUFFERMODE_Picking,
	
	DRAWBUFFERMODE_Count
};

#endif