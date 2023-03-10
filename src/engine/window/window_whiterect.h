#ifndef __WINDOW_WHITERECT_H__
#define __WINDOW_WHITERECT_H__

#include "window_component.h"

BEGIN_SUPER_CLASS(WindowWhiteRect,WindowComponent)
public:
	WindowWhiteRect(S32 _x, S32 _y, S32 _sizex, S32 _sizey);
	virtual ~WindowWhiteRect() {}

	virtual Bool Create(const Window* _pParentWindow);
	virtual Bool Destroy();

	virtual void	SetSize(S32 _x, S32 _y, S32 _sizex, S32 _sizey);

    const S32 GetHeight() const { return m_height; }
    const S32 GetWidth() const { return m_width; }
	const S32 GetPosX() const { return m_x; }
	const S32 GetPosY() const { return m_y; }

	virtual void OnResize(S32 _width, S32 _height) { SetSize(GetPosX(),GetPosY(),_width,_height); }

private:
	//forbidden
	WindowWhiteRect(){}
	WindowWhiteRect(const WindowWhiteRect&){}
    const WindowWhiteRect& operator=(const WindowWhiteRect&){ return *this; }

protected:
	S32		m_x,m_y;
	S32		m_width,m_height;
END_SUPER_CLASS

#endif