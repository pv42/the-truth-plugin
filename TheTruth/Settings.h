#pragma once
#include <string>
#include <vector>
#include "json.hpp"

using nlohmann::json;
using std::string;
using std::vector;

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

class Settings {
public:
	Settings();
	string sheetId;
	string ownName;
	string namesRange;
	string getWingRolesRange(int wing);
	string getWingHeaderRange(int wing);
	void setWingRolesRange(int wing, string range);
	void setWingHeaderRange(int wing, string range);
	bool showBgColorInRolesTable;
	bool showBgColorInOwnRoles;
	bool ownWindowShowTitle;
	bool lockOwnRoleWindow;
	bool showHeaderInOwnRoles;
	int weekday;
	bool showWings[7];
	bool showCurrentWing;
	int windowToggleKey;
	ShowOwnRolesMode showOwnRolesMode;
	ShowAllRolesMode showAllRolesMode;
	void save() const;
private:
	void loadFromJson(json j);
	json toJson() const;
	vector<string> wingRolesRanges;
	vector<string> wingHeaderRanges;
	
};

