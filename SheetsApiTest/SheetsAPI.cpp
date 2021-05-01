#include "SheetsAPI.h"
#include <string>
#include <cpr/cpr.h>
#include "json.hpp"
#include "logger.h"
#include "string_format.hpp"

using namespace nlohmann;
using std::vector;
using std::string;
using std::unique_ptr;

SheetsAPI::SheetsAPI() : sheetId(""), api_key("") {
	downloading = false;
	Logger::d("default constr");
}

SheetsAPI::SheetsAPI(string sheetId_, string api_key_) : sheetId(sheetId_), api_key(api_key_) {
	downloading = false;
	Logger::d(string_format("sid:%s", sheetId.c_str()));
}

unique_ptr<json> getSheetValuesJson(const string& sheetId, const string& range, const string api_key) {
	cpr::Url url = cpr::Url("https://sheets.googleapis.com/v4/spreadsheets/" + sheetId + "/values/" + range + "?key=" + api_key);
	//, cpr::Parameters{ { "key", api_key } }
	cpr::Response r = cpr::Get(url);
	if (r.status_code != 200) {
		Logger::e("HTTP returned " + r.status_code);
		return NULL;
	}
	Logger::i(r.text);
	json j = "{\"range\": \"'The Truth'!A5:A14\",\"majorDimension\" : \"ROWS\",\"values\" : [[\"Ewanye\"],[\"Fra\"],[\"Make\"],[\"Zerumi\"],[\"Pitt\"],[\"Miruna\"],[\"Pv\"],[\"Lynn\"],[\"Lele\"],[\"Rey\"]]}"_json;
	return std::make_unique<json>(j);
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
	//unique_ptr<json> j = NULL;
	unique_ptr<json> j = getSheetValuesJson(sheetId, range, api_key);
	Logger::i("downloading names s2");
	
	//json j = "{\"range\": \"'The Truth'!A5:A14\",\"majorDimension\" : \"ROWS\",\"values\" : [[\"Ewanye\"],[\"Fra\"],[\"Make\"],[\"Zerumi\"],[\"Pitt\"],[\"Miruna\"],[\"Pv\"],[\"Lynn\"],[\"Lele\"],[\"Rey\"]]}"_json;

	Logger::i("downloading names s3");
	vector<string> v;
	Logger::i("downloading names s4");
	if (j == NULL) return v;
	Logger::i("downloading names s5");
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

vector<vector<string>> getValuesAs2DArray(const string& sheetId, const string& range, const string api_key) {
	vector<vector<string>> v;
	if(0==0) return v;
	unique_ptr<json> j = getSheetValuesJson(sheetId, range, api_key);
	if (j == NULL || 0 == 0) return v;
	if (!j->contains("values") || !(*j)["values"].is_array()) {
		return v;
	}
	for (auto& row : (*j)["values"]) {
		if (!row.is_array()) {
			continue;
		}
		vector<string> sub_vector;
		for (auto& cell : row) {
			sub_vector.push_back(cell);
		}
		v.push_back(sub_vector);
	}
	return v;
}

void SheetsAPI::downloadNames() {
	try {
		Logger::d("downloading names");
		const string range = "%27The%20Truth%27%21A5%3AA14"; // "'The Truth'!A5:A14";
		Logger::i("downloaded names s1");
		Logger::d(string_format("ran:%s", range.c_str()));
		Logger::d(string_format("key len=%d", api_key.length()));
		Logger::d(string_format("sid len=%d", sheetId.length()));
		Logger::d(string_format("key=%s", api_key.c_str()));
		Logger::d(string_format("sid=%s", sheetId.c_str()));
		names_cache = std::make_shared<vector<string>>(getValuesAs1DArray(sheetId, range, api_key));
		Logger::i("downloaded names");
	} catch (std::exception e) {
		Logger::e("failed to download names");
		Logger::e(e.what());
	}
}

void SheetsAPI::downloadWing(int wing) {
	try {
		Logger::i("downloading wing");
		string range;
		switch (wing) {
		case 4:
			range = "'The Truth'!B22:E31";
			break;
		case 5:
			range = "'The Truth'!B5:E14";
			break;
		case 6:
			range = "'The Truth'!G5:I14";
			break;
		case 7:
			range = "'The Truth'!K5:M14";
			break;
		default:
			return;
		}
		roles_cache[wing] = getValuesAs2DArray(sheetId, range, api_key);
		Logger::i("downloaded wing");
	} catch (std::exception) {
		Logger::e("failed to download wing");
	}
	downloading = false;
}

void SheetsAPI::requestWing(int wing) {
	if (downloading) return;
	downloading = true;
	Logger::i("getting");
	if (names_cache == NULL) {
		Logger::i("getting names");
		std::thread t2(&SheetsAPI::downloadNames, this);
		//t2.join();
	}
	std::thread t1(&SheetsAPI::downloadWing, this, wing);
	

}

bool SheetsAPI::hasWing(int wing) {
	if (names_cache == NULL) return false;
	return roles_cache.count(wing) == 1;
}

vector<vector<string>> SheetsAPI::getWing(int wing) {
	return roles_cache[wing];
}

shared_ptr<vector<string>> SheetsAPI::getNames() {
	return names_cache;
}