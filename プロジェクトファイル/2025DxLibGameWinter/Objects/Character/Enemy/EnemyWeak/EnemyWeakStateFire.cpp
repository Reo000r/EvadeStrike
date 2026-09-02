#include "EnemyWeakStateFire.h"
#include "EnemyWeakStateIdle.h"
#include "Objects/Character/Enemy/EnemyAnimationData.h"
#include "Library/System/AttackRangeIndicator.h"

namespace {
	// 射撃アニメーション中、弾を発射するタイミング(進行度)
	constexpr float kFireProjectileFrameRate = 0.25f;

	// 攻撃範囲
	constexpr float kIndicatorWidth = 150.0f;
	constexpr float kIndicatorLength = 1800.0f;
}

EnemyWeakStateFire::EnemyWeakStateFire(std::weak_ptr<EnemyWeak> parent) :
	EnemyWeakStateBase(parent),
	_hasFired(false),
	_rangeIndicator(std::make_shared<AttackRangeIndicator>())
{
}

void EnemyWeakStateFire::OnEnter()
{
	_hasFired = false;

	// 射撃アニメーションに変更する
	GetAnimator()->ChangeAnim(WeakAnimData::kAnimNameFire, false, 1.0f);

	// 攻撃範囲を初期化する
	_rangeIndicator->InitAsRect(
		GetParentPtr()->GetPos(), kIndicatorWidth, kIndicatorLength,
		GetParentPtr()->GetRotAngleY());
}

void EnemyWeakStateFire::Update()
{
	if (UpdateStateTransition()) return;

	const std::shared_ptr<AnimationModel::AnimData> animData = GetAnimator()->GetCurrentAnimData();
	const float currentAnimFrameRate = animData->frame / animData->totalFrame;

	// 弾を発射するまでは狙いを定め続ける
	if (!_hasFired) {
		RotateToPlayer(0.2f);
		_rangeIndicator->UpdateRectTransform(
			GetParentPtr()->GetPos(), GetParentPtr()->GetRotAngleY());
	}

	// 発射タイミングに合わせて不透明度を変更
	_rangeIndicator->UpdateAlphaByRate(currentAnimFrameRate, 0.0f, kFireProjectileFrameRate);

	// 発射タイミングに到達し、まだ発射していなければ発射する
	if (!_hasFired && currentAnimFrameRate >= kFireProjectileFrameRate) {
		GetParentPtr()->FireProjectile();
		_hasFired = true;
	}
}

void EnemyWeakStateFire::OnExit()
{
}

void EnemyWeakStateFire::DrawAttackRangeIndicator() const
{
	_rangeIndicator->Draw();
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateFire::CheckStateTransition()
{
	// アニメーションが終わっていなければキャンセルしない
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// 終了していればIdleに戻り、次の行動を再判定する
	return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
}