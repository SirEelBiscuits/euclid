#include "system/Files.h"

PRE_STD_LIB
#include <Windows.h>
#include <map>
POST_STD_LIB

namespace System { namespace Events {

static std::map<std::string, std::pair<FILETIME, handler_type>> filenames;
static decltype(begin(filenames)) iter;

FILETIME GetLastWriteTime(char const *filename) {
	//TODO error handling, aliased filenames
	WIN32_FIND_DATA findinfo;
	FindClose(FindFirstFile(filename, &findinfo));
	return findinfo.ftLastWriteTime;
}

bool RegisterFileToWatch(char const *filename, handler_type handler) {
	auto writetime = GetLastWriteTime(filename);
	filenames[filename] = std::pair<FILETIME,handler_type>(writetime, handler);

	iter = begin(filenames);

	return true;
}

bool IsFileBeingWatched(char const *filename) {
	return filenames.find(filename) != filenames.end();
}

bool cmpFileTime(FILETIME a, FILETIME b) {
	return a.dwHighDateTime != b.dwHighDateTime
		|| a.dwLowDateTime != b.dwLowDateTime;
}

void TickFileWatchers() {
	iter++;
	if(iter == end(filenames))
		iter = begin(filenames);
	auto & kvp = *iter;
	auto writetime = GetLastWriteTime(kvp.first.c_str());
	if(cmpFileTime(kvp.second.first, writetime)) {
		kvp.second.first = writetime;
		kvp.second.second(kvp.first.c_str());
	}
}

}}