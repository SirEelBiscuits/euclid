#include "../Files.h"

PRE_STD_LIB
#include <Windows.h>
#include <map>
POST_STD_LIB

namespace System { namespace Events {

static std::map<std::string, std::pair<FILETIME, handler_type>> filenames;

bool RegisterFileToWatch(char const *filename, handler_type handler) {
	//TODO error handling, aliased filenames
	auto file = CreateFile(
		filename,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	FILETIME writetime;
	GetFileTime(
		file,
		NULL,
		NULL,
		&writetime
	);
	CloseHandle(file);
	filenames[filename] = std::pair<FILETIME,handler_type>(writetime, handler);

	return true;
}

bool cmpFileTime(FILETIME a, FILETIME b) {
	return a.dwHighDateTime < b.dwHighDateTime
		|| (a.dwHighDateTime == b.dwHighDateTime && a.dwLowDateTime < b.dwLowDateTime);
}

void TickFileWatchers() {
	for(auto & kvp : filenames) {
		auto file = CreateFile(
			kvp.first.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		FILETIME writetime;
		GetFileTime(
			file,
			NULL,
			NULL,
			&writetime
		);
		CloseHandle(file);
		if(cmpFileTime(kvp.second.first, writetime)) {
			kvp.second.first = writetime;
			kvp.second.second(kvp.first.c_str());
		}
	}
}

}}