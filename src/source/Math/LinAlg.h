#pragma once
#define SAFE_DEBUG

#include <Directxmath.h>
#include <iostream>
#include <stdio.h>
#include <Windows.h>

/**
* basic 3-component Vector class implementation
*/
class Vector3 {
public:
	constexpr Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) { }
	constexpr Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) { }
	constexpr Vector3() : x(0), y(0), z(0) { }

	constexpr static Vector3 Z(){ return Vector3(0, 0, 1); }

	operator DirectX::XMFLOAT3() {
		return DirectX::XMFLOAT3(x, y, z);
	}

	inline float Dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	inline Vector3 Cross(const Vector3& other) const {
		return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	inline float GetLength() const {
		return sqrt(x * x + y * y + z * z);
	}

	inline Vector3 Normalized() const {
		float l = GetLength();
		if (l == 0) {
			return *this;
		}
		return Vector3(x / l, y / l, z / l);
	}

	Vector3& operator=(const Vector3& other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	Vector3& operator*=(const float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	float x;
	float y;
	float z;
};

/**
* Primarily used with matrices. Encodes whether the vector is a point or a vector using the fourth value ('1' for point, '0' for vector)
*/
class Vector4 {
public:
	constexpr Vector4() : x(0), y(0), z(0), w(0) { }
	constexpr Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
	constexpr Vector4(const Vector3& vector, float w) : x(vector.x), y(vector.y), z(vector.z), w(w) { }

	static Vector4 AsPoint(const Vector3& v) {
		return Vector4(v, 1);
	}
	static Vector4 AsVector(const Vector3& v) { 
		return Vector4(v, 0);
	}

	inline float Dot(const Vector4& v) const {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	// basic conversion just drops the fourth component
	operator Vector3() {
		return Vector3(x, y, z);
	}

	float x;
	float y;
	float z;
	float w;
};

class Vector2 {
public:
	Vector2(const Vector2& other) : x(other.x), y(other.y) { }
	constexpr Vector2(const float x, const float y) : x(x), y(y) { }
	constexpr Vector2() : x(0), y(0) { }
	constexpr Vector2& operator=(const Vector2& other) {
		x = other.x;
		y = other.y;
		return *this;
	}

	// conversion
	operator Vector3() { 
		return Vector3(this->x, this->y, 0);
	}

	inline float Dot(const Vector2& other) const {
		return x * other.x + y * other.y;
	}

	float x;
	float y;
};

// Vector-related operator overloads

/**
* Simple implementation. A more particular one implementing deltas may be added later
*/
constexpr bool operator== (const Vector2& lhs, const Vector2& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

constexpr bool operator!= (const Vector2& lhs, const Vector2& rhs) {
	return lhs.x != rhs.x || lhs.y != rhs.y;
}

/**
* Simple implementation. A more particular one implementing deltas may be added later
*/
constexpr bool operator== (const Vector3& lhs, const Vector3& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

constexpr bool operator!= (const Vector3& lhs, const Vector3& rhs) {
	return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}

constexpr Vector3 operator* (const float s, const Vector3& vector) {
	return Vector3(s * vector.x, s * vector.y, s * vector.z);
}

constexpr Vector3 operator+ (const Vector3& lhs, const Vector3& rhs) {
	return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

constexpr Vector3 operator- (const Vector3& lhs, const Vector3& rhs) {
	return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline std::ostream& operator <<(std::ostream& stream, const Vector3& vector) {
	return stream << "x: " << vector.x << ", y: " << vector.y << ", z: " << vector.z << std::endl;
}

/**
* Simple implementation. A more particular one implementing deltas may be added later
*/
constexpr bool operator== (const Vector4& lhs, const Vector4& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

/**
* Quaternion class with optimized vector rotation
*/
class Quaternion {
public:
	constexpr Quaternion() : w(1), ijk(0, 0, 0) {}

	// a raw constructor. Only use if trying to accomplish something very specific, entries are not normalized
	constexpr Quaternion(float w, float i, float j, float k) : w(w), ijk(i, j, k) { }
	constexpr Quaternion(float w, const Vector3& vector) : w(w), ijk(vector) { }
	Quaternion(const Vector3& axis, float theta) : w(0), ijk() {
		float cos = cosf(theta / 2);
		float sin = sinf(theta / 2);
		ijk = sin * axis.Normalized();
		w = cos;
	}

	void operator=(const Quaternion& other) {
		w = other.w;
		ijk = other.ijk;
	}

	bool operator==(const Quaternion& other) {
		return ijk == other.ijk && w == other.w;
	}

	// returns the inverse of the quaternion;
	inline Quaternion Conjugate() const {
		return Quaternion(w, -1 * ijk);
	}

	// single multiplication
	inline Quaternion operator*(const Quaternion& other) const {
		return Quaternion(w * other.w - other.ijk.Dot(ijk), w * other.ijk + other.w * ijk + ijk.Cross(other.ijk));
	}

	/**
	* This is an optimized rotation, simplified from multiplying q * p * q` where q` is the Conjugate of q
	*/
	inline Vector3 Rotate(const Vector3& v) const {
		return 2 * (ijk.Dot(v) * ijk + w * (ijk.Cross(v))) + (w * w - ijk.Dot(ijk)) * v;
	}

	float w;
	Vector3 ijk;
};

// a simple 4x4 matrix class. The goal is to emphasize speed and semantic simplicity over versatility.
// the constructors take data in row-major order, but the data are actually stored in column-major order
class Mat4x4 {
public:
	Mat4x4() {
		ZeroMemory(data, sizeof(Mat4x4));
	}

	Mat4x4(const Mat4x4& other) {
		memcpy(data, other.data, sizeof(Mat4x4));
	}

	// matrix entries for this constructor are input in row-major order
	Mat4x4(float m0n0, float m0n1, float m0n2, float m0n3,
		float m1n0, float m1n1, float m1n2, float m1n3,
		float m2n0, float m2n1, float m2n2, float m2n3,
		float m3n0, float m3n1, float m3n2, float m3n3) {
		data[0] = m0n0; data[4] = m0n1; data[8] = m0n2; data[12] = m0n3;
		data[1] = m1n0; data[5] = m1n1; data[9] = m1n2; data[13] = m1n3;
		data[2] = m2n0; data[6] = m2n1; data[10] = m2n2; data[14] = m2n3;
		data[3] = m3n0; data[7] = m3n1; data[11] = m3n2; data[15] = m3n3;
	}

	// The vectors of this constructor represent the rows of the matrix
	Mat4x4(const Vector4& m0, const Vector4& m1, const Vector4& m2, const Vector4& m3) {
		data[0] = m0.x; data[4] = m0.y; data[8] = m0.z; data[12] = m0.w;
		data[1] = m1.x; data[5] = m1.y; data[9] = m1.z; data[13] = m1.w;
		data[2] = m2.x; data[6] = m2.y; data[10] = m2.z; data[14] = m2.w;
		data[3] = m3.x; data[7] = m3.y; data[11] = m3.z; data[15] = m3.w;
	}

	// returns a new, transposed matrix
	Mat4x4 Transpose() {
		Vector4 m0 = GetColumn(0);
		Vector4 m1 = GetColumn(1);
		Vector4 m2 = GetColumn(2);
		Vector4 m3 = GetColumn(3);
		return Mat4x4(m0, m1, m2, m3);
	}

	float& operator()(const UINT m, const UINT n){
#ifdef SAFE_DEBUG
		assert(m < 4 && n < 4);
#endif
		return data[n * 4 + m];
	}

	inline Vector4 GetColumn(UINT n) const {
#ifdef SAFE_DEBUG
		assert(n < 4);
#endif
		n *= 4;
		return Vector4(data[0 + n], data[1 + n], data[2 + n], data[3 + n]);
	}

	inline Vector4 GetRow(UINT m) const {
#ifdef SAFE_DEBUG
		assert(m < 4);
#endif
		return Vector4(data[m + 0], data[m + 4], data[m + 8], data[m + 12]);
	}

	Mat4x4& operator=(const Mat4x4& other) {
		memcpy(data, other.data, 64);
		return *this;
	}

private:
	float data[16];
};

inline Vector4 operator* (const float f, const Vector4& vec4) {
	return Vector4(vec4.x * f, vec4.y * f, vec4.z * f, vec4.w * f);
}

inline Vector4 operator* (const Mat4x4& mat, const Vector4& vec4) {
	return Vector4(mat.GetRow(0).Dot(vec4), mat.GetRow(1).Dot(vec4), mat.GetRow(2).Dot(vec4), mat.GetRow(3).Dot(vec4));
}

inline Vector4 operator* (const Vector4& vec4, const Mat4x4& mat) {
	return Vector4(mat.GetColumn(0).Dot(vec4), mat.GetColumn(1).Dot(vec4), mat.GetColumn(2).Dot(vec4), mat.GetColumn(3).Dot(vec4));
}

inline Mat4x4 operator* (const Mat4x4& lhs, const Mat4x4& rhs) {
	return Mat4x4(lhs.GetRow(0) * rhs, lhs.GetRow(1) * rhs, lhs.GetRow(2) * rhs, lhs.GetRow(3) * rhs);
}

