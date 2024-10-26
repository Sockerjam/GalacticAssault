#pragma once
#include <string>
#include <vector>

enum LogType {
	LOG_MESSAGE,
	LOG_WARNING,
	LOG_ERROR
};

struct LogEntry {
	LogType logType;
	std::string message;
};

class Logger {

private:
	static std::vector<LogEntry> logEntries;
	static std::string time();
	static void addLogEntry(LogType logType, const std::string& message);

public:
	static void Log(const std::string& message);
	static void LogErr(const std::string& message);
};