#include "TheTruth.h"
#include "logger.h"
#include "GW2.h"
#include "imgui/imgui.h"
#include "string_format.hpp"
#include <algorithm>
#include "api_key.h"
#include "Widgets.h"
#include "base64.h"


std::unique_ptr<TheTruth> TheTruth::theTruth;

bool TheTruth::crashed = false;

TheTruth::TheTruth() {
	string key = GOOGLE_API_KEY;
	settings = std::make_shared<Settings>();
	sheetsAPI = std::make_shared<SheetsAPI>(key, settings);
	showSmallUI = true;
	showBigUI = false;
	showGoogleSheetSetup = false;
}

arcdps_exports* TheTruth::s_init() {
	try {
		return TheTruth::theTruth->Init();
	} catch (std::exception e) {
		crashed = true;
		Logger::e("crash at top level during init, trying to disable:");
		Logger::e(e.what());
	}
	return NULL;
}

uintptr_t TheTruth::s_release() {
	if (crashed) return 0;
	try {
		return TheTruth::theTruth->Release();
	} catch (std::exception e) {
		crashed = true;
		Logger::e("crash at top level during release, trying to disable:");
		Logger::e(e.what());
	}
	return 0;
}

uintptr_t TheTruth::s_imgui(uint32_t not_charsel_or_loading) {
	if (crashed) return 0;
	try {
		TheTruth::theTruth->ImGui(not_charsel_or_loading);
	} catch (std::exception e) {
		crashed = true;
		Logger::e("crash at top level during imgui, trying to disable:");
		Logger::e(e.what());
	}
	return 0;
}

void TheTruth::s_uiOptions() {
	if (crashed) return;
	try {
		TheTruth::theTruth->UIOptions();
	} catch (std::exception e) {
		crashed = true;
		Logger::e("crash at top level during uioptions, trying to disable:");
		Logger::e(e.what());
	}
}

uintptr_t TheTruth::s_window_nofilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (crashed) return uMsg;
	try {
		return TheTruth::theTruth->WindowNFCallback(hWnd, uMsg, wParam, lParam);
	} catch (std::exception e) {
		crashed = true;
		Logger::e("crash at top level during unfiltered window callback, trying to disable:");
		Logger::e(e.what());
	}
	return uMsg;
}


arcdps_exports* TheTruth::Init() {
	mumbleApi.initialize();
	arc_exports.sig = 0x69428;
	arc_exports.imguivers = 18000;
	arc_exports.size = sizeof(arcdps_exports);
	arc_exports.out_name = "The Truth";
	arc_exports.out_build = __DATE__;
	arc_exports.wnd_nofilter = TheTruth::s_window_nofilter;
	arc_exports.wnd_filter = NULL;
	arc_exports.combat = NULL;
	arc_exports.combat_local = NULL;
	arc_exports.imgui = TheTruth::s_imgui;
	arc_exports.options_windows = NULL;
	arc_exports.options_end = TheTruth::s_uiOptions;
	Logger::i("loaded The Truth\n");
	if (!theTruth) {
		Logger::i("[MAIN] init did not work :/");
	} else if (theTruth.get() != this) {
		Logger::i("[MAIN] init value is different");
	}
	return &arc_exports;
}

uintptr_t TheTruth::Release() {
	settings->save();
	Logger::free();
	return 0;
}

boolean isRaidMap(int mapId) {
	return mapId == WING_1_MAP || mapId == WING_2_MAP || mapId == WING_3_MAP || mapId == WING_4_MAP || mapId == WING_5_MAP || mapId == WING_6_MAP || mapId == WING_7_MAP;
}

