#include "system/Files.h"

PRE_STD_LIB
POST_STD_LIB

namespace System { namespace Events {

bool RegisterFileToWatch(char const *filename, handler_type handler) {}
void TickFileWatchers() {}
bool IsFileBeingWatched(char const * filename) { return true; }

}}
