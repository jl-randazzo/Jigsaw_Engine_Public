#ifndef _JIGSAW_SCENES_OBSERVER_H_
#define _JIGSAW_SCENES_OBSERVER_H_

#include "Ref.h"
#include "Scene/JigsawScene.h"
#include "Entities/JigsawEntityClusterService.h"
#include "Machines/JigsawMachineObserver.h"
#include "System/UID.h"
#include <map>

namespace Jigsaw {
	namespace Scene {
		/// <summary>
		/// JigsawScenesObserver maintains all of the scenes in the world hierarchy and provides an interface for activating them, 
		/// deactivating them, and accessing JigsawMachineObservers within the active world hierarchy. 
		/// </summary>
		class JigsawScenesObserver {
		public:
			JigsawScenesObserver(const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService>& cluster_service)
				: cluster_service(cluster_service) {}

			/// <summary>
			/// Called from the root orchestration context when a new scene is available for activation
			/// </summary>
			/// <param name="scene"></param>
			void ActivateScene(const Jigsaw::Ref<Jigsaw::Scene::JigsawScene>& scene);

			/// <summary>
			/// Called from the root orchestration context when a new scene is ready to be deactivated
			/// </summary>
			/// <param name="scene_id"></param>
			void DeactivateScene(const Jigsaw::System::UID& scene_id);

			/// <summary>
			/// </summary>
			/// <param name="machine_id"></param>
			/// <returns></returns>
			const Jigsaw::Ref<Jigsaw::Machines::JigsawMachineObserver> GetMachineObserver(const Jigsaw::System::UID& machine_id);

			/// <summary>
			/// Returns a temporary array of the active scene ids in the scene hierarchy
			/// </summary>
			/// <param name="out_size"></param>
			/// <returns></returns>
			Jigsaw::Unique<Jigsaw::System::UID[]> GetActiveSceneIds(size_t& out_size) const;

		private:
			/// <summary>
			/// An internal implementation that allows the JigsawScenesObserver to observe, augment, and serialize an individual JigsawScene 
			/// object active in the World
			/// </summary>
			class _SceneObserver {
			public:
				_SceneObserver(const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService>& cluster_service, const Jigsaw::Ref<Jigsaw::Scene::JigsawScene>& scene);
				_SceneObserver(const _SceneObserver& other);
				_SceneObserver(_SceneObserver&& other);

				_SceneObserver& operator=(const _SceneObserver& other);
				_SceneObserver& operator=(_SceneObserver&& other);

				/// <summary>
				/// Awakens the underlying JigsawScene object, activating all of the JigsawMachines in the World hierarchy,
				/// and Fabricating all of their serialized JigsawEntities 
				/// </summary>
				void Awake();

				/// <summary>
				/// Removes the underlying JigsawScene object from the World hierarchy, serializing all of the active, non-ephemeral JigsawEntities
				/// in the process and 
				/// </summary>
				void Destroy();

				const Jigsaw::System::UID& GetUID() const;

				bool GetMachineObserver(const Jigsaw::System::UID& machine_id, Jigsaw::Ref<Jigsaw::Machines::JigsawMachineObserver>* observer_out);

			private:
				/// <summary>
				/// This internal, private class is used for additional Machine observation functions that should really only be done in context
				/// of scene observation such as initialization or destruction. 
				/// </summary>
				class _JGSW_MachineObserver : public Jigsaw::Machines::JigsawMachineObserver {
				public:
					_JGSW_MachineObserver(const Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>& machine, Jigsaw::Entities::JigsawEntityCluster& cluster);

					~_JGSW_MachineObserver() override {}

					void Init(Jigsaw::Machines::MULTI_FABRICATION_ARGS& machine_fabrication_data);

					void Destroy();

				};

				Jigsaw::Ref<Jigsaw::Scene::JigsawScene> scene;
				Jigsaw::System::UID scene_id;
				Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> cluster_service;
				std::map<const Jigsaw::System::UID, Jigsaw::Ref<Jigsaw::Machines::JigsawMachineObserver>> jgsw_machines;

			};

			std::vector<_SceneObserver> scenes;
			const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> cluster_service;

		};
	}
}
#endif