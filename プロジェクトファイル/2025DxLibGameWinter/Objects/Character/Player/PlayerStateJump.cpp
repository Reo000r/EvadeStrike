#include "PlayerStateJump.h"
#include "PlayerStateFall.h"
//#include "PlayerStateAirAttack.h"
//#include "PlayerStateAirDodge.h"
#include "PlayerAnimationData.h"

PlayerStateJump::PlayerStateJump(std::weak_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateJump::OnEnter()
{
	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameJump, false, 1.0f);

	// 上昇
	AddTransformUp(kJumpForce);
}

void PlayerStateJump::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 回転
	Rotate();
	// 移動入力があれば
	if (CanDashInput()) {
		// 入力方向に移動
		AddTransformStick(kAirMoveSpeed);
	}
}

void PlayerStateJump::OnExit()
{
}

std::shared_ptr<PlayerStateBase> PlayerStateJump::CheckStateTransition()
{
	bool isEndAnim = GetAnimator()->GetCurrentAnimData()->isEnd;
	
	// 上昇アニメーションが終わっていたら
	if (isEndAnim) {
		// 落下ステートに入る
		return std::make_shared<PlayerStateFall>(GetPlayerPtr());
	}
	// 攻撃入力があったら
	if (false) {
		// 空中攻撃ステートに入る
		//return std::make_shared<PlayerStateAirAttack>(GetPlayerPtr());
	}
	// 回避入力があったら
	if (false) {
		// 空中回避ステートに入る
		//return std::make_shared<PlayerStateAirDodge>(GetPlayerPtr());
	}
	// 接地していたら
	if (false) {
		// 着地ステートに入る
		//return std::make_shared<PlayerState >(GetPlayerPtr());
	}

	return nullptr;
}
