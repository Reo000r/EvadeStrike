#include "EnemyWeakStateRetreatWait.h"
#include "EnemyWeakStateIdle.h"
#include "EnemyWeakStateFire.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"

EnemyWeakStateRetreatWait::EnemyWeakStateRetreatWait(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent)
{
}

void EnemyWeakStateRetreatWait::OnEnter()
{
	SetStateTransitionTime(kPreFireWaitTime);

	// 移動量をなくす
	Stop();

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(WeakAnimData::kAnimNameIdle, true, 1.0f);
}

void EnemyWeakStateRetreatWait::Update()
{
	if (UpdateStateTransition()) return;

	// プレイヤーの方向を向く
	RotateToPlayer(kTurnSpeed);
}

void EnemyWeakStateRetreatWait::OnExit()
{
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateRetreatWait::CheckStateTransition()
{
	// 射撃行動権を失った場合はキャンセルし、Idleで次の行動を再判定する
	if (!GetParentPtr()->HasRetreatShootAuthority()) {
		return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
	}

	// 待機時間が経過したら射撃へ
	if (IsNothingStateTransitionTime()) {
		return std::make_shared<EnemyWeakStateFire>(GetParentPtr());
	}

	return nullptr;
}