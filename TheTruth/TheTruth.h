#pragma once
#include <memory>
#include "arcdps.h"
#include "MumbleApi.h"
#include "Settings.h"
#include "SheetsAPI.h"

class TheTruth {
public:
	TheTruth();
	static std::unique_ptr<TheTruth> theTruth;
	static arcdps_exports* s_init();
	static uintptr_t s_release();
	static uintptr_t s_imgui(uint32_t not_charsel_or_loading);
	static void s_uiOptions();
	static uintptr_t s_window_nofilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	uintptr_t WindowNFCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	arcdps_exports* Init();
	uintptr_t Release();
	void ImGui(uint32_t not_charsel_or_loading);
	void UIOptions();
private:
	void drawSettingsUI();
	void drawBigUI(int currentWing);
	void drawSmallUI(int wing, vector<string>& roles, map<string, ImVec4> colorMap, const string& mainRole, shared_ptr<ImVec4> mainRoleColor);
	MumbleApi mumbleApi;
	shared_ptr<SheetsAPI> sheetsAPI;
	Settings settings;
	arcdps_exports arc_exports;
	bool showGoogleSheetSetup;
	bool showCustomColorSetup;
	bool showBigUI;
	bool showSmallUI;
	static bool crashed; // if an uncaught exception is encountered at top level try to disable this plugin at top level
	// keyIn
	void InputKey(const char* label, Key* key, arc_config& cf);
	const char* currentInputKeyLabel;
	int* currentInputKeyTarget;
};

