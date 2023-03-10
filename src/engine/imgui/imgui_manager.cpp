#include "imgui_manager.h"
#ifdef USE_IMGUI

#include <input/input_manager.h>
#include <renderer/renderer.h>
#include <file/base_file.h>
#include <platform/platform_manager.h>

SINGLETON_Define(ImguiManager);

ImguiManager::ImguiManager()
{
    m_PrimitiveIdx = 0;
}

//------------------------------------------

ImguiManager::~ImguiManager()
{
	Release();
}

//------------------------------------------

void SetStyleFromFlags(const ImGuiViewportFlags& _flags, Window::Params& _params)
{
    if (_flags & ImGuiViewportFlags_NoDecoration)
        _params.m_Style |= WindowStyle_Borderless;

    if (_flags & ImGuiViewportFlags_NoTaskBarIcon)
        _params.m_Style &= (~WindowStyle_InTaskBar);

    if (_flags & ImGuiViewportFlags_TopMost)
        _params.m_Style |= WindowStyle_TopMost;
}

//------------------------------------------

void ImguiManager::Platform_CreateWindow(ImGuiViewport* viewport)
{
    // Get Parent
    Window* pParentWindow = NULL;
    if (viewport->ParentViewportId != 0)
        if (ImGuiViewport* pParentViewport = ImGui::FindViewportByID(viewport->ParentViewportId))
            pParentWindow = (Window*)pParentViewport->PlatformHandle;

    // Params
    Window::Params params;
    params.m_x = (S32)viewport->Pos.x;
    params.m_y = (S32)viewport->Pos.y;
    params.m_iWidth = (S32)viewport->Size.x;
    params.m_iHeight = (S32)viewport->Size.y;
    params.m_sTitle = "ImGui Platform"; // replace by window name
    SetStyleFromFlags(viewport->Flags, params);

    // 3D Screen
    Window3DScreen* p3DScreen = NEW Window3DScreen(0, 0, params.m_iWidth, params.m_iHeight);
    p3DScreen->GetRenderer()->SetPipeline(RendererPipeline::ImGui);

    // Create window
    Window* pNewWindow = NEW Window(params);
    pNewWindow->AddComponent(p3DScreen);
    pNewWindow->Create(pParentWindow);

    viewport->PlatformRequestResize = false;
    viewport->PlatformHandle = viewport->PlatformHandleRaw = pNewWindow;
    viewport->RendererUserData = p3DScreen;
    ImguiManager::GetInstance().m_ViewportScreens.AddLastItem(p3DScreen);
}

//------------------------------------------

void ImguiManager::Platform_DestroyWindow(ImGuiViewport* viewport)
{
    Window* pMainWindow = (Window*)ImGui::GetMainViewport()->PlatformHandle;
    Window* pWindow = (Window*)viewport->PlatformHandle;
    if (pWindow && pMainWindow != pWindow)
    {
        if (pWindow->HasMouseCapture())
        {
            Window* pParentWindow = NULL;
            if (viewport->ParentViewportId != 0)
                if (ImGuiViewport* pParentViewport = ImGui::FindViewportByID(viewport->ParentViewportId))
                    pWindow->TransferMouseCapture((Window*)pParentViewport->PlatformHandle);
            
        }
        
        pWindow->Destroy();
        DELETE pWindow;

        U32 idx = 0;
        if (ImguiManager::GetInstance().m_ViewportScreens.Find((Window3DScreen*)viewport->RendererUserData, idx))
            ImguiManager::GetInstance().m_ViewportScreens.RemoveItems(idx);
    }

    viewport->PlatformUserData = viewport->PlatformHandle = NULL;
    viewport->RendererUserData = NULL;
}

//------------------------------------------

void ImguiManager::Platform_ShowWindow(ImGuiViewport* viewport)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    pWindow->Show();
    if ((viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing) == 0)
        pWindow->SetFocus();
}

//------------------------------------------

void ImguiManager::Platform_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    pWindow->SetPos((S32)pos.x, (S32)pos.y);
}

//------------------------------------------

