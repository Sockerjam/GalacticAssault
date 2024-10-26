#include "Logger.h"
#include <iostream>
#include <chrono>
#include "date.h"

std::vector<LogEntry> Logger::logEntries;

void Logger::Log(const std::string& message)
{
	addLogEntry(LOG_MESSAGE, message);
	std::cout << "\033[32m" << " LOG: [" << time() << "] " << message << "\033[0m" << std::endl;
};

void Logger::LogErr(const std::string& message)
{
	addLogEntry(LOG_ERROR, message);
	std::cout << "\033[31m" << "LOG: [" << time() << "] " << message << "\033[0m" << std::endl;
};

std::string Logger::time()
{
	return date::format("%F %H:%M", std::chrono::system_clock::now());
};

void Logger::addLogEntry(LogType logType, const std::string& message)
{
	LogEntry logEntry;
	logEntry.logType = logType;
	logEntry.message = message;

	Logger::logEntries.push_back(logEntry);
};