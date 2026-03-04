#include "PlayerStateLanding.h"
#include "PlayerStateIdle.h"
#include "PlayerStateDash.h"
#include "PlayerStateJump.h"
#include "PlayerStateFall.h"
#include "PlayerStateDodge.h"
#include "PlayerStateDodgeFail.h"

PlayerStateLanding::PlayerStateLanding(std::weak_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateLanding::OnEnter()
{
	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameLanding, false, 1.0f);
	// アニメーション微調整
	GetAnimator()->Update(3.0f);
}

void PlayerStateLanding::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 回転
	Rotate();
	// 移動入力があれば
	if (CanDashInput()) {
		// 入力方向に移動
		AddTransformStick(kDashSpeed);
	}
}

void PlayerStateLanding::OnExit()
{
}

std::shared_ptr<PlayerStateBase> PlayerStateLanding::CheckStateTransition()
{
	// 接地していなければ
	if (!IsGround()) {
		// 落下ステートに入る
		return std::make_shared<PlayerStateFall>(GetPlayerPtr());
	}

	// 入力が効く状態でなければreturn
	if (!CanInputAllowance(kCanInputProgress)) return nullptr;

	// 回避入力があれば
	if (CanDodgeInput()) {
		// 回避成功していれば
		if (IsSuccessDodge()) {
			// 回避ステートに入る
			return std::make_shared<PlayerStateDodge>(GetPlayerPtr());
		}
		// 回避失敗であれば
		else {
			// 回避失敗ステートに入る
			return std::make_shared<PlayerStateDodgeFail>(GetPlayerPtr());
		}
	}
	// 接地しているかつ
	// ジャンプ入力があれば
	if (IsGround() && CanJumpInput()) {
		// ジャンプステートに入る
		return std::make_shared<PlayerStateJump>(GetPlayerPtr());
	}
	// 移動入力があれば
	if (CanDashInput()) {
		// 移動ステートに入る
		return std::make_shared<PlayerStateDash>(GetPlayerPtr());
	}

	// アニメーションが終わっていなければこれ以降は遷移しない
	if (!GetAnimator()->IsEnd(kAnimNameLanding)) return nullptr;

	// アニメーションが終わっていたら
	// 待機ステートに入る
	return std::make_shared<PlayerStateIdle>(GetPlayerPtr());

	return nullptr;
}
