#pragma once
#include <string>
#include <vector>
#include "json.hpp"
#include "imgui/imgui.h"

using nlohmann::json;
using std::string;
using std::vector;
using std::pair;

enum ShowOwnRolesMode {
	OWN_ALWAYS = 0,
	OWN_TIMED = 1,
	OWN_NEVER = 2
};

enum ShowAllRolesMode {
	ALL_ALWAYS = 0,
	ALL_AERODROME = 1,
	ALL_AERODROME_AND_RAIDS = 2,
	ALL_NEVER = 3
};

typedef struct {
	int keyCode;
	bool requireArcMod1;
	bool requireArcMod2;
} Key;

class Settings {
public:
	Settings();
	string sheetId;
	string ownName;
	string namesRange;
	string mainRolesRange;
	string getWingRolesRange(int wing) const;
	string getWingHeaderRange(int wing) const;
	void setWingRolesRange(int wing, string range);
	void setWingHeaderRange(int wing, string range);
	bool showBgColorInRolesTable;
	bool showBgColorInOwnRoles;
	bool useSheetsConditionalColors;
	float sheetColorAlpha;
	bool ownWindowShowTitle;
	bool lockOwnRoleWindow;
	bool showHeaderInOwnRoles;
	bool flipRowsAndCols;
	int weekday;
	bool showWings[7];
	bool showCurrentWing;
	Key windowToggleKey;
	ShowOwnRolesMode showOwnRolesMode;
	ShowAllRolesMode showAllRolesMode;
	bool showInCharSelectAndLoading;
	vector<pair<string, ImColor>> customColors;
	void save() const;
private:
	void loadFromJson(json j);
	json toJson() const;
	vector<string> wingRolesRanges;
	vector<string> wingHeaderRanges;

};

