#include "editor_3dview.h"
#ifdef USE_EDITOR

#include "application.h"
#include "input/input_manager.h"

extern S32 g_GLDefaultFrameBuffer;

//..

Editor3DView::Editor3DView(QWidget* _parent):QOpenGLWidget(_parent)
{  
	setMouseTracking(TRUE);
	m_lastMousePos = Vec2i::Zero;

	m_Format = QSurfaceFormat::defaultFormat();
	m_Format.setSwapInterval(0);
	setFormat(m_Format);
}

//..

void Editor3DView::initializeGL() 
{
	g_GLDefaultFrameBuffer = defaultFramebufferObject();
}

//..

void Editor3DView::resizeGL(int _w, int _h) 
{
	Application::GetInstance().SetWindowSize(_w,_h);
}

//..

void Editor3DView::paintGL()
{
	g_GLDefaultFrameBuffer = defaultFramebufferObject();
	format().setSwapInterval(0);
	DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
	mouse.UpdateValue(m_lastMousePos);
	Application::GetInstance().DoFrame();
	update();
}

//..

void Editor3DView::mouseMoveEvent(QMouseEvent *_event) 
{
	SUPER::mouseMoveEvent(_event);
	m_lastMousePos = Vec2i(_event->localPos().x(),_event->localPos().y());
}


#endif // USE_EDITOR