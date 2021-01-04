#include "Math/LinAlg.h"
#include "Marshalling/JigsawMarshalling.h"

START_REGISTER_SERIALIZABLE_CLASS(Vector3)
REGISTER_SERIALIZABLE_FIELD(Vector3, float, x)
REGISTER_SERIALIZABLE_FIELD(Vector3, float, y)
REGISTER_SERIALIZABLE_FIELD(Vector3, float, z)
END_REGISTER_SERIALIZABLE_CLASS(Vector3)

START_REGISTER_SERIALIZABLE_CLASS(Quaternion)
REGISTER_SERIALIZABLE_FIELD(Quaternion, Vector3, ijk)
REGISTER_SERIALIZABLE_FIELD(Quaternion, float, w)
END_REGISTER_SERIALIZABLE_CLASS(Quaternion)