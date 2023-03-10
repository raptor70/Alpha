#ifndef __WINDOW_3DSCREEN_H__
#define __WINDOW_3DSCREEN_H__

#include "window_whiterect.h"
#include "renderer/renderer.h"

BEGIN_SUPER_CLASS(Window3DScreen,WindowWhiteRect)
public:
	Window3DScreen(S32 _x, S32 _y, S32 _sizex, S32 _sizey);
	virtual ~Window3DScreen(){}

	virtual Bool Create(const Window* _pParentWindow);
	virtual Bool Destroy();

	virtual void	SetSize(S32 _x, S32 _y, S32 _sizex, S32 _sizey);

	void Redraw(RendererDriver* _pDriver);
	void Swap(RendererDriver* _pDriver);
	void Finalize(RendererDriver* _pDriver);

	Renderer*	GetRenderer() { return &m_Renderer; }

	void SetUseEditor(Bool _use) { m_bUseEditorScreen = _use; }
	Bool UseEditor() const { return m_bUseEditorScreen; }

protected:
	Renderer	m_Renderer;
	Bool		m_bInitializeDriver;
	Bool		m_bSizeChanged;
	Bool		m_bUseEditorScreen;
END_SUPER_CLASS

#endif