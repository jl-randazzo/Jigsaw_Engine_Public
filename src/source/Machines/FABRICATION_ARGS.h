#ifndef _FABRICATION_ARGS_H_
#define _FABRICATION_ARGS_H_

#include "Util/etype_info.h"
#include "Machines/SerializableMachineData.h"
#include "Debug/j_debug.h"
#include "Marshalling/JSONNode.h"
#include "Marshalling/MarshallingRegistry.h"
#include "Entities/JigsawEntity.h"

namespace Jigsaw {
	namespace Machines {
		/// <summary>
		/// FABRICATION_ARGS are created to override some of the values associated with a given JigsawEntity fabrication
		/// operation. Values can be overriden with either Serialized Data or with a direct Rvalue object override. 
		/// 
		/// The JigsawMachineObserver will validate the passed in parameters at the time of JigsawEntity fabrication. 
		/// In Debug mode, an exception should be thrown. In release code, invalid parameters yield undefined behavior. 
		/// </summary>
		class FABRICATION_ARGS {
		public:
			FABRICATION_ARGS() : scope(Jigsaw::Entities::JigsawEntity::SCOPE_EPHEMERAL) {}

			/// <summary>
			/// Pushes a value override object associated with the FABRICATION_ARGS. If an object of the same type is passed,
			/// then the already-allocated memory will be overwritten
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <param name="value"></param>
			/// <returns></returns>
			template<typename T>
			FABRICATION_ARGS& WithValue(T&& value) {

				const Jigsaw::Util::etype_info& t_info = Jigsaw::Util::etype_info::Id<T>();
				auto iter = std::find_if(value_overrides.begin(), value_overrides.end(), [&t_info](FABRICATION_VALUE_OVERRIDE& ovrd) { return ovrd.type_index == t_info; });
				if (iter == value_overrides.end()) {
					FABRICATION_VALUE_OVERRIDE value_override;
					value_override.raw_data = Jigsaw::Ref<BYTE[]>(new BYTE[t_info.size]);
					t_info.GetUnsafeFunctions().Move(value_override.raw_data.get(), static_cast<BYTE*>((void*)&value));
					value_override.type_index = t_info;
					value_overrides.push_back(value_override);
				}
				else {
					FABRICATION_VALUE_OVERRIDE & override = *iter;
					t_info.GetUnsafeFunctions().Move(override.raw_data.get(), static_cast<BYTE*>((void*)&value));
				}

				return *this;
			}

			// this implementation may need to change
			FABRICATION_ARGS& WithSerializedData(const Jigsaw::Entities::SerializableEntityData& entity_data);

			/// <summary>
			/// Adds the given 'scope' to the FABRICATION_ARGS
			/// </summary>
			/// <param name="scope"></param>
			/// <returns></returns>
			FABRICATION_ARGS& WithScope(Jigsaw::Entities::JigsawEntity::SCOPE scope);

			/// <summary>
			/// Populates the 'destination' with the relevant type data if the relevant type is present in the
			/// FABRICATION_VALUE_OVERRIDE vector
			/// </summary>
			/// <param name="t_info"></param>
			/// <param name="destination"></param>
			/// <returns>True if the passed in 't_info' object is represented in the fabrication data</returns>
			bool GetData(const Jigsaw::Util::etype_info& t_info, BYTE* destination);

			/// <summary>
			/// Return the entity id to be fabricated
			/// </summary>
			/// <returns></returns>
			Jigsaw::System::UID GetEntityUID() const;

		private:

			struct FABRICATION_VALUE_OVERRIDE {
				Jigsaw::Util::etype_index type_index;
				Jigsaw::Ref<BYTE[]> raw_data;

				BYTE* GetData();
			};

			std::vector<FABRICATION_VALUE_OVERRIDE> value_overrides;
			Jigsaw::Entities::JigsawEntity::JigsawEntity::SCOPE scope;
			Jigsaw::System::UID entity_id;

		};

		/// <summary>
		/// This simple wrapper holds multiple FABRICATION_ARGS instances for use with a single machine. 
		/// </summary>
		class MULTI_FABRICATION_ARGS {
		public:
			std::vector<FABRICATION_ARGS> entity_fabrication_args;

		};

	}
}
#endif