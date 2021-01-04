#include "SystemRegistry.h"

namespace Jigsaw {
	namespace Systems {
		std::vector<SYSTEM_REGISTRY_BEAN>& Jigsaw::Systems::SystemRegistry::GetBeans() {
			static std::vector<SYSTEM_REGISTRY_BEAN> vector;
			return vector;
		}
	}
}
