#ifndef __EDITOR_3DVIEW_H__
#define __EDITOR_3DVIEW_H__
#include "editor_include.h"
#ifdef USE_EDITOR

BEGIN_EDITOR_CLASS(Editor3DView,QOpenGLWidget)
public :    
    Editor3DView(QWidget* _parent);

protected :
	virtual void initializeGL() OVERRIDE;
    virtual void resizeGL(int _w, int _h) OVERRIDE;
    virtual void paintGL() OVERRIDE;

	virtual void mouseMoveEvent(QMouseEvent *_event) OVERRIDE;
	
	Vec2i	m_lastMousePos;
	QSurfaceFormat	m_Format;
END_EDITOR_CLASS

#endif // USE_EDITOR

#endif // __EDITOR_3DVIEW_H__