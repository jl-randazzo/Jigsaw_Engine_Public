#ifndef _ASSET_LOAD_RESULT_H_
#define _ASSET_LOAD_RESULT_H_
#include <vector>
#include "System/UID.h"
#include <functional>
#include <future>

namespace Jigsaw {
	namespace Assets {

		enum RESULT {
			INCOMPLETE, COMPLETE, UNRESOLVED_REFERENCES, FAILURE
		};

		struct UNRESOLVED_REFERENCE {
			Jigsaw::System::UID ref_id;

			// function that takes the resolved asset reference and injects it back into the object in question
			std::function<void(void*)> injector;
		};

		struct ASSET_LOAD_RESULT {
			RESULT result = INCOMPLETE;
			std::vector<UNRESOLVED_REFERENCE> unresolved_references;
		};

	}
}

#endif // !_ASSET_LOAD_RESULT_H_

