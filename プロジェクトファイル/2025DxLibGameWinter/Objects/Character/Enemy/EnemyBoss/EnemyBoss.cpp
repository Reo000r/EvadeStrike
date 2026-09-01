#include "EnemyBoss.h"
#include "EnemyBossStateBase.h"
#include "EnemyBossStateSpawn.h"
#include "EnemyBossStateDeath.h"
#include "EnemyBossStateStagger.h"
#include "Objects/Projectile/EnemyBossBressProjectile.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Library/Objects/AttackCol.h"
#include "Library/Geometry/Calculation.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>
#include <algorithm>
#include <cassert>

namespace {
	// 体力
	constexpr float kHitPoint = 1000.0f;

	constexpr float kModelScale = 5.0f;

	// 当たり判定設定
	constexpr float kColRadius = 50.0f * kModelScale;
	constexpr float kColHeight = 175.0f * kModelScale;
	const Vector3 kColStartToEnd = Vector3Up() * (kColHeight - kColRadius * 2.0f);

	// モデル描画オフセット
	constexpr float kModelDrawOffsetAngle = DX_PI_F;
	const Vector3 kDrawOffset = Vector3(0, -kColRadius, 0);
	const Position3 kDefaultPos = Position3(0, 0, 2000);

	// 近接攻撃当たり判定
	constexpr float kAttack1ColRad = 40.0f * kModelScale;
	constexpr float kAttack1ColHeight = 70.0f * kModelScale;
	const Vector3   kAttack1ColOffset = Vector3(0, 0, 15) * kModelScale;
	const Vector3   kAttack1Scale = Vector3(1, 1, 1);
	const Vector3   kAttack1Angle = Vector3Zero();
	constexpr float kAttack1Power = 20.0f;

	// 範囲攻撃当たり判定
	constexpr float kAttack2ColRad = 150.0f * kModelScale;
	constexpr float kAttack2ColHeight = 50.0f * kModelScale;
	const Vector3   kAttack2ColOffset = 
		Vector3(0, -kAttack2ColRad / kModelScale * 0.5f, 0);
	const Vector3   kAttack2Scale = Vector3(1, 1, 1);
	const Vector3   kAttack2Angle = Vector3Up();
	constexpr float kAttack2Power = 30.0f;

	// ノックバック減衰量
	constexpr float kKnockbackDecayAmount = 0.0f;

	// ブレス弾の発射起点
	const Vector3 kHeadOffset = Vector3(0, kColHeight * 0.7f, 0);

	// フェーズ2移行体力割合
	constexpr float kPhase2HpRate = 0.5f;

	// 怯み蓄積値の自然回復速度（毎秒）
	constexpr float kStaggerRecoveryPerSec = 5.0f;
	// フェーズ1の怯み閾値
	constexpr float kStaggerThresholdPhase1 = 150.0f;
	// 怯みのたびに増加する閾値量
	constexpr float kStaggerThresholdIncrement = 50.0f;
	
	// ヒットストップ時のモデル振動量
	constexpr float kShakeMagnitude = 5.0f * kModelScale;
}