ImVec2 ImguiManager::Platform_GetWindowPos(ImGuiViewport* viewport)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    return ImVec2((Float)pWindow->GetX(), (Float)pWindow->GetY());
}

//------------------------------------------

void ImguiManager::Platform_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    pWindow->SetSize((S32)size.x, (S32)size.y);
}

//------------------------------------------

ImVec2 ImguiManager::Platform_GetWindowSize(ImGuiViewport* viewport)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    return ImVec2((Float)pWindow->GetWidth(), (Float)pWindow->GetHeight());
}

//------------------------------------------

void ImguiManager::Platform_SetWindowFocus(ImGuiViewport* viewport)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    pWindow->SetFocus();
}

//------------------------------------------

bool ImguiManager::Platform_GetWindowFocus(ImGuiViewport* viewport)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    return pWindow->HasFocus();
}

//------------------------------------------

bool ImguiManager::Platform_GetWindowMinimized(ImGuiViewport* viewport)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    return pWindow->IsMinimized();
}

//------------------------------------------

void ImguiManager::Platform_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    pWindow->SetTitle(title);
}

//------------------------------------------

void ImguiManager::Platform_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
    // todo
}

//------------------------------------------

void ImguiManager::Platform_UpdateWindow(ImGuiViewport* viewport)
{
    Window* pWindow = (Window*)viewport->PlatformHandle;
    Window::Params params;
    SetStyleFromFlags(viewport->Flags, params);
    if (params.m_Style != pWindow->GetStyle())
    {
        pWindow->SetStyle(params.m_Style);
        viewport->PlatformRequestMove = viewport->PlatformRequestResize = true;
    }
}

//------------------------------------------

float ImguiManager::Platform_GetWindowDpiScale(ImGuiViewport* viewport) // FIXME-DPI
{
    return 1.f;
}

//------------------------------------------

void ImguiManager::Platform_OnChangedViewport(ImGuiViewport* viewport) // FIXME-DPI
{
    // todo
}

//------------------------------------------

void ImguiManager::Renderer_CreateWindow(ImGuiViewport* vp)
{

}

//------------------------------------------

void ImguiManager::Renderer_DestroyWindow(ImGuiViewport* vp)
{

}

//------------------------------------------

void ImguiManager::Renderer_SetWindowSize(ImGuiViewport* vp, ImVec2 size)
{

}

//------------------------------------------

void ImguiManager::Renderer_RenderWindow(ImGuiViewport* vp, void* render_arg)
{
    Window3DScreen* p3DScreen = (Window3DScreen*)vp->RendererUserData;
    ImguiManager::GetInstance().DrawData(vp->DrawData, p3DScreen->GetRenderer());
}

//------------------------------------------

void ImguiManager::Renderer_SwapBuffers(ImGuiViewport* vp, void* render_arg)
{

}

//------------------------------------------

