#include "Sample.h"

PRE_STD_LIB
#include <map>
POST_STD_LIB

namespace Audio {
	namespace SampleStore {
		static std::map<std::string, SampleRef> SampleLookup;

		SampleRef GetSample(char const *filename) {
			auto &test = SampleLookup[filename];
			if(test.get() == nullptr)
				test = LoadSample(filename);
			return test;
		}

		SampleRef GetSample(Sample const * sample) {
			for( auto &i : SampleLookup) {
			if(i.second.get() == sample)
				return i.second;
			}
			return SampleRef{nullptr};
		}

		std::string GetSampleFilename(Sample const * tex) {
			for(auto &kvp : SampleLookup) {
				if(kvp.second.get() == tex)
					return kvp.first;
			}
			return "";
		}

		void ClearUnique() {
			for(auto &kvp : SampleLookup)
				if(kvp.second.unique())
					kvp.second.reset();
		}
	}
}