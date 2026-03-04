#pragma once

class Vector2;
typedef Vector2 Position2;
class Vector3;

class Vector2 final {
public:
	float x, y;

public:
	Vector2() : x(0.0f), y(0.0f) {}

	Vector2(float x_, float y_) : x(x_), y(y_) {}
	~Vector2() = default;

	void operator+=(const Vector2& vec);
	void operator-=(const Vector2& vec);
	void operator*=(float scale);
	void operator/=(float scale);
	Vector2 operator+(const Vector2& vec) const;
	Vector2 operator-(const Vector2& vec) const;
	Vector2 operator*(float scale) const;
	Vector2 operator/(float scale) const;

	void operator=(const Vector3& vec3);
	void operator+=(const Vector3& vec3);
	void operator-=(const Vector3& vec3);
	Vector3 operator+(const Vector3& vec3) const;

	/// <summary>
	/// 符号反転
	/// </summary>
	/// <returns></returns>
	Vector2 operator-() const;

	/// <summary>
	/// 自身の2乗の長さを返す
	/// </summary>
	/// <returns></returns>
	float SqrMagnitude() const;
	/// <summary>
	/// 自身の長さを返す
	/// </summary>
	/// <returns></returns>
	float Magnitude() const;

	/// <summary>
	/// 自身を正規化する
	/// </summary>
	void Normalized();
	/// <summary>
	/// 正規化したベクトルを返す
	/// </summary>
	/// <returns></returns>
	Vector2 Normalize() const;
};

/// <summary>
/// aとbの間の距離を返す
/// (Distance(a, b) は (a-b).magnitudeと同じ)
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
float Distance(const Vector2& a, const Vector2& b);

/// <summary>
/// <para> aからb へ tの割合だけ近づいた点を返す </para>
/// <para> tは 0-1 の範囲 </para>
/// <para> tが 0   の場合、a を返す </para>
/// <para> tが 1   の場合、b を返す </para>
/// <para> tが 0.5 の場合、a と b の中点が返される </para>
/// <para> tが 0以下 の場合 a を返す(t=0と同じ) </para>
/// <para> tが 1以上 の場合 b を返す(t=1と同じ) </para>
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="t"></param>
/// <returns></returns>
Vector2 Lerp(const Vector2& a, const Vector2& b, const float& t);