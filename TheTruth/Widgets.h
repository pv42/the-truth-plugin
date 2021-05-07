/**
 *  imgui spinner extension
 *	 from arcdps-extension by knoxfighter 
 *	 licensed under MIT license
 *	 modified by pv42
 */

#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace ImGuiEx {
	bool Spinner(const char* label, float radius, float thickness, const ImU32& color);
	//bool SpinnerAligned(const char* label, float radius, float thickness, const ImU32& color, Alignment alignment);
}

