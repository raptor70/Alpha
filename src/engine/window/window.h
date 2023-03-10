#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "window_menu.h"
#include "window_component.h"
#include "window_ptr.h"

enum WindowStyle
{
    WindowStyle_None = 0,
    WindowStyle_Fullscreen = (1 << 0),
    WindowStyle_Borderless = (1 << 1),
    WindowStyle_InTaskBar = (1 << 2),
    WindowStyle_TopMost = (1 << 3)
};
DECLARE_FLAG_OPERATOR(WindowStyle)

class Window
{
public:
    struct Params
    {
        S32		m_x = 0;
        S32     m_y = 0;
        S32		m_iWidth = 1280;
        S32     m_iHeight = 720;
        Str		m_sTitle = "Untitled";
        WindowStyle m_Style = WindowStyle_InTaskBar;
    };

    static void InitPlatform();
    static void DestroyPlatform();

    Window();
    Window(Params& _params);
    virtual ~Window() {}

    virtual Bool Create(const Window* _pParentWindow);
    virtual Bool Destroy();
    void	ManageMessages(U32Array& _commands);

    // pos (client pos - not full window)
    S32 GetX() { return m_Params.m_x; }
    S32 GetY() { return m_Params.m_y; }
    void    SetPos(S32 _x, S32 _y);

    // size
    S32 GetHeight() const { return m_Params.m_iHeight; }
    S32 GetWidth() const { return m_Params.m_iWidth; }
    void	SetSize(S32 _sizex, S32 _sizey);
    void	SetPosAndSize(S32 _x, S32 _y, S32 _sizex, S32 _sizey);

    // title
    void SetTitle(const Char* _pTitle);

    // style
    const WindowStyle& GetStyle() const { return (m_Params.m_Style); }
    void SetStyle(WindowStyle _style);
    const Bool IsFullscreen() const { return (m_Params.m_Style & WindowStyle_Fullscreen) != 0; }

    void    Show();
    void    SetFocus();
    Bool    HasFocus() const;
    Bool    IsMinimized() const;
    Bool    HasMouseCapture() const;
    void    TransferMouseCapture(Window* _pTargetWindow);

    Bool	MustKill() const { return m_bMustKill; }
    void	Kill() { m_bMustKill = TRUE; }
    Bool    IsManagingInput() const { return m_bManageInput; }
    void	ManageInput(Bool _manage) { m_bManageInput = _manage; }

    void AddMenu(const WindowMenuRef& _Menu);
    void AddComponent(const WindowComponentRef& _Component);

    PtrTo<Object>   GetPlatformData() const { return m_pPlatformData; }
    //..

protected:
    Params  m_Params;
    Bool	m_bMustKill;
    Bool	m_bManageInput;

    //..

private:

    // forbidden
    Window(Window&) {}
    Window& operator=(Window&) { return *this; }

    //..

#ifdef WIN32
    RefTo<Object>	m_pPlatformData;
#endif
    WindowMenuRefArray		m_aMenus;
    WindowComponentRefArray m_aComponents;
};


#endif