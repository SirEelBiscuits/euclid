#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <cstdlib>
#include <memory>
POST_STD_LIB

#include "CommandLine.h"
#include "lib/gargamel.h"
#include "rendering/RenderingSystem.h"

std::unique_ptr<Rendering::Context> Initialise();

int Main(int argc, char* argv[]) {
	Gargamel::SetArguments(Arguments, 0);
	Gargamel::Process(argc, argv);
	//GetConfig?
	//Initialise Graphics Context
	auto ctx = Initialise();
	//Run Mode
	return EXIT_SUCCESS;
}

std::unique_ptr<Rendering::Context> Initialise() {
	if (Gargamel::ArgumentValues[(int)CLArgs::Help].isArgumentPresent) {
		Gargamel::ShowUsage();
		exit(EXIT_SUCCESS);
	}

	auto ctx = Rendering::GetContext(
		static_cast<unsigned int>(Gargamel::ArgumentValues[(int)CLArgs::Width].intValue()),
		static_cast<unsigned int>(Gargamel::ArgumentValues[(int)CLArgs::Height].intValue()),
		Gargamel::ArgumentValues[(int)CLArgs::RenderScale].intValue(),
		Gargamel::ArgumentValues[(int)CLArgs::Fullscreen].isArgumentPresent,
		"Euclid Game Engine"
	);

	return ctx;
}