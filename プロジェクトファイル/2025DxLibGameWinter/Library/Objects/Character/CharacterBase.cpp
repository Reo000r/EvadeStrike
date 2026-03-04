#include "CharacterBase.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>
#include <functional>
#include <algorithm>

CharacterBase::CharacterBase(
	std::weak_ptr<Physics> physics,
	PhysicsData::Priority priority, 
	PhysicsData::GameObjectTag tag, 
	PhysicsData::ColliderKind colliderKind, 
	bool isTrigger, bool isCollision, bool useGravity) :
	GameObject(
		physics,
		priority,
		tag,
		colliderKind,
		isTrigger,
		isCollision,
		useGravity),
	_hitPoint(0.0f),
	_breakPoint(0.0f),
	_knockbackVel(Vector3Zero()),
	_isFirstKnockback(false),
	_invincibilityTime(0.0f)
{
	
}

void CharacterBase::SetColliderParent()
{
	// Colliderと自身(GameObject)を紐づける
	std::shared_ptr<GameObject> parent = shared_from_this();
	_collider->SetParent(parent);
}

void CharacterBase::UpdateInvTime()
{
	if (_invincibilityTime > 0.0f) {
		_invincibilityTime -= GetCurrentTimeScale() / Statistics::kFPS;
	}
	if (_invincibilityTime < 0.0f) {
		_invincibilityTime = 0.0f;
	}
}

void CharacterBase::UpdateKnockback(float decayAmount)
{
	// 値を丸める
	decayAmount = std::clamp<float>(decayAmount, 0.0f, 1.0f);
	// 地面についていたかつ
	// 初めてのノックバックではない場合
	if (IsGround() && !_isFirstKnockback) {
		// 移動量を減衰させる
		_knockbackVel *= decayAmount;
	}
	// 移動していないとみなされる量であれば
	if (_knockbackVel.Magnitude() < PhysicsData::kSleepThreshold) {
		// ノックバックを無くす
		_knockbackVel = Vector3Zero();
	}
	AddVel(_knockbackVel);
	_isFirstKnockback = false;
}

void CharacterBase::UpdateHitStop()
{
	// ヒットストップ中でないならreturn
	if (!IsHitStop()) {
		return;
	}

	// ヒットストップ中なら
	if (IsHitStop()) {
		_hitStopFrame -= GetCurrentTimeScale() / Statistics::kFPS;
	}
	// ヒットストップ中でなくなったなら
	if (!IsHitStop()) {
		_hitStopFrame = 0.0f;
		_isHitStopShake = false;
		_currentHitStopShakeVec = Vector3(0, 0, 0);
		return;
	}

	// ヒットストップ中の振動を行うなら
	if (_isHitStopShake) {
		_currentHitStopShakeVec = CalcShakeVec(_hitStopMagnitude);
	}
}

bool CharacterBase::IsGround() const
{
	// 接地していたらtrue
	return _collider->IsFloor();
}


void CharacterBase::AddKnockback(Vector3 knockback, Vector3 dir)
{
	const Vector3 back = Vector3Back();
	Vector3 axis = Cross(back, dir);
	float axisLen = axis.Magnitude();
	float dot = VDot(back, dir);
	float angle = acosf(dot);	// front と dir の角度
	if (axisLen < 1e-6f) {
		// ほぼ同じ方向 回転なし
		// ほぼ真逆 180度回転 軸はYなど
	}
	axis.Normalized();
	MATRIX rot = MGetRotAxis(axis, angle);
	Vector3 knockbackVel;
	knockbackVel = VTransform(knockback, rot);

	AddVel(knockbackVel);
	knockbackVel.y = 0.0f;
	_knockbackVel += knockbackVel;
	_isFirstKnockback = true;
}

void CharacterBase::ResetKnockback()
{
	_knockbackVel = Vector3Zero();
	_isFirstKnockback = false;
}

void CharacterBase::SetHitPoint(float hitPoint)
{
	_hitPoint = hitPoint;
}

void CharacterBase::Damage(float damage)
{
	_hitPoint -= damage;
}