const map<int, string> keycodes = {
	{VK_LBUTTON, "Mouse 1"},
	{VK_RBUTTON, "Mouse 2"},
	{VK_CANCEL, "CtrBrk"},
	{VK_MBUTTON, "Mouse 3"},
	// VK_XBUTTON1, VK_XBUTTON2
	{VK_BACK, "BACK"},
	{VK_TAB, "Tab"},
	{VK_CLEAR, "Clear"},
	{VK_RETURN, "Return"},
	{VK_SHIFT, "Shift"},
	{VK_CONTROL, "Ctrl"},
	{VK_MENU, "Alt"},
	{VK_PAUSE, "Pause"},
	{VK_CAPITAL, "Caps"},
	// varios ime keys
	{VK_ESCAPE, "Escape"},
	// varios ime keys
	{VK_SPACE, "Space"},
	{VK_PRIOR, "Page Up"},
	{VK_NEXT, "Page Down"},
	{VK_END, "End"},
	{VK_HOME, "Home"},
	{VK_LEFT, "Left"},
	{VK_UP, "Up"},
	{VK_RIGHT, "Right"},
	{VK_DOWN, "Down"},
	{VK_SELECT, "Select"},
	{VK_PRINT, "Print"},
	{VK_EXECUTE, "Execute"},
	{VK_SNAPSHOT, "PrtScrn"},
	{VK_INSERT, "Insert"},
	{VK_DELETE, "Delete"},
	{VK_HELP, "Help"},
	// x30 .. x39 -> 0 .. 9
	// x41 .. x5a -> A .. Z
	{VK_LWIN, "Windows"},
	{VK_RWIN, "RWindows"},
	{VK_APPS, "Application"},
	{VK_SLEEP, "Sleep"},
	// x60 .. x69 -> NP0 .. NP9
	{VK_MULTIPLY, "*"},
	{VK_ADD, "+"},
	{VK_SEPARATOR, "|"},
	{VK_SUBTRACT, "-"},
	{VK_DECIMAL, "."},
	{VK_DIVIDE, "/"},
	// x70 .. x87 -> F1 .. F24
	{VK_NUMLOCK, "NumLock"},
	{VK_SCROLL, "ScrollLock"},
	{VK_LSHIFT, "LShift"},
	{VK_RSHIFT, "RShift"},
	{VK_LCONTROL, "LCtrl"},
	{VK_RCONTROL, "RCtrl"},
	{VK_LMENU, "LAlt"},
	{VK_RMENU, "RAlt"},
	// some browser keys, volume keys, media keys, oem keys
};

const string getKeyName(const int keycode) {
	if (keycode == 0) {
		return "None";
	} else if (('0' <= keycode && keycode <= '9') || 'A' <= keycode && keycode <= 'Z') {
		return string(1, (char)keycode);
	} else if (VK_NUMPAD0 <= keycode && keycode <= VK_NUMPAD9) {
		return string_format("Numpad%d", keycode - VK_NUMPAD0);
	} else if (VK_F1 <= keycode && keycode <= VK_F24) {
		return string_format("F%d", keycode - VK_F1 + 1);
	} else if (keycodes.count(keycode) > 0) {
		return keycodes.at(keycode);
	} else {
		return string_format("<%d>", keycode);
	}
}

void TheTruth::InputKey(const char* label, Key* key, arc_config& cf) {
	char nameBuff[96];
	if (currentInputKeyLabel != 0 && strcmp(label, currentInputKeyLabel) == 0) {
		sprintf_s(nameBuff, "> <###bt_ik_%s", label);
		if (ImGui::Button(nameBuff, ImVec2(80, 0))) {
			currentInputKeyLabel = 0;
			currentInputKeyTarget = 0;
		}
	} else {
		sprintf_s(nameBuff, "%s###bt_ik_%s", getKeyName(key->keyCode).c_str(), label);
		if (ImGui::Button(nameBuff, ImVec2(80, 0))) {
			currentInputKeyLabel = label;
			currentInputKeyTarget = &(key->keyCode);
		}
	}
	ImGui::SameLine();
	sprintf_s(nameBuff, "%s##cbam1_%s", getKeyName(cf.mod1Key).c_str(), label);
	ImGui::Checkbox(nameBuff, &(key->requireArcMod1));
	ImGui::SameLine();
	sprintf_s(nameBuff, "%s##cbam2_%s", getKeyName(cf.mod2Key).c_str(), label);
	ImGui::Checkbox(nameBuff, &(key->requireArcMod2));
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(25,0));
	ImGui::SameLine();
	ImGui::Text(label);
}


