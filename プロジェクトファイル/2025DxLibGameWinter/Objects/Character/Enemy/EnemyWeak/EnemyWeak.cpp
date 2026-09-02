#include "EnemyWeak.h"
#include "EnemyWeakStateBase.h"
#include "EnemyWeakStateSpawn.h"
#include "EnemyWeakStateReact.h"
#include "EnemyWeakStateDeath.h"
#include "Objects/Projectile/EnemyWeakProjectile.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/Objects/AttackCol.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Statistics.h"
#include "Scene/ResultDataHolder.h"

#include <DxLib.h>

namespace {
	constexpr float kHitPoint = 100.0f;
	
	constexpr float kModelScale = 2.0f;

	constexpr float kColRadius = 50.0f * kModelScale;
	constexpr float kColHeight = 175.0f * kModelScale;

	constexpr float kModelDrawOffsetAngle = DX_PI_F;

	// カプセルの始点(足元)から終点(頭頂部)までのベクトル
	const Vector3 kColStartToEnd = Vector3Up() * (kColHeight - kColRadius * 2.0f);

	const Position3 kDefaultPos = Position3(0, 0, 200);
	const Vector3 kDrawOffset = Vector3(0, -kColRadius, 0);

	// 攻撃情報
	constexpr float kAttackColRad = 40.0f * kModelScale;
	constexpr float kAttackColHeight = 75.0f * kModelScale;
	const Vector3 kAttackColTransOffset = Vector3(0, 0, kColRadius);
	const Vector3 kAttackColScale = Vector3(1, 1, 1);
	const Vector3 kAttackColAngle = Vector3Zero();
	constexpr float kAttackPower = 10.0f;

	// ノックバック減衰量
	constexpr float kKnockbackDecayAmount = 0.85f;

	// ヒットストップ時のモデル振動量
	constexpr float kShakeMagnitude = 5.0f * kModelScale;

	// 獲得スコア
	constexpr int kBossDeathScore = 100;
}

EnemyWeak::EnemyWeak(std::weak_ptr<Physics> physics, int modelHandle) :
	EnemyBase(physics, modelHandle),
	_currentState(),
	_attackCol(std::make_shared<AttackCol>(physics, 
		PhysicsData::GameObjectTag::EnemyAttack))
{
	// 体力設定
	SetHitPoint(kHitPoint);

	SetColliderData(kColRadius, kColStartToEnd);

	SetPos(kDefaultPos);
	MV1SetPosition(_animator->GetHandle(), GetPos());
	MV1SetScale(_animator->GetHandle(), Vector3(1, 1, 1) * kModelScale);
	MV1SetRotationXYZ(_animator->GetHandle(),
		Vector3(0, _rotAngleY + kModelDrawOffsetAngle, 0));

	// 使用するアニメーションを全て入れる
	_animator->SetAnimData(WeakAnimData::kAnimNameSpawn, WeakAnimData::kSpawnAnimSpeed, false);
	_animator->SetAnimData(WeakAnimData::kAnimNameIdle, WeakAnimData::kBaseAnimSpeed, true);
	_animator->SetAnimData(WeakAnimData::kAnimNameDash, WeakAnimData::kBaseAnimSpeed, true);
	_animator->SetAnimData(WeakAnimData::kAnimNameHeavyAttack1, WeakAnimData::kAttackAnimSpeed, false);
	_animator->SetAnimData(WeakAnimData::kAnimNameFire, WeakAnimData::kAttackAnimSpeed, false);
	_animator->SetAnimData(WeakAnimData::kAnimNameReact, WeakAnimData::kBaseAnimSpeed, false);
	_animator->SetAnimData(WeakAnimData::kAnimNameDeath, WeakAnimData::kDeathAnimSpeed, false);

	// 最初のアニメーションを設定する
	_animator->SetStartAnim(WeakAnimData::kAnimNameSpawn);
}

EnemyWeak::~EnemyWeak()
{
}

void EnemyWeak::Init()
{
	// Colliderと自身を紐づける
	SetColliderParent();

	// ステート作成
	_currentState = std::make_shared<EnemyWeakStateSpawn>(GetParentPtr());
	// ステートの初めの処理を行う
	_currentState->OnEnter();

	AttackColStats stats;
	stats.colRad = kAttackColRad;
	stats.colHeight = kAttackColHeight;
	stats.transOffset = kAttackColTransOffset;
	stats.scale = kAttackColScale;
	stats.angle = kAttackColAngle;
	stats.attackPower = kAttackPower;
	std::shared_ptr<AttackCol> ptr =
		std::make_shared<AttackCol>(_physics, PhysicsData::GameObjectTag::EnemyAttack);
	ptr->SetAttackData(stats);
	ptr->SetOwnerStatus(GetParentPtr());
	_attackCol = ptr;

	_animator->SetDrawOffset(kDrawOffset);
}

void EnemyWeak::Update()
{
	// 無敵時間の更新
	UpdateInvTime();

	// ヒットストップ中であれば
	if (IsHitStop()) {
		// 前回のヒットストップ振動量を削除
		_animator->SetDrawOffset(-GetHitStopShakeVec() + kDrawOffset);
		// ヒットストップ状態を更新
		UpdateHitStop();

		// 更新後もヒットストップ中であれば
		if (IsHitStop()) {
			// ヒットストップ振動量を与える
			_animator->SetDrawOffset(GetHitStopShakeVec() + kDrawOffset);
			// 以降の処理は行わない
			return;
		}
	}

	// ノックバック量更新
	UpdateKnockback(kKnockbackDecayAmount);

	// ステート待機時間更新
	UpdateStateTransitionTime();
	// 攻撃待機時間更新
	UpdateAttackInterval();

	// アニメーションの更新を行う
	float scale = GetCurrentTimeScale();
	_animator->Update(scale);

	// ステートに応じた更新を行う
	_currentState->Update();

	// 攻撃判定を更新
	UpdateAttackCol();
	UpdateProjectiles();
}

