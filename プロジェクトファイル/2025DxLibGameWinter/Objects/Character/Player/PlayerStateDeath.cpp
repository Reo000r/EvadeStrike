#include "PlayerStateDeath.h"

namespace {
	// 透明度変更を開始するフレーム
	constexpr float kOpacityStartRate = 0.0f;
}

PlayerStateDeath::PlayerStateDeath(std::weak_ptr<Player> player) :
	PlayerStateBase(player)
{
}

void PlayerStateDeath::OnEnter()
{
	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameDeath, false, 1.0f);
}

void PlayerStateDeath::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	std::shared_ptr<AnimationModel::AnimData> data =
		GetAnimator()->GetCurrentAnimData();
	float rate = data->frame / data->totalFrame;
	// 開始条件を満たしていれば
	if (rate >= kOpacityStartRate) {
		// 0除算しなさそうなら
		if (kOpacityStartRate < 1.0f) {
			rate /= (1.0f - kOpacityStartRate);
		}
	}
	// 0.0f-1.0fから1.0f-0.0fに変換する
	rate = (rate * -1.0f) + 1.0f;

	// 1.0不透明 0.0透明
	MV1SetOpacityRate(GetAnimator()->GetHandle(), rate);

	if (rate <= 0.0f) {
		GetPlayerPtr()->SetAliveState(false);
	}
}

void PlayerStateDeath::OnExit()
{
	// 処理なし
}

std::shared_ptr<PlayerStateBase> PlayerStateDeath::CheckStateTransition()
{
	// ステートは変わらない
	return nullptr;
}
