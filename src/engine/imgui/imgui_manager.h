#pragma once
#ifndef __IMGUI_MANAGER_H__
#define __IMGUI_MANAGER_H__

#ifdef USE_IMGUI

#include <window/window.h>

class Renderer;

class ImguiManager
{
	SINGLETON_Declare(ImguiManager);

public:
	ImguiManager();
	~ImguiManager();

	void Init(WindowPtr& _pMainWindow, Window3DScreen* _p3DScreen);

	void Update(Float _dTime);
	void Draw(Renderer* _pRenderer);
	void PrepareDraw(RendererDriver* _pDriver);
	void ReDraw(RendererDriver* _pDriver);
	void Swap(RendererDriver* _pDriver);

	void Release();

	void AddInputCharacter(U32 _char);
	Bool UpdateMouseCursor();

	Window3DScreen* GetMain3DScreen();

protected:
	void UpdateInput();
	void AddListToBuffers(const ImDrawList* _pDrawList, VertexBuffer* _pVB, IndexBuffer* _pIB);
	void UpdateMonitors();

	void DrawData(ImDrawData* _pDrawData, Renderer* _pRenderer);

	static void		Platform_CreateWindow(ImGuiViewport* viewport);
	static void		Platform_DestroyWindow(ImGuiViewport* viewport);
	static void		Platform_ShowWindow(ImGuiViewport* viewport);
	static void		Platform_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
	static ImVec2	Platform_GetWindowPos(ImGuiViewport* viewport);
	static void		Platform_SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
	static ImVec2	Platform_GetWindowSize(ImGuiViewport* viewport);
	static void		Platform_SetWindowFocus(ImGuiViewport* viewport);
	static bool		Platform_GetWindowFocus(ImGuiViewport* viewport);
	static bool		Platform_GetWindowMinimized(ImGuiViewport* viewport);
	static void		Platform_SetWindowTitle(ImGuiViewport* viewport, const char* title);
	static void		Platform_SetWindowAlpha(ImGuiViewport* viewport, float alpha);
	static void		Platform_UpdateWindow(ImGuiViewport* viewport);
	static float	Platform_GetWindowDpiScale(ImGuiViewport* viewport); // FIXME-DPI
	static void		Platform_OnChangedViewport(ImGuiViewport* viewport); // FIXME-DPI

	static void		Renderer_CreateWindow(ImGuiViewport* vp);
	static void		Renderer_DestroyWindow(ImGuiViewport* vp);
	static void		Renderer_SetWindowSize(ImGuiViewport* vp, ImVec2 size);
	static void		Renderer_RenderWindow(ImGuiViewport* vp, void* render_arg);
	static void		Renderer_SwapBuffers(ImGuiViewport* vp, void* render_arg);

	Primitive3DRefArray	m_Primitives;
	U32					m_PrimitiveIdx; // frame value
	MaterialRefArray	m_Materials;
	TextureRef			m_Texture;
	ArrayOf<Window3DScreen*> m_ViewportScreens;
	ImGuiMouseCursor m_PreviousCursor;
	Bool	m_bUpdateMonitors;
};

#define IMGUIMGR_Init(_window,_3dscreen)	ImguiManager::GetInstance().Init(_window,_3dscreen)
#define IMGUIMGR_Update(_dTime)				ImguiManager::GetInstance().Update(_dTime)
#define IMGUIMGR_Draw(_renderer)			ImguiManager::GetInstance().Draw(_renderer)
#define IMGUIMGR_PrepareDraw(_driver)		ImguiManager::GetInstance().PrepareDraw(_driver)
#define IMGUIMGR_ReDraw(_driver)			ImguiManager::GetInstance().ReDraw(_driver)
#define IMGUIMGR_Swap(_driver)				ImguiManager::GetInstance().Swap(_driver)
#define IMGUIMGR_Release()					ImguiManager::DestroyInstance()
#define IMGUIMGR_AddInputCharacter(_char)	ImguiManager::GetInstance().AddInputCharacter(_char)
#define IMGUIMGR_UpdateMouseCursor()		ImguiManager::GetInstance().UpdateMouseCursor()

#else

#define IMGUIMGR_Init(_window,_3dscreen)
#define IMGUIMGR_Update(_dTime)
#define IMGUIMGR_Draw(_renderer)
#define IMGUIMGR_PrepareDraw(_driver)
#define IMGUIMGR_ReDraw(_driver)
#define IMGUIMGR_Swap(_driver)
#define IMGUIMGR_Release()
#define IMGUIMGR_AddInputCharacter(_char)
#define IMGUIMGR_UpdateMouseCursor()			FALSE

#endif

#endif