#pragma once

#include "Library/Geometry/Vector3.h"

/// <summary>
/// 物理、衝突判定に必要なデータの塊
/// </summary>
class Rigidbody final
{
public:
	Rigidbody();
	~Rigidbody() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="useGravity">重力使用可否</param>
	void Init(bool useGravity = false);

	// getter
	const Position3 GetPos() const { return _pos; }
	const Position3 CalcNextPos(float timeScale) const { return _pos + _vel * timeScale; }
	const Vector3 GetDir() const { return _dir; }
	const Vector3 GetVel() const { return _vel; }
	bool UseGravity() const { return _useGravity; }
	float GetGravityScale() const { return _gravityScale; }
	void SetGravityScale(float scale) { _gravityScale = scale; }
	// setter
	void SetPos(const Position3& set) { _pos = set; }
	void SetVel(const Vector3& set);
	void SetPosY(const float set) { _pos.y = set; }
	void SetVelY(const float set);
	void SetUseGravity(bool set) { _useGravity = set; }
	
	void AddPos(const Position3& add) { SetPos(_pos + add); }
	void AddVel(const Vector3& add) { SetVel(_vel + add); }

private:
	Position3	_pos;
	Vector3	_vel;
	Vector3	_dir;
	bool	_useGravity;
	float	_gravityScale;
};

