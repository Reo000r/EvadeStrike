#include "PlayerStateIdle.h"
#include "PlayerStateDash.h"
#include "PlayerStateJump.h"
#include "PlayerStateAttack.h"
#include "PlayerStateDodge.h"
#include "PlayerStateDodgeFail.h"
#include "PlayerAnimationData.h"

namespace {
}

PlayerStateIdle::PlayerStateIdle(std::weak_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateIdle::OnEnter()
{
	// プレイヤーの移動量をなくす
	Stop();

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameIdle, true, 1.0f);
}

void PlayerStateIdle::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 待機
}

void PlayerStateIdle::OnExit()
{
	// 処理なし
}

std::shared_ptr<PlayerStateBase> PlayerStateIdle::CheckStateTransition()
{
	// 攻撃入力があれば
	if (CanPunchAttackInput()) {
		// 通常攻撃ステートに入る
		const PlayerActionKind kind = PlayerActionKind::Punch;
		PlayerAttackAnimData data = GetDataLoader()->GetAttackAnimData(GetPunchAttackID());
		std::shared_ptr<AttackCol> col = GetAttackCol(kind);
		return std::make_shared<PlayerStateAttack>(GetPlayerPtr(), data,
			col, kind);
	}
	if (CanKickAttackInput()) {
		// 強攻撃ステートに入る
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
	return nullptr;
}
