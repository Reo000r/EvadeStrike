#include "EnemyWeakStateSpawn.h"
#include "EnemyWeakStateIdle.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Library/Geometry/Easing.h"
#include <DxLib.h>

namespace {
	// 拡縮アニメーション進行度の範囲
	constexpr float kStartScaleRate = 0.0f;
	constexpr float kEndScaleRate = 0.6f;
}

EnemyWeakStateSpawn::EnemyWeakStateSpawn(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent),
	_targetScale(1.0f)
{
}

void EnemyWeakStateSpawn::OnEnter()
{
	// 移動量をなくす
	Stop();

	// アニメーションを変更する
	GetAnimator()->ChangeAnim(WeakAnimData::kAnimNameSpawn, false, 1.0f);

	// プレイヤーの方向を向く
	RotateToPlayer(DX_PI_F);

	// 開始時の拡縮を保存
	Vector3 scale = MV1GetScale(GetAnimator()->GetHandle());
	_targetScale = scale.x;
	MV1SetScale(GetAnimator()->GetHandle(), Vector3(0,0,0));
}

void EnemyWeakStateSpawn::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// プレイヤーの方向を向く
	RotateToPlayer(DX_PI_F);

	const std::shared_ptr<AnimationModel::AnimData> animData =
		GetAnimator()->GetCurrentAnimData();
	float rate = animData->frame / animData->totalFrame;

	// 拡縮アニメーション
	if (rate > kStartScaleRate &&
		rate < kEndScaleRate) {
		float scaleRate = rate / (kEndScaleRate - kStartScaleRate);
		scaleRate = Easing::Get(scaleRate, EasingType::EaseOutBack);
		MV1SetScale(GetAnimator()->GetHandle(),
			Vector3(1, 1, 1) * _targetScale * scaleRate);
	}


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
