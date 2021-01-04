#ifndef _JIGSAW_SCENE_H_
#define _JIGSAW_SCENE_H_

#include <vector>
#include "Machines/JigsawMachine.h"
#include "Assets/DataAssets.h"
#include <map>

namespace Jigsaw {
	namespace Scene {

		class JigsawScene {
		public:
			JigsawScene(const Jigsaw::System::UID& scene_id, std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS>&& machine_fabrication_args, std::vector<Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>>&& machines);

			const std::vector<Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>>& GetMachines();

			const std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS>& GetMachineFabricationArgs() const;

			const Jigsaw::System::UID& GetUID() const;
			
		private:

			const Jigsaw::System::UID& scene_id;
			const std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS> machine_fabrication_args;
			std::vector<Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>> machines;
		};
	}
}

#endif // !_JIGSAW_SCENE_H_
