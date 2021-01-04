#include "THREAD_SAFE_SYSTEM_RESOURCES.h"
#include "Application/ApplicationRootProperties.h"

namespace Jigsaw {
	namespace System {
		THREAD_SAFE_RESOURCE_POOL& Jigsaw::System::THREAD_SAFE_RESOURCE_POOL::operator=(THREAD_SAFE_RESOURCE_POOL&& other) {
			resource_queue = std::move(other.resource_queue);
			this->cmd_list_exec = other.cmd_list_exec;
			return *this;
		}

		unsigned int Jigsaw::System::THREAD_SAFE_RESOURCE_POOL::PoolSize() {
			return this->resource_queue.size();
		}

		Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE THREAD_SAFE_RESOURCE_POOL::GetType() const {
			return type;
		}

		THREAD_SAFE_RESOURCE_POOL Jigsaw::System::THREAD_SAFE_RESOURCE_POOL::Split(unsigned int n)
		{
			std::queue<THREAD_SAFE_SYSTEM_RESOURCES> r_queue;
			for (int i = 0; i < n; i++) {
				r_queue.push(resource_queue.front());
				resource_queue.pop();
			}

			return THREAD_SAFE_RESOURCE_POOL(r_queue, cmd_list_exec, type);
		}

		void Jigsaw::System::THREAD_SAFE_RESOURCE_POOL::Merge(THREAD_SAFE_RESOURCE_POOL&& other) {
			while (other.PoolSize() != 0) {
				this->Enqueue(other.Get());
			}
			auto queue = other.filled_resources;
			while (!queue.empty()) {
				auto front = queue.front();
				filled_resources.push(front);
				queue.pop();
			}
		}

		THREAD_SAFE_SYSTEM_RESOURCES Jigsaw::System::THREAD_SAFE_RESOURCE_POOL::Get() {
			THREAD_SAFE_SYSTEM_RESOURCES sys_resources = resource_queue.front();
			resource_queue.pop();
			return sys_resources;
		}

		void Jigsaw::System::THREAD_SAFE_RESOURCE_POOL::Enqueue(THREAD_SAFE_SYSTEM_RESOURCES&& resources) {
			if (resources.cmd_list->HasCommands()) {
				filled_resources.push(resources);
			}
			else {
				resource_queue.push(resources);
			}
		}

		std::queue<THREAD_SAFE_SYSTEM_RESOURCES>& THREAD_SAFE_RESOURCE_POOL::GetFilledResources() {
			return filled_resources;
		}

		THREAD_SAFE_RESOURCE_POOL THREAD_SAFE_RESOURCE_PRODUCER::ProduceLoadResources(const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& context, unsigned int n) {
			THREAD_SAFE_RESOURCE_POOL pool(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE_LOAD);

			pool.cmd_list_exec = context->GetCommandListExecutor(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE_LOAD);

			for (int i = 0; i < n; i++) {
				THREAD_SAFE_SYSTEM_RESOURCES sys_resources;
				sys_resources.cmd_list = context->GetCommandList(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE_LOAD);
				sys_resources.db = Jigsaw::MakeRef<Jigsaw::Assets::AssetDatabaseReader>(ApplicationRootProperties::Get().db_connection.c_str());
				pool.Enqueue(std::move(sys_resources));
			}

			return pool;
		}

		void THREAD_SAFE_RESOURCE_PRODUCER::RecycleResources(THREAD_SAFE_RESOURCE_POOL& resource_pool, const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& context) {
			auto type = resource_pool.GetType();

			std::queue<THREAD_SAFE_SYSTEM_RESOURCES> f_resource_queue = resource_pool.GetFilledResources();
			while (!f_resource_queue.empty()) {
				THREAD_SAFE_SYSTEM_RESOURCES sys_resources = f_resource_queue.front();
				f_resource_queue.pop();
				sys_resources.cmd_list = context->GetCommandList(type);
				resource_pool.Enqueue(std::move(sys_resources));
			}
		}

	}
}
