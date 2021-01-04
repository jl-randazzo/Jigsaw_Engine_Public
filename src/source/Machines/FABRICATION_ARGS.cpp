#include "FABRICATION_ARGS.h"

namespace Jigsaw {
	namespace Machines {

		FABRICATION_ARGS& FABRICATION_ARGS::WithSerializedData(const Jigsaw::Entities::SerializableEntityData& entity_data) {
			FABRICATION_ARGS args;
			scope = entity_data.scope;
			entity_id = entity_data.entity_id;
			for (int i = 0; i < entity_data.aligned_types.size(); i++) {
				std::istringstream ss(entity_data.string_vector.strings.at(i));
				Jigsaw::Marshalling::JSONNodeReader node(ss, Jigsaw::Marshalling::MarshallingRegistry::GetMarshallingMap(entity_data.aligned_types.at(i).Get()));
				auto result = node.BuildNode();

				FABRICATION_VALUE_OVERRIDE override;
				override.raw_data = Jigsaw::Ref<BYTE[]>(static_cast<BYTE*>(result.raw_data));
				override.type_index = entity_data.aligned_types.at(i);
				value_overrides.push_back(override);
			}
			return *this;
		}

		FABRICATION_ARGS& FABRICATION_ARGS::WithScope(Jigsaw::Entities::JigsawEntity::SCOPE scope) {
			this->scope = scope;
			return *this;
		}

		bool FABRICATION_ARGS::GetData(const Jigsaw::Util::etype_info& t_info, BYTE* destination) {
			auto iter = std::find_if(value_overrides.begin(), value_overrides.end(), [&t_info](const FABRICATION_VALUE_OVERRIDE & override) { return override.type_index == t_info; });
			if (iter != value_overrides.end()) {
				t_info.GetUnsafeFunctions().Move(destination, iter->GetData());
				value_overrides.erase(iter);
				return true;
			}
			return false;
		}

		Jigsaw::System::UID FABRICATION_ARGS::GetEntityUID() const {
			return entity_id;
		}

		BYTE* FABRICATION_ARGS::FABRICATION_VALUE_OVERRIDE::GetData() {
			return raw_data.get();
		}
	}
}

