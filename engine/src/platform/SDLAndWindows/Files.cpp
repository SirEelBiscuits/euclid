#include "system/Files.h"

PRE_STD_LIB
#include <Windows.h>
#include <map>
POST_STD_LIB

namespace System { namespace Events {

static std::map<std::string, std::pair<FILETIME, handler_type>> filenames;

FILETIME GetLastWriteTime(char const *filename) {
	//TODO error handling, aliased filenames
	WIN32_FIND_DATA findinfo;
	FindClose(FindFirstFile(filename, &findinfo));
	return findinfo.ftLastWriteTime;
}

bool RegisterFileToWatch(char const *filename, handler_type handler) {
	auto writetime = GetLastWriteTime(filename);
	filenames[filename] = std::pair<FILETIME,handler_type>(writetime, handler);

	return true;
}

bool cmpFileTime(FILETIME a, FILETIME b) {
	return a.dwHighDateTime != b.dwHighDateTime
		|| a.dwLowDateTime != b.dwLowDateTime;
}

void TickFileWatchers() {
	for(auto & kvp : filenames) {
		auto writetime = GetLastWriteTime(kvp.first.c_str());
		if(cmpFileTime(kvp.second.first, writetime)) {
			kvp.second.first = writetime;
			kvp.second.second(kvp.first.c_str());
		}
	}
}

}}