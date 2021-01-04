#include "JigsawScenesObserver.h"
#include "Machines/FABRICATION_ARGS.h"

#include "Assets/DataAssets.h"

namespace Jigsaw {
	namespace Scene {
		JigsawScenesObserver::_SceneObserver::_SceneObserver(const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService>& cluster_service, const Jigsaw::Ref<Jigsaw::Scene::JigsawScene>& scene)
			: cluster_service(cluster_service), scene(scene), scene_id(scene->GetUID()) {
			auto machines = scene->GetMachines();
			for (const Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine> machine : machines) {

				Jigsaw::Entities::JigsawEntityCluster& cluster = (*cluster_service)[*machine->GetSignature().get()];
				jgsw_machines.insert(std::make_pair(machine->machine_id, Ref<Jigsaw::Machines::JigsawMachineObserver>(new _JGSW_MachineObserver(machine, cluster))));

			}
		}

		JigsawScenesObserver::_SceneObserver::_SceneObserver(const _SceneObserver& other) : scene(other.scene), scene_id(other.scene_id), cluster_service(other.cluster_service), jgsw_machines(other.jgsw_machines) { }

		JigsawScenesObserver::_SceneObserver::_SceneObserver(_SceneObserver&& other) : scene(other.scene), scene_id(other.scene_id), cluster_service(other.cluster_service), jgsw_machines(std::move(other.jgsw_machines)) { }

		JigsawScenesObserver::_SceneObserver& JigsawScenesObserver::_SceneObserver::operator=(const _SceneObserver& other) {
			scene = other.scene;
			scene_id = other.scene_id;
			cluster_service = other.cluster_service;
			jgsw_machines = other.jgsw_machines;
			return *this;
		}

		JigsawScenesObserver::_SceneObserver& JigsawScenesObserver::_SceneObserver::operator=(_SceneObserver&& other) {
			scene = std::move(other.scene);
			scene_id = other.scene_id;
			cluster_service = std::move(other.cluster_service);
			jgsw_machines = std::move(other.jgsw_machines);
			return *this;
		}

		void JigsawScenesObserver::_SceneObserver::Awake() {
			auto machine_fabrication_args = scene->GetMachineFabricationArgs();
			for (std::pair<const Jigsaw::System::UID, Ref<Jigsaw::Machines::JigsawMachineObserver>>& machine_pair : jgsw_machines) {
				const Jigsaw::System::UID& machine_id = machine_pair.first;
				_JGSW_MachineObserver* machine_observer = static_cast<_JGSW_MachineObserver*>(machine_pair.second.get());

				auto iter = machine_fabrication_args.find(machine_id);

				if (iter != machine_fabrication_args.end()) {
					machine_observer->Init(iter->second);
				}
			}
		}

		void JigsawScenesObserver::_SceneObserver::Destroy() {
			for (std::pair<const Jigsaw::System::UID, Ref<Jigsaw::Machines::JigsawMachineObserver>>& pair : this->jgsw_machines) {
				_JGSW_MachineObserver* machine_observer = static_cast<_JGSW_MachineObserver*>(pair.second.get());
				machine_observer->Destroy();
			}
		}

		const Jigsaw::System::UID& JigsawScenesObserver::_SceneObserver::GetUID() const {
			return scene->GetUID();
		}

		bool JigsawScenesObserver::_SceneObserver::GetMachineObserver(const Jigsaw::System::UID& machine_id, Jigsaw::Ref<Jigsaw::Machines::JigsawMachineObserver>* observer_out) {
			auto iter = jgsw_machines.find(machine_id);
			if (iter != jgsw_machines.end()) {
				*observer_out = iter->second;
				return true;
			}
			return false;
		}

		void JigsawScenesObserver::ActivateScene(const Jigsaw::Ref<Jigsaw::Scene::JigsawScene>& scene) {
			scenes.push_back(_SceneObserver(cluster_service, scene));
			scenes.back().Awake();
		}

		void JigsawScenesObserver::DeactivateScene(const Jigsaw::System::UID& scene_id) {
			auto iter = std::find_if(scenes.begin(), scenes.end(), [scene_id](_SceneObserver& scene) { return scene_id == scene.GetUID(); });
			(*iter).Destroy();
			scenes.erase(iter);
		}

		const Ref<Jigsaw::Machines::JigsawMachineObserver> JigsawScenesObserver::GetMachineObserver(const Jigsaw::System::UID& machine_id) {
			Ref<Jigsaw::Machines::JigsawMachineObserver> machine_observer;
			auto iter = std::find_if(scenes.begin(), scenes.end(), [&machine_observer, machine_id](JigsawScenesObserver::_SceneObserver& observer) { return observer.GetMachineObserver(machine_id, &machine_observer); });
			return machine_observer;
		}

		Jigsaw::Unique<Jigsaw::System::UID[]> JigsawScenesObserver::GetActiveSceneIds(size_t& out_size) const {
			out_size = scenes.size();
			Jigsaw::Unique<Jigsaw::System::UID[]> ids = Jigsaw::Unique<Jigsaw::System::UID[]>(new Jigsaw::System::UID[out_size]);
			int i = 0;
			std::for_each(scenes.begin(), scenes.end(), [&](const Jigsaw::Scene::JigsawScenesObserver::_SceneObserver& scene_observer) { ids.get()[i++] = scene_observer.GetUID(); });
			return std::move(ids);
		}

		JigsawScenesObserver::_SceneObserver::_JGSW_MachineObserver::_JGSW_MachineObserver(const Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>& machine, Jigsaw::Entities::JigsawEntityCluster& cluster)
			: JigsawMachineObserver(machine, cluster) { }

		void JigsawScenesObserver::_SceneObserver::_JGSW_MachineObserver::Init(Jigsaw::Machines::MULTI_FABRICATION_ARGS& machine_fabrication_data) {
			for (Jigsaw::Machines::FABRICATION_ARGS& entity_fabrication_args : machine_fabrication_data.entity_fabrication_args) {
				Jigsaw::Entities::JigsawEntity entity = machine->FabricateEntity(cluster, entity_fabrication_args);
				entity_ids.push_back(entity.GetUID());
			}
		}

		void JigsawScenesObserver::_SceneObserver::_JGSW_MachineObserver::Destroy() {
			for (Jigsaw::System::UID& id : entity_ids) {
				cluster.RemoveEntity(id);
			}
		}
	}
}

