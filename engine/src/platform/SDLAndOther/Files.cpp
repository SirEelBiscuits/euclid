#include "system/Files.h"

PRE_STD_LIB
#include <set>
#include <string>
POST_STD_LIB

namespace System { namespace Events {
	static std::set<std::string> fileList{};

	bool RegisterFileToWatch(char const *filename, handler_type handler) {
		printf("\n*loading %s\n", filename);
		fileList.insert(filename);
		return true;
	}

	void TickFileWatchers() {}

	bool IsFileBeingWatched(char const * filename) {
		auto ret = fileList.find(filename) != fileList.end();
		printf("check %s, returning: %s\n", filename, ret ? "true" : "false");
		return ret;
	}

}}
