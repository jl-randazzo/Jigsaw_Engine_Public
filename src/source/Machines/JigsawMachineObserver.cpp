#include "JigsawMachineObserver.h"
#include "Debug/j_debug.h"
#include "Machines/FABRICATION_ARGS.h"

namespace Jigsaw {
	namespace Machines {
		JigsawMachineObserver::JigsawMachineObserver(const Jigsaw::Assets::SerializedRef<JigsawMachine>& machine, Jigsaw::Entities::JigsawEntityCluster& cluster) : machine(machine), cluster(cluster) {}

		Jigsaw::System::UID JigsawMachineObserver::GetUID() const {
			return machine->machine_id;
		}

		Jigsaw::Entities::JigsawEntity JigsawMachineObserver::FabricateEntity(FABRICATION_ARGS& args) {
			Jigsaw::Entities::JigsawEntity e = machine->FabricateEntity(cluster, args);
			entity_ids.push_back(e.GetUID());
			return e;
		}

		Jigsaw::Entities::JigsawEntity JigsawMachineObserver::FabricateEntity() {
			FABRICATION_ARGS args;
			Jigsaw::Entities::JigsawEntity e = machine->FabricateEntity(cluster, args);
			entity_ids.push_back(e.GetUID());
			return e;
		}

		const Jigsaw::System::Signature& JigsawMachineObserver::GetMachineSignature() {
			return cluster.GetSignature();
		}

		const std::vector<Jigsaw::System::UID>& JigsawMachineObserver::GetEntityIds() const {
			return entity_ids;
		}
	}
}

