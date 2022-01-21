#include "SheetsAPI.h"
#include <string>
#include <cpr/cpr.h>
#include "json.hpp"
#include "logger.h"
#include "string_format.hpp"
#include "imgui/imgui.h"

using namespace nlohmann;
using std::vector;
using std::string;
using std::unique_ptr;

SheetsAPI::SheetsAPI() : api_key("") , settings(), hasMetaData(false) {
	downloading = false;
	Logger::d("default constr");
}

SheetsAPI::SheetsAPI(const string api_key_, Settings& setttings_) : api_key(api_key_), settings(setttings_) {
	downloading = false;
	hasMetaData = false;
}

unique_ptr<json> getSheetValuesJson(const string& sheetId, const string& range, const string api_key) {
	cpr::Url url = cpr::Url("https://sheets.googleapis.com/v4/spreadsheets/" + sheetId + "/values/" + range);
	Logger::d(string_format("getting url %s", url.c_str()));
	cpr::Response r = cpr::Get(url, cpr::Parameters{ { "key", api_key } });
	if (r.status_code != 200) {
		Logger::e("HTTP returned " + r.status_code);
		return NULL;
	}
	json j = json::parse(r.text);
	return std::make_unique<json>(j);
}

shared_ptr<json> getSheetJson(const string& sheetId, const string api_key) {
	cpr::Url url = cpr::Url("https://sheets.googleapis.com/v4/spreadsheets/" + sheetId);
	Logger::d(string_format("getting url %s", url.c_str()));
	cpr::Response r = cpr::Get(url, cpr::Parameters{ { "key", api_key } });
	if (r.status_code != 200) {
		Logger::e("HTTP returned " + r.status_code);
		return NULL;
	}
	json j = json::parse(r.text);
	return std::make_shared<json>(j);
}

vector<string> getValuesAsArray(const string& sheetId, const string& range,  const string api_key) {
	unique_ptr<json> j = getSheetValuesJson(sheetId, range, api_key);
	vector<string> v;
	if (j == NULL) return v;
	if (!j->contains("values") || !(*j)["values"].is_array()) {
		return v;
	}
	for (auto& row : (*j)["values"]) {
		if (!row.is_array()) {
			continue;
		}
		for (auto& cell : row) {
			v.push_back(cell);
		}
	}
	return v;
}

vector<string> getValuesAs1DArray(const string& sheetId, const string& range, const string api_key) {
	//json j = "{\"range\": \"'The Truth'!A5:A14\",\"majorDimension\" : \"ROWS\",\"values\" : [[\"Ewanye\"],[\"Fra\"],[\"Make\"],[\"Zerumi\"],[\"Pitt\"],[\"Miruna\"],[\"Pv\"],[\"Lynn\"],[\"Lele\"],[\"Rey\"]]}"_json;
	unique_ptr<json> j = getSheetValuesJson(sheetId, range, api_key);
	vector<string> v;
	if (j == NULL) return v;
	if (!j->contains("values") || !(*j)["values"].is_array()) {
		return v;
	}
	for (auto& row : (*j)["values"]) {
		if (!row.is_array()) {
			continue;
		}
		for (auto& cell : row) {
			v.push_back(cell);
		}
	}
	return v;
}

vector<vector<string>> getValuesAs2DArray(const string& sheetId, const string& range, const string api_key, const boolean flipRowsAndCols = false) {
	vector<vector<string>> v;
	unique_ptr<json> j = getSheetValuesJson(sheetId, range, api_key);
	if (j == NULL) return v;
	if (!j->contains("values") || !(*j)["values"].is_array()) {
		return v;
	}
	if (flipRowsAndCols) {
		int rowIndex = 0;
		for (auto& row : (*j)["values"]) {
			if (!row.is_array()) {
				continue;
			}
			// ensure enought subvectors are present
			while (v.size() < row.size()) {
				vector<string> sub_vector;
				for (int i = 0; i < rowIndex; i++) { // enusre enought elements in new sub vectors
					sub_vector.push_back("");
				}
				v.push_back(sub_vector);
			}
			//vector<string> sub_vector;
			for (int columIndex = 0; columIndex < v.size(); columIndex++) {
				if(columIndex < row.size()) {
					v[columIndex].push_back(row[columIndex]);
				} else {
					v[columIndex].push_back("");
				}
			}
			rowIndex++;
		}
	} else {
		for (auto& row : (*j)["values"]) { // 1 sub vertor per row
			if (!row.is_array()) {
				continue;
			}
			vector<string> sub_vector;
			for (auto& cell : row) {
				sub_vector.push_back(cell);
			}
			v.push_back(sub_vector);
		}
	}
	return v;
}

