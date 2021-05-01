#pragma once
#include <vector>
#include <string>
#include <map>
#include <memory>

using std::vector;
using std::string;
using std::map;
using std::shared_ptr;

class SheetsAPI {
public:
	SheetsAPI();
	SheetsAPI(string sheetId, string api_key);
	void downloadNames();
	void downloadWing(int wing);
	bool hasWing(int wing);
	void requestWing(int wing);
	vector<vector<string>> getWing(int wing);
	shared_ptr<vector<string>> getNames();
private:
	bool downloading;
	map<int, vector<vector<string>>> roles_cache;
	shared_ptr<vector<string>> names_cache;
	const string sheetId;
	const string api_key;
};

