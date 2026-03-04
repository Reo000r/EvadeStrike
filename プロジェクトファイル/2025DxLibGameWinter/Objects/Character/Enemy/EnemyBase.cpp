#include "EnemyBase.h"
#include "EnemyManager.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/TimeScaleManager.h"
#include <DxLib.h>
#include <algorithm>

namespace {
	// モデル描画時の回転補正
	constexpr float kModelDrawOffsetAngle = DX_PI_F;
}

EnemyBase::EnemyBase(std::weak_ptr<Physics> physics, int modelHandle) :
	CharacterBase(physics, PhysicsData::Priority::Middle, 
		PhysicsData::GameObjectTag::Enemy, PhysicsData::ColliderKind::Capsule,
		false, true, true),
	_animator(std::make_shared<AnimationModel>()),
	_manager(),
	_rotAngleY(0.0f),
	_targetRotAngleY(0.0f),
	_stateTransitionTime(0.0f),
	_attackInterval(0.0f),
	_canAttack(false),
	_canDelete(false)
{
	_animator->Init(modelHandle);
}

EnemyBase::~EnemyBase()
{
}

bool EnemyBase::HasAttackAuthority()
{
	return _canAttack;
}

bool EnemyBase::CanDelete()
{
	return _canDelete;
}

void EnemyBase::SetAttackState(bool canAttack)
{
	_canAttack = canAttack;
}

void EnemyBase::SetDeleteState(bool canDelete)
{
	_canDelete = canDelete;
}

void EnemyBase::ReleaseCameraTarget()
{
	_manager.lock()->ReleaseCameraTarget(
		std::static_pointer_cast<CharacterBase>(shared_from_this()));
}

void EnemyBase::SetColliderData(float rad, Vector3 startToEnd)
{
	// 当たり判定データ設定
	_collider->CreateColliderDataCapsule(
		rad,		// 半径
		startToEnd,	// 始点から終点のベクトル
		false,		// isTrigger
		true		// isCollision
	);
	// 自身の武器とは当たり判定を行わない
	_collider->AddThroughTag(PhysicsData::GameObjectTag::EnemyAttack);

	// physicsに登録する
	EntryPhysics(_physics);
}

void EnemyBase::ReleasePhysics()
{
	GameObject::ReleasePhysics();
}

Matrix4x4 EnemyBase::GetModelMatrix() const
{
	Matrix4x4 ret = MV1GetMatrix(_animator->GetHandle());
	Matrix4x4 offset = MatRotateY(kModelDrawOffsetAngle);
	return MatMultiple(offset, ret);
}

Matrix4x4 EnemyBase::GetForwardMatrix() const
{
	// 行列作成
	Matrix4x4 rotY = MatRotateY(_rotAngleY);
	Matrix4x4 pos = MatTranslate(GetPos());
	return MatMultiple(rotY, pos);	// 位置とY軸回転情報
}

void EnemyBase::RotateToPlayer()
{
	if (_manager.expired()) return;

	// プレイヤーの方向を向く
	Position3 playerPos = _manager.lock()->GetPlayerPos();
	// プレイヤーへの方向ベクトル
	Vector3 dirToPlayer = (playerPos - GetPos());
	if (dirToPlayer.SqrMagnitude() > 0.0f) {
		// Y軸回転角度を計算
		_rotAngleY = atan2f(dirToPlayer.x, dirToPlayer.z);
	}
}

void EnemyBase::RotateToPlayer(float speed)
{
	if (_manager.expired()) return;

	Position3 playerPos = _manager.lock()->GetPlayerPos();
	// プレイヤーへの方向ベクトル
	Vector3 dirToPlayer = playerPos - GetPos();
	if (dirToPlayer.SqrMagnitude() == 0.0f) return; // 距離がゼロなら何もしない
	dirToPlayer.Normalized();

	// Y軸回転角度を計算
	float targetAngle = atan2f(dirToPlayer.x, dirToPlayer.z);

	// 現在の角度から目標角度までの差分を計算
	float diff = Calc::RadianNormalize(targetAngle - _rotAngleY);
	// タイムスケールを適用
	diff *= GetCurrentTimeScale();

	// 回転量を制限
	float turnAmount = std::clamp<float>(diff, -speed, speed);
	_rotAngleY += turnAmount * TimeScaleManager::GetInstance().GetOtherCurrentScale();

	// モデルに回転を適用
	MV1SetRotationXYZ(_animator->GetHandle(), 
		Vector3(0, _rotAngleY + Calc::ToRadian(180.0f), 0));
}

bool EnemyBase::CanAttackRangeInPlayer(float attackRangeInPlayer)
{
	Position3 playerPos = _manager.lock()->GetPlayerPos();
	Vector3 toPlayer = playerPos - GetCenterPos();
	float length = toPlayer.Magnitude();
	return (length <= attackRangeInPlayer);
}

bool EnemyBase::IsNothingStateTransitionTime() const
{
	return (_stateTransitionTime <= 0.0f);
}

bool EnemyBase::IsNothingAttackInterval() const
{
	return (_attackInterval <= 0.0f);
}

void EnemyBase::SetStateTransitionTime(float time)
{
	_stateTransitionTime = time;
}

void EnemyBase::SetAttackInterval(float interval)
{
	_attackInterval = interval;
}

void EnemyBase::AddTransformForward(float add)
{
	_collider->AddVel(VTransformSR({ 0,0,add }, GetModelMatrix()));
}

void EnemyBase::HitPlayerAttack()
{
	_manager.lock()->HitPlayerAttack();
}
