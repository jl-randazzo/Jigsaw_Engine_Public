#ifndef _SYSTEM_REGISTRY_H_
#define _SYSTEM_REGISTRY_H_

#include <functional>
#include <vector>
#include "System/Signature.h"

namespace Jigsaw {
	namespace Systems {
		class RootSystem;

		/// <summary>
		/// The SYSTEM_REGISTRY_BEAN contains all registry information needed for the SystemRegistry to keep track of
		/// all of the systems and how to instantiate them.
		/// </summary>
		struct SYSTEM_REGISTRY_BEAN {
			Jigsaw::Ref<Jigsaw::System::Signature> signature;
			std::function<Jigsaw::Unique<RootSystem>()> generate_system_function;
			const Jigsaw::Util::etype_info* type_info;
		};

		/// <summary>
		/// The SystemRegistry is where SYSTEM_REGISTRY_BEANs are passed and retrieved later
		/// </summary>
		class SystemRegistry {
		public:
			static std::vector<SYSTEM_REGISTRY_BEAN>& GetBeans();
		};
	}
}

#endif