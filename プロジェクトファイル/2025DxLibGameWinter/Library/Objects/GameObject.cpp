#include "GameObject.h"
#include "Library/Physics/Physics.h"
#include "Library/System/TimeScaleManager.h"
#include <cassert>
#include <functional>

namespace {
	// 揺れの細かさ 増やすと細分化される
	constexpr int kShakeDimAmount = 500;
}

GameObject::GameObject(
	std::weak_ptr<Physics> physics,
	PhysicsData::Priority priority, 
	PhysicsData::GameObjectTag tag, 
	PhysicsData::ColliderKind colliderKind, 
	bool isTrigger, bool isCollision, bool useGravity) :
	_physics(physics),
	_toDefaultScaleFrame(0),
	_hitStopFrame(0.0f),
	_isHitStopShake(false),
	_hitStopMagnitude(0.0f),
	_currentHitStopShakeVec()
{
	// 初期化
	_collider = std::make_shared<Collider>(priority, tag, colliderKind,
		isTrigger, isCollision, useGravity);
}

void GameObject::EntryPhysics(std::weak_ptr<Physics> physics)
{
	_physics = physics;
	// Physicsに自身を登録
	_collider->EntryPhysics(_physics);
}

void GameObject::ReleasePhysics()
{
	_collider->ReleasePhysics();
}

void GameObject::UpdateTimeScale()
{
	// ヒットストップ更新
	if (IsHitStop()) {
		--_hitStopFrame;
		if (_hitStopFrame < 0) {
			_hitStopFrame = 0;
		}
		else {
			return;
		}
	}

	// 通常のタイムスケールに戻す期間更新
	if (_toDefaultScaleFrame <= 0) {
		_toDefaultScaleFrame = 0;
		return;
	}
	--_toDefaultScaleFrame;
}

float GameObject::GetCurrentTimeScale() const
{
	TimeScaleManager& timeScaleManager = TimeScaleManager::GetInstance();
	float scale = timeScaleManager.GetOtherCurrentScale();
	// 不正な当たり判定であればデフォルトを返す
	if (_collider == nullptr) {
		return timeScaleManager.GetDefaultScale();
	}
	// プレイヤーであれば
	if (_collider->GetTag() == PhysicsData::GameObjectTag::Player) {
		scale = timeScaleManager.GetPlayerCurrentScale();
	}
	// 通常タイムスケールに戻すなら
	if (_toDefaultScaleFrame > 0) {
		scale = timeScaleManager.GetDefaultScale();
	}
	// ヒットストップ中であれば
	if (IsHitStop()) {
		scale = 0.0f;
	}
	return scale;
}

float GameObject::SetToDefaultScaleFrame(int frame)
{
	return _toDefaultScaleFrame = frame;
}

void GameObject::SetHitStop(int frame, bool isShake, float magnitude)
{
	_hitStopFrame = frame;
	_isHitStopShake = isShake;
	// 振動させるなら
	if (isShake) {
		_hitStopMagnitude = magnitude;
		_currentHitStopShakeVec = CalcShakeVec(_hitStopMagnitude);
	}
	// でなければ
	else {
		_hitStopMagnitude = 0.0f;
		_currentHitStopShakeVec = Vector3(0, 0, 0);
	}
}

Vector3 GameObject::CalcShakeVec(float magnitude)
{
	if (magnitude <= 0.0f) {
		return Vector3Zero();
	}

	// 揺れを生成
	const int randAmount = kShakeDimAmount * 2;
	// ランダムな揺れの量を返す
	std::function<float()> GetShakeAmount =
		[this]() {return (static_cast<float>(GetRand(randAmount)) / kShakeDimAmount - 1.0f) * _hitStopMagnitude;};
	// 揺れの量を設定
	float shakeX = GetShakeAmount();
	float shakeY = GetShakeAmount();
	float shakeZ = GetShakeAmount();

	return Vector3(shakeX, shakeY, shakeZ);
}