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
		Logger::i("no settings file found, using default settings");
		ownWindowShowTitle = true;
		lockOwnRoleWindow = false;
		showHeaderInOwnRoles = false;
		flipRowsAndCols = false;
		weekday = 6;
		for (int wing = 1; wing <= 7; wing++) {
			showWings[wing - 1] = wing >= 5;
		}
		showAllRolesMode = ALL_AERODROME;
		showCurrentWing = true;
		showOwnRolesMode = OWN_ALWAYS;
		windowToggleKey.keyCode = 82; // r
		windowToggleKey.requireArcMod1 = true;
		windowToggleKey.requireArcMod2 = true;
		// color
		sheetColorAlpha = 0.8;
		showBgColorInRolesTable = true;
		showBgColorInOwnRoles = true;
		useSheetsConditionalColors = true;
		// sheet setup
		ownName = "Pv";
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
	if (j.contains("useSheetsConditionalColors") && j["useSheetsConditionalColors"].is_boolean()) {
		useSheetsConditionalColors = j["useSheetsConditionalColors"];
	}
	if (j.contains("sheetColorAlpha") && j["sheetColorAlpha"].is_number_float()) {
		sheetColorAlpha = j["sheetColorAlpha"];
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
	if (j.contains("mainRolesRange") && j["mainRolesRange"].is_string()) {
		mainRolesRange = j["mainRolesRange"];
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
	if (j.contains("showInCharSelectAndLoading") && j["showInCharSelectAndLoading"].is_boolean()) {
		showInCharSelectAndLoading = j["showInCharSelectAndLoading"];
	}
	if (j.contains("showHeaderInOwnRoles") && j["showHeaderInOwnRoles"].is_boolean()) {
		showHeaderInOwnRoles = j["showHeaderInOwnRoles"];
	}
	if (j.contains("flipRowsAndCols") && j["flipRowsAndCols"].is_boolean()) {
		flipRowsAndCols = j["flipRowsAndCols"];
	}
	if (j.contains("windowToggleKey") && j["windowToggleKey"].is_number_integer()) {
		windowToggleKey.keyCode = j["windowToggleKey"];
	}
	if (j.contains("windowToggleKeyMod1") && j["windowToggleKeyMod1"].is_boolean()) {
		windowToggleKey.requireArcMod1 = j["windowToggleKeyMod1"];
	}
	if (j.contains("windowToggleKeyMod2") && j["windowToggleKeyMod2"].is_boolean()) {
		windowToggleKey.requireArcMod2 = j["windowToggleKeyMod2"];
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
	if (j.contains("customColors") && j["customColors"].is_object()) {
		for (auto& element : j["customColors"].items()) {
			int r, g, b, a;
			if (!element.value().is_string()) continue;
			string value = element.value();
			if (sscanf_s(value.c_str(), "#%02x%02x%02x%02x", &r, &g, &b, &a) < 4) continue;
			ImColor color = ImColor(r, g, b, a);
			customColors.push_back(pair(element.key(), color));
		}
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

static inline float Saturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; } // limits float to [0..1]

void setColor(json& j, const string& key, const ImColor& color) {
	j[key] = json();
	int r = ((int)(Saturate(color.Value.x) * 255.0f + 0.5f));
	int g = ((int)(Saturate(color.Value.y) * 255.0f + 0.5f));
	int b = ((int)(Saturate(color.Value.z) * 255.0f + 0.5f));
	int a = ((int)(Saturate(color.Value.w) * 255.0f + 0.5f));
	j[key] = string_format("#%02x%02x%02x%02x", r, g, b, a);
}

json Settings::toJson() const {
	json j;
	j["version"] = 1;
	j["showBgColorInRolesTable"] = showBgColorInRolesTable;
	j["showBgColorInOwnRoles"] = showBgColorInOwnRoles;
	j["useSheetsConditionalColors"] = useSheetsConditionalColors;
	j["sheetColorAlpha"] = sheetColorAlpha;
	j["sheetId"] = sheetId;
	j["ownName"] = ownName;
	j["namesRange"] = namesRange;
	j["ownWindowShowTitle"] = ownWindowShowTitle;
	j["lockOwnRoleWindow"] = lockOwnRoleWindow;
	j["showOwnRolesMode"] = showOwnRolesMode;
	j["weekday"] = weekday;
	j["showCurrentWing"] = showCurrentWing;
	j["showAllRolesMode"] = showAllRolesMode;
	j["showInCharSelectAndLoading"] = showInCharSelectAndLoading;
	j["flipRowsAndCols"] = flipRowsAndCols;
	j["windowToggleKey"] = windowToggleKey.keyCode;
	j["windowToggleKeyMod1"] = windowToggleKey.requireArcMod1;
	j["windowToggleKeyMod2"] = windowToggleKey.requireArcMod2;
	j["showHeaderInOwnRoles"] = showHeaderInOwnRoles;
	j["showWings"] = json::array();
	for (int i = 0; i < 7; i++) {
		j["showWings"][i] = showWings[i];
	}
	j["mainRolesRange"] = mainRolesRange;
	j["wingRolesRanges"] = json::array();
	for (string range : wingRolesRanges) {
		j["wingRolesRanges"].push_back(range);
	}
	j["wingHeaderRanges"] = json::array();
	for (string range : wingHeaderRanges) {
		j["wingHeaderRanges"].push_back(range);
	}
	j["customColors"] = json::object();
	for (pair<string, ImColor> cc : customColors) {
		setColor(j["customColors"], cc.first, cc.second);
	}
	return j;
}

string Settings::getWingRolesRange(int wing) const {
	if (wingRolesRanges.size() < wing) return "";
	return wingRolesRanges[wing - 1];
}

string Settings::getWingHeaderRange(int wing) const {
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
