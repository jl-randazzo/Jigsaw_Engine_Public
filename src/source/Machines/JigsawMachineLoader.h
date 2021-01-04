#ifndef _JIGSAW_MACHINE_LOADER_H_
#define _JIGSAW_MACHINE_LOADER_H_

#include "Assets/ASSET_LOAD_RESULT.h"
#include "System/THREAD_SAFE_SYSTEM_RESOURCES.h"
#include "Graphics/CommandListExecutor.h"
#include "Ref.h"
#include "JigsawMachine.h"
#include "System/ASYNC_JOB.h"
#include "Assets/DataAssets.h"

namespace Jigsaw {
	namespace Machines {
		/// <summary>
		/// Arguments needed to launch a JigsawMachineLoader. resource_pool must not be empty
		/// </summary>
		struct MACHINE_LOAD_ARGS {
			Jigsaw::Unique<Jigsaw::System::UID[]> machine_ids;
			size_t count;
			Jigsaw::System::THREAD_SAFE_RESOURCE_POOL resource_pool;
		};

		/// <summary>
		/// The result produced contains a Unique array of SerializedRef<JigsawMachine> objects. The output 'count'
		/// will match the input 'count'
		/// </summary>
		struct MACHINE_LOAD_RESULT {
			Jigsaw::Unique<Jigsaw::Assets::SerializedRef<JigsawMachine>[]> jgsw_machines;
			size_t count;
			Jigsaw::System::THREAD_SAFE_RESOURCE_POOL recycled_resources;
		};

		/// <summary>
		/// The JigsawMachineLoader launches a ASYNC_JOB that produces a MACHINE_LOAD_RESULT upon completion
		/// </summary>
		class JigsawMachineLoader {
		public:
			JigsawMachineLoader(MACHINE_LOAD_ARGS&& args) : args(std::move(args)) {}

			/// <summary>
			/// Once the call to LoadMachine is made, the underlying JigsawMachineLoader is no longer valid.
			/// </summary>
			/// <returns></returns>
			Jigsaw::Ref<Jigsaw::System::ASYNC_JOB<MACHINE_LOAD_RESULT>> LoadMachine();

		private:
			/// <summary>
			/// Private class used to override the 'Load' action of the ASYNC_JOB
			/// </summary>
			class _MACHINE_ASYNC_JOB : public Jigsaw::System::ASYNC_JOB<MACHINE_LOAD_RESULT> {
			public:
				_MACHINE_ASYNC_JOB(MACHINE_LOAD_ARGS&& args) : args(std::move(args)), ASYNC_JOB() {}

			protected:
				MACHINE_LOAD_RESULT Execute() override;

			private:
				MACHINE_LOAD_ARGS args;

			};

			MACHINE_LOAD_ARGS args;
		};
	}
}
#endif // !_JIGSAW_MACHINE_LOADER_H_
