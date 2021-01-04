#define _USE_MATH_DEFINES
#include "Camera.h"

Mat4x4 Camera::GetRotationMatrix() const {
	Vector3 up(0, 0, 1);
	Vector3 tangent = viewing_dir.Cross(up).Normalized();
	Vector3 bi_tangent = tangent.Cross(viewing_dir).Normalized();

	return Mat4x4(Vector4::AsVector(-1 * tangent), Vector4::AsVector(-1 * bi_tangent), (-1) * Vector4::AsVector(viewing_dir), Vector4(0, 0, 0, 1));
}

Mat4x4 Camera::GetTranslationMatrix() const {
	return Mat4x4(Vector4(1, 0, 0, -position.x), Vector4(0, 1, 0, -position.y), Vector4(0, 0, 1, -position.z), Vector4(0, 0, 0, 1));
}

Mat4x4 Camera::GetViewMatrix() const {
	return GetRotationMatrix() * GetTranslationMatrix();
}

Mat4x4 Camera::GetProjectionMatrix(const float wh_ratio) const {
	float x_fov_rad = x_fov * 2 * M_PI / 360;
	float x_fov_2 = x_fov_rad / 2;
	float cot = cosf(x_fov_2) / sinf(x_fov_2);
	float cot_y = cot * wh_ratio;
	float a = (near_plane + far_plane) / (far_plane - near_plane);
	float b = (2 * far_plane * near_plane) / (far_plane - near_plane);

	return Mat4x4(cot, 0, 0, 0,
		0, cot_y, 0, 0,
		0, 0, a, b,
		0, 0, 1, 0);
}