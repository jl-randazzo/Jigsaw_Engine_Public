#ifndef _SERIALIZABLE_MACHINE_ENTITY_DATA_H_
#define _SERIALIZABLE_MACHINE_ENTITY_DATA_H_

#include <vector>
#include "Entities/SerializableEntityData.h"

namespace Jigsaw {
	namespace Machines {
		class SerializableMachineData {
		public:
			std::vector<Jigsaw::Entities::SerializableEntityData> entity_data;

		};
	}
}
#endif