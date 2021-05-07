#include "Settings.h"
#include "logger.h"
#include "string_format.hpp"
#include "json.hpp"
#include <filesystem>

using nlohmann::json;

// todo json load/write

Settings::Settings() {
	std::filesystem::path p = std::filesystem::current_path() / "addons" / "thetruth-settings.json";
	if (std::filesystem::exists(p)) {
		try {
			json j;
			std::ifstream file(p);
			file >> j;
			file.close();
			loadFromJson(j);
		} catch (std::exception e) {
			Logger::e("could not load settings");
			Logger::e(e.what());
		}
	} else {
		Logger::i("no settings file found, using default settings for [Cute]");
		showBgColorInRolesTable = true;
		showBgColorInOwnRoles = true;
		ownWindowShowTitle = true;
		lockOwnRoleWindow = false;
		showHeaderInOwnRoles = false;
		weekday = 6;
		for (int wing = 1; wing <= 7; wing++) {
			showWings[wing - 1] = wing >= 5;
		}
		showAllRolesMode = ALL_AERODROME;
		showCurrentWing = true;
		showOwnRolesMode = OWN_ALWAYS;
		sheetId = "19AEHyOVnXCzTlVmmKROu7AHLn9NYN7JzjLCNm7KE9Tc";
		ownName = "Pv";
		namesRange = "'The Truth'!A5:A14";
		wingRolesRanges = vector<string>{ "", "", "'The Truth'!G22:I31", "'The Truth'!B22:E31", "'The Truth'!B5:E14", "'The Truth'!G5:I14", "'The Truth'!K5:M14"};
		wingHeaderRanges = vector<string>{ "", "", "'The Truth'!G21:I21", "'The Truth'!B21:E21" ,"'The Truth'!B4:E4", "'The Truth'!G4:I4", "'The Truth'!K4:M4" };
		windowToggleKey = 82; // r
	}
}

void Settings::loadFromJson(json j) {
	if (!j.contains("version") || j["version"] != 1) {
		Logger::w("settings version mismatch: expected 1.0");
		return;
	}
	if (j.contains("showBgColorInRolesTable") && j["showBgColorInRolesTable"].is_boolean()) {
		showBgColorInRolesTable = j["showBgColorInRolesTable"];
	}
	if (j.contains("showBgColorInOwnRoles") && j["showBgColorInOwnRoles"].is_boolean()) {
		showBgColorInOwnRoles = j["showBgColorInOwnRoles"];
	}
	if (j.contains("sheetId") && j["sheetId"].is_string()) {
		sheetId = j["sheetId"];
	}
	if (j.contains("ownName") && j["ownName"].is_string()) {
		ownName = j["ownName"];
	}
	if (j.contains("namesRange") && j["namesRange"].is_string()) {
		namesRange = j["namesRange"];
	}
	if (j.contains("ownWindowShowTitle") && j["ownWindowShowTitle"].is_boolean()) {
		ownWindowShowTitle = j["ownWindowShowTitle"];
	}
	if (j.contains("lockOwnRoleWindow") && j["lockOwnRoleWindow"].is_boolean()) {
		lockOwnRoleWindow = j["lockOwnRoleWindow"];
	}
	if (j.contains("showOwnRolesMode") && j["showOwnRolesMode"].is_number_integer() && j["showOwnRolesMode"] >= 0 && j["showOwnRolesMode"] <= 2) {
		showOwnRolesMode = (ShowOwnRolesMode)j["showOwnRolesMode"];
	}
	if (j.contains("weekday") && j["weekday"].is_number_integer() && j["weekday"] >= 0 && j["weekday"] <= 6) {
		weekday = j["weekday"];
	}
	if (j.contains("showAllRolesMode") && j["showAllRolesMode"].is_number_integer() && j["showAllRolesMode"] >= 0 && j["showAllRolesMode"] <= 3) {
		showAllRolesMode = (ShowAllRolesMode)j["showAllRolesMode"];
	}
	if (j.contains("showCurrentWing") && j["showCurrentWing"].is_boolean()) {
		lockOwnRoleWindow = j["showCurrentWing"];
	}
	if (j.contains("showHeaderInOwnRoles") && j["showHeaderInOwnRoles"].is_boolean()) {
		showHeaderInOwnRoles = j["showHeaderInOwnRoles"];
	}
	if (j.contains("windowToggleKey") && j["windowToggleKey"].is_number_integer()) {
		windowToggleKey = j["windowToggleKey"];
	}
	if (j.contains("showWings") && j["showWings"].is_array()) {
		int count = j["showWings"].size();
		for (int i = 0; i < 7; i++) {
			if (i < count && j["showWings"][i].is_boolean()) {
				showWings[i] = j["showWings"][i];
			} else {
				showWings[i] = false;
			}
		}
	}
	int rolesCount = 0;
	if (j.contains("wingRolesRanges") && j["wingRolesRanges"].is_array()) {
		rolesCount = j["wingRolesRanges"].size();
		wingRolesRanges = vector<string>();
		for (json element : j["wingRolesRanges"]) {
			if (element.is_string()) {
				wingRolesRanges.push_back(element);
			} else {
				wingRolesRanges.push_back("");
			}
		}
	}
	// fill if too few json elements
	for (int i = rolesCount; i < 7; i++) {
		j["wingRolesRanges"].push_back("");
	}
	int headerCount = 0;
	if (j.contains("wingHeaderRanges") && j["wingHeaderRanges"].is_array()) {
		headerCount = j["wingHeaderRanges"].size();
		wingHeaderRanges = vector<string>();
		for (json element : j["wingHeaderRanges"]) {
			if (element.is_string()) {
				wingHeaderRanges.push_back(element);
			} else {
				wingHeaderRanges.push_back("");
			}
		}
	}
	// fill if too few json elements
	for (int i = headerCount; i < 7; i++) {
		j["wingHeaderRanges"].push_back("");
	}
}

