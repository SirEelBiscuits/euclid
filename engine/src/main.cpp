#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <cstdlib>
#include <memory>
POST_STD_LIB

#include "CommandLine.h"
#include "lib/gargamel.h"
#include "rendering/RenderingSystem.h"
#include "system/Config.h"
#include "system/RunnableMode.h"
#include "modes/game/Game.h"
#include "modes/editor/Editor.h"

std::unique_ptr<Rendering::Context> Initialise(System::Config &cfg);
void OverrideConfigWithCommandLineArguments(System::Config &cfg);
std::unique_ptr<System::RunnableMode> GetMode(Rendering::Context &ctx, System::Config &cfg);

int Main(int argc, char* argv[]) {
	Gargamel::SetArguments(Arguments, 0);
	Gargamel::Process(argc, argv);

	System::Config cfg("config.cfg");
	OverrideConfigWithCommandLineArguments(cfg);

	auto ctx = Initialise(cfg);
	auto mode = GetMode(*ctx, cfg);


	mode->Run();

	while(mode->Update());

	return EXIT_SUCCESS;
}

std::unique_ptr<Rendering::Context> Initialise(System::Config &cfg) {
	if (Gargamel::ArgumentValues[(int)CLArgs::Help].isArgumentPresent) {
		Gargamel::ShowUsage();
		exit(EXIT_SUCCESS);
	}

	if(Gargamel::ArgumentValues[(int)CLArgs::Editor].isArgumentPresent)
		cfg.SetValue(stringFromCLArgs(CLArgs::RenderScale), 1);

	auto ctx = Rendering::GetContext(
		cfg.GetValue<int>(stringFromCLArgs(CLArgs::Width)),
		cfg.GetValue<int>(stringFromCLArgs(CLArgs::Height)),
		cfg.GetValue<int>(stringFromCLArgs(CLArgs::RenderScale)),
		cfg.GetValue<bool>(stringFromCLArgs(CLArgs::Fullscreen)),
		"Euclid Game Engine"
	);

	return ctx;
}

void OverrideConfigWithCommandLineArguments(System::Config &cfg) {
	//if the config isn't set, the command line contains the default value anyway
	if(!cfg.IsValueSet(stringFromCLArgs(CLArgs::Width)) || Gargamel::ArgumentValues[(int)CLArgs::Width].isArgumentPresent)
		cfg.SetValue(stringFromCLArgs(CLArgs::Width), Gargamel::ArgumentValues[(int)CLArgs::Width].intValue());

	if(!cfg.IsValueSet(stringFromCLArgs(CLArgs::Height)) || Gargamel::ArgumentValues[(int)CLArgs::Height].isArgumentPresent)
		cfg.SetValue(stringFromCLArgs(CLArgs::Height), Gargamel::ArgumentValues[(int)CLArgs::Height].intValue());

	if(!cfg.IsValueSet(stringFromCLArgs(CLArgs::RenderScale)) || Gargamel::ArgumentValues[(int)CLArgs::RenderScale].isArgumentPresent)
		cfg.SetValue(stringFromCLArgs(CLArgs::RenderScale), Gargamel::ArgumentValues[(int)CLArgs::RenderScale].intValue());

	if(!cfg.IsValueSet(stringFromCLArgs(CLArgs::StartScript)) || Gargamel::ArgumentValues[(int)CLArgs::StartScript].isArgumentPresent)
		cfg.SetValue(stringFromCLArgs(CLArgs::StartScript), Gargamel::ArgumentValues[(int)CLArgs::StartScript].argumentValue);

	if(!cfg.IsValueSet(stringFromCLArgs(CLArgs::Fullscreen)))
		cfg.SetValue(stringFromCLArgs(CLArgs::Fullscreen), false);
	else if(Gargamel::ArgumentValues[(int)CLArgs::Fullscreen].isArgumentPresent)
		cfg.SetValue(stringFromCLArgs(CLArgs::Fullscreen), true);
}

std::unique_ptr<System::RunnableMode> GetMode(Rendering::Context &ctx, System::Config &cfg) {
	if(Gargamel::ArgumentValues[(int)CLArgs::Editor].isArgumentPresent)
		return std::make_unique<Modes::Editor>(ctx, cfg);
	return std::make_unique<Modes::Game>(ctx, cfg);
}