void TheTruth::UIOptions() {
	arc_config arc_cfg = readArcConfig();
	if(ImGui::Checkbox("Own roles", &showSmallUI)) {
		if (showSmallUI) {
			settings->showOwnRolesMode = OWN_ALWAYS;
		} else {
			settings->showOwnRolesMode = OWN_NEVER;
		}
	}
	if (ImGui::Checkbox("All roles", &showBigUI)) {
		int mapId = mumbleApi.getMapId();
		if (showBigUI) {
			if (mapId == AERODROME_MAP) {
				settings->showAllRolesMode = ALL_AERODROME;
			} else if (isRaidMap(mapId)) {
				settings->showAllRolesMode = ALL_AERODROME_AND_RAIDS;
			} else {
				settings->showAllRolesMode = ALL_ALWAYS;
			}
		} else {
			if (mapId == AERODROME_MAP) {
				settings->showAllRolesMode = ALL_NEVER;
			} else {
				settings->showAllRolesMode = ALL_AERODROME;
			}
		}
	}
	ImGui::Separator();
	ImGui::Text("own roles, only displayed in a raid");
	const static char* ownRoleDisplayModes[] = {
		"Always",
		"Timed",
		"Never"
	};
	ImGui::Combo("show own roles", (int*)&settings->showOwnRolesMode, ownRoleDisplayModes, 3);
	if (settings->showOwnRolesMode == OWN_TIMED) {
		//char start_time[char_buff_size];
		//char end_time[char_buff_size];
		//ImGui::InputText("start time", start_time, char_buff_size);
		//ImGui::InputText("end time", end_time, char_buff_size);
		const static char* weekdays[] = {
			"Monday",
			"Tuesday",
			"Wednesday",
			"Thursday",
			"Friday",
			"Saturday",
			"Sunday"
		};
		ImGui::Combo("weekday", &settings->weekday, weekdays, 7);
	}
	ImGui::Checkbox("fixate own roles position", &settings->lockOwnRoleWindow);
	ImGui::Checkbox("show during loading screens", &settings->showInCharSelectAndLoading);
	ImGui::Checkbox("show title bar in own roles", &settings->ownWindowShowTitle);
	ImGui::Checkbox("show bosses in own roles", &settings->showHeaderInOwnRoles);
	ImGui::Separator();
	ImGui::Text("all roles");
	const static char* allRoleDisplayModes[] = {
		"Always",
		"Aerodrome",
		"Aerodrome and Raids",
		"Never"
	};
	ImGui::Combo("show in maps", (int*)&settings->showAllRolesMode, allRoleDisplayModes, 4);
	InputKey("all roles toggle key", &settings->windowToggleKey, arc_cfg);
	ImGui::Separator();
	ImGui::Text("Colors");
	ImGui::Checkbox("colors in own roles", &settings->showBgColorInOwnRoles);
	ImGui::Checkbox("colors in all roles", &settings->showBgColorInRolesTable);
	ImGui::Checkbox("use sheet conditional formation colors", &settings->useSheetsConditionalColors);
	ImGui::SliderFloat("conditional formating alpha", &settings->sheetColorAlpha, 0, 1);
	if (ImGui::Button("custom colors###tt_cst_color_btn")) {
		showCustomColorSetup = true;
	}
	ImGui::Separator();
	const int char_buff_size = 128;
	char name_c[char_buff_size];
	strcpy_s(name_c, settings->ownName.c_str());
	if (ImGui::InputText("own name", name_c, char_buff_size)) {
		settings->ownName = string(name_c);
	}
	if (ImGui::Button("Google sheet setup##thetruthgooglesheetsettings")) {
		showGoogleSheetSetup = true;
	}
}

int getWingByMap(int mapId) {
	switch (mapId)
	{
	case AERODROME_MAP:
		return -1;
	case WING_1_MAP:
		return 1;
	case WING_2_MAP:
		return 2;
	case WING_3_MAP:
		return 3;
	case WING_4_MAP:
		return 4;
	case WING_5_MAP:
		return 5;
	case WING_6_MAP:
		return 6;
	case WING_7_MAP:
		return 7;
	default:
		return -1;
	}
}

bool useInvertedTextColor(const ImVec4& color) {
	float max = color.x;
	float min = color.x;
	if (color.y > max) max = color.y;
	if (color.y < min) min = color.y;
	if (color.z > max) max = color.z;
	if (color.z < min) min = color.z;
	return color.x * 0.299 + color.y * 0.587 + (color.z * 0.114) > 150./255.;
}

ImVec4 getTextColor(const ImVec4& bgColor) {
	ImVec4 textCol = ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_Text));
	if (useInvertedTextColor(bgColor)) { // determin if black or white text depending on the bg color			
		textCol.x = 1.0f - textCol.x;
		textCol.y = 1.0f - textCol.y;
		textCol.z = 1.0f - textCol.z;
	}
	return textCol;
}

