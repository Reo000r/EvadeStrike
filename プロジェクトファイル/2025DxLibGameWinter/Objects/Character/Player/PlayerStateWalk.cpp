#include "PlayerStateWalk.h"

PlayerStateWalk::PlayerStateWalk(std::weak_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateWalk::OnEnter()
{
	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameIdle, false, 1.0f);
}

void PlayerStateWalk::Update()
{
	// ステート遷移が可能か確認
	std::shared_ptr<PlayerStateBase> nextState = CheckStateTransition();
	// ステート遷移が可能な場合
	if (nextState != nullptr) {
		// ステート変更後は処理を書かない
		ChangeState(nextState);
		return;
	}

	// 待機
}

void PlayerStateWalk::OnExit()
{
	// 処理なし
}

std::shared_ptr<PlayerStateBase> PlayerStateWalk::CheckStateTransition()
{
	std::shared_ptr<PlayerStateBase> ret = nullptr;

	return ret;
}