#pragma once
#include "Math/LinAlg.h"

class Transform {

public:
	Vector3 position;
	Vector3 scale;
	Quaternion rotation;

	Transform() {
		scale = Vector3(1, 1, 1);
		position = Vector3(0, 0, 0);
	}

	Transform& operator=(const Transform& other);

	bool operator==(const Transform& other);

	Mat4x4 getRotationMatrix() const;
	Mat4x4 getTRSMatrix() const;

private:
	Transform* parent;

};

