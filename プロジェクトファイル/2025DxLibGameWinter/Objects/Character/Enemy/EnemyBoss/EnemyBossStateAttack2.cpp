#include "EnemyBossStateAttack2.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/Objects/AttackCol.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/SoundManager.h"

namespace {
	// 溜めフェーズ（この範囲ではプレイヤーを向き続けるだけ）
	constexpr float kChargeEndRate = 0.3f;

	// 判定を出す進行度の範囲
	constexpr float kStartAttackColRate = 0.3f;
	constexpr float kEndAttackColRate = 0.35f;

	//エフェクトのスケール
	constexpr float kEffectScale = 5.0f;
	constexpr float kEffectPlaySpeed = 2.0f;
}

EnemyBossStateAttack2::EnemyBossStateAttack2(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent),
	_effectSpawned(false)
{
}

void EnemyBossStateAttack2::OnEnter()
{
	// 移動量をなくす
	Stop();

	// 範囲攻撃アニメーションを再生する
	GetAnimator()->ChangeAnim(BossAnimData::kAnimNameHeavyAttack, false, 1.0f);

	// 攻撃判定を有効化する
	GetAttackColHeavy()->Enable();
	GetAttackColHeavy()->SetCollisionState(false);

	_effectSpawned = false;
}

void EnemyBossStateAttack2::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	const std::shared_ptr<AnimationModel::AnimData> animData =
		GetAnimator()->GetCurrentAnimData();
	const float rate = animData->frame / animData->totalFrame;

	// 溜めフェーズ中はプレイヤーの方向を向き続ける
	if (rate < kChargeEndRate) {
		RotateToPlayer(kBossTurnSpeed);
	}

	// ジャスト回避中は攻撃判定を無効化する
	if (GetEnemyManager().lock()->ShouldBeDisableAttackCol()) {
		if (GetAttackColHeavy()->GetCollisionState()) {
			GetAttackColHeavy()->SetCollisionState(false);
		}
	}
	// 攻撃タイミング
	// 有効範囲内かつ未攻撃
	else if (rate > kStartAttackColRate && rate < kEndAttackColRate
		&& !GetAttackColHeavy()->GetCollisionState()) {
		if (!GetAttackColHeavy()->IsAlreadyAttack()) {
			GetAttackColHeavy()->SetCollisionState(true);
			SoundManager::GetInstance().PlaySoundType(SEType::AttackSwing);

			// エフェクトを足元に生成する
			if (!_effectSpawned) {
				_effectSpawned = true;
				_currentEffect = EffectManager::GetInstance().GenerateEffect(
					"Shockwave.efkefc", GetParentPtr()->GetPos());
				if (!_currentEffect.expired()) {
					_currentEffect.lock()->SetScale(
						Vector3(kEffectScale, kEffectScale, kEffectScale));
					_currentEffect.lock()->SetPlaySpeed(kEffectPlaySpeed);
				}
			}
		}
	}
	// 判定を消すタイミング
	else if (rate > kEndAttackColRate && 
		GetAttackColHeavy()->GetCollisionState()) {
		GetAttackColHeavy()->SetCollisionState(false);
	}
}

void EnemyBossStateAttack2::OnExit()
{
	// 攻撃判定を完全に無効化する
	GetAttackColHeavy()->Disable();

	// 攻撃インターバルを設定する
	float interval = kBossAttack2Interval;
	if (GetPhase() >= 2) {
		interval *= kPhase2IntervalRate;
	}
	SetAttackInterval(interval);
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateAttack2::CheckStateTransition()
{
	// アニメーションが終了していなければ遷移しない
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// 終了後は待機ステートへ
	return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
}
