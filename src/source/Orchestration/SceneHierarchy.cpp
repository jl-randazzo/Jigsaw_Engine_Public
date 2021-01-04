#include "SceneHierarchy.h"
#include "Application/ApplicationRootProperties.h"
#include "Scene/JigsawSceneLoader.h"

namespace Jigsaw {
	namespace Orchestration {
		Jigsaw::Ref<SceneHierarchy> SceneHierarchy::orchestrator;
		
		void SceneHierarchy::LoadAndActivateScene(const Jigsaw::System::UID& scene_id) {

			Jigsaw::Scene::SCENE_LOAD_ARGS args;
			args.resource_pool = resource_pool.Split(resource_pool.PoolSize());
			args.scene_id = scene_id;

			Jigsaw::Scene::JigsawSceneLoader scene_loader(std::move(args));
			Jigsaw::Ref<Jigsaw::System::ASYNC_JOB<Jigsaw::Scene::SCENE_LOAD_RESULT>> job = scene_loader.LoadScene();
			Jigsaw::Scene::SCENE_LOAD_RESULT result = job->Await();
			job->GetEvent().Notify();

			Jigsaw::System::THREAD_SAFE_RESOURCE_PRODUCER::RecycleResources(result.recycled_resources, render_context);
			resource_pool.Merge(std::move(result.recycled_resources));
			scenes_observer->ActivateScene(result.loaded_scene);
		}


		Jigsaw::Unique<Jigsaw::System::UID[]> SceneHierarchy::GetActiveSceneIds(size_t& out_size) {
			return orchestrator->scenes_observer->GetActiveSceneIds(out_size);
		}

		Jigsaw::Ref<const Jigsaw::System::EVENT_LISTENER> SceneHierarchy::LoadScene(const Jigsaw::System::UID& scene_id) {
			Jigsaw::Scene::SCENE_LOAD_ARGS args;
			args.resource_pool = orchestrator->resource_pool.Split(orchestrator->resource_pool.PoolSize());
			args.scene_id = scene_id;

			Jigsaw::Scene::JigsawSceneLoader scene_loader(std::move(args));
			Jigsaw::Ref<Jigsaw::System::ASYNC_JOB<Jigsaw::Scene::SCENE_LOAD_RESULT>> job = scene_loader.LoadScene();
			orchestrator->active_load_jobs.push_back(job);
			return job->GetListener();
		}

		SceneHierarchy::SceneHierarchy(const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& render_context, const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService>& cluster_service)
			: render_context(render_context), resource_pool(Jigsaw::System::THREAD_SAFE_RESOURCE_PRODUCER::ProduceLoadResources(render_context, POOL_SIZE)),
			scenes_observer(Jigsaw::MakeRef<Jigsaw::Scene::JigsawScenesObserver>(cluster_service)) { }

		Jigsaw::Ref<SceneHierarchy> SceneHierarchy::Init(const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& render_context, const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService>& cluster_service) {
			orchestrator = Jigsaw::Ref<SceneHierarchy>(new SceneHierarchy(render_context, cluster_service));
			return orchestrator;
		}

		void SceneHierarchy::Update()
		{
			auto iter = active_load_jobs.begin();
			while (iter != active_load_jobs.end()) {
				Jigsaw::System::ASYNC_JOB<Jigsaw::Scene::SCENE_LOAD_RESULT>& job = **iter;
				if (job.Ready()) {
					// activate the resulting scene from the scene load job if it is ready
					Jigsaw::Scene::SCENE_LOAD_RESULT result = job.Get();
					Jigsaw::System::THREAD_SAFE_RESOURCE_PRODUCER::RecycleResources(result.recycled_resources, render_context);
					resource_pool.Merge(std::move(result.recycled_resources));
					scenes_observer->ActivateScene(result.loaded_scene);

					iter = active_load_jobs.erase(iter);
					job.GetEvent().Notify();
				}
				else {
					// otherwise, we can examine the next active load job
					iter++;
				}
			}
		}
	}
}

