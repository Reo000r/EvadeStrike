#include "EnemyWeakStateReact.h"
#include "EnemyWeakStateIdle.h"
#include "Objects/Character/Player/PlayerAnimationData.h"

EnemyWeakStateReact::EnemyWeakStateReact(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent)
{
}

void EnemyWeakStateReact::OnEnter()
{
	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameReact, false, 1.0f);
	GetAnimator()->ResetCurrentAnimProgress();

	// プレイヤーの方向を向く
	RotateToPlayer();
}

void EnemyWeakStateReact::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 待機
}

void EnemyWeakStateReact::OnExit()
{
	// 処理なし
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateReact::CheckStateTransition()
{
	// アニメーションが終わっていないならreturn
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// アニメーションが終わっている場合
	// 待機ステートに入る
	return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
}
