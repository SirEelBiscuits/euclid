#include "Editor.h"

namespace Modes {

	Editor::Editor(Rendering::Context & ctx, System::Config & cfg) 
		: RunnableMode(ctx, cfg)
	{
	}

	Editor::~Editor() {
	}

	void Editor::Run() {
	}

	bool Editor::Update() {
		return false;
	}
}