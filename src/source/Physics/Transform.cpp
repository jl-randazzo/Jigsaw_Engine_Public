#include "Physics/Transform.h"
#include "Marshalling/JigsawMarshalling.h"

Transform& Transform::operator=(const Transform& other) {
	position = other.position;
	scale = other.scale;
	rotation = other.rotation;
	return *this;
}

bool Transform::operator==(const Transform& other) {
	return scale == other.scale && position == other.position && rotation == other.rotation;
}

Mat4x4 Transform::getRotationMatrix() const {
	float w = rotation.w;
	float w_squared = rotation.w * rotation.w;
	float a = rotation.ijk.x;
	float b = rotation.ijk.y;
	float c = rotation.ijk.z;
	return Mat4x4(1 - 2 * b * b - 2 * c * c, 2 * a * b - 2 * w * c, 2 * a * c + 2 * w * b, 0,
		2 * a * b + 2 * w * c, 1 - 2 * a * a - 2 * c * c, 2 * b * c - 2 * a * w, 0,
		2 * a * c - 2 * b * w, 2 * b * c + 2 * a * w, 1 - 2 * a * a - 2 * b * b, 0,
		0, 0, 0, 1);
}

Mat4x4 Transform::getTRSMatrix() const {
	Mat4x4 translation
	(1, 0, 0, position.x,
		0, 1, 0, position.y,
		0, 0, 1, position.z,
		0, 0, 0, 1);

	Mat4x4 rotation = getRotationMatrix();

	Mat4x4 scale(scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1);

	return translation * rotation* scale;
}

START_REGISTER_SERIALIZABLE_CLASS(Transform)
REGISTER_SERIALIZABLE_FIELD(Transform, Vector3, position)
REGISTER_SERIALIZABLE_FIELD(Transform, Vector3, scale)
REGISTER_SERIALIZABLE_FIELD(Transform, Quaternion, rotation)
END_REGISTER_SERIALIZABLE_CLASS(Transform)
