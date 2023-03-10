#ifndef __WINDOW_TEXTBOX_H__
#define __WINDOW_TEXTBOX_H__

#include "window_component.h"

BEGIN_SUPER_CLASS(WindowTextBox,WindowComponent)
public:
	WindowTextBox(S32 _x, S32 _y, S32 _sizex, S32 _sizey);
	virtual ~WindowTextBox() {}

	virtual Bool Create(const Window* _pParentWindow);
	virtual Bool Destroy();

    const S32 GetHeight() const { return m_height; }
    const S32 GetWidth() const { return m_width; }
	const S32 GetPosX() const { return m_x; }
	const S32 GetPosY() const { return m_y; }

	const Char* GetText();
	void	ClearText();

private:
	//forbidden
	WindowTextBox(){}
	WindowTextBox(const WindowTextBox&){}
const WindowTextBox& operator=(const WindowTextBox&){ return *this; }

protected:
	S32		m_x,m_y;
	S32		m_width,m_height;
	Char	m_sText[256];
END_SUPER_CLASS

#endif