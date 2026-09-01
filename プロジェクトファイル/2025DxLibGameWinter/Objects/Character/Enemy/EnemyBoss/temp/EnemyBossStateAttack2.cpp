#include "EnemyBossStateAttack2.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/Objects/AttackCol.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/SoundManager.h"

namespace
{
	// 溜めフェーズ（この範囲ではプレイヤーを向き続けるだけ）
	constexpr float kChargeEndRate = 0.4f;

	// 叩きつけ判定を出す進行度の範囲
	constexpr float kStartAttackColRate = 0.42f;
	constexpr float kEndAttackColRate   = 0.60f;

	// 衝撃波エフェクトのスケール
	constexpr float kShockwaveScale = 4.0f;
}

EnemyBossStateAttack2::EnemyBossStateAttack2(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent),
	_shockwaveSpawned(false)
{
}

void EnemyBossStateAttack2::OnEnter()
{
	// 移動量をゼロにして溜め動作に備える
	Stop();

	// 範囲攻撃アニメーションを再生する
	// （ボス専用アニメーション名があれば差し替えること）
	GetAnimator()->ChangeAnim(kAnimNameSpecialAttack2, false, 1.0f);

	// Attack2の攻撃判定を有効化する（まだ当たり判定は出さない）
	GetAttackCol2()->Enable();
	GetAttackCol2()->SetCollisionState(false);

	_shockwaveSpawned = false;
}

void EnemyBossStateAttack2::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	const std::shared_ptr<AnimationModel::AnimData> animData =
		GetAnimator()->GetCurrentAnimData();
	const float rate = animData->frame / animData->totalFrame;

	// 溜めフェーズ中はプレイヤーの方向を向き続ける
	if (rate < kChargeEndRate)
	{
		RotateToPlayer(kBossTurnSpeed);
	}

	// ジャスト回避中は攻撃判定を強制無効化する
	if (GetEnemyManager().lock()->ShouldBeDisableAttackCol())
	{
		if (GetAttackCol2()->GetCollisionState())
		{
			GetAttackCol2()->SetCollisionState(false);
		}
	}
	// 叩きつけタイミング：有効範囲内かつ未攻撃
	else if (rate > kStartAttackColRate && rate < kEndAttackColRate
		&& !GetAttackCol2()->GetCollisionState())
	{
		if (!GetAttackCol2()->IsAlreadyAttack())
		{
			GetAttackCol2()->SetCollisionState(true);
			SoundManager::GetInstance().PlaySoundType(SEType::AttackSwing);

			// 衝撃波エフェクトを足元に生成する（1度だけ）
			if (!_shockwaveSpawned)
			{
				_shockwaveSpawned = true;
				_shockwaveEffect = EffectManager::GetInstance().GenerateEffect(
					"Shockwave.efkefc", GetParentPtr()->GetPos());
				if (_shockwaveEffect.lock())
				{
					_shockwaveEffect.lock()->SetScale(
						Vector3(kShockwaveScale, kShockwaveScale, kShockwaveScale));
				}
			}
		}
	}
	// 叩きつけ判定を消すタイミング
	else if (rate > kEndAttackColRate && GetAttackCol2()->GetCollisionState())
	{
		GetAttackCol2()->SetCollisionState(false);
	}
}

void EnemyBossStateAttack2::OnExit()
{
	// 攻撃判定を完全に無効化する
	GetAttackCol2()->Disable();

	// 攻撃インターバルを設定する（長め）
	float interval = kBossAttack2Interval;
	if (GetPhase() >= 2)
	{
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
