#include "Marshalling/JigsawMarshalling.h"
#include "Util/StringVector.h"

namespace Jigsaw {
	namespace Util {
		START_REGISTER_SERIALIZABLE_CLASS(StringVector)
		REGISTER_SERIALIZABLE_VECTOR(StringVector, std::string, strings)
		END_REGISTER_SERIALIZABLE_CLASS(StringVector)
	}
}