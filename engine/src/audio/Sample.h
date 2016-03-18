#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <memory>
#include <string>
POST_STD_LIB

namespace Audio {

	struct Sample{
		virtual ~Sample(){}

		virtual void Play() = 0;
	};

	namespace SampleStore {
		using SampleRef = std::shared_ptr<Sample>;

		SampleRef LoadSample(char const *filename);

		SampleRef GetSample(char const *filename);

		SampleRef GetSample(Sample const *sample);

		std::string GetSampleFilename(Sample const *sample);

		void ClearUnique();
	}
}