void ImguiManager::Init(WindowPtr& _pMainWindow, Window3DScreen* _p3DScreen)
{
     // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup backend capabilities flags
    m_bUpdateMonitors = TRUE;
    m_PreviousCursor = ImGuiMouseCursor_COUNT;

    io.BackendPlatformName = "imgui_impl_win32";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    //io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can call io.AddMouseViewportEvent() with correct data (optional)
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
        
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = _pMainWindow.GetPtr();
    main_viewport->RendererUserData = _p3DScreen;

#define IMGUI_INI   "imgui.ini"
#define IMGUI_LOG   "imgui.log"

    io.IniFilename = SAVE_DIR IMGUI_INI;
    io.LogFilename = LOG_DIR IMGUI_LOG;

    // to create SAVE dir
    {
        OldFile file(IMGUI_INI, OldFile::USE_TYPE(OldFile::USE_TYPE_Write | OldFile::USE_TYPE_Data));
        file.Open();
        file.Close();
    }

    // to create LOG dir
    {
        OldFile file(IMGUI_LOG, OldFile::USE_TYPE(OldFile::USE_TYPE_Write | OldFile::USE_TYPE_Log));
        file.Open();
        file.Close();
    }

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Tab] =           KEY_Tab;
    io.KeyMap[ImGuiKey_LeftArrow] =     KEY_Left;
    io.KeyMap[ImGuiKey_RightArrow] =    KEY_Right;
    io.KeyMap[ImGuiKey_UpArrow] =       KEY_Up;
    io.KeyMap[ImGuiKey_DownArrow] =     KEY_Down;
    io.KeyMap[ImGuiKey_PageUp] =        KEY_PageUp;
    io.KeyMap[ImGuiKey_PageDown] =      KEY_PageDown;
    io.KeyMap[ImGuiKey_Home] =          KEY_Home;
    io.KeyMap[ImGuiKey_End] =           KEY_End;
    io.KeyMap[ImGuiKey_Insert] =        KEY_Insert;
    io.KeyMap[ImGuiKey_Delete] =        KEY_Delete;
    io.KeyMap[ImGuiKey_Backspace] =     KEY_BackSpace;
    io.KeyMap[ImGuiKey_Space] =         KEY_Space;
    io.KeyMap[ImGuiKey_Enter] =         KEY_Enter;
    io.KeyMap[ImGuiKey_Escape] =        KEY_Escape;
    io.KeyMap[ImGuiKey_KeyPadEnter] =   KEY_Enter;
    io.KeyMap[ImGuiKey_A] =             KEY_A;
    io.KeyMap[ImGuiKey_C] =             KEY_C;
    io.KeyMap[ImGuiKey_V] =             KEY_V;
    io.KeyMap[ImGuiKey_X] =             KEY_X;
    io.KeyMap[ImGuiKey_Y] =             KEY_Y;
    io.KeyMap[ImGuiKey_Z] =             KEY_Z;

    UpdateMonitors();

    // Register platform interface (will be coupled with a renderer interface)
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Platform_CreateWindow = ImguiManager::Platform_CreateWindow;
    platform_io.Platform_DestroyWindow = ImguiManager::Platform_DestroyWindow;
    platform_io.Platform_ShowWindow = ImguiManager::Platform_ShowWindow;
    platform_io.Platform_SetWindowPos = ImguiManager::Platform_SetWindowPos;
    platform_io.Platform_GetWindowPos = ImguiManager::Platform_GetWindowPos;
    platform_io.Platform_SetWindowSize = ImguiManager::Platform_SetWindowSize;
    platform_io.Platform_GetWindowSize = ImguiManager::Platform_GetWindowSize;
    platform_io.Platform_SetWindowFocus = ImguiManager::Platform_SetWindowFocus;
    platform_io.Platform_GetWindowFocus = ImguiManager::Platform_GetWindowFocus;
    platform_io.Platform_GetWindowMinimized = ImguiManager::Platform_GetWindowMinimized;
    platform_io.Platform_SetWindowTitle = ImguiManager::Platform_SetWindowTitle;
    platform_io.Platform_SetWindowAlpha = ImguiManager::Platform_SetWindowAlpha;
    platform_io.Platform_UpdateWindow = ImguiManager::Platform_UpdateWindow;
    platform_io.Platform_GetWindowDpiScale = ImguiManager::Platform_GetWindowDpiScale; // FIXME-DPI
    platform_io.Platform_OnChangedViewport = ImguiManager::Platform_OnChangedViewport; // FIXME-DPI
    platform_io.Renderer_CreateWindow = ImguiManager::Renderer_CreateWindow;
    platform_io.Renderer_DestroyWindow = ImguiManager::Renderer_DestroyWindow;
    platform_io.Renderer_SetWindowSize = ImguiManager::Renderer_SetWindowSize;
    platform_io.Renderer_RenderWindow = ImguiManager::Renderer_RenderWindow;
    platform_io.Renderer_SwapBuffers = ImguiManager::Renderer_SwapBuffers;


    // Font
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Setup Dear ImGui style (TODO : options imgui)
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    
    Texture* pTexture = NEW Texture();
    pTexture->Init(width, height, Texture::TEXTUREFORMAT_RGBA8, pixels);

    MaterialRef& baseMat = m_Materials.AddLastItem(RendererDatas::GetInstance().CreateMaterial());
    baseMat->SetTexture(pTexture);
}

//------------------------------------------

