#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "logger.h"
#include <mutex>
#include <mutex>


Logger* Logger::mLogger = NULL;

void Logger::e(const string& message) {
	if(mLogger) mLogger->me(message);
}
void Logger::w(const string& message) {
	if(mLogger) mLogger->mw(message);
}
void Logger::i(const string& message) {
	if(mLogger) mLogger->mi(message);
}
void Logger::d(const string& message) {
	if(mLogger) mLogger->md(message);
}
void Logger::init(const string& logfilename) {
	mLogger = new Logger(logfilename);
}

void Logger::free() {
	delete mLogger;
	mLogger = NULL;
}

Logger::Logger(const std::string& logFilename_) : logFilename(logFilename_){
	//std::unique_lock<std::mutex> lock1(fileMutex, std::defer_lock);
}

/*Logger::Logger(const Logger& other) {
	logFile = other.logFile;
}*/
/*
Logger::~Logger() {

}*/

void Logger::me(const string& msg) {
	write("[ERR] ", msg , "\n");
}
void Logger::mw(const string& msg) {
	write("[WRN] ", msg, "\n");
}
void Logger::mi(const string& msg) {
	write("[INF] ", msg, "\n");
}
void Logger::md(const string& msg) {
	write("[DBG] ", msg, "\n");
}

void Logger::write(const std::string& msg0) {
	write(msg0, "", "");
}

void Logger::write(const std::string& msg0, const std::string& msg1) {
	write(msg0, msg1, "");
}

void Logger::write(const std::string& msg0, const std::string& msg1, const std::string& msg2) {
	//lock1.lock();
	std::cout << msg0;
	std::cout << msg1;
	std::cout << msg2;
	//file.flush();
	//file.close();
	//lock1.unlock();
}

bool Logger::is_valid(const std::ofstream& logFile) {
	return logFile.is_open();
}

