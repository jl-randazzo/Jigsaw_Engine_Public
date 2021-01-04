#include "SerializableMachineData.h"
#include "Marshalling/JigsawMarshalling.h"

using namespace Jigsaw::Entities;

namespace Jigsaw {
	namespace Machines {
		START_REGISTER_SERIALIZABLE_CLASS(SerializableMachineData)
		REGISTER_SERIALIZABLE_VECTOR(SerializableMachineData, SerializableEntityData, entity_data)
		END_REGISTER_SERIALIZABLE_CLASS(SerializableMachineData)
	}
}