#include "SerializableEntityData.h"
#include "Marshalling/JigsawMarshalling.h"

using namespace Jigsaw::System;
using namespace Jigsaw::Util;

namespace Jigsaw {
	namespace Entities {
		START_REGISTER_SERIALIZABLE_CLASS(SerializableEntityData)
		REGISTER_SERIALIZABLE_FIELD(SerializableEntityData, UID, entity_id)
		REGISTER_SERIALIZABLE_FIELD(SerializableEntityData, JigsawEntity::SCOPE, scope)
		REGISTER_SERIALIZABLE_VECTOR(SerializableEntityData, etype_index, aligned_types)
		REGISTER_SERIALIZABLE_FIELD(SerializableEntityData, StringVector, string_vector)
		END_REGISTER_SERIALIZABLE_CLASS(SerializableEntityData)
	}
}