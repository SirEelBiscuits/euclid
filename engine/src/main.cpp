#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <cstdlib>
#include <memory>
POST_STD_LIB

#include "CommandLine.h"
#include "lib/gargamel.h"
#include "rendering/RenderingSystem.h"
#include "audio/AudioSystem.h"
#include "system/Config.h"
#include "system/RunnableMode.h"
#include "modes/game/Game.h"

std::unique_ptr<Rendering::Context> InitialiseGraphics(System::Config &cfg);
std::unique_ptr<Audio::Context> InitialiseAudio(System::Config &cfg);
void OverrideConfigWithCommandLineArguments(System::Config &cfg);
std::unique_ptr<System::RunnableMode> GetMode(Rendering::Context &ctx, Audio::Context &audioCtx, System::Config &cfg);

int Main(int argc, char* argv[]) {
	std::string configName = "";
	if(argc > 1 && argv[1][0] != '-') {
		configName = argv[1];
		configName += ".cfg";
	}

	Gargamel::SetArguments(Arguments, 0);
	Gargamel::Process(argc, argv);

	if(Gargamel::ArgumentValues[(int)CLArgs::Config].isArgumentPresent || configName.length() == 0)
		configName = Gargamel::ArgumentValues[(int)CLArgs::Config].argumentValue.c_str();

	System::Config cfg(configName.c_str());
	OverrideConfigWithCommandLineArguments(cfg);

	auto ctx = InitialiseGraphics(cfg);
	auto audio = InitialiseAudio(cfg);
	auto mode = GetMode(*ctx, *audio, cfg);


	mode->Run();

	while(mode->Update());

	return EXIT_SUCCESS;
}

std::unique_ptr<Rendering::Context> InitialiseGraphics(System::Config &cfg) {
	if (Gargamel::ArgumentValues[(int)CLArgs::Help].isArgumentPresent) {
		Gargamel::ShowUsage();
		exit(EXIT_SUCCESS);
	}

	auto width = cfg.GetValue<int>(stringFromCLArgs(CLArgs::Width));
	auto height = cfg.GetValue<int>(stringFromCLArgs(CLArgs::Height));

	auto ctx = Rendering::GetContext(
		width,
		height,
		cfg.GetValue<int>(stringFromCLArgs(CLArgs::RenderScale)),
		cfg.GetValue<bool>(stringFromCLArgs(CLArgs::Fullscreen)),
		"Euclid Game Engine"
	);

	auto vFOV = cfg.GetValue<float>(stringFromCLArgs(CLArgs::FOV));
	auto hFOV = std::atan(std::tan(vFOV * PI / 180) * (float)width / height) * 180 / PI;
	ctx->SetvFov(vFOV);
	ctx->SethFov(hFOV);

	return ctx;
}

std::unique_ptr<Audio::Context> InitialiseAudio(System::Config &cfg) {
	return Audio::GetContext();
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

	if(!cfg.IsValueSet(stringFromCLArgs(CLArgs::FOV)) || Gargamel::ArgumentValues[(int)CLArgs::FOV].isArgumentPresent)
		cfg.SetValue(stringFromCLArgs(CLArgs::FOV), Gargamel::ArgumentValues[(int)CLArgs::FOV].floatValue());

	if(!cfg.IsValueSet(stringFromCLArgs(CLArgs::Fullscreen)))
		cfg.SetValue(stringFromCLArgs(CLArgs::Fullscreen), false);
	else if(Gargamel::ArgumentValues[(int)CLArgs::Fullscreen].isArgumentPresent)
		cfg.SetValue(stringFromCLArgs(CLArgs::Fullscreen), true);
}

//TODO delete this shit?
std::unique_ptr<System::RunnableMode> GetMode(Rendering::Context &ctx, Audio::Context &audioCtx, System::Config &cfg) {
	return std::make_unique<Modes::Game>(ctx, audioCtx, cfg);
}