void Settings::save() const {
	try {
		std::filesystem::path p = std::filesystem::current_path() / "addons" / "thetruth-settings.json";
		std::ofstream file(p);
		file << toJson().dump(2);
		file.close();
		Logger::i("wrote settings to file");
	} catch (std::exception e) {
		Logger::e("could not save settings");
		Logger::e(e.what());
	}
}

json Settings::toJson() const {
	json j;
	j["version"] = 1;
	j["showBgColorInRolesTable"] = showBgColorInRolesTable;
	j["showBgColorInOwnRoles"] = showBgColorInOwnRoles;
	j["sheetId"] = sheetId;
	j["ownName"] = ownName;
	j["namesRange"] = namesRange;
	j["ownWindowShowTitle"] = ownWindowShowTitle;
	j["lockOwnRoleWindow"] = lockOwnRoleWindow;
	j["showOwnRolesMode"] = showOwnRolesMode;
	j["weekday"] = weekday;
	j["showCurrentWing"] = showCurrentWing;
	j["showAllRolesMode"] = showAllRolesMode;
	j["windowToggleKey"] = windowToggleKey;
	j["showHeaderInOwnRoles"] = showHeaderInOwnRoles;
	j["showWings"] = json::array();
	for (int i = 0; i < 7; i++) {
		j["showWings"][i] = showWings[i];
	}
	j["wingRolesRanges"] = json::array();
	for (string range : wingRolesRanges) {
		j["wingRolesRanges"].push_back(range);
	}
	j["wingHeaderRanges"] = json::array();
	for (string range : wingHeaderRanges) {
		j["wingHeaderRanges"].push_back(range);
	}
	return j;
}

string Settings::getWingRolesRange(int wing) {
	if (wingRolesRanges.size() < wing) return "";
	return wingRolesRanges[wing - 1];
}

string Settings::getWingHeaderRange(int wing) {
	if (wingHeaderRanges.size() < wing) return "";
	return wingHeaderRanges[wing - 1];
}

void Settings::setWingRolesRange(int wing, string range) {
	if (wing < 1 || wing > wingRolesRanges.size()) {
		Logger::w("invalid wing, did not set roles range");
		return;
	}
	wingRolesRanges[wing - 1] = range;
}
void Settings::setWingHeaderRange(int wing, string range) {
	if (wing < 1 || wing > wingHeaderRanges.size()) {
		Logger::w("invalid wing, did not set header range");
		return;
	}
	wingHeaderRanges[wing - 1] = range;
}
