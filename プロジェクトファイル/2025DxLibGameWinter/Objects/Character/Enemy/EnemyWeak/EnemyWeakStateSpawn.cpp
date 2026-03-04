#include "EnemyWeakStateSpawn.h"
#include "EnemyWeakStateIdle.h"
#include "Objects/Character/Player/PlayerAnimationData.h"

EnemyWeakStateSpawn::EnemyWeakStateSpawn(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent)
{
}

void EnemyWeakStateSpawn::OnEnter()
{
	// 移動量をなくす
	Stop();

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameSpecialAttack2, false, 1.0f);

	// プレイヤーの方向を向く
	RotateToPlayer();
}

void EnemyWeakStateSpawn::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 待機
}

void EnemyWeakStateSpawn::OnExit()
{
	// 処理なし
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateSpawn::CheckStateTransition()
{
	// アニメーションが終わっていないならreturn
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// アニメーションが終わっている場合
	// 待機ステートに入る
	return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
}
