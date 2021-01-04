#include "JigsawMachineLoader.h"

#include "Assets/AssetManagement.h"
#include "Debug/j_log.h"

using namespace Jigsaw::Assets;
using namespace Jigsaw::System;

namespace Jigsaw {
	namespace Machines {
		MACHINE_LOAD_RESULT Jigsaw::Machines::JigsawMachineLoader::_MACHINE_ASYNC_JOB::Execute() {
			J_LOG_INFO(JigsawMachineLoader, "Dispatching thread to load machines");
			MACHINE_LOAD_RESULT result;

			THREAD_SAFE_SYSTEM_RESOURCES resources = args.resource_pool.Get();
			Unique<Ref<DataAsset>[]> machines = FetchAssets(args.machine_ids, args.count, resources);
			Ref<DataAsset>* machines_ = machines.get();

			result.count = args.count;
			J_LOG_INFO(JigsawMachineLoader, "Loading {0:d} machines", result.count);
			result.jgsw_machines = Unique<SerializedRef<JigsawMachine>[]>(new SerializedRef<JigsawMachine>[result.count]);
			for (int i = 0; i < args.count; i++) {
				Ref<JsonAsset> json_asset = std::dynamic_pointer_cast<JsonAsset>(machines_[i]);
				result.jgsw_machines[i] = SerializedRef<JigsawMachine>(json_asset);
			}

			if (resources.cmd_list->HasCommands()) {
				args.resource_pool.cmd_list_exec->SubmitCommandList(resources.cmd_list);
			}

			result.recycled_resources.Enqueue(std::move(resources));

			return result;
		}

		Jigsaw::Ref<ASYNC_JOB<MACHINE_LOAD_RESULT>> Jigsaw::Machines::JigsawMachineLoader::LoadMachine() {
			return Jigsaw::Ref<ASYNC_JOB<MACHINE_LOAD_RESULT>>(new _MACHINE_ASYNC_JOB(std::move(args)));
		}
	}
}
