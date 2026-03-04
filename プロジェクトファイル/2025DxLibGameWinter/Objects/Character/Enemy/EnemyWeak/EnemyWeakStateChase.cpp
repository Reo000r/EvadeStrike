#include "EnemyWeakStateChase.h"
#include "EnemyWeakStateIdle.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Objects/Character/Enemy/EnemyManager.h"

EnemyWeakStateChase::EnemyWeakStateChase(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent)
{
}

void EnemyWeakStateChase::OnEnter()
{
	// 移動量をなくす
	Stop();

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameDash, true, 1.0f);
}

void EnemyWeakStateChase::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// プレイヤーの方向を向く
	RotateToPlayer(kTurnSpeed);

	// 向いている方向へ前進する
	AddTransformForward(kChaseSpeed);
}

void EnemyWeakStateChase::OnExit()
{
	// 処理なし
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateChase::CheckStateTransition()
{
	// 攻撃権を持っているなら
	if (GetParentPtr()->HasAttackAuthority()) {
		// プレイヤーが攻撃範囲にいるなら
		if (CanAttackRangeInPlayer(kAttackRangeInPlayer)) {
			// 待機ステートに入る
			return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
		}
	}
	else {
		// プレイヤーが待機遷移範囲にいるなら
		if (CanAttackRangeInPlayer(kIdleRangeInPlayer)) {
			// 待機ステートに入る
			return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
		}
		// 一定範囲内に別の敵がいるなら(正常な敵が帰ってきたなら)
		if (!(GetEnemyManager().lock()->IsExistEnemyWithinRange(GetParentPtr()->GetCenterPos(), kRangeEnemyDist)
			.expired())) {
			return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
		}
	}

	return nullptr;
}
