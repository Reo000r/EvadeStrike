#include "Vector2.h"
#include "Vector3.h"

#include <cmath>		// 各種計算用

void Vector2::operator+=(const Vector2& vec) {
	x += vec.x;
	y += vec.y;
}

void Vector2::operator-=(const Vector2& vec) {
	x -= vec.x;
	y -= vec.y;
}

void Vector2::operator*=(float scale) {
	x *= scale;
	y *= scale;
}

void Vector2::operator/=(float scale) {
	if (scale == 0.0f) {
#ifdef USE_ASSERT_GEOMETRY
		assert(false && "0除算");
#endif // USE_ASSERT_GEOMETRY
		x = 0.0f;
		y = 0.0f;
		return;
	}
	x /= scale;
	y /= scale;
}

Vector2 Vector2::operator+(const Vector2& vec) const {
	return Vector2(x + vec.x, y + vec.y);
}

Vector2 Vector2::operator-(const Vector2& vec) const {
	return Vector2(x - vec.x, y - vec.y);
}

Vector2 Vector2::operator*(float scale) const {
	return Vector2(x * scale, y * scale);
}

Vector2 Vector2::operator/(float scale) const {
	if (scale == 0.0f) {
#ifdef USE_ASSERT_GEOMETRY
		assert(false && "0除算");
#endif // USE_ASSERT_GEOMETRY
		return Vector2(0.0f, 0.0f);
	}
	return Vector2(x / scale, y / scale);
}

void Vector2::operator=(const Vector3& vec3) {
	x = vec3.x;
	y = vec3.y;
}

void Vector2::operator+=(const Vector3& vec3) {
	x += vec3.x;
	y += vec3.y;
}

void Vector2::operator-=(const Vector3& vec3) {
	x -= vec3.x;
	y -= vec3.y;
}

Vector3 Vector2::operator+(const Vector3& vec3) const {
	return Vector3(x + vec3.x, y + vec3.y, vec3.z);
}

Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

float Vector2::SqrMagnitude() const {
	return (x * x + y * y);
}

float Vector2::Magnitude() const {
	return sqrtf(SqrMagnitude());
}

void Vector2::Normalized() {
	(*this) = Normalize();
}

Vector2 Vector2::Normalize() const {
	float abs = sqrtf(SqrMagnitude());
	if (abs == 0.0f) {
		//#ifdef USE_ASSERT_GEOMETRY
		//		assert(false && "0除算");
		//#endif // USE_ASSERT_GEOMETRY
		return *this;
	}
	return Vector2(*this / abs);
}

float Distance(const Vector2& a, const Vector2& b) {
	return sqrtf((a - b).SqrMagnitude());
}

Vector2 Lerp(const Vector2& a, const Vector2& b, const float& t) {
	// 長さが0-1ではないなら相応しい値を返す
	if (t < 0.0f) return a;	// 短い場合
	if (t > 1.0f) return b;	// 長い場合
	return a + (b - a) * t;
}