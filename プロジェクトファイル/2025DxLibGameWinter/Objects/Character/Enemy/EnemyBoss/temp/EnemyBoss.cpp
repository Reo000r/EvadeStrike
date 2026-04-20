#include "EnemyBoss.h"
#include "EnemyBossStateBase.h"
#include "EnemyBossStateSpawn.h"
#include "EnemyBossStateDeath.h"
#include "EnemyBossStateStagger.h"
#include "EnemyBossBressProjectile.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/Objects/AttackCol.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>
#include <algorithm>
#include <cassert>

namespace
{
	// 体力
	constexpr float kHitPoint = 1000.0f;

	// コリジョン設定
	constexpr float kColRadius = 150.0f;
	constexpr float kColHeight = 500.0f;
	const Vector3 kColStartToEnd = Vector3Up() * (kColHeight - kColRadius * 2.0f);

	// モデル描画オフセット
	constexpr float kModelDrawOffsetAngle = DX_PI_F;
	const Vector3 kDrawOffset = Vector3(0, -kColRadius, 0);
	const Position3 kDefaultPos = Position3(0, 0, 2000);

	// Attack1（近接）当たり判定
	constexpr float kAttack1ColRad    = 120.0f;
	constexpr float kAttack1ColHeight = 200.0f;
	const Vector3   kAttack1ColOffset = Vector3(0, 0, 150);
	const Vector3   kAttack1Scale     = Vector3(1, 1, 1);
	const Vector3   kAttack1Angle     = Vector3Zero();
	constexpr float kAttack1Power     = 20.0f;

	// Attack2（範囲）当たり判定
	constexpr float kAttack2ColRad    = 300.0f;
	constexpr float kAttack2ColHeight = 80.0f;
	const Vector3   kAttack2ColOffset = Vector3(0, 50, 0);	// 足元中心
	const Vector3   kAttack2Scale     = Vector3(1, 1, 1);
	const Vector3   kAttack2Angle     = Vector3Zero();
	constexpr float kAttack2Power     = 30.0f;

	// ノックバック減衰量
	constexpr float kKnockbackDecayAmount = 0.0f;

	// 頭部オフセット（ブレス弾の発射起点）
	const Vector3 kHeadOffset = Vector3(0, 420, 0);

	// フェーズ2移行体力割合
	constexpr float kPhase2HpRate = 0.5f;
}

EnemyBoss::EnemyBoss(std::weak_ptr<Physics> physics, int modelHandle) :
	EnemyBase(physics, modelHandle),
	_currentState(),
	_attackCol1(nullptr),
	_attackCol2(nullptr),
	_bressProjectiles(),
	_phase(1),
	_phaseChangedToTwo(false),
	_staggerPoint(0.0f),
	_staggerThreshold(kStaggerThresholdPhase1)
{
	// 体力設定
	SetHitPoint(kHitPoint);

	// 当たり判定設定
	SetColliderData(kColRadius, kColStartToEnd);

	// 初期位置設定
	SetPos(kDefaultPos);
	MV1SetPosition(_animator->GetHandle(), GetPos());
	MV1SetScale(_animator->GetHandle(), Vector3(1, 1, 1) * 3.0f);	// EnemyWeakより大きく
	MV1SetRotationXYZ(_animator->GetHandle(),
		Vector3(0, _rotAngleY + kModelDrawOffsetAngle, 0));

	// 使用するアニメーションを全て登録する
	// ※ アニメーション名は PlayerAnimationData の定数を流用するか、
	//    ボス専用アニメーション名を別途定義して使用すること
	//_animator->SetAnimData(kAnimNameIdle,          kBaseAnimSpeed, true);
	//_animator->SetAnimData(kAnimNameDash,          kBaseAnimSpeed, true);
	//_animator->SetAnimData(kAnimNameHeavyAttack1,  kAttackAnimSpeed, false);	// Attack1
	//_animator->SetAnimData(kAnimNameSpecialAttack2, kBaseAnimSpeed, false);	// Attack2
	//_animator->SetAnimData(kAnimNameDeath,         kDeathAnimSpeed, false);

	// 最初のアニメーションを設定する
	//_animator->SetStartAnim(kAnimNameIdle);
}

EnemyBoss::~EnemyBoss()
{
}

