#include "TheTruth.h"
#include "logger.h"
#include "GW2.h"
#include "imgui/imgui.h"
#include "string_format.hpp"
#include <algorithm>


std::unique_ptr<TheTruth> TheTruth::theTruth;

bool TheTruth::crashed = false;

TheTruth::TheTruth() {
	string key = "AIzaSyDXE1rxSTrA_6g0T8ax9goY4MUB4UO5MSg";
	sheetsAPI = std::make_shared<SheetsAPI>(key, settings);
	showSmallUI = true;
	showBigUI = false;
	showSetting = false;
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
	settings.save();
	Logger::free();
	return 0;
}

boolean isRaidMap(int mapId) {
	return mapId == WING_1_MAP || mapId == WING_2_MAP || mapId == WING_3_MAP || mapId == WING_4_MAP || mapId == WING_5_MAP || mapId == WING_6_MAP || mapId == WING_7_MAP;
}

void TheTruth::UIOptions() {
	if (ImGui::BeginMenu("The Truth")) {
		if(ImGui::Checkbox("Own roles", &showSmallUI)) {
			if (showSmallUI) {
				settings.showOwnRolesMode = OWN_ALWAYS;
			} else {
				settings.showOwnRolesMode = OWN_NEVER;
			}
		}
		if (ImGui::Checkbox("All roles", &showBigUI)) {
			int mapId = mumbleApi.getMapId();
			if (showBigUI) {
				if (mapId == AERODROME_MAP) {
					settings.showAllRolesMode = ALL_AERODROME;
				} else if (isRaidMap(mapId)) {
					settings.showAllRolesMode = ALL_AERODROME_AND_RAIDS;
				} else {
					settings.showAllRolesMode = ALL_ALWAYS;
				}
			} else {
				if (mapId == AERODROME_MAP) {
					settings.showAllRolesMode = ALL_NEVER;
				} else {
					settings.showAllRolesMode = ALL_AERODROME;
				}
			}
		}
		ImGui::Checkbox("Setting##thetruthsettings", &showSetting);
		ImGui::EndMenu();
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

float getColorLightness(ImVec4& color) {
	float max = color.x;
	float min = color.x;
	if (color.y > max) max = color.y;
	if (color.y < min) min = color.y;
	if (color.z > max) max = color.z;
	if (color.z < min) min = color.z;
	return (max + min) / 2.0f;
}

ImVec4 getTextColor(ImVec4& bgColor) {
	ImVec4 textCol = ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_Text));
	if (getColorLightness(bgColor) >= 0.5f) { // determin if black or white text depending on the bg color			
		textCol.x = 1.0f - textCol.x;
		textCol.y = 1.0f - textCol.y;
		textCol.z = 1.0f - textCol.z;
	}
	return textCol;
}