map<string, ImVec4> getColorMap(const shared_ptr<SheetsAPI> api, const std::shared_ptr<Settings> settings) {
	map<string, ImVec4> colorMap;
	if (settings->useSheetsConditionalColors) {
		colorMap = api->getColors();
		for (auto iter = colorMap.begin(); iter != colorMap.end(); iter++) {
			iter->second.w = settings->sheetColorAlpha;
		}
	}
	for (pair<string, ImColor> cc : settings->customColors) {
		string key = cc.first;
		transform(key.begin(), key.end(), key.begin(), ::tolower);
		colorMap[key] = cc.second.Value;
	}
	return colorMap;
}

void TheTruth::drawSmallUI(int wing, vector<string>& roles, map<string, ImVec4> colorMap, const string& mainRole,  const shared_ptr<ImVec4> mainRoleColor) {
	if(showSmallUI) {
		if(settings->showHeaderInOwnRoles) {
			ImGui::SetNextWindowSizeConstraints(ImVec2(100, 0), ImVec2(150, -1));
		} else {
			ImGui::SetNextWindowSizeConstraints(ImVec2(50,0), ImVec2(100, -1));
		}
		string title = string_format("Wing %d###CurrentWingRole", wing);
		ImVec2 size(-1, ImGui::GetTextLineHeightWithSpacing() * (roles.size() + (settings->ownWindowShowTitle ? 1 : 0)) + (settings->ownWindowShowTitle ? 9 : 5));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		if (settings->lockOwnRoleWindow) {
			flags |= ImGuiWindowFlags_NoInputs;
		}
		if (!settings->ownWindowShowTitle)
			flags |= ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin(title.c_str(), &showSmallUI, flags)) {
			if (!showSmallUI) { 
				settings->showOwnRolesMode = OWN_NEVER; 
			}
			ImGui::SetWindowSize(size, 0); 
			int index = 0;
			if(ImGui::BeginTable("ownRolesTable", settings->showHeaderInOwnRoles ? 2 : 1, ImGuiTableFlags_NoPadOuterX)) {
				vector<string> headers = sheetsAPI->getHeader(wing);
				for (int bossIndex = 0; bossIndex < roles.size(); bossIndex ++) {
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (settings->showHeaderInOwnRoles) {
						ImGui::TextUnformatted(headers.size() > bossIndex ? headers[bossIndex].c_str() :"");
						ImGui::TableSetColumnIndex(1);
					}
					string mapKey = roles[bossIndex];
					transform(mapKey.begin(), mapKey.end(), mapKey.begin(), ::tolower);
					bool showColor = settings->showBgColorInOwnRoles && ((colorMap.count(mapKey) > 0) || mainRoleColor != NULL);
					if (showColor) {
						if (colorMap.count(mapKey) > 0) {
							ImGui::PushStyleColor(ImGuiCol_Text, getTextColor(colorMap[mapKey]));
							ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(colorMap[mapKey]));
						} else {
							ImGui::PushStyleColor(ImGuiCol_Text, getTextColor(*mainRoleColor));
							ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(*mainRoleColor));
						}
					}
					string role = roles[bossIndex];
					if (role == string()) {
						role = mainRole;
					}
					ImGui::TextUnformatted(role.c_str());
					if (showColor) {
						ImGui::PopStyleColor(1);
					}
					index++;
				}
				ImGui::EndTable();
			}
		}
		ImGui::End(); 
		ImGui::PopStyleVar();
	}
}


