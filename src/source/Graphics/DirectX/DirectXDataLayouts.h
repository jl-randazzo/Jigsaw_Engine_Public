#pragma once
#include <Directxmath.h>
#include "Math/LinAlg.h"

typedef struct _mvpMatrixConstant {
	Mat4x4 modelRotMatrix;
	Mat4x4 mvpMatrix;
} MVPMatrixConstant;

typedef struct _vertexPositionAndColorAndNormal {
	Vector3 position;
	Vector3 color;
	Vector3 normal;
} PositionColorNormal;

typedef struct _worldLight {
	int type;
	float intensity;
	float angle;
	const float padding = 0;

	Vector4 position;
	Vector4 direction;
	Vector4 color;
} WorldLight;

typedef struct _worldLightDefBlock {
	WorldLight lights[4];
} WorldLightDefBlock;
