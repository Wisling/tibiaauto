#include <stdafx.h>

#include "Logger.h"

#include "Utils.h"

#include <time.h>

const int Logger::ERRORconst = 1;
const int Logger::DEBUGconst = 2;

void Logger::log(const string &msg, int logType) {
	switch (logType) {
		case ERRORconst:
			log(msg, "error");
			break;
		case DEBUGconst:
			log(msg, "debug");
			break;
		default:
			break;
	}
}

void Logger::log(const string &msg, const string &file, bool header) {
	fstream f;
	if (openLog(f, file + ".log")) {
		if (header) {
			f << toUpper(file) << ": " << timestamp() << endl;
		}
		f << msg << endl << endl;
		closeLog(f);
	}
}

string Logger::timestamp() {
	//	FORMAT: 16-12-2002 01:32 PM
	time_t t;
	struct tm *now;
	char buffer[256];
	
	t = time(NULL);
	now = localtime(&t);
	
	strftime(buffer, 256, "%d-%m-%Y %I:%M %p", now);
	
	return toUpper(string(buffer));
}

bool Logger::openLog(fstream &file, const string &name) {
	file.open(name.c_str(), ios::out | ios::app);
	if (file.is_open()) {
		file.seekp(0, ios::end);
		return true;
	}
	return false;
}

void Logger::closeLog(fstream &file) {
	if (file.is_open()) {
		file.close();
	}
}