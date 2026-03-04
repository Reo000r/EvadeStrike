#include "PlayerStateFall.h"
#include "PlayerStateLanding.h"
#include "PlayerStateAirDodge.h"
#include "PlayerAnimationData.h"

namespace {
	const Position3 kDrawOffset = Position3(0, 120, 0);
}

PlayerStateFall::PlayerStateFall(std::weak_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateFall::OnEnter()
{
	// アニメーションを変更する
	GetAnimator()->SetDrawOffset(
		kDrawOffset + GetAnimator()->GetDrawOffset());
	GetAnimator()->ChangeAnim(kAnimNameFalling, false, 1.0f);
	// アニメーション微調整
	GetAnimator()->Update(5.0f);
}

void PlayerStateFall::Update()
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

void PlayerStateFall::OnExit()
{
	GetAnimator()->SetDrawOffset(
		GetAnimator()->GetDrawOffset() - kDrawOffset);
}

std::shared_ptr<PlayerStateBase> PlayerStateFall::CheckStateTransition()
{
	// 接地していたら
	if (IsGround()) {
		// 着地ステートに入る
		return std::make_shared<PlayerStateLanding>(GetPlayerPtr());
	}
	// 攻撃入力があったら
	if (CanAttackInput()) {
		PlayerActionKind actionKind = PlayerActionKind::None;
		if (CanPunchAttackInput()) {
			actionKind = PlayerActionKind::Punch;
		}
		else if (CanKickAttackInput()) {
			actionKind = PlayerActionKind::Kick;
		}
		// 遷移先のデータのみが入った
		PlayerAttackAnimData data = GetDataLoader()->GetBeforeAtackAnimData();
		// コンボ遷移判定を行い、返ってきたステートに遷移させる
		std::shared_ptr<PlayerStateBase> state = CheckAirComboTransition(actionKind, data);
		// 遷移すべきなら
		if (state != nullptr) {
			// ステート遷移
			return state;
		}
	}
	// 回避入力があったら
	if (CanDodgeInput()) {
		// 空中回避ステートに入る
		return std::make_shared<PlayerStateAirDodge>(GetPlayerPtr());
	}

	return nullptr;
}
