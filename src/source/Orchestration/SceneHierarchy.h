#ifndef _SCENE_CONTEXT_ORCHESTRATOR_H_
#define _SCENE_CONTEXT_ORCHESTRATOR_H_

#include "Graphics/RenderContext.h"
#include "System/THREAD_SAFE_SYSTEM_RESOURCES.h"
#include "Scene/JigsawScenesObserver.h"
#include "Scene/JigsawSceneLoader.h"
#include "System/UID.h"

#define POOL_SIZE 3

HRESULT Run();

namespace Jigsaw {
	namespace Orchestration {
		class SceneHierarchy {
			friend HRESULT Run();
		public:
			static Jigsaw::Ref<SceneHierarchy> Init(const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& render_context, 
				const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService>& cluster_service);

			~SceneHierarchy() {}
			
			/// <summary>
			/// Update should be called once per frame on the actual instance. If there are any completed scene load jobs, they will be processed
			/// and entered into the SceneHierarchy, spawning their initial entities.
			/// </summary>
			void Update();

			/// <summary>
			/// This is a privileged function only to be called by those who have the instance of the SceneHierarchy. 
			/// 
			/// As soon as the load job is completed, the JigsawScene will be activated in the SceneHierarchy.
			/// </summary>
			/// <param name="scene_id"></param>
			/// <returns></returns>
			void LoadAndActivateScene(const Jigsaw::System::UID& scene_id);

			/// <summary>
			/// Returns a temporary array of the active scene ids in the scene hierarchy
			/// </summary>
			/// <param name="out_size"></param>
			/// <returns></returns>
			static Jigsaw::Unique<Jigsaw::System::UID[]> GetActiveSceneIds(size_t& out_size);

			/// <summary>
			/// Scene load argument instructs the SceneHierarchy to load the JigsawScene and all associated JigsawMachines
			/// into the SceneHierarchy. An EVENT_LISTENER is returned that other processes can use to monitor the status
			/// of the load.
			/// 
			/// The contents of the scene will not be activated in the hierarchy until the next call to 'Update'
			/// </summary>
			/// <param name="scene_id"></param>
			/// <returns></returns>
			static Jigsaw::Ref<const Jigsaw::System::EVENT_LISTENER> LoadScene(const Jigsaw::System::UID& scene_id);

		protected:

			SceneHierarchy(const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& render_context, 
				const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService>& cluster_service);

			Jigsaw::Ref<Jigsaw::Graphics::RenderContext> render_context;
			Jigsaw::System::THREAD_SAFE_RESOURCE_POOL resource_pool;
			Jigsaw::Ref<Jigsaw::Scene::JigsawScenesObserver> scenes_observer;
			std::vector<Jigsaw::Ref<Jigsaw::System::ASYNC_JOB<Jigsaw::Scene::SCENE_LOAD_RESULT>>> active_load_jobs;

			static Jigsaw::Ref<SceneHierarchy> orchestrator;

		};
	}
}
#endif