EnemyBoss::EnemyBoss(std::weak_ptr<Physics> physics, int modelHandle) :
	EnemyBase(physics, modelHandle, 
		PhysicsData::Priority::High, 
		PhysicsData::GameObjectTag::EnemyBoss),
	_currentState(),
	_attackColLight(nullptr),
	_attackColHeavy(nullptr),
	_bressProjectiles(),
	_phaseNum(1),
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
	MV1SetScale(_animator->GetHandle(), Vector3(1, 1, 1) * kModelScale);
	MV1SetRotationXYZ(_animator->GetHandle(),
		Vector3(0, _rotAngleY + kModelDrawOffsetAngle, 0));

	// 使用するアニメーションを全て登録する
	_animator->SetAnimData(BossAnimData::kAnimNameSpawn,		BossAnimData::kSpawnAnimSpeed, false);
	_animator->SetAnimData(BossAnimData::kAnimNameIdle,			BossAnimData::kBaseAnimSpeed, true);
	_animator->SetAnimData(BossAnimData::kAnimNameDash,			BossAnimData::kBaseAnimSpeed, true);
	_animator->SetAnimData(BossAnimData::kAnimNameLightAttack,	BossAnimData::kAttackLightAnimSpeed, false);
	_animator->SetAnimData(BossAnimData::kAnimNameHeavyAttack,	BossAnimData::kAttackHeavyAnimSpeed, false);
	_animator->SetAnimData(BossAnimData::kAnimNameBressAttack,	BossAnimData::kAttackBressAnimSpeed, false);
	_animator->SetAnimData(BossAnimData::kAnimNameReact,		BossAnimData::kReactAnimSpeed, false);
	_animator->SetAnimData(BossAnimData::kAnimNameDeath,		BossAnimData::kDeathAnimSpeed, false);

	// 最初のアニメーションを設定する
	_animator->SetStartAnim(BossAnimData::kAnimNameSpawn);
}

EnemyBoss::~EnemyBoss()
{
}

void EnemyBoss::Init()
{
	// 当たり判定と自身を紐づける
	SetColliderParent();

	// 近接攻撃判定を生成する
	AttackColStats statsFir;
	statsFir.colRad = kAttack1ColRad;
	statsFir.colHeight = kAttack1ColHeight;
	statsFir.transOffset = kAttack1ColOffset;
	statsFir.scale = kAttack1Scale;
	statsFir.angle = kAttack1Angle;
	statsFir.attackPower = kAttack1Power;
	_attackColLight = std::make_shared<AttackCol>(_physics, PhysicsData::GameObjectTag::EnemyAttack);
	_attackColLight->SetAttackData(statsFir);
	_attackColLight->SetOwnerStatus(GetParentPtr());

	// 範囲攻撃判定を生成する
	AttackColStats statsSec;
	statsSec.colRad = kAttack2ColRad;
	statsSec.colHeight = kAttack2ColHeight;
	statsSec.transOffset = kAttack2ColOffset;
	statsSec.scale = kAttack2Scale;
	statsSec.angle = kAttack2Angle;
	statsSec.attackPower = kAttack2Power;
	_attackColHeavy = std::make_shared<AttackCol>(_physics, PhysicsData::GameObjectTag::EnemyAttack);
	_attackColHeavy->SetAttackData(statsSec);
	_attackColHeavy->SetOwnerStatus(GetParentPtr());

	// 初期ステート設定
	_currentState = std::make_shared<EnemyBossStateSpawn>(GetParentPtr());
	_currentState->OnEnter();

	_animator->SetDrawOffset(kDrawOffset);
}