void drawWing(int wing, shared_ptr<SheetsAPI> sheetsAPI, const std::shared_ptr<Settings> settings, int numColumns, const map<string, ImVec4>& colorMap, const map<int, ImVec4>& mainRoleColors) {
	int rowCount = 0;
	if (!sheetsAPI->hasWing(wing)) {
		sheetsAPI->requestWing(wing);
		return;
	}
	vector<vector<string>> roles = sheetsAPI->getWing(wing); 
	vector<string> headers = sheetsAPI->getHeader(wing);
	if (roles.size() > 0) rowCount = roles[0].size();
	for (int row = 0; row < rowCount; row++) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		if (headers.size() > row) ImGui::TextUnformatted(headers[row].c_str());
		for (int column = 0; column < numColumns; column++) {
			ImGui::TableSetColumnIndex(column + 1);
			if (roles.size() > column  && roles[column].size() > row) {
				string text = roles[column][row];
				string mapKey = text;
				transform(mapKey.begin(), mapKey.end(), mapKey.begin(), ::tolower);
				bool showColor = settings->showBgColorInRolesTable && ((colorMap.count(mapKey) > 0) || (mainRoleColors.count(column) > 0));
				if (showColor) {
					if (colorMap.count(mapKey) > 0) {
						ImGui::PushStyleColor(ImGuiCol_Text, getTextColor(colorMap.at(mapKey)));
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(colorMap.at(mapKey)));
					} else {
						ImGui::PushStyleColor(ImGuiCol_Text, getTextColor(mainRoleColors.at(column)));
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(mainRoleColors.at(column)));
					}
				}
				ImGui::TextUnformatted(text.c_str());
				if (showColor) {
					ImGui::PopStyleColor(1);
				}
			} else {
				bool showColor = settings->showBgColorInRolesTable && (mainRoleColors.count(column) > 0);
				if (showColor) {
					ImGui::PushStyleColor(ImGuiCol_Text, getTextColor(mainRoleColors.at(column)));
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(mainRoleColors.at(column)));
				}
				ImGui::TextUnformatted("");
				if (showColor) {
					ImGui::PopStyleColor(1);
				}
			}
		}
	}
}

void TheTruth::drawBigUI(int currentWing) {
	if(showBigUI) {
		if (ImGui::Begin("The Truth", &showBigUI, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
			if (!showBigUI) {
				if (mumbleApi.getMapId() == AERODROME_MAP) {
					settings->showAllRolesMode = ALL_NEVER;
				} else {
					settings->showAllRolesMode = ALL_AERODROME;
				}
			}
			ImGui::Checkbox("current wing", &settings->showCurrentWing);
			for (int wing = 1; wing <= 7; wing++) {
				ImGui::SameLine();
				ImGui::Checkbox(string_format("wing %d", wing).c_str(), &settings->showWings[wing-1]);
			}
			ImGui::SameLine();
			if (ImGui::Button("Refresh")) {
				if(!sheetsAPI->isDownloading()) sheetsAPI->clearCache();
			}
			if (sheetsAPI->isDownloading()) {
				ImGui::SameLine();
				ImGuiEx::Spinner("downloadingSpinner", ImGui::GetTextLineHeight() / 2.f, 3.f, ImGui::GetColorU32(ImGuiCol_Text));
			}
			shared_ptr<vector<string>> names_ptr = sheetsAPI->getNames();
			if(names_ptr) {
				vector<string> names = *(names_ptr);
				map<string, ImVec4> colorMap = getColorMap(sheetsAPI, settings);
				if (ImGui::BeginTable("truthTable", names.size() + 1, ImGuiTableFlags_SizingStretchSame)) {
					ImGui::TableSetupColumn("Boss");
					for (string& name : names) {
						ImGui::TableSetupColumn(name.c_str());
					}
					ImGui::TableHeadersRow();
					bool first = true;
					shared_ptr<vector<string>> mainRoles = sheetsAPI->getMainRoles();
					map<int, ImVec4> mainRoleColors;
					if (mainRoles != NULL && mainRoles->size() > 0) {
						first = false;
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted("Main Role");

						for (int column = 0; column < names.size(); column++) {
							ImGui::TableSetColumnIndex(column + 1);
							if (mainRoles->size() > column) {
								string text = (*mainRoles)[column];
								string mapKey = text;
								transform(mapKey.begin(), mapKey.end(), mapKey.begin(), ::tolower);
								bool showColor = settings->showBgColorInRolesTable && (colorMap.count(mapKey) > 0);
								if (showColor) {
									mainRoleColors[column] = colorMap[mapKey];
									ImGui::PushStyleColor(ImGuiCol_Text, getTextColor(colorMap[mapKey]));
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(colorMap[mapKey]));
								}
								ImGui::TextUnformatted(text.c_str());
								if (showColor) {
									ImGui::PopStyleColor(1);
								}
							} else {
								ImGui::TextUnformatted("");
							}
						}
					}
					for (int wing = 1; wing <= 7; wing++) {
						if (settings->showWings[wing - 1] || (currentWing == wing && settings->showCurrentWing)) {
							if (!first) ImGui::Separator();
							first = false;
							drawWing(wing, sheetsAPI, settings, names.size(), colorMap, mainRoleColors);
						}
					}
					ImGui::EndTable();
				}
			}
		}
		ImGui::End();
	}
}

