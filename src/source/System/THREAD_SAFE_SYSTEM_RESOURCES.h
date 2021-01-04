#ifndef _THREAD_SAFE_SYSTEM_RESOURCES_H_
#define _THREAD_SAFE_SYSTEM_RESOURCES_H_
#include "Assets/AssetDatabase.h"
#include "Ref.h"
#include "d3d12.h"
#include "Graphics/CommandListExecutor.h"
#include "Graphics/CommandList.h"
#include "Graphics/RenderContext.h"
#include <queue>

namespace Jigsaw {
	namespace System {

		/// <summary>
		/// Global Asset Managment functions such as FetchAssets require a set of system resource that have been allocated specifically to the calling thread.
		/// The Asset loader can liberally manage the internal resources as needed to resolve the requested Assets. 
		/// </summary>
		struct THREAD_SAFE_SYSTEM_RESOURCES {
			Jigsaw::Ref<Jigsaw::Graphics::CommandList> cmd_list;
			Jigsaw::Ref<Jigsaw::Assets::AssetDatabaseReader> db;
		};

		/// <summary>
		/// 
		/// </summary>
		class THREAD_SAFE_RESOURCE_POOL {
		public:
			/// <summary>
			/// Basic constructor
			/// </summary>
			THREAD_SAFE_RESOURCE_POOL(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE type = Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE_LOAD) : type(type) {}

			/// <summary>
			/// Move constructor
			/// </summary>
			/// <param name="other"></param>
			THREAD_SAFE_RESOURCE_POOL(THREAD_SAFE_RESOURCE_POOL&& other) : resource_queue(std::move(other.resource_queue)), type(other.type), cmd_list_exec(other.cmd_list_exec) {}

			/// <summary>
			/// Move constructor. THREAD_SAFE_RESOURCE_POOLS need to be moved, not copied. 
			/// </summary>
			THREAD_SAFE_RESOURCE_POOL& operator=(THREAD_SAFE_RESOURCE_POOL&& other);

			/// <summary>
			/// Returns the number of elements currently in the resource pool. 
			/// </summary>
			/// <returns></returns>
			unsigned int PoolSize();

			/// <summary>
			/// Returns the type of this resource pool
			/// </summary>
			/// <returns></returns>
			Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE GetType() const;

			/// <summary>
			/// Generates a new a pool with 'n' resources, if they are available. If they are not available, returns as many
			/// as are available. If none are available, the behavior is undefined.
			/// </summary>
			/// <param name="n"></param>
			/// <returns></returns>
			THREAD_SAFE_RESOURCE_POOL Split(unsigned int n);

			/// <summary>
			/// Merges the resources of 'other' with this THREAD_SAFE_RESOURCE_POOL
			/// </summary>
			/// <param name="other"></param>
			void Merge(THREAD_SAFE_RESOURCE_POOL&& other);

			/// <summary>
			/// Dequeues and returns one THREAD_SAFE_SYSTEM_RESOURCES instance for use in the calling thread.
			/// </summary>
			/// <returns></returns>
			THREAD_SAFE_SYSTEM_RESOURCES Get();

			/// <summary>
			/// Enqueues the given 'resources' object. It is assumed that the resources submitted are still in a valid state.
			/// </summary>
			void Enqueue(THREAD_SAFE_SYSTEM_RESOURCES&& resources);

			/// <summary>
			/// Returns a queue of the resources whose command lists have been populated
			/// </summary>
			/// <returns></returns>
			std::queue<THREAD_SAFE_SYSTEM_RESOURCES>& GetFilledResources();

			Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor> cmd_list_exec;

		private:
			/// <summary>
			/// Internal constructor is used for split operations
			/// </summary>
			THREAD_SAFE_RESOURCE_POOL(std::queue<THREAD_SAFE_SYSTEM_RESOURCES> resource_queue, const Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor>& cmd_list_exec, Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE type) : resource_queue(resource_queue), cmd_list_exec(cmd_list_exec), type(type) {}

			Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE type;
			std::queue<THREAD_SAFE_SYSTEM_RESOURCES> resource_queue;
			std::queue<THREAD_SAFE_SYSTEM_RESOURCES> filled_resources;

		};

		class THREAD_SAFE_RESOURCE_PRODUCER {
		public:
			static THREAD_SAFE_RESOURCE_POOL ProduceLoadResources(const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& context, unsigned int n);
			static void RecycleResources(THREAD_SAFE_RESOURCE_POOL& resource_pool, const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& context);
		};
	}
}

#endif