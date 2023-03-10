#include "imgui_ex.h"
#ifdef USE_IMGUI

#include INCLUDE_LIB(LIB_IMGUI, imgui_internal.h)

bool ImGuiEx::SmallInputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    // same as SmallButton()
    ImGuiContext& g = *GImGui;
    float backup_padding_y = g.Style.FramePadding.y;
    g.Style.FramePadding.y = 0.0f;
    bool changed = ImGui::InputText(label, buf, buf_size, flags, callback, user_data);
    g.Style.FramePadding.y = backup_padding_y;
    return changed;
}

void ImGuiEx::Columns(int count, const char* str_id, bool border, float* ratios)
{
    if (ratios)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImGuiID id = ImGui::GetColumnsID(str_id, count);
        ImGuiOldColumns* columns = ImGui::FindOrCreateColumns(window, id);
        IM_ASSERT(columns->ID == id);

        if (columns->Columns.Size == 0)
        {
            columns->Columns.reserve(count + 1);

            ImGuiOldColumnData column;
            column.OffsetNorm = 0.f;
            columns->Columns.push_back(column);

            for (int n = 0; n < count; n++)
            {
                column.OffsetNorm += ratios[n];
                columns->Columns.push_back(column);
            }
        }
    }

    ImGui::Columns(count, str_id, border);
}

void ImGuiEx::PushDisabled()
{
    ImGuiContext& g = *GImGui;
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g.Style.Alpha * 0.6f);
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
}

void ImGuiEx::PopDisabled()
{
    ImGuiContext& g = *GImGui;
    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
}

ImGuiID ImGuiEx::SplitDockspace(ImGuiID _dockspaceID, ImGuiDir _dockspaceDir, Float _fRatio, ImGuiID* _outNewDockspaceID)
{
    return ImGui::DockBuilderSplitNode(_dockspaceID, _dockspaceDir, _fRatio, nullptr, _outNewDockspaceID);
}

void ImGuiEx::DockWindow(ImGuiID _splitID, const char* _window_name)
{
    ImGui::DockBuilderDockWindow(_window_name, _splitID);
}

//..


#endif
