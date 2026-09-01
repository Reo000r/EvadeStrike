#include "EnemyWeakStateRetreatMove.h"
#include "EnemyWeakStateIdle.h"
#include "EnemyWeakStateRetreatWait.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Library/System/Statistics.h"

EnemyWeakStateRetreatMove::EnemyWeakStateRetreatMove(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent),
	_timer(kRetreatMoveTimeLimit)
{
}

void EnemyWeakStateRetreatMove::OnEnter()
{
	_timer = kRetreatMoveTimeLimit;

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(WeakAnimData::kAnimNameDash, true, 1.0f);
}

void EnemyWeakStateRetreatMove::Update()
{
	if (UpdateStateTransition()) return;

	// プレイヤーの反対方向(進行方向)を向く
	RotateOppositeToPlayer(kTurnSpeed);

	// 向いている方向(反対方向)へ前進する
	AddTransformForward(kRetreatMoveSpeed);

	// タイマーを減算
	_timer -= GetParentPtr()->GetCurrentTimeScale() / Statistics::kFPS;
	if (_timer < 0.0f) _timer = 0.0f;
}

void EnemyWeakStateRetreatMove::OnExit()
{
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateRetreatMove::CheckStateTransition()
{
	// 射撃行動権を失った場合はキャンセルし、Idleで次の行動を再判定する
	if (!GetParentPtr()->HasRetreatShootAuthority()) {
		return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
	}

	// 指定距離に到達したら待機へ
	if (IsRetreatDistanceSatisfied()) {
		return std::make_shared<EnemyWeakStateRetreatWait>(GetParentPtr());
	}

	// 時間制限に達したら、指定距離未達でも待機へ強制移行する
	// (プレイヤーに追いつかれる、壁際で移動できない等でずっと逃げ続けることを防ぐ)
	if (_timer <= 0.0f) {
		return std::make_shared<EnemyWeakStateRetreatWait>(GetParentPtr());
	}

	return nullptr;
}