void EnemyBoss::Init()
{
	// コライダーと自身を紐づける
	SetColliderParent();

	// Attack1（近接）の攻撃判定を生成する
	{
		AttackColStats stats;
		stats.colRad       = kAttack1ColRad;
		stats.colHeight    = kAttack1ColHeight;
		stats.transOffset  = kAttack1ColOffset;
		stats.scale        = kAttack1Scale;
		stats.angle        = kAttack1Angle;
		stats.attackPower  = kAttack1Power;
		_attackCol1 = std::make_shared<AttackCol>(_physics, PhysicsData::GameObjectTag::EnemyAttack);
		_attackCol1->SetAttackData(stats);
		_attackCol1->SetOwnerStatus(GetParentPtr());
	}

	// Attack2（範囲叩きつけ）の攻撃判定を生成する
	{
		AttackColStats stats;
		stats.colRad       = kAttack2ColRad;
		stats.colHeight    = kAttack2ColHeight;
		stats.transOffset  = kAttack2ColOffset;
		stats.scale        = kAttack2Scale;
		stats.angle        = kAttack2Angle;
		stats.attackPower  = kAttack2Power;
		_attackCol2 = std::make_shared<AttackCol>(_physics, PhysicsData::GameObjectTag::EnemyAttack);
		_attackCol2->SetAttackData(stats);
		_attackCol2->SetOwnerStatus(GetParentPtr());
	}

	// 最初のステートを Spawn に設定する
	_currentState = std::make_shared<EnemyBossStateSpawn>(GetParentPtr());
	_currentState->OnEnter();

	_animator->SetDrawOffset(kDrawOffset);
}

void EnemyBoss::Update()
{
	// 無敵時間の更新
	UpdateInvTime();

	// ヒットストップ中であれば
	if (IsHitStop())
	{
		_animator->SetDrawOffset(-GetHitStopShakeVec() + kDrawOffset);
		UpdateHitStop();

		if (IsHitStop())
		{
			_animator->SetDrawOffset(GetHitStopShakeVec() + kDrawOffset);
			return;
		}
	}

	// ノックバック量の更新
	UpdateKnockback(kKnockbackDecayAmount);

	// タイマー更新
	UpdateStateTransitionTime();
	UpdateAttackInterval();

	// 怯み蓄積値の自然回復
	UpdateStaggerPoint();

	// フェーズ更新
	UpdatePhase();

	// アニメーション更新
	float scale = GetCurrentTimeScale();
	_animator->Update(scale);

	// ステートに応じた更新
	_currentState->Update();

	// 攻撃判定の位置更新
	UpdateAttackCol();

	// ブレス弾の更新
	UpdateBressProjectiles();
}

void EnemyBoss::Draw() const
{
	_animator->AttachPos(GetPos());
	_animator->Draw();
}

void EnemyBoss::OnCollide(const std::weak_ptr<Collider> collider)
{
	// 必要に応じて実装する
}

void EnemyBoss::TakeDamage(std::shared_ptr<AttackableGameObject> attacker)
{
	// 無敵状態ならreturn
	if (IsInv()) return;

	// ダメージを受ける
	Damage(attacker->GetAttackPower());
	printf("BossHP : %.02f\n", GetHitPoint());

	// プレイヤーの攻撃が当たったことをManagerへ通知する
	HitPlayerAttack();

	// ノックバック耐久値を削る
	_breakPoint -= attacker->GetBreakPower();

	// ノックバック耐久値がないなら吹き飛ばす
	if (_breakPoint <= 0.0f)
	{
		Vector3 knockbackForce = attacker->GetKnockbackForce();
		Position3 otherPos = attacker->GetCenterPos();
		if (!attacker->GetOwnerStatus().expired())
		{
			otherPos = attacker->GetOwnerStatus().lock()->GetCenterPos();
		}
		Vector3 fromOther = GetCenterPos() - otherPos;
		fromOther.Normalized();
		AddKnockback(knockbackForce, fromOther);
	}

	// ヒットストップ
	SetHitStop(8, true, 4.0f);
	_animator->SetDrawOffset(GetHitStopShakeVec() + kDrawOffset);

	// 被弾時のタイムスケールを一瞬戻す
	SetToDefaultScaleFrame(kReactDefaultScaleFrame);

	// 体力が尽きた場合は死亡ステートへ
	if (!IsAlive())
	{
		_currentState->ChangeState(
			std::make_shared<EnemyBossStateDeath>(GetParentPtr()));
		return;
	}

	// 怯み蓄積値を増やす
	_staggerPoint += attacker->GetBreakPower();

	// 怯み閾値を超えていれば怯みステートへ
	if (IsStagger())
	{
		ResetStaggerPoint();
		_staggerThreshold += kStaggerThresholdIncrement;	// 次回の閾値を引き上げる
		_currentState->ChangeState(
			std::make_shared<EnemyBossStateStagger>(GetParentPtr()));
		return;
	}

	// それ以外は何もしない（EnemyBossStateReactは存在しない）
}