std::string replace(std::string str, const std::string& sub1, const std::string& sub2) {
	if (sub1.empty())
		return str;
	std::size_t pos;
	while ((pos = str.find(sub1)) != std::string::npos)
		str.replace(pos, sub1.size(), sub2);
	return str;
}

string escape_table_range(string& unescaped_range) {
	string s = replace(unescaped_range, "'", "%27");
	s = replace(s, " ", "%20");
	s = replace(s, "!", "%21");
	s = replace(s, ":", "%3A");
	return s;
}

void SheetsAPI::downloadNames() {
	try {
		Logger::d("downloading names");
		string range = settings.namesRange;
		range = escape_table_range(range);
		names_cache = std::make_shared<vector<string>>(getValuesAs1DArray(settings.sheetId, range, api_key));
		Logger::d(string_format("got %d names", names_cache->size()));
		Logger::d("downloaded names");
		downloading = false;
	} catch (std::exception e) {
		Logger::e("failed to download names");
		Logger::e(e.what());
	}
}

void SheetsAPI::downloadMainRoles() {
	if (settings.mainRolesRange.length() == 0) {
		Logger::d("no main roles range");
		main_roles_cache = std::make_shared<vector<string>>();
	} else {
		try {
			Logger::d("downloading main roles");
			string range = settings.mainRolesRange;
			range = escape_table_range(range);
			main_roles_cache = std::make_shared<vector<string>>(getValuesAs1DArray(settings.sheetId, range, api_key));
			Logger::d(string_format("got %d main roles", main_roles_cache->size()));
			Logger::d("downloaded main roles");
			downloading = false;
		} catch (std::exception e) {
			Logger::e("failed to download names");
			Logger::e(e.what());
		}
	}
}

void SheetsAPI::downloadWing(int wing) {
	try {
		Logger::d(string_format("downloading wing %d", wing));
		string range = settings.getWingRolesRange(wing);
		if (range.empty()) {
			downloading = false;
			return;
		}
		roles_cache[wing] = getValuesAs2DArray(settings.sheetId, escape_table_range(range), api_key, settings.flipRowsAndCols);
		string header_range = settings.getWingHeaderRange(wing);
		if (header_range.empty()) {
			downloading = false;
			return;
		}
		header_cache[wing] = getValuesAs1DArray(settings.sheetId, escape_table_range(header_range), api_key);
		Logger::d(string_format("downloaded wing %d", wing));
	} catch (std::exception e) {
		Logger::e("failed to download wing");
		Logger::e(e.what());
	}
	downloading = false;
}

json getSheetFromRangeAndJson(const shared_ptr<json> whole_json, const string& range) {
	if (whole_json == NULL) throw(std::exception("json handle is null"));
	if (!whole_json->contains("sheets") || !(*whole_json)["sheets"].is_array() || (*whole_json)["sheets"].size() == 0) 
		throw(std::exception("json does not contain a sheets"));
	int colon_index = -1;
	for(int i = 1; i < range.size(); i++) { // empty named sheets are not allowed
		if (range.at(i) == '\'') {
			colon_index = i;
			break;
		}
	}
	if (colon_index > 0) {
		string name = range.substr(0, colon_index);
		name = replace(name, "'", "");
		for (int i = 0; i < (*whole_json)["sheets"].size(); i++) {
			json sheet = (*whole_json)["sheets"][i];
			if (sheet.contains("properties") && sheet["properties"].contains("title") && sheet["properties"]["title"].is_string() && sheet["properties"]["title"] == name) {
				return sheet;
			}
		}
		return (*whole_json)["sheets"][0];
	} else {
		return (*whole_json)["sheets"][0];
	}
}

