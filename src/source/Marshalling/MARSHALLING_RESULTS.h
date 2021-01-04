#ifndef _UNMARSHALLING_RESULTS_H_
#define _UNMARSHALLING_RESULTS_H_

#include <vector>
#include "Assets/ASSET_LOAD_RESULT.h"

namespace Jigsaw {
	namespace Marshalling {
		/// <summary>
		/// Basic struct for describing the results of an Unmarshalling operation
		/// </summary>
		struct UNMARSHALLING_RESULT {
			void* raw_data = nullptr;
			Jigsaw::Assets::RESULT result = Jigsaw::Assets::INCOMPLETE;
			std::vector<Jigsaw::Assets::UNRESOLVED_REFERENCE> unresolved_references;
		};

		/// <summary>
		/// Basic struct for describing the results of an marshalling operation
		/// </summary>
		struct MARSHALLING_RESULT {
			std::string marshalled_object;
			Jigsaw::Assets::RESULT result = Jigsaw::Assets::INCOMPLETE;
		};
	}
}
#endif
