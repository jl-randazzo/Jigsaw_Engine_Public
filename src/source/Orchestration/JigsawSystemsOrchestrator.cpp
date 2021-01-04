#include "JigsawSystemsOrchestrator.h"

namespace Jigsaw {
	namespace Orchestration {
		JigsawSystemsOrchestrator::JigsawSystemsOrchestrator(Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> cluster_service) : cluster_service(cluster_service) { }
		void JigsawSystemsOrchestrator::Update() {
			const std::vector<Jigsaw::Systems::SYSTEM_REGISTRY_BEAN> sys_beans = Jigsaw::Systems::SystemRegistry::GetBeans();

			// iterating through all of the active systems in the world
			for (const Jigsaw::Systems::SYSTEM_REGISTRY_BEAN& sys_bean : sys_beans) {

				// iterating through all of the active clusters in the cluster service
				for (auto cluster_iterator : (*cluster_service)) {

					Jigsaw::Unique<Jigsaw::System::SignatureMap> conversion_map;
					Jigsaw::Entities::JigsawEntityCluster& cluster = *cluster_iterator.second;

					if (cluster.GetSignature().HasConversionMap(*sys_bean.signature, &conversion_map)) {
						SynchronousDispatch(cluster, *conversion_map, sys_bean);
					}
				}
			}
		}

		void JigsawSystemsOrchestrator::SynchronousDispatch(Jigsaw::Entities::JigsawEntityCluster& cluster, const Jigsaw::System::SignatureMap& conversion_map, const Jigsaw::Systems::SYSTEM_REGISTRY_BEAN& sys_bean) {
			Jigsaw::Unique<Jigsaw::Systems::RootSystem> system = sys_bean.generate_system_function();
			for (int i = 0; i < cluster.GetClusterCount(); i++) {
				Jigsaw::Entities::JigsawEntityCluster::JIGSAW_ENTITY_ITERATOR e_iterator = cluster.GetNodeIterator(i);
				while (e_iterator) {
					Jigsaw::Entities::JigsawEntity entity = *e_iterator;
					system->Update(entity, conversion_map);
					e_iterator++;
				}
			}
		}
	}
}
