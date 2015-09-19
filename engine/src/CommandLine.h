#include "lib/PortabilityHelper.h"

PREAMBLE

#include "lib/gargamel.h"

enum class CLArgs {
	Unknown,
	Help,
	Fullscreen,
	Width,
	Height,
	RenderScale,
};

Gargamel::ArgumentList Arguments {
	GGM_DESCRIBE_ARG        (CLArgs::Help,       'h', "help",         NoArgument,               "Display this help test"),
	GGM_DESCRIBE_ARG        (CLArgs::Fullscreen, 'f', "fullscreen",   NoArgument,               "Render in fullscreen mode"),
	GGM_DESCRIBE_ARG_DEFAULT(CLArgs::Width,           "width",        RequiredArgument, "1024", "Width to render"),
	GGM_DESCRIBE_ARG_DEFAULT(CLArgs::Height,          "height",       RequiredArgument, "768",  "Height to render"),
	GGM_DESCRIBE_ARG_DEFAULT(CLArgs::RenderScale,     "render-scale", RequiredArgument, "4",    "Scale down the render"),
};