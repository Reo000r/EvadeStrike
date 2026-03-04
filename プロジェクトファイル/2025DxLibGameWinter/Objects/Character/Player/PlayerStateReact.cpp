#include "PlayerStateReact.h"
#include "PlayerStateIdle.h"
#include "PlayerStateDash.h"
#include "PlayerStateJump.h"

PlayerStateReact::PlayerStateReact(std::weak_ptr<Player> player) :
    PlayerStateBase(player)
{
}

void PlayerStateReact::OnEnter()
{
	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameReact, false, 1.0f);
}

void PlayerStateReact::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 待機
}

void PlayerStateReact::OnExit()
{
	// 処理なし
}

std::shared_ptr<PlayerStateBase> PlayerStateReact::CheckStateTransition()
{
	// アニメーションが終わっていなければreturn
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// ジャンプ入力があれば
	if (CanJumpInput()) {
		// ジャンプステートに入る
		return std::make_shared<PlayerStateJump>(GetPlayerPtr());
	}
	// 移動入力があれば
	if (CanDashInput()) {
		// 移動ステートに入る
		return std::make_shared<PlayerStateDash>(GetPlayerPtr());
	}
	// 移動入力がなければ
	if (!CanDashInput()) {
		// 待機ステートに入る
		return std::make_shared<PlayerStateIdle>(GetPlayerPtr());
	}
}
