#include "PlayerStateDash.h"
#include "PlayerAnimationData.h"
#include "PlayerStateIdle.h"
#include "PlayerStateJump.h"
#include "PlayerStateFall.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDodge.h"
#include "PlayerStateDodgeFail.h"
#include "Player.h"
#include <DxLib.h>

PlayerStateDash::PlayerStateDash(std::weak_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateDash::OnEnter()
{
	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameDash, true, 1.0f);
}

void PlayerStateDash::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 回転
	Rotate();
	// 入力方向に移動
	AddTransformStick(kDashSpeed);
}

void PlayerStateDash::OnExit()
{
}

std::shared_ptr<PlayerStateBase> PlayerStateDash::CheckStateTransition()
{
	// 接地していなければ
	if (!IsGround()) {
		// 落下ステートに入る
		return std::make_shared<PlayerStateFall>(GetPlayerPtr());
	}
	// 攻撃入力があれば
	if (CanPunchAttackInput()) {
		// 移動時に攻撃を入力すると追加で前進する
		//AddTransformForward(kDashToAttackVel);
		// 攻撃ステートに入る
		const PlayerActionKind kind = PlayerActionKind::Punch;
		PlayerAttackAnimData data = GetDataLoader()->GetAttackAnimData(GetPunchAttackID());
		std::shared_ptr<AttackCol> col = GetAttackCol(kind);
		return std::make_shared<PlayerStateAttack>(GetPlayerPtr(), data, 
			col, kind);
	}
	if (CanKickAttackInput()) {
		// 移動時に攻撃を入力すると追加で前進する
		//AddTransformForward(kDashToAttackVel);
		// 攻撃ステートに入る
		const PlayerActionKind kind = PlayerActionKind::Kick;
		PlayerAttackAnimData data = GetDataLoader()->GetAttackAnimData(GetKickAttackID());
		std::shared_ptr<AttackCol> col = GetAttackCol(kind);
		return std::make_shared<PlayerStateAttack>(GetPlayerPtr(), data, 
			col, kind);
	}
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
	// 移動入力がなければ
	if (!CanDashInput()) {
		// 待機ステートに入る
		return std::make_shared<PlayerStateIdle>(GetPlayerPtr());
	}

	return nullptr;
}
