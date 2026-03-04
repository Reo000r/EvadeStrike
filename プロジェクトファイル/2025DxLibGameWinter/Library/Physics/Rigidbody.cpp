#include "Rigidbody.h"

Rigidbody::Rigidbody() :
	_pos(),
	_dir(),
	_vel(),
	_useGravity(false),
	_gravityScale(1.0f)
{
}

void Rigidbody::Init(bool useGravity)
{
	this->_useGravity = useGravity;
}

void Rigidbody::SetVel(const Vector3& set)
{
	_vel = set;
	// 長さがあるなら
	if (_vel.SqrMagnitude() > 0) {
		// 正規化したベクトルを向きとして代入
		_dir = _vel.Normalize();
	}
}

void Rigidbody::SetVelY(const float set)
{
	SetVel(Vector3(_vel.x, set, _vel.z));
}
