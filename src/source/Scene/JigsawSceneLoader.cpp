#include "JigsawSceneLoader.h"
#include "Assets/AssetManagement.h"
#include "Machines/JigsawMachineLoader.h"
#include "Debug/j_log.h"
#include "Application/ApplicationRootProperties.h"
#include "Marshalling/MarshallingUtil.h"

using namespace Jigsaw::Scene;
using namespace Jigsaw::Machines;
using namespace Jigsaw::System;
using namespace Jigsaw::Assets;

Jigsaw::Scene::JigsawSceneLoader::JigsawSceneLoader(SCENE_LOAD_ARGS&& args) : args(std::move(args)) { }

Jigsaw::Ref<Jigsaw::System::ASYNC_JOB<SCENE_LOAD_RESULT>> Jigsaw::Scene::JigsawSceneLoader::LoadScene() {
	return Jigsaw::Ref<ASYNC_JOB<SCENE_LOAD_RESULT>>(new _SCENE_ASYNC_JOB(std::move(args)));
}


Jigsaw::Scene::JigsawSceneLoader::_SCENE_ASYNC_JOB::_SCENE_ASYNC_JOB(SCENE_LOAD_ARGS&& args) : args(std::move(args)) { }

std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS> LoadFabricationArgs(const Jigsaw::Unique<Jigsaw::System::UID[]>& ids, const size_t count) {

	std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS> fabrication_args;
	Jigsaw::System::UID* ids_ = ids.get();
	for (int i = 0; i < count; i++) {
		
		// building the file path using the ApplicationRootProperties
		Jigsaw::File_IO::FILE_DATA file_data;
		std::string id_str = Jigsaw::System::UIDToString(ids_[i]);
		std::string full_path = ApplicationRootProperties::Get().lib_path + id_str.substr(0, 2) + '/' + id_str;

		if (Jigsaw::File_IO::GetFileIfExists(full_path, &file_data)) {

			// building the fabrication args associated with this machine
			Jigsaw::Machines::MULTI_FABRICATION_ARGS fab_args;
			Jigsaw::Ref<Jigsaw::Machines::SerializableMachineData> machine_data = Jigsaw::Marshalling::MarshallingUtil::Unmarshal<Jigsaw::Machines::SerializableMachineData>(file_data);
			std::fclose(file_data.file);

			// loading all of the fabrication args for these entities
			for (Jigsaw::Entities::SerializableEntityData& e_data : machine_data->entity_data) {
				fab_args.entity_fabrication_args.push_back(Jigsaw::Machines::FABRICATION_ARGS().WithSerializedData(e_data));
			}

			fabrication_args.insert(std::make_pair(ids_[i], fab_args));
		}
	}

	return fabrication_args;
}

SCENE_LOAD_RESULT Jigsaw::Scene::JigsawSceneLoader::_SCENE_ASYNC_JOB::Execute() {
	J_LOG_INFO(JigsawSceneLoader, "Thread dispatched to load jigsaw scene" );

	THREAD_SAFE_SYSTEM_RESOURCES resources = args.resource_pool.Get();
	size_t machine_count = 0;
	Unique<UID[]> machine_ids = resources.db->FetchSceneToMachineAssociations(args.scene_id, &machine_count);

	J_LOG_INFO(JigsawSceneLoader, "Loading {0:d} machines", machine_count);

	args.resource_pool.Enqueue(std::move(resources));

	std::vector<SerializedRef<JigsawMachine>> machines;

	size_t pool_c = args.resource_pool.PoolSize();
	unsigned int machines_per_tsr = machine_count / pool_c;
	unsigned int remainder = machine_count % pool_c;

	// initiate the load jobs
	std::vector<Ref<ASYNC_JOB<MACHINE_LOAD_RESULT>>> load_jobs;
	unsigned int running_count = 0;
	for (int i = 0; i < pool_c && running_count < machine_count; i++) {
		unsigned int c = i < remainder ? machines_per_tsr + 1 : machines_per_tsr;

		MACHINE_LOAD_ARGS load_args = { UniqueSplit(machine_ids, running_count, c), c, args.resource_pool.Split(1) };
		JigsawMachineLoader machine_loader(std::move(load_args));
		running_count += c;
		load_jobs.push_back(machine_loader.LoadMachine());
	}

	// while the machine load jobs are going, fetch the fabrication data associated with each of the machines
	std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS> fab_args = LoadFabricationArgs(machine_ids, machine_count);

	// collect the completed load jobs
	SCENE_LOAD_RESULT result;
	for (int i = 0; i < load_jobs.size(); i++){
		while (!load_jobs.at(i)->Ready()) {}
		MACHINE_LOAD_RESULT complete = load_jobs.at(i)->Get();
		for (int j = 0; j < complete.count; j++) {
			machines.push_back(complete.jgsw_machines.get()[j]);
		}

		result.recycled_resources.Merge(std::move(complete.recycled_resources));
	}

	// recycle resources and await command list execution
	{
		args.resource_pool.cmd_list_exec->SignalAndWait();
	}

	// populated the laoded scene in the result
	result.loaded_scene = Ref<JigsawScene>(new JigsawScene(args.scene_id, std::move(fab_args), std::move(machines)));

	return result;
}
