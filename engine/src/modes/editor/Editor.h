#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB

POST_STD_LIB

#include "system/RunnableMode.h"

namespace Modes {
	class Editor : public System::RunnableMode  {
	public:
		Editor(Rendering::Context &ctx, System::Config &cfg);
		virtual ~Editor();

		virtual void Run() override;
		virtual bool Update() override;

	private:
	};
}
