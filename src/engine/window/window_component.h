#ifndef __WINDOW_COMPONENT_H__
#define __WINDOW_COMPONENT_H__

#include "window_ptr.h"

class WindowComponent
{
public:
	WindowComponent();
	virtual ~WindowComponent(){}

	PtrTo<Object>   GetPlatformData() const { return m_pPlatformData; }

	virtual Bool Create(const Window* _pParentWindow) { return TRUE; }
	virtual Bool Destroy() = 0;

	virtual void OnResize(S32 _width, S32 _height) {}

protected:
	RefTo<Object>	m_pPlatformData;
};

typedef RefTo<WindowComponent> WindowComponentRef;
typedef ArrayOf<WindowComponentRef> WindowComponentRefArray;

#endif