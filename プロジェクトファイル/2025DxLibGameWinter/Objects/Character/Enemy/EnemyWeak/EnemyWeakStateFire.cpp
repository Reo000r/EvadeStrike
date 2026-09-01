#include "EnemyWeakStateFire.h"
#include "EnemyWeakStateIdle.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"

namespace {
	// 射撃アニメーション中、弾を発射するタイミング(進行度)
	constexpr float kFireProjectileFrameRate = 0.25f;
}

EnemyWeakStateFire::EnemyWeakStateFire(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent),
	_hasFired(false)
{
}

void EnemyWeakStateFire::OnEnter()
{
	_hasFired = false;

	// 射撃アニメーションに変更する
	GetAnimator()->ChangeAnim(WeakAnimData::kAnimNameFire, false, 1.0f);
}

void EnemyWeakStateFire::Update()
{
	if (UpdateStateTransition()) return;

	// 狙いを定め続ける
	RotateToPlayer(0.2f);

	const std::shared_ptr<AnimationModel::AnimData> animData = GetAnimator()->GetCurrentAnimData();
	const float currentAnimFrameRate = animData->frame / animData->totalFrame;

	// 発射タイミングに到達し、まだ発射していなければ発射する
	if (!_hasFired && currentAnimFrameRate >= kFireProjectileFrameRate) {
		GetParentPtr()->FireProjectile();
		_hasFired = true;
	}
}

void EnemyWeakStateFire::OnExit()
{
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateFire::CheckStateTransition()
{
	// アニメーションが終わっていなければキャンセルしない
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// 終了していればIdleに戻り、次の行動を再判定する
	return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
}