#pragma once
#include <string>
#include <fstream>
#include <mutex>

using string = std::string;

class Logger {
public:
	static Logger* mLogger;
	static void e(const string& message);
	static void w(const string& message);
	static void i(const string& message);
	static void d(const string& message);
	static void init(const string& logfilename);
	static void free();
	Logger(const string& logfilename);
private:
	string logFilename;
	void me(const string& message);
	void mw(const string& message);
	void mi(const string& message);
	void md(const string& message);
	void write(const string&);
	void write(const string&, const string&);
	void write(const string&, const string&, const string&);
	bool is_valid(const std::ofstream& logFile);
	//std::mutex fileMutex;
	//std::unique_lock<std::mutex> lock1;
};
