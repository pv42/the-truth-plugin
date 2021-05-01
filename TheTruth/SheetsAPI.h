#pragma once
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <thread>
#include "imgui/imgui.h"
#include "Settings.h"
#include <atomic>

using std::vector;
using std::string;
using std::map;
using std::shared_ptr;
using std::thread;
using std::atomic_bool;

class SheetsAPI {
public:
	SheetsAPI();
	~SheetsAPI();
	SheetsAPI(const string api_key, Settings& setttings);
	void downloadNames();
	void downloadWing(int wing);
	void downloadSheetMetaData();
	bool hasWing(int wing);
	void requestWing(int wing);
	void requestNames();
	vector<vector<string>> getWing(int wing);
	vector<string> getHeader(int wing);
	shared_ptr<vector<string>> getNames();
	map<string, ImVec4> getColors();
	void clearCache();
private:
	Settings settings;
	atomic_bool downloading;
	bool hasMetaData;
	string api_key;
	map<int, vector<vector<string>>> roles_cache;
	shared_ptr<vector<string>> names_cache;
	map<int, vector<string>> header_cache;
	map<string, ImVec4> colors_cache;
	thread downloadThread;
};



