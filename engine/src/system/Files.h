#include "lib/PortabilityHelper.h"

PREAMBLE

#include "Events.h"

namespace System { namespace Events {
	
using handler_type = std::function<void(char const *)>;
bool RegisterFileToWatch(char const * filename, handler_type handler);
bool IsFileBeingWatched(char const * filename);
void TickFileWatchers();

}}