void ImguiManager::Update(Float _dTime)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = _dTime;

    UpdateInput();

    // tmp
    Window* pMainWindow = (Window * )ImGui::GetMainViewport()->PlatformHandle;
    io.DisplaySize = ImVec2((float)pMainWindow->GetWidth(), (float)pMainWindow->GetHeight());

    if (m_bUpdateMonitors)
        UpdateMonitors();

    ImGui::NewFrame();

    if(DEBUGFLAG(ImGuiDemo))
        ImGui::ShowDemoWindow(DebugFlagList::GetInstance().GetFlagPtr(DEBUGFLAG_ImGuiDemo));

        /*if (Renderer::GetInstance()->IsInitialized())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
            if (ImGui::Begin("TestViewport",NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
            {
                // TODO : register ImGui Material
                if (m_Materials.GetCount() < 2)
                {
                    MaterialRef& gameMat = m_Materials.AddLastItem(RendererDatas::GetInstance().CreateMaterial());
                    gameMat->SetTexture(Renderer::GetInstance()->GetGameRendererTexture());
                }

                if (m_Materials.GetCount() > 1)
                {
                    Texture* pTexture = m_Materials[1]->GetTexture();
                    Float textureRatio = Float(pTexture->GetSizeX()) / Float(pTexture->GetSizeY());
                    ImVec2 windowSize = ImGui::GetWindowSize();
                    ImVec2 imageSize = windowSize;
                    Float newY = imageSize.x / textureRatio;
                    if (newY > imageSize.y)
                    {
                        imageSize.x = imageSize.y * textureRatio;
                        //ImGui::SameLine((windowSize.x - imageSize.x) / 2.f);
                    }
                    else
                        imageSize.y = newY;

                    ImGui::SetCursorPos(ImVec2((windowSize.x - imageSize.x) / 2.f, (windowSize.y - imageSize.y) / 2.f));
                    ImGui::Image(1, imageSize, ImVec2(0, 1), ImVec2(1, 0));
                }
                ImGui::End();
            }
            ImGui::PopStyleVar(3);
        }*/
}

//------------------------------------------

void ImguiManager::UpdateInput()
{
    ImGuiIO& io = ImGui::GetIO();

    // Keyboard
    DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
    io.KeyCtrl = keyboard.IsPressed(KEY_Control) || keyboard.IsPressed(KEY_LControl);
    io.KeyShift = keyboard.IsPressed(KEY_Shift) || keyboard.IsPressed(KEY_LShift);
    io.KeyAlt = keyboard.IsPressed(KEY_Alt) || keyboard.IsPressed(KEY_LAlt);
    io.KeySuper = false;
    for (S32 i = 0; i < 256; i++)
        io.KeysDown[i] = keyboard.IsPressed((KEY)i);

    // Text (TODO)
    // io.AddInputCharacter((unsigned int)wParam);

    // Mouse
    DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    if (mouse.IsInside())
        io.MousePos = ImVec2((Float)mouse.GetPos().x, (Float)mouse.GetPos().y);
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        io.MousePos = ImVec2((Float)mouse.GetScreenPos().x, (Float)mouse.GetScreenPos().y);
    io.MouseDown[0] = mouse.IsPressed(MOUSE_Left);
    io.MouseDown[1] = mouse.IsPressed(MOUSE_Right);
    io.MouseDown[2] = mouse.IsPressed(MOUSE_Middle);
    io.MouseWheel = mouse.GetWheelDelta();

    // Mouse cursor
    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (m_PreviousCursor != imgui_cursor)
    {
        m_PreviousCursor = imgui_cursor;
        UpdateMouseCursor();
    }
}

//------------------------------------------