void writeTextToClipboard(const string& text) {
	const size_t len = text.length() + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy_s(GlobalLock(hMem), len, text.c_str(), len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}

string get_export_string(const shared_ptr<Settings> settings) {
	string data = "";
	data += (char)(settings->flipRowsAndCols ? 1 : 0);
	data += settings->sheetId;
	data += '\0';
	data += settings->namesRange;
	data += '\0';
	data += settings->mainRolesRange;
	data += '\0';
	for (int i = 0; i < 7; i++) {
		data += settings->getWingHeaderRange(i + 1);
		data += '\0';
		data += settings->getWingRolesRange(i + 1);
		data += '\0';
	}
	return base64encode((void*)data.c_str(), data.length());
}

string getStringByteVector(const vector<char> v, int* pos) {
	string ret = "";
	while (*pos < v.size()) {
		if (v[*pos] == 0) {
			(*pos)++;
			break;
		}
		ret += (char)v[*pos];
		(*pos)++;
	}
	return ret;
}

static bool import_sheets_string(const string importStr, shared_ptr<Settings> settings) {
	vector<char> data = base64decode(importStr);
	if (data.size() < 18) {
		Logger::w("Importing sheet settings failed. (Insuficent data)");
		return false;
	}
	bool flip = (data[0] != 0);
	int pos = 1;
	string sheetId = getStringByteVector(data, &pos);
	if (sheetId.size() > 90 || sheetId.size() < 10) { 
		Logger::w("Importing sheet settings failed. (Encoding problem)");
		return false;
	}
	settings->flipRowsAndCols = flip;
	settings->sheetId = sheetId;
	settings->namesRange = getStringByteVector(data, &pos);
	settings->mainRolesRange = getStringByteVector(data, &pos);
	for (int i = 0; i < 7; i++) {
		settings->setWingHeaderRange(i + 1, getStringByteVector(data, &pos));
		settings->setWingRolesRange(i + 1, getStringByteVector(data, &pos));
	}
	return true;
}

const int import_str_size = 2048;
char import_c[import_str_size] = "";

void TheTruth::drawSettingsUI() {
	const int char_buff_size = 2048;
	if (showGoogleSheetSetup) {
		if (ImGui::Begin("Google sheets setup", &showGoogleSheetSetup, ImGuiWindowFlags_NoCollapse)) {
			ImGui::Text("Google sheets settings, may only take effect after refresh");
			// import/export
			if (ImGui::Button("export to clipboard")) {
				writeTextToClipboard(get_export_string(settings));
			}
			ImGui::InputText("###import_sheet_txt", import_c, import_str_size);
			ImGui::SameLine();
			if (ImGui::Button("import settings")) {
				string import_s = string(import_c);
				if (import_s.length() > 0) {
					import_sheets_string(import_s, settings);
				}
			}
			ImGui::Separator();
			char sheet_c[char_buff_size];
			strcpy_s(sheet_c, settings->sheetId.c_str());
			if (ImGui::InputText("google sheet id", sheet_c, char_buff_size)) {
				settings->sheetId = string(sheet_c);
			}
			char name_range_c[char_buff_size];
			strcpy_s(name_range_c, settings->namesRange.c_str());
			if (ImGui::InputText("names range", name_range_c, char_buff_size)) {
				settings->namesRange = string(name_range_c);
			}

			ImGui::Separator();
			ImGui::Checkbox("Flip rows and cols", &settings->flipRowsAndCols);
			char mainr_range_c[char_buff_size];
			strcpy_s(mainr_range_c, settings->mainRolesRange.c_str());
			if (ImGui::InputText("main roles range", mainr_range_c, char_buff_size)) {
				settings->mainRolesRange = string(mainr_range_c);
			}
			for (int wing = 1; wing <= 7; wing++) {
				char range_c[char_buff_size];
				strcpy_s(range_c, settings->getWingRolesRange(wing).c_str());
				if (ImGui::InputText(string_format("wing %d roles range", wing).c_str(), range_c, char_buff_size)) {
					settings->setWingRolesRange(wing, string(range_c));
				}
			}
			ImGui::Separator();
			for (int wing = 1; wing <= 7; wing++) {
				char range_c[char_buff_size];
				strcpy_s(range_c, settings->getWingHeaderRange(wing).c_str());
				if (ImGui::InputText(string_format("wing %d header range", wing).c_str(), range_c, char_buff_size)) {
					settings->setWingHeaderRange(wing, string(range_c));
				}
			}
		}
		ImGui::End();
	}
	if (showCustomColorSetup) {
		if (ImGui::Begin("Custom colors", &showCustomColorSetup, ImGuiWindowFlags_NoCollapse)) {
			int deletionIndex = -1;
			for (int i = 0; i < settings->customColors.size(); i++) {
				ImGui::PushID(i);
				if (ImGui::Button("x###tt_cst_col_delbtn")) {
					deletionIndex = i;
				}
				ImGui::SameLine();
				float colorBuf[4];
				colorBuf[0] = settings->customColors[i].second.Value.x;
				colorBuf[1] = settings->customColors[i].second.Value.y;
				colorBuf[2] = settings->customColors[i].second.Value.z;
				colorBuf[3] = settings->customColors[i].second.Value.w;
				if (ImGui::ColorEdit4("###tt_cst_col_c4", colorBuf, ImGuiColorEditFlags_NoInputs)) {
					settings->customColors[i].second = ImColor(colorBuf[0], colorBuf[1], colorBuf[2], colorBuf[3]);
				}
				ImGui::SameLine();
				char buff[char_buff_size];
				strcpy_s(buff, settings->customColors[i].first.c_str());
				if (ImGui::InputText("###tt_cst_col_txt", buff, char_buff_size)) {
					settings->customColors[i].first = buff;
				}

				ImGui::PopID();
			}
			if (deletionIndex >= 0) {
				settings->customColors.erase(settings->customColors.begin() + deletionIndex);
			}
			if (ImGui::Button("  +  ###tt_cst_col_addbtn")) {
				settings->customColors.push_back(pair<string, ImColor>("Tank", ImColor(0.5f, 0.5f, 0.5f)));
			}
		}
		ImGui::End();
	}
}


bool imported;
bool import_result;
void TheTruth::drawFirstTimeSetup() {
	if (ImGui::Begin("TheTruth Setup", (bool*)0, ImGuiWindowFlags_NoCollapse)) {
		ImGui::Text("Please configure the settings. This is only required once. You may change the settings at any time.");
		ImGui::Text("Import Google sheet settings:");
		const size_t buff_size = 2048;
		char buff_gsimp[buff_size];
		ImGui::InputText("Import data###firttime_import_gsheet", import_c, import_str_size);
		if (ImGui::Button("Import")) {
			imported = true;
			import_result = import_sheets_string(string(import_c), settings);
			if(import_result) sheetsAPI->clearCache();
		}
		if (imported) {
			ImGui::SameLine();
			if (import_result) {
				ImGui::Text("Import succesful.");
			} else {
				ImGui::Text("Import failed.");
			}
		}
		const static char* weekdays[] = {
			"Monday",
			"Tuesday",
			"Wednesday",
			"Thursday",
			"Friday",
			"Saturday",
			"Sunday"
		};
		ImGui::Combo("raid weekday", &settings->weekday, weekdays, 7);
		char buff_name[buff_size];
		strcpy_s(buff_name, settings->ownName.c_str());
		if (ImGui::InputText("Your name in the google sheet", buff_name, buff_size)) {
			settings->ownName = buff_name;
		}
		if (ImGui::Button("OK###tt_fts_okbtn")) {
			settings->showFirstTimeSetup = false;
		}
	}
	ImGui::End();
}

bool checkTimeCondition(int weekday) {
	auto now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	tm local_tm;
	localtime_s(&local_tm, &tt);
	int currend_weekday = (local_tm.tm_wday + 6) % 7;
	return weekday == currend_weekday;
}

bool shouldShowBigUI(int map, ShowAllRolesMode mode) {
	switch (mode) {
		case  ALL_ALWAYS: return true;
		case  ALL_AERODROME: return map == AERODROME_MAP;
		case  ALL_AERODROME_AND_RAIDS: return map == AERODROME_MAP || isRaidMap(map);
		case  ALL_NEVER: return false;
	}
}

void TheTruth::ImGui(uint32_t not_charsel_or_loading) {
	int mapId = mumbleApi.getMapId();
	if (not_charsel_or_loading || settings->showInCharSelectAndLoading) {
		int currentWing = getWingByMap(mapId);
		if (currentWing > 0) {
			if (sheetsAPI->hasWing(currentWing)) {
				int ownNameIndex = -1;
				int i = 0;
				shared_ptr<vector<string>> names = sheetsAPI->getNames();
				if(names != NULL) {
					for (string name : *names) {
						if (name == settings->ownName) {
							ownNameIndex = i;
						}
						i++;
					}
				}
				if (ownNameIndex >= 0) {
					vector<vector<string>> wing_roles = sheetsAPI->getWing(currentWing);
					if (wing_roles.size() > ownNameIndex) {
						showSmallUI = settings->showOwnRolesMode == OWN_ALWAYS || (settings->showOwnRolesMode == OWN_TIMED && checkTimeCondition(settings->weekday));
						shared_ptr<ImVec4> mainRoleColor = NULL;
						map<string, ImVec4> colorMap = getColorMap(sheetsAPI, settings);
						string ownMainRole = "";
						if (sheetsAPI->getMainRoles() != NULL && sheetsAPI->getMainRoles()->size() > ownNameIndex) {
							ownMainRole = (*sheetsAPI->getMainRoles())[ownNameIndex];
							string roleKey = ownMainRole;
							transform(roleKey.begin(), roleKey.end(), roleKey.begin(), ::tolower);
							if (colorMap.count(roleKey) > 0) {
								mainRoleColor = std::make_shared<ImVec4>(colorMap[roleKey]);
							}
						}
						drawSmallUI(currentWing, wing_roles[ownNameIndex], colorMap, ownMainRole, mainRoleColor);
					} else {
						Logger::i(string_format("wing roles size is %d but index is %d", wing_roles.size(), ownNameIndex));
					}
				}
			} else {
				sheetsAPI->requestWing(currentWing);
			}
		}
		showBigUI = shouldShowBigUI(mapId, settings->showAllRolesMode);
		if(showBigUI) {
			if (sheetsAPI->getNames() == NULL) {
				sheetsAPI->requestNames();
			} else {
				drawBigUI(currentWing);
			}
		}
	}
	if (settings->showFirstTimeSetup) {
		drawFirstTimeSetup();
	}
	drawSettingsUI();
}

uintptr_t TheTruth::WindowNFCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	arc_config cf = readArcConfig();
	uint16_t arc_global_mod1 = cf.mod1Key;
	uint16_t arc_global_mod2 = cf.mod2Key;
	auto const io = &ImGui::GetIO();
	const int vkey = (int)wParam;
	switch (uMsg) {
	case WM_KEYUP:
	case WM_SYSKEYUP:
		io->KeysDown[vkey] = false;
		if (vkey == VK_CONTROL) {
			io->KeyCtrl = false;
		} else if (vkey == VK_MENU) {
			io->KeyAlt = false;
		} else if (vkey == VK_SHIFT) {
			io->KeyShift = false;
		}
		break;
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (vkey == VK_CONTROL) {
			io->KeyCtrl = true;
		} else if (vkey == VK_MENU) {
			io->KeyAlt = true;
		} else if (vkey == VK_SHIFT) {
			io->KeyShift = true;
		}
		io->KeysDown[vkey] = true;
		if (currentInputKeyLabel != NULL) {
			if (vkey == VK_ESCAPE) {
				*currentInputKeyTarget = 0;
			} else {
				*currentInputKeyTarget = vkey;
			}
			currentInputKeyLabel = NULL;
			return 0;
		}
		if (vkey == settings->windowToggleKey.keyCode && (!settings->windowToggleKey.requireArcMod1 || io->KeysDown[arc_global_mod1]) && (!settings->windowToggleKey.requireArcMod2 || io->KeysDown[arc_global_mod2])) {
			int mapId = mumbleApi.getMapId();
			if (!showBigUI) {
				if (mapId == AERODROME_MAP) {
					settings->showAllRolesMode = ALL_AERODROME;
				} else if (isRaidMap(mapId)) {
					settings->showAllRolesMode = ALL_AERODROME_AND_RAIDS;
				} else {
					settings->showAllRolesMode = ALL_ALWAYS;
				}
			} else {
				if (mapId == AERODROME_MAP) {
					settings->showAllRolesMode = ALL_NEVER;
				} else {
					settings->showAllRolesMode = ALL_AERODROME;
				}
			}
		}
		break;
	case WM_ACTIVATEAPP:
		if (!wParam) {
			io->KeysDown[arc_global_mod1] = false;
			io->KeysDown[arc_global_mod2] = false;
		}
		break;
	default:
		break;
	}
	return uMsg;
}
