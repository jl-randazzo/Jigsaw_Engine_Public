#ifndef _JIGSAW_SCENE_LOADER_H_
#define _JIGSAW_SCENE_LOADER_H_

#include "Ref.h"
#include "JigsawScene.h"
#include "System/THREAD_SAFE_SYSTEM_RESOURCES.h"
#include <future>
#include "Graphics/CommandListExecutor.h"
#include "System/ASYNC_JOB.h"

namespace Jigsaw {
	namespace Scene {
		/// <summary>
		/// Required arguments for a scene load operation. The THREAD_SAFE_RESOURCE_POOL must not be empty
		/// </summary>
		struct SCENE_LOAD_ARGS {
			Jigsaw::System::UID scene_id;
			Jigsaw::System::THREAD_SAFE_RESOURCE_POOL resource_pool;
		};

		/// <summary>
		/// A JigsawSceneLoader produces a ASYNC_JOB that delivers a SCENE_LOAD_RESULT
		/// </summary>
		struct SCENE_LOAD_RESULT {
			Jigsaw::Ref<JigsawScene> loaded_scene;
			Jigsaw::System::THREAD_SAFE_RESOURCE_POOL recycled_resources;
		};

		/// <summary>
		/// The JigsawSceneLoader manufactures a ASYNC_JOB that will eventually return a 'SCENE_LOAD_RESULT' 
		/// </summary>
		class JigsawSceneLoader {
		public:
			JigsawSceneLoader(SCENE_LOAD_ARGS&& args);

			/// <summary>
			/// This method can only be called one time. If you want to prepare a job for execution later, you may, but as soon as
			/// 'LoadScene' is called, the underlying JigsawSceneLoader will no longer be valid
			/// </summary>
			/// <returns></returns>
			Jigsaw::Ref<Jigsaw::System::ASYNC_JOB<SCENE_LOAD_RESULT>> LoadScene();

		private:
			/// <summary>
			/// Private class manufactures a ASYNC_JOB object that produces a SCENE_LOAD_RESULT
			/// </summary>
			class _SCENE_ASYNC_JOB : public Jigsaw::System::ASYNC_JOB<SCENE_LOAD_RESULT> {
			public:
				_SCENE_ASYNC_JOB(): ASYNC_JOB<SCENE_LOAD_RESULT>(Jigsaw::System::NOTIFY::OWNER_NOTIFY) {}
				/// <summary>
				/// Constructor has the load args moved in
				/// </summary>
				/// <param name="args"></param>
				_SCENE_ASYNC_JOB(SCENE_LOAD_ARGS&& args);

			protected:
				/// <summary>
				/// Overriden method from base class that is called internally to launch the asynchronous task
				/// </summary>
				/// <returns></returns>
				SCENE_LOAD_RESULT Execute() override;

			private:
				SCENE_LOAD_ARGS args;

			};

			SCENE_LOAD_ARGS args;
		};
	}
}
#endif // !_JIGSAW_SCENE_FACTORY_H_