Bool ImguiManager::UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
        return FALSE;
    
    DeviceMouse& mouse = InputManager::GetInstance().GetMouse();
    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        mouse.SetCursor(DeviceMouseCursor_None);
    }
    else
    {
        // Show OS mouse cursor
        DeviceMouseCursor newCursor = DeviceMouseCursor_None;
        switch (imgui_cursor)
        {
        case ImGuiMouseCursor_Arrow:        newCursor = DeviceMouseCursor_Arrow; break;
        case ImGuiMouseCursor_TextInput:    newCursor = DeviceMouseCursor_TextInput; break;
        case ImGuiMouseCursor_ResizeAll:    newCursor = DeviceMouseCursor_ResizeAll; break;
        case ImGuiMouseCursor_ResizeEW:     newCursor = DeviceMouseCursor_ResizeEW; break;
        case ImGuiMouseCursor_ResizeNS:     newCursor = DeviceMouseCursor_ResizeNS; break;
        case ImGuiMouseCursor_ResizeNESW:   newCursor = DeviceMouseCursor_ResizeNESW; break;
        case ImGuiMouseCursor_ResizeNWSE:   newCursor = DeviceMouseCursor_ResizeNWSE; break;
        case ImGuiMouseCursor_Hand:         newCursor = DeviceMouseCursor_Hand; break;
        case ImGuiMouseCursor_NotAllowed:   newCursor = DeviceMouseCursor_NotAllowed; break;
        }
        mouse.SetCursor(newCursor);
    }
    return TRUE;
}

//------------------------------------------

Window3DScreen* ImguiManager::GetMain3DScreen()
{
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    return (Window3DScreen*)main_viewport->RendererUserData;
}

//------------------------------------------

void ImguiManager::DrawData(ImDrawData* _pDrawData, Renderer* _pRenderer)
{
    if (_pDrawData->CmdListsCount != 0)
    {
        Window* pWindow = (Window *)_pDrawData->OwnerViewport->PlatformHandle;
        if (!pWindow)
            return;

        Float WindowX = (Float)pWindow->GetX();
        Float WindowY = (Float)pWindow->GetY();

        /*
        Float L = _pDrawData->DisplayPos.x;
        Float R = _pDrawData->DisplayPos.x + _pDrawData->DisplaySize.x;
        Float T = _pDrawData->DisplayPos.y;
        Float B = _pDrawData->DisplayPos.y + _pDrawData->DisplaySize.y;
        */

        for (int n = 0; n < _pDrawData->CmdListsCount; n++)
        {
            const ImDrawList* pList = _pDrawData->CmdLists[n];

            // create primitive
            Primitive3DRef pPrimitive;
            if (m_PrimitiveIdx >= m_Primitives.GetCount())
            {
                pPrimitive = Primitive3D::Create(Str("imgui_%d", m_PrimitiveIdx), TRUE);
                m_Primitives.AddLastItem(pPrimitive);
            }
            else
                pPrimitive = m_Primitives[m_PrimitiveIdx];
            m_PrimitiveIdx++;

            // set primitive
            VertexBufferRef vb;
            IndexBufferRef ib;
            pPrimitive->Lock(vb, ib);
            vb->SetVertexCount(0);
            ib->m_Array.SetItemCount(0);
            AddListToBuffers(pList, vb, ib);
            pPrimitive->Unlock();

            for (int cmd_i = 0; cmd_i < pList->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &pList->CmdBuffer[cmd_i];

                Renderer::SDrawPtr pDraw = _pRenderer->CreateDraw();
                pDraw->m_Type = Geometry_TrianglesList;
                pDraw->m_Primitive = pPrimitive;
                pDraw->m_pMaterial = m_Materials[pcmd->TextureId];
                pDraw->m_iIndiceCount = pcmd->ElemCount; // nb triangles
                pDraw->m_iIBOffset = pcmd->IdxOffset;
                //pDraw->m_iVBOffset = pcmd->VtxOffset; // need enable flag ImGuiBackendFlags_RendererHasVtxOffset
                pDraw->m_mModelMatrix.SetTranslation(Vec3f(-WindowX, -WindowY, 0.f));

                pDraw->m_bClipped = TRUE;
                pDraw->m_ClipRect.m_vTopLeft.x = pcmd->ClipRect.x - WindowX;
                pDraw->m_ClipRect.m_vTopLeft.y = pcmd->ClipRect.y - WindowY;
                pDraw->m_ClipRect.m_vSize.x = pcmd->ClipRect.z - pcmd->ClipRect.x;
                pDraw->m_ClipRect.m_vSize.y = pcmd->ClipRect.w - pcmd->ClipRect.y;

                _pRenderer->PushDraw(DRAWBUFFER_ImGui, pDraw);

                /*BmTexId_Z bmpTexId = IsValidTexId(pcmd->TextureId.texId) ? pcmd->TextureId.texId : s_FontTexture->GetTexId();
                gData.MainRdr->SetActiveTextureNoLock(bmpTexId, pcmd->TextureId.userResourceSlot);
                */
            }
        }
    }
}

