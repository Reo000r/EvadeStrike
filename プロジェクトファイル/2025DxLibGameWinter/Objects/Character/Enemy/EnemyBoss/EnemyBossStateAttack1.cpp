#include "EnemyBossStateAttack1.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/Objects/AttackCol.h"
#include "Library/System/SoundManager.h"

namespace {
	// 踏み込み移動を行うアニメーション進行度の範囲
	constexpr float kStartForwardRate = 0.15f;
	constexpr float kEndForwardRate = 0.50f;

	// 攻撃判定を有効化する進行度の範囲
	constexpr float kStartAttackColRate = 0.15f;
	constexpr float kEndAttackColRate = 0.55f;
}

EnemyBossStateAttack1::EnemyBossStateAttack1(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent)
{
}

void EnemyBossStateAttack1::OnEnter()
{
	// 通常攻撃アニメーションを再生する
	GetAnimator()->ChangeAnim(BossAnimData::kAnimNameLightAttack, false, 1.0f);

	// Attack1の攻撃判定を有効化する（まだ当たり判定は出さない）
	GetAttackColLight()->Enable();
	GetAttackColLight()->SetCollisionState(false);
}

void EnemyBossStateAttack1::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	const std::shared_ptr<AnimationModel::AnimData> animData =
		GetAnimator()->GetCurrentAnimData();
	const float rate = animData->frame / animData->totalFrame;

	// 踏み込み移動
	if (rate > kStartForwardRate && 
		rate < kEndForwardRate) {
		AddTransformForward(kBossEnterForwardVel);
	}

	// ジャスト回避中は攻撃判定を強制的に無効化する
	if (GetEnemyManager().lock()->ShouldBeDisableAttackCol()) {
		if (GetAttackColLight()->GetCollisionState()) {
			GetAttackColLight()->SetCollisionState(false);
		}
	}
	// 有効範囲内なら
	else if (rate > kStartAttackColRate && rate < kEndAttackColRate
		&& !GetAttackColLight()->GetCollisionState()) {
		// 未攻撃なら
		if (!GetAttackColLight()->IsAlreadyAttack()) {
			// 攻撃判定を出す
			GetAttackColLight()->SetCollisionState(true);
			SoundManager::GetInstance().PlaySoundType(SEType::AttackSwing);
		}
	}
	// 有効範囲を過ぎたなら
	else if (rate > kEndAttackColRate && 
		GetAttackColLight()->GetCollisionState()) {
		// 攻撃判定を消す
		GetAttackColLight()->SetCollisionState(false);
	}
}

void EnemyBossStateAttack1::OnExit()
{
	// 攻撃判定を無効化する
	GetAttackColLight()->Disable();

	// 攻撃インターバルを設定する
	float interval = kBossAttack1Interval;
	if (GetPhase() >= 2) {
		interval *= kPhase2IntervalRate;
	}
	SetAttackInterval(interval);
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateAttack1::CheckStateTransition()
{
	// アニメーションが終了していなければ遷移しない
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// 終了後は待機ステートへ
	return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
}
