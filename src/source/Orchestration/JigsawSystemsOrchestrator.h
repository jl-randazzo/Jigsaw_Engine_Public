#ifndef _JIGSAW_SYSTEM_ORCHESTRATOR_H_
#define _JIGSAW_SYSTEM_ORCHESTRATOR_H_

#include "Entities/JigsawEntityClusterService.h"
#include "Systems/RootSystem.h"
#include "Ref.h"

namespace Jigsaw {
	namespace Orchestration {
		class JigsawSystemsOrchestrator {
		public:
			JigsawSystemsOrchestrator(Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> cluster_service);

			void Update();

		private:
			void SynchronousDispatch(Jigsaw::Entities::JigsawEntityCluster& cluster, const Jigsaw::System::SignatureMap& conversion_map, const Jigsaw::Systems::SYSTEM_REGISTRY_BEAN& sys_bean);

			const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> cluster_service;
		};

	}
}
#endif