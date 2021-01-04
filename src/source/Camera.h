#pragma once
#include "Graphics/DirectX/DX_Context.h"
#include "Graphics/DirectX/DirectXDataLayouts.h"
#include <DirectXMath.h>
#include "Math/LinAlg.h"

#define _NEAR 1.f
#define _FAR 100000.f

class Camera {

public:
	Vector3 viewing_dir;
	Vector3 position;
	float x_fov;
	float near_plane;
	float far_plane;

public:
	Camera(Vector3 position, Vector3 viewing_dir) : viewing_dir(viewing_dir), position(position) {
		near_plane = _NEAR;
		far_plane = _FAR;
		x_fov = 90;
	}

	Mat4x4 GetRotationMatrix() const;
	Mat4x4 GetTranslationMatrix() const;
	Mat4x4 GetViewMatrix() const;
	Mat4x4 GetProjectionMatrix(const float wh_ratio) const;

private:

};
