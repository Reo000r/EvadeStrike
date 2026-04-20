#include "EnemyBossStateChase.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"

EnemyBossStateChase::EnemyBossStateChase(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent)
{
}

void EnemyBossStateChase::OnEnter()
{
	// 移動量をゼロにする
	Stop();

	// 走りアニメーションを再生する
	GetAnimator()->ChangeAnim(BossAnimData::kAnimNameDash, true, 1.0f);
}

void EnemyBossStateChase::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// プレイヤーの方向へゆっくり回転する
	RotateToPlayer(kBossTurnSpeed);

	// 向いている方向へ前進する
	AddTransformForward(kBossChaseSpeed);
}

void EnemyBossStateChase::OnExit()
{
	// 処理なし
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateChase::CheckStateTransition()
{
	// 攻撃権がある場合は
	if (GetParentPtr()->HasAttackAuthority()) {
		// 近接攻撃範囲に入っていれば
		if (CanAttackRangeInPlayer(kBossIdleRange)) {
			// 待機ステートへ
			return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
		}
	}
	// 攻撃権がない場合は
	else {
		// 待機距離内に入っていれば
		if (CanAttackRangeInPlayer(kBossIdleRange)) {
			// 待機ステートへ
			return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
		}
	}

	return nullptr;
}