void EnemyWeak::Draw() const
{
	// 当たり判定を行ってからモデルの位置を設定する
	_animator->AttachPos(GetPos());
	// モデルの描画
	_animator->Draw();
}

void EnemyWeak::DrawAttackRangeIndicator() const
{
	// 現在のステートの攻撃範囲を描画する
	_currentState->DrawAttackRangeIndicator();
}

void EnemyWeak::OnCollide(const std::weak_ptr<Collider> collider)
{
}

void EnemyWeak::TakeDamage(std::shared_ptr<AttackableGameObject> attacker,
	bool isReact)
{
	// 無敵状態ならreturn
	if (IsInv()) return;

	// ダメージを受ける
	Damage(attacker->GetAttackPower());
	printf("EnemyHP : %.02f\n", GetHitPoint());

	// プレイヤーの攻撃が当たったことを通知する
	HitPlayerAttack();

	// 削り値だけノックバック耐久値を削る
	_breakPoint -= attacker->GetBreakPower();

	// ノックバック耐久値がないなら
	if (_breakPoint <= 0.0f) {
		// 指定量だけノックバックする
		// attackerからのknockBackForce
		Vector3 knockbackForce = attacker->GetKnockbackForce();
		// ノックバック
		Position3 otherPos = attacker->GetCenterPos();
		// 攻撃側の親が存在するなら
		if (!attacker->GetOwnerStatus().expired()) {
			// 親の位置を参照
			otherPos = attacker->GetOwnerStatus().lock()->GetCenterPos();
		}

		Vector3 fromOther = GetCenterPos() - otherPos;
		fromOther.Normalized();
		AddKnockback(knockbackForce, fromOther);
	}

	// ヒットストップを行う
	SetHitStop(10, true, kShakeMagnitude);
	// ヒットストップ振動量を与える
	_animator->SetDrawOffset(GetHitStopShakeVec() + kDrawOffset);

	// 一瞬タイムスケールを通常に戻す
	SetToDefaultScaleFrame(kReactDefaultScaleFrame);
	
	// 生存していないなら
	if (!IsAlive()) {
		// 球を削除
		for (auto projectile : _projectiles) {
			projectile->SetDeletable();
		}
		if (!_scoreAdded) {
			ResultDataHolder::GetInstance().AddScore(kBossDeathScore);
			_scoreAdded = true;
		}
		// 死亡ステートに遷移
		_currentState->ChangeState(std::make_shared<EnemyWeakStateDeath>(GetParentPtr()));
		return;
	}

	// でなければ被弾ステートに遷移
	_currentState->ChangeState(std::make_shared<EnemyWeakStateReact>(GetParentPtr()));
	printf("Enemy被弾\n");
}

float EnemyWeak::GetMaxHitPoint()
{
	return kHitPoint;
}

bool EnemyWeak::IsAlive()
{
	return _hitPoint > 0.0f;
}

bool EnemyWeak::CanAttack()
{
	// 行動待機時間などを考慮した攻撃可否を返す
	return HasAttackAuthority() && (_stateTransitionTime <= 0.0f)
		&& (_attackInterval <= 0.0f);
}

void EnemyWeak::DisableAttackCol()
{
	_attackCol->Disable();
}

void EnemyWeak::FireProjectile()
{
	if (_manager.expired()) return;

	Position3 firePos = GetPos() + Vector3(0, kAttackColRad, 0);	// 手元程度の高さから発射
	Vector3 dirToPlayer = _manager.lock()->GetPlayerPos() - firePos;
	if (dirToPlayer.SqrMagnitude() <= 0.0f) return;
	dirToPlayer.Normalized();

	auto projectile = std::make_shared<EnemyWeakProjectile>(_physics, _manager, firePos, dirToPlayer);
	projectile->SetOwnerStatus(GetParentPtr());
	projectile->Init();

	_projectiles.emplace_back(projectile);
}

void EnemyWeak::UpdateAttackCol()
{
	// 位置更新
	Matrix4x4 forward = GetModelMatrix();
	if (_attackCol->GetCollisionState()) {
		// 当たり判定の半径分だけ浮かす
		Vector3 offset = Vector3(0, kAttackColRad, 0);
		forward = MatMultiple(forward, MatTranslate(offset));
		_attackCol->PositionUpdate(forward);
		_attackCol->Update();
	}
}

void EnemyWeak::UpdateStateTransitionTime()
{
	if (_stateTransitionTime <= 0.0f) {
		_stateTransitionTime = 0.0f;
		return;
	}
	_stateTransitionTime -= GetCurrentTimeScale() / Statistics::kFPS;
	if (_stateTransitionTime < 0.0f) {
		_stateTransitionTime = 0.0f;
	}
}

void EnemyWeak::UpdateAttackInterval()
{
	if (_attackInterval <= 0.0f) {
		_attackInterval = 0.0f;
		return;
	}
	_attackInterval -= GetCurrentTimeScale() / Statistics::kFPS;
	if (_attackInterval < 0.0f) {
		_attackInterval = 0.0f;
	}
}

void EnemyWeak::UpdateProjectiles()
{
	// 弾の更新
	for (auto& proj : _projectiles) {
		if (proj && !proj->CanDelete()) {
			proj->Update();
		}
	}
	// 削除可能な弾があれば削除する
	std::erase_if(
		_projectiles,
		[](const std::shared_ptr<EnemyWeakProjectile>& p) {
			return !p || p->CanDelete();
		});
}