void EnemyBoss::Update()
{
	// 無敵時間の更新
	UpdateInvTime();

	// ヒットストップ中であれば
	if (IsHitStop()) {
		_animator->SetDrawOffset(-GetHitStopShakeVec() + kDrawOffset);
		UpdateHitStop();

		if (IsHitStop()) {
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
}

void EnemyBoss::TakeDamage(std::shared_ptr<AttackableGameObject> attacker,
	bool isReact)
{
	// 無敵状態ならreturn
	if (IsInv()) return;

	// ダメージを受ける
	Damage(attacker->GetAttackPower());
	printf("BossHP : %.02f\n", GetHitPoint());

	// プレイヤーの攻撃が当たったことをManagerへ通知する
	HitPlayerAttack();

	// ヒットストップ
	SetHitStop(8, true, kShakeMagnitude);
	_animator->SetDrawOffset(GetHitStopShakeVec() + kDrawOffset);

	// 被弾時のタイムスケールを一瞬戻す
	SetToDefaultScaleFrame(kReactDefaultScaleFrame);

	// 体力が尽きた場合は死亡ステートへ
	if (!IsAlive()) {
		_currentState->ChangeState(
			std::make_shared<EnemyBossStateDeath>(GetParentPtr()));
		return;
	}

	// 怯み蓄積値を増やす
	_staggerPoint += attacker->GetBreakPower();

	// 怯み閾値を超えていれば怯みステートへ
	if (IsStagger()) {
		ResetStaggerPoint();
		// 次回の閾値を引き上げる
		_staggerThreshold += kStaggerThresholdIncrement;
		
		_currentState->ChangeState(
			std::make_shared<EnemyBossStateStagger>(GetParentPtr()));
		return;
	}
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
	if (_attackColLight) _attackColLight->Disable();
	if (_attackColHeavy) _attackColHeavy->Disable();
}

bool EnemyBoss::IsStagger() const
{
	return _staggerPoint >= _staggerThreshold;
}

void EnemyBoss::ResetStaggerPoint()
{
	_staggerPoint = 0.0f;
}

std::weak_ptr<EnemyBossBressProjectile> EnemyBoss::FireBressProjectile(Vector3 dir)
{
	std::shared_ptr<EnemyBossBressProjectile> projectile;

	if (_manager.expired()) return projectile;

	// 発射位置
	Position3 headPos = GetPos() + kHeadOffset;
	if (dir.SqrMagnitude() <= 0.0f) return projectile;
	dir.Normalized();

	// ブレス弾生成
	projectile = std::make_shared<EnemyBossBressProjectile>(
		_physics,
		headPos,
		dir);
	projectile->SetOwnerStatus(GetParentPtr());
	projectile->Init();

	_bressProjectiles.emplace_back(projectile);

	return projectile;
}

Vector3 EnemyBoss::GetMomentBressDir()
{
	// 発射位置
	Position3 headPos = GetPos() + kHeadOffset;
	// プレイヤーへの方向
	Position3 playerPos = _manager.lock()->GetPlayerPos();
	return (playerPos - headPos);
}

void EnemyBoss::UpdatePhase()
{
	// フェーズ2移行済みならreturn
	if (_phaseChangedToTwo) return;

	// 体力割合が閾値を下回ったらフェーズ進行
	float hpRate = _hitPoint / GetMaxHitPoint();
	if (hpRate <= kPhase2HpRate) {
		_phaseNum = 2;
		_phaseChangedToTwo = true;
		printf("Bossフェーズ移行n");
	}
}

void EnemyBoss::UpdateStaggerPoint()
{
	if (_staggerPoint <= 0.0f) return;

	_staggerPoint -= kStaggerRecoveryPerSec / Statistics::kFPS;
	if (_staggerPoint < 0.0f) {
		_staggerPoint = 0.0f;
	}
}

void EnemyBoss::UpdateAttackCol()
{
	Matrix4x4 forward = GetModelMatrix();

	// 近接攻撃の位置更新
	if (_attackColLight && _attackColLight->GetCollisionState()) {
		Vector3 offset = Vector3(0, kAttack1ColRad, 0);
		Matrix4x4 mat = MatMultiple(forward, MatTranslate(offset));
		_attackColLight->PositionUpdate(mat);
		_attackColLight->Update();
	}

	// 範囲攻撃の位置更新
	if (_attackColHeavy && _attackColHeavy->GetCollisionState()) {
		Vector3 offset = Vector3(0, kAttack2ColRad, 0);
		Matrix4x4 mat = MatMultiple(forward, MatTranslate(offset));
		_attackColHeavy->PositionUpdate(mat);
		_attackColHeavy->Update();
	}
}

void EnemyBoss::UpdateBressProjectiles()
{
	// 各ブレス弾を更新する
	for (auto& proj : _bressProjectiles) {
		if (proj && !proj->CanDelete()) {
			proj->Update();
		}
	}

	// 削除フラグが立っているブレス弾を削除
	std::erase_if(
		_bressProjectiles,
		[](const std::shared_ptr<EnemyBossBressProjectile>& p)
		{
			return !p || p->CanDelete();
		});
}

void EnemyBoss::UpdateStateTransitionTime()
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

void EnemyBoss::UpdateAttackInterval()
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
