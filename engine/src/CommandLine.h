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
	Editor,
	StartScript,
	FOV,
	Config,
};

char const* stringFromCLArgs(CLArgs in) {
	switch(in) {
	case CLArgs::Fullscreen:
		return "fullscreen";
	case CLArgs::Width:
		return "width";
	case CLArgs::Height:
		return "height";
	case CLArgs::RenderScale:
		return "renderscale";
	case CLArgs::StartScript:
		return "startscript";
	case CLArgs::FOV:
		return "FOV";
	default:;
	}
	return nullptr;
}

Gargamel::ArgumentList Arguments {
	GGM_DESCRIBE_ARG        (CLArgs::Help,        'h',  "help",         NoArgument,                   "Display this help test"),
	GGM_DESCRIBE_ARG        (CLArgs::Fullscreen,  'f',  "fullscreen",   NoArgument,                   "Render in fullscreen mode"),
	GGM_DESCRIBE_ARG_DEFAULT(CLArgs::Width,             "width",        RequiredArgument, "1024",     "Width to render"),
	GGM_DESCRIBE_ARG_DEFAULT(CLArgs::Height,            "height",       RequiredArgument, "768",      "Height to render"),
	GGM_DESCRIBE_ARG_DEFAULT(CLArgs::RenderScale,       "render-scale", RequiredArgument, "4",        "Scale down the render"),
	GGM_DESCRIBE_ARG        (CLArgs::Editor,      'e',  "editor",       NoArgument,                   "Launch the editor"),
	GGM_DESCRIBE_ARG        (CLArgs::StartScript, '\0', "start-script", RequiredArgument,             "The script to launch"),
	GGM_DESCRIBE_ARG_DEFAULT(CLArgs::FOV,               "fov",          RequiredArgument, "45",       "The vertical FOV to use"),
	GGM_DESCRIBE_ARG_DEFAULT(CLArgs::Config,            "config",       RequiredArgument, "game.cfg", "The game config to use"),
};