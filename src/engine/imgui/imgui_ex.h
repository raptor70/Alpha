#pragma once
#ifndef __IMGUI_EX_H__
#define __IMGUI_EX_H__

#ifdef USE_IMGUI

namespace ImGuiEx
{
	bool SmallInputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

	void Columns(int count = 1, const char* id = NULL, bool border = true, float* ratios = NULL);

	void PushDisabled();
	void PopDisabled();

	ImGuiID SplitDockspace(ImGuiID _dockspaceID, ImGuiDir _dockspaceDir, Float _fRatio, ImGuiID* _outNewDockspaceID);
	void DockWindow(ImGuiID _splitID, const char* _window_name);
}

#endif

#endif