//------------------------------------------

void ImguiManager::Draw(Renderer* _pRenderer)
{
    ImGui::Render();

    m_PrimitiveIdx = 0;

    // Render command lists
    DrawData(ImGui::GetDrawData(), _pRenderer);
}

//------------------------------------------

void ImguiManager::PrepareDraw(RendererDriver* _pDriver)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) // in prepare draw to be threadsafe
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    for (auto& screen : m_ViewportScreens)
        screen->GetRenderer()->PrepareDraw(_pDriver);
    for (auto& prim : m_Primitives)
        prim->PrepareDraw();
}

//------------------------------------------

void ImguiManager::ReDraw(RendererDriver* _pDriver)
{
    for (auto& screen : m_ViewportScreens)
        screen->Redraw(_pDriver);
}

//------------------------------------------

void ImguiManager::Swap(RendererDriver* _pDriver)
{
    for (auto& screen : m_ViewportScreens)
        screen->Swap(_pDriver);
}

//------------------------------------------

void ImguiManager::AddListToBuffers(const ImDrawList* _pDrawList, VertexBuffer* _pVB, IndexBuffer* _pIB)
{
    U16 index = _pVB->GetVertexCount();
    _pVB->SetVertexCount(index + _pDrawList->VtxBuffer.Size);
    for (S32 i = 0; i < _pDrawList->VtxBuffer.Size; i++, index++)
    {
        const ImDrawVert& v = _pDrawList->VtxBuffer.Data[i];

        _pVB->SetVertexAtIndex(
            index,
            Vec3f(v.pos.x, v.pos.y, 0.f),
            Vec2f(v.uv.x, v.uv.y),
            -Vec3f::ZAxis,
            Color(v.col)
        );
    }

    U32 count = _pIB->m_Array.GetCount();
    _pIB->m_Array.SetItemCount(count +_pDrawList->IdxBuffer.Size);
    memcpy(_pIB->m_Array.GetPointer() + count, _pDrawList->IdxBuffer.Data, _pDrawList->IdxBuffer.Size * sizeof(U16));
#ifdef _DEBUG
    DEBUG_Require(sizeof(U16) == sizeof(ImDrawIdx));
#endif
}

//------------------------------------------

void ImguiManager::UpdateMonitors()
{
    ImGui::GetPlatformIO().Monitors.resize(0);
    PlatformManager::GetInstance().VisitMonitors([&](auto& _monitor)
        {
            ImGuiPlatformMonitor imgui_monitor;
            imgui_monitor.MainPos = ImVec2(_monitor.m_vPos.x, _monitor.m_vPos.y);
            imgui_monitor.MainSize = ImVec2(_monitor.m_vSize.x, _monitor.m_vSize.y);
            imgui_monitor.WorkPos = ImVec2(_monitor.m_vWorkPos.x, _monitor.m_vWorkPos.y);
            imgui_monitor.WorkSize = ImVec2(_monitor.m_vWorkSize.x, _monitor.m_vWorkSize.y);
            imgui_monitor.DpiScale = 1.f;// ImGui_ImplWin32_GetDpiScaleForMonitor(monitor);
            ImGuiPlatformIO& io = ImGui::GetPlatformIO();
            if (_monitor.m_bPrimary)
                io.Monitors.push_front(imgui_monitor);
            else
                io.Monitors.push_back(imgui_monitor);
        });
    m_bUpdateMonitors = FALSE;
}

//------------------------------------------

void ImguiManager::Release()
{
    ImGui::DestroyContext();
}

//------------------------------------------

void ImguiManager::AddInputCharacter(U32 _char)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter((unsigned int)_char);
}
#endif