void TheTruth::drawSmallUI(int wing, vector<string>& roles) {
	if(showSmallUI) {
		string title = string_format("Wing %d###CurrentWingRole", wing);
		ImVec2 size(100, ImGui::GetTextLineHeightWithSpacing() * (roles.size() + (settings.ownWindowShowTitle ? 1 : 0)) + (settings.ownWindowShowTitle ? 9 : 5));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		if (settings.lockOwnRoleWindow) {
			flags |= ImGuiWindowFlags_NoInputs;
		}
		if (!settings.ownWindowShowTitle)
			flags |= ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin(title.c_str(), &showSmallUI, flags)) {
			if (!showSmallUI) { 
				settings.showOwnRolesMode = OWN_NEVER; 
			}
			ImGui::SetWindowSize(size, 0); 
			int index = 0;
			if(ImGui::BeginTable("ownRolesTable", 1, ImGuiTableFlags_NoPadOuterX)) {
				map<string, ImVec4> colorMap = sheetsAPI->getColors();
				for (string role : roles) {
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					string mapKey = role;
					transform(mapKey.begin(), mapKey.end(), mapKey.begin(), ::tolower);
					bool showColor = (colorMap.count(mapKey) > 0) && settings.showBgColorInOwnRoles;
					if (showColor) {
						ImGui::PushStyleColor(ImGuiCol_Text, getTextColor(colorMap[mapKey]));
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(colorMap[mapKey]));
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

void drawWing(int wing, shared_ptr<SheetsAPI> sheetsAPI, bool showColors) {
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
		for (int column = 0; column < roles.size(); column++) {
			ImGui::TableSetColumnIndex(column + 1);
			if (roles[column].size() > row) {
				string text = roles[column][row];
				string mapKey = text;
				transform(mapKey.begin(), mapKey.end(), mapKey.begin(), ::tolower);
				map<string, ImVec4 > colorMap = sheetsAPI->getColors();
				bool showColor = (colorMap.count(mapKey) > 0) && showColors;
				if (showColor) {
					ImGui::PushStyleColor(ImGuiCol_Text, getTextColor(colorMap[mapKey]));
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(colorMap[mapKey]));
				}
				ImGui::TextUnformatted(text.c_str());
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
					settings.showAllRolesMode = ALL_NEVER;
				} else {
					settings.showAllRolesMode = ALL_AERODROME;
				}
			}
			ImGui::Checkbox("current wing", &settings.showCurrentWing);
			for (int wing = 1; wing <= 7; wing++) {
				ImGui::SameLine();
				ImGui::Checkbox(string_format("wing %d", wing).c_str(), &settings.showWings[wing-1]);
			}
			ImGui::SameLine();
			if (ImGui::Button("Refresh")) {
				sheetsAPI->clearCache();
			}
			vector<string> names = *(sheetsAPI->getNames());
			if (ImGui::BeginTable("truthTable", names.size() + 1, ImGuiTableFlags_SizingStretchSame)) {
				ImGui::TableSetupColumn("Boss");
				for (string& name : names) {
					ImGui::TableSetupColumn(name.c_str());
				}
				ImGui::TableHeadersRow();
				bool first = true;
				for (int wing = 1; wing <= 7; wing++) {
					if (settings.showWings[wing - 1] || (currentWing == wing && settings.showCurrentWing)) {
						if (!first) ImGui::Separator();
						first = false;
						drawWing(wing, sheetsAPI, settings.showBgColorInRolesTable);
					}
				}
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}
}

void TheTruth::drawSettingsUI() {
	if (showSetting) {
		const int char_buff_size = 128;
		if (ImGui::Begin("The Truth Settings", &showSetting, ImGuiWindowFlags_NoCollapse)) {
			ImGui::Text("own roles, only displayed in a raid");
			const static char* ownRoleDisplayModes[] = {
				"Always",
				"Timed",
				"Never"
			};
			ImGui::Combo("show own roles", (int*)&settings.showOwnRolesMode, ownRoleDisplayModes, 3);
			if (settings.showOwnRolesMode == OWN_TIMED) {
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
				ImGui::Combo("weekday", &settings.weekday, weekdays, 7);
			}
			ImGui::Checkbox("colors in own roles", &settings.showBgColorInOwnRoles);
			ImGui::Checkbox("lock own roles position", &settings.lockOwnRoleWindow);
			ImGui::Checkbox("title bar in own roles", &settings.ownWindowShowTitle);
			char name_c[char_buff_size];
			strcpy_s(name_c, settings.ownName.c_str());
			ImGui::Separator();
			ImGui::Text("all roles");
			const static char* allRoleDisplayModes[] = {
				"Always",
				"Aerodrome",
				"Aerodrome and Raids"
			};
			ImGui::Combo("show in maps", (int*)&settings.showAllRolesMode, allRoleDisplayModes, 3);
			ImGui::Checkbox("colors in all roles", &settings.showBgColorInRolesTable);
			ImGui::InputInt("Toggle all roles windows key", &settings.windowToggleKey);
			ImGui::Separator();
			ImGui::Text("Google sheets settings, may only take effect after refresh");
			char sheet_c[char_buff_size];
			strcpy_s(sheet_c, settings.sheetId.c_str());
			if (ImGui::InputText("own name", name_c, char_buff_size)) {
				settings.ownName = string(name_c);
			}
			if (ImGui::InputText("google sheet id", sheet_c, char_buff_size)) {
				settings.sheetId = string(sheet_c);
			}
			char name_range_c[char_buff_size];
			strcpy_s(name_range_c, settings.namesRange.c_str());
			if (ImGui::InputText("names range", name_range_c, char_buff_size)) {
				settings.namesRange = string(name_range_c);
			}
			ImGui::Separator();
			for (int wing = 1; wing <= 7; wing++) {
				char range_c[char_buff_size];
				strcpy_s(range_c, settings.getWingRolesRange(wing).c_str());
				if (ImGui::InputText(string_format("wing %d roles range", wing).c_str(), range_c, char_buff_size)) {
					settings.setWingRolesRange(wing, string(range_c));
				}
			}
			ImGui::Separator();
			for (int wing = 1; wing <= 7; wing++) {
				char range_c[char_buff_size];
				strcpy_s(range_c, settings.getWingHeaderRange(wing).c_str());
				if (ImGui::InputText(string_format("wing %d header range", wing).c_str(), range_c, char_buff_size)) {
					settings.setWingHeaderRange(wing, string(range_c));
				}
			}
		}
		ImGui::End();
	}
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
	if (not_charsel_or_loading) {
		
		int currentWing = getWingByMap(mapId);
		if (currentWing > 0) {
			if (sheetsAPI->hasWing(currentWing)) {
				int ownNameIndex = -1;
				int i = 0;
				shared_ptr<vector<string>> names = sheetsAPI->getNames();
				for (string name : *names) {
					if (name == settings.ownName) {
						ownNameIndex = i;
					}
					i++;
				}
				if (ownNameIndex >= 0) {
					vector<vector<string>> wing_roles = sheetsAPI->getWing(currentWing);
					if (wing_roles.size() > ownNameIndex) {
						showSmallUI = settings.showOwnRolesMode == OWN_ALWAYS || (settings.showOwnRolesMode == OWN_TIMED && checkTimeCondition(settings.weekday));
						drawSmallUI(currentWing, wing_roles[ownNameIndex]);
					} else {
						Logger::i(string_format("wing roles size is %d but index is %d", wing_roles.size(), ownNameIndex));
					}
				}
			} else {
				sheetsAPI->requestWing(currentWing);
			}
		}
		showBigUI = shouldShowBigUI(mapId, settings.showAllRolesMode);
		if(showBigUI) {
			if (sheetsAPI->getNames() == NULL) {
				sheetsAPI->requestNames();
			} else {
				drawBigUI(currentWing);
			}
		}
	}
	drawSettingsUI();
}

uintptr_t TheTruth::WindowNFCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	const int vkey = (int)wParam;
	switch (uMsg) {
	case WM_KEYUP:
	case WM_SYSKEYUP:
		//io->KeysDown[vkey] = false;
		if (vkey == VK_MENU) {
			io_altDown = false;
		} else if (vkey == VK_SHIFT) {
			io_shiftDown = false;
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		// toggle killproof window
		if (io_altDown && io_shiftDown && vkey == settings.windowToggleKey) {
			int mapId = mumbleApi.getMapId();
			if (!showBigUI) {
				if (mapId == AERODROME_MAP) {
					settings.showAllRolesMode = ALL_AERODROME;
				} else if (isRaidMap(mapId)) {
					settings.showAllRolesMode = ALL_AERODROME_AND_RAIDS;
				} else {
					settings.showAllRolesMode = ALL_ALWAYS;
				}
			} else {
				if (mapId == AERODROME_MAP) {
					settings.showAllRolesMode = ALL_NEVER;
				} else {
					settings.showAllRolesMode = ALL_AERODROME;
				}
			}
			return 0;
		}
		if (vkey == VK_MENU) {
			io_altDown = true;
		} else if (vkey == VK_SHIFT) {
			io_shiftDown = true;
		}
		break;
	case WM_ACTIVATEAPP:
		if (!wParam) {
			io_shiftDown = false;
			io_altDown = false;
		}
		break;
	default:
		break;
	}
	return uMsg;
}