float EnemyBoss::GetMaxHitPoint()
{
	return kHitPoint;
}

bool EnemyBoss::IsAlive()
{
	return _hitPoint > 0.0f;
}

bool EnemyBoss::CanAttack()
{
	return HasAttackAuthority()
		&& (_stateTransitionTime <= 0.0f)
		&& (_attackInterval <= 0.0f);
}

void EnemyBoss::DisableAttackCol()
{
	if (_attackCol1) _attackCol1->Disable();
	if (_attackCol2) _attackCol2->Disable();
}

bool EnemyBoss::IsStagger() const
{
	return _staggerPoint >= _staggerThreshold;
}

void EnemyBoss::ResetStaggerPoint()
{
	_staggerPoint = 0.0f;
}

void EnemyBoss::FireBressProjectile()
{
	if (_manager.expired()) return;

	// 頭部位置を計算する（足元位置 + 頭部オフセット）
	Position3 headPos = GetPos() + kHeadOffset;

	// プレイヤーへの方向を計算する
	Position3 playerPos = _manager.lock()->GetPlayerPos();
	Vector3 dir = playerPos - headPos;
	if (dir.SqrMagnitude() <= 0.0f) return;
	dir.Normalized();

	// ブレス弾を生成し初期化する
	auto projectile = std::make_shared<EnemyBossBressProjectile>(
		_physics,
		headPos,
		dir,
		GetParentPtr());
	projectile->Init();

	_bressProjectiles.emplace_back(projectile);
}

void EnemyBoss::UpdatePhase()
{
	// フェーズ2移行済みならスキップ
	if (_phaseChangedToTwo) return;

	// 体力割合が閾値を下回ったらフェーズ2へ移行する
	float hpRate = _hitPoint / GetMaxHitPoint();
	if (hpRate <= kPhase2HpRate)
	{
		_phase = 2;
		_phaseChangedToTwo = true;
		printf("Boss Phase 2!\n");
		// フェーズ移行を Idle ステートへの強制遷移で表現する
		// （ステート側でフェーズを参照して演出を切り替える）
	}
}

void EnemyBoss::UpdateStaggerPoint()
{
	if (_staggerPoint <= 0.0f) return;

	_staggerPoint -= kStaggerRecoveryPerSec / Statistics::kFPS;
	if (_staggerPoint < 0.0f)
	{
		_staggerPoint = 0.0f;
	}
}

void EnemyBoss::UpdateAttackCol()
{
	Matrix4x4 forward = GetModelMatrix();

	// Attack1 の位置更新
	if (_attackCol1 && _attackCol1->GetCollisionState())
	{
		Vector3 offset = Vector3(0, kAttack1ColRad, 0);
		Matrix4x4 mat = MatMultiple(forward, MatTranslate(offset));
		_attackCol1->PositionUpdate(mat);
		_attackCol1->Update();
	}

	// Attack2 の位置更新（足元中心なのでオフセットのみ）
	if (_attackCol2 && _attackCol2->GetCollisionState())
	{
		Vector3 offset = Vector3(0, kAttack2ColRad, 0);
		Matrix4x4 mat = MatMultiple(forward, MatTranslate(offset));
		_attackCol2->PositionUpdate(mat);
		_attackCol2->Update();
	}
}

void EnemyBoss::UpdateBressProjectiles()
{
	// 各ブレス弾を更新する
	for (auto& proj : _bressProjectiles)
	{
		if (proj && !proj->CanDelete())
		{
			proj->Update();
		}
	}

	// 削除フラグが立っているブレス弾を除去する
	std::erase_if(
		_bressProjectiles,
		[](const std::shared_ptr<EnemyBossBressProjectile>& p)
		{
			return !p || p->CanDelete();
		});
}

void EnemyBoss::UpdateStateTransitionTime()
{
	if (_stateTransitionTime <= 0.0f)
	{
		_stateTransitionTime = 0.0f;
		return;
	}
	_stateTransitionTime -= GetCurrentTimeScale() / Statistics::kFPS;
	if (_stateTransitionTime < 0.0f)
	{
		_stateTransitionTime = 0.0f;
	}
}

void EnemyBoss::UpdateAttackInterval()
{
	if (_attackInterval <= 0.0f)
	{
		_attackInterval = 0.0f;
		return;
	}
	_attackInterval -= GetCurrentTimeScale() / Statistics::kFPS;
	if (_attackInterval < 0.0f)
	{
		_attackInterval = 0.0f;
	}
}