void SheetsAPI::downloadSheetMetaData() {
	try {
		string range = "";
		// find a valid range
		int wing = 1;
		while (wing <= 7 && range.size() == 0) {
			range = settings.getWingHeaderRange(wing);
			wing++;
		}
		if (range.size() == 0) {
			Logger::w("could not find valid range to retrive conditional formating");
			return;
		}
		shared_ptr<json> whole_json = getSheetJson(settings.sheetId, api_key);
		json sheet_json = getSheetFromRangeAndJson(whole_json, range);
		if (sheet_json.contains("conditionalFormats") && sheet_json["conditionalFormats"].is_array()) {
			for (json format_json : sheet_json["conditionalFormats"]) {
				if (format_json.contains("booleanRule") &&
					format_json["booleanRule"].contains("format") &&
					format_json["booleanRule"]["format"].contains("backgroundColor") &&
					format_json["booleanRule"].contains("condition") &&
					format_json["booleanRule"]["condition"].contains("type") &&
					format_json["booleanRule"]["condition"]["type"] == "TEXT_EQ" &&
					format_json["booleanRule"]["condition"].contains("values") &&
					format_json["booleanRule"]["condition"]["values"].is_array()) {
					float R = 0, G = 0, B = 0;
					if (format_json["booleanRule"]["format"]["backgroundColor"].contains("red") && format_json["booleanRule"]["format"]["backgroundColor"]["red"].is_number()) {
						R = format_json["booleanRule"]["format"]["backgroundColor"]["red"];
					}
					if (format_json["booleanRule"]["format"]["backgroundColor"].contains("green") && format_json["booleanRule"]["format"]["backgroundColor"]["green"].is_number()) {
						G = format_json["booleanRule"]["format"]["backgroundColor"]["green"];
					}
					if (format_json["booleanRule"]["format"]["backgroundColor"].contains("blue") && format_json["booleanRule"]["format"]["backgroundColor"]["blue"].is_number()) {
						B = format_json["booleanRule"]["format"]["backgroundColor"]["blue"];
					}
					for (json value_json : format_json["booleanRule"]["condition"]["values"]) {
						if (value_json.contains("userEnteredValue")) {
							ImVec4 color(R, G, B, 1.0f);
							string mapKey = (string)(value_json["userEnteredValue"]);
							transform(mapKey.begin(), mapKey.end(), mapKey.begin(), ::tolower);
							Logger::d(string_format("set color for %s to %.2f %.2f %.2f", mapKey.c_str(), color.x, color.y, color.z));
							colors_cache[mapKey] = color;
						}
					}
				}
			}
		}
	} catch (std::exception e) {
		Logger::e("failed to download meta data");
		Logger::e(e.what());
	}
	hasMetaData = true; // dont try again if it failed
	downloading = false;
}

void SheetsAPI::requestNames() {
	if (downloading) return;
	if (names_cache == NULL) {
		Logger::d("getting names");
		downloading = true;
		if (downloadThread.joinable()) downloadThread.join();
		downloadThread = thread(&SheetsAPI::downloadNames, this);
	}
}

void SheetsAPI::requestMainRoles() {
	if (downloading) return;
	if (main_roles_cache == NULL) {
		Logger::d("getting names");
		downloading = true;
		if (downloadThread.joinable()) downloadThread.join();
		downloadThread = thread(&SheetsAPI::downloadMainRoles, this);
	}
}

void SheetsAPI::requestWing(int wing) {
	if (downloading) return;
	if (names_cache == NULL) {
		requestNames();
	}
	if (main_roles_cache == NULL) {
		requestMainRoles();
	}
	if (!downloading && !hasMetaData) {
		Logger::d("getting meta data");
		downloading = true;
		if (downloadThread.joinable()) downloadThread.join();
		downloadThread = thread(&SheetsAPI::downloadSheetMetaData, this);
	}
	if(!downloading) {
		Logger::d("getting wing");
		if (downloadThread.joinable()) downloadThread.join();
		downloading = true;
		downloadThread = thread(&SheetsAPI::downloadWing, this, wing);
	}
}

void SheetsAPI::clearCache() {
	main_roles_cache = NULL;
	roles_cache.clear();
	colors_cache.clear();
	hasMetaData = false;
}

bool SheetsAPI::hasWing(int wing) {
	if (names_cache == NULL) return false;
	return roles_cache.count(wing) == 1;
}

vector<vector<string>> SheetsAPI::getWing(int wing) {
	if (roles_cache.count(wing) == 0) {
		return vector<vector<string>>();
	}
	return roles_cache[wing];
}

vector<string> SheetsAPI::getHeader(int wing) {
	if (header_cache.count(wing) == 0) {
		return vector<string>();
	}
	return header_cache[wing];
}

shared_ptr<vector<string>> SheetsAPI::getNames() {
	return names_cache;
}

shared_ptr<vector<string>> SheetsAPI::getMainRoles() {
	return main_roles_cache;
}

map<string, ImVec4> SheetsAPI::getColors() {
	return colors_cache;
}

bool SheetsAPI::isDownloading() {
	return downloading;
}

SheetsAPI::~SheetsAPI() {
	if (downloadThread.joinable()) downloadThread.join();
}
