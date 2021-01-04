#ifndef _JIGSAW_MACHINE_OBSERVER_H_
#define _JIGSAW_MACHINE_OBSERVER_H_

#include "Entities/JigsawEntityCluster.h"
#include "Machines/JigsawMachine.h"
#include "Machines/SerializableMachineData.h"
#include "Assets/DataAssets.h"

namespace Jigsaw {
	namespace Machines {
		/// <summary>
		/// JigsawMachineObserver is an interface used for Fabricating entities primarily, but it can also provide insight
		/// into the Entities themselves. 
		/// </summary>
		class JigsawMachineObserver {
		public:
			virtual ~JigsawMachineObserver() {}

			/// <summary>
			/// Fetch the entity UID's associated with this machine
			/// </summary>
			/// <returns></returns>
			const std::vector<Jigsaw::System::UID>& GetEntityIds() const;

			/// <summary>
			/// Fetch the machine UID
			/// </summary>
			/// <returns></returns>
			Jigsaw::System::UID GetUID() const;

			/// <summary>
			/// This override allows the caller to specify FABRICATION_ARGS associated with the fabrication operation
			/// </summary>
			/// <param name="args"></param>
			/// <returns></returns>
			Jigsaw::Entities::JigsawEntity FabricateEntity(FABRICATION_ARGS& args);

			/// <summary>
			/// Fabricates a single JigsawEntity with the default parameters of the JigsawMachine that this object observes
			/// </summary>
			/// <returns></returns>
			Jigsaw::Entities::JigsawEntity FabricateEntity();

			/// <summary>
			/// Returns a signature associated with this JigsawMachine
			/// </summary>
			/// <returns></returns>
			const Jigsaw::System::Signature& GetMachineSignature();

		protected:
			JigsawMachineObserver(const Jigsaw::Assets::SerializedRef<JigsawMachine>& machine, Jigsaw::Entities::JigsawEntityCluster& cluster);

			std::vector<Jigsaw::System::UID> entity_ids;
			const Jigsaw::System::UID uid;
			const Jigsaw::Assets::SerializedRef<JigsawMachine> machine;
			Jigsaw::Entities::JigsawEntityCluster& cluster;

		};
	}
}
#endif