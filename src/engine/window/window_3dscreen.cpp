#include "window_3dscreen.h"

Window3DScreen::Window3DScreen(S32 _x, S32 _y, S32 _sizex, S32 _sizey):SUPER(_x,_y,_sizex,_sizey)
{
	m_bSizeChanged = FALSE;
	m_bUseEditorScreen = FALSE;
}

void	Window3DScreen::SetSize(S32 _x, S32 _y, S32 _sizex, S32 _sizey)
{
	SUPER::SetSize(_x,_y,_sizex,_sizey);
	m_bSizeChanged = TRUE;
}

//..

Bool Window3DScreen::Create(const Window* _pParentWindow)
{
	if (!SUPER::Create(_pParentWindow))
		return FALSE;

	m_bInitializeDriver = TRUE;
	m_bSizeChanged = TRUE; // to force first init
	m_Renderer.Initialize(this);

	return TRUE;
}

//..

Bool Window3DScreen::Destroy()
{
	return SUPER::Destroy();
}

//..

void Window3DScreen::Finalize(RendererDriver* _pDriver)
{
	m_Renderer.Finalize();
	m_Renderer.FinalizeDriver(_pDriver, this);
}

//..

void Window3DScreen::Redraw(RendererDriver* _pDriver)
{
	if( m_bInitializeDriver )
	{
		if( !m_Renderer.InitializeDriver(_pDriver, this) )
		{
			LOGGER_LogError("Can't initialize renderer.\n");
		}
		else
			m_bInitializeDriver = FALSE;
	}



	if( m_bSizeChanged )
	{
		m_Renderer.ChangeViewport(_pDriver, GetWidth(),GetHeight());
		m_bSizeChanged = FALSE;
	}

	m_Renderer.Draw(_pDriver);
}

void Window3DScreen::Swap(RendererDriver* _pDriver)
{
	m_Renderer.Swap(_pDriver);
}