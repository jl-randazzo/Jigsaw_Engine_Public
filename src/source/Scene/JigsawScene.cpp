#include "JigsawScene.h"

using namespace Jigsaw::Scene;

Jigsaw::Scene::JigsawScene::JigsawScene(const Jigsaw::System::UID& scene_id, std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS>&& machine_fabrication_args, std::vector<Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>>&& machines)
	:scene_id(scene_id), machine_fabrication_args(machine_fabrication_args), machines(machines) {}

const std::vector<Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>>& Jigsaw::Scene::JigsawScene::GetMachines() {
	return this->machines;
}

const std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS>& Jigsaw::Scene::JigsawScene::GetMachineFabricationArgs() const {
	return machine_fabrication_args;
}

const Jigsaw::System::UID& Jigsaw::Scene::JigsawScene::GetUID() const {
	return scene_id;
}
