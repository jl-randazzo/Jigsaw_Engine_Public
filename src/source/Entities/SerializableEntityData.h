#ifndef _SERIALIZABLE_ENTITY_DATA_H_
#define _SERIALIZABLE_ENTITY_DATA_H_

#include "JigsawEntity.h"
#include "System/UID.h"
#include "Util/StringVector.h"

namespace Jigsaw {
	namespace Entities {
		class SerializableEntityData {
		public:
			Jigsaw::System::UID entity_id;
			Jigsaw::Entities::JigsawEntity::SCOPE scope;
			std::vector<Jigsaw::Util::etype_index> aligned_types;
			Jigsaw::Util::StringVector string_vector;

		};
	}
}
#endif