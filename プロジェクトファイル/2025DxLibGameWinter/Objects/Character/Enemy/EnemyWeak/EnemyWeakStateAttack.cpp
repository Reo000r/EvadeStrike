#include "EnemyWeakStateAttack.h"
#include "EnemyWeakStateIdle.h"
#include "Objects/Character/Enemy/EnemyManager.h"
#include "Library/Objects/AttackCol.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Library/System/SoundManager.h"
#include <cassert>

namespace{
	constexpr float kStartTransformFrameRate = 0.0f;
	constexpr float kEndTransformFrameRate = 0.2f;

	constexpr float kStartAttackColActiveFrameRate = 0.1f;
	constexpr float kEndAttackColActiveFrameRate = 0.5f;
}

EnemyWeakStateAttack::EnemyWeakStateAttack(std::weak_ptr<EnemyWeak> parent, 
	std::shared_ptr<AttackCol> attackCol) :
	EnemyWeakStateBase(parent),
	_attackCol(attackCol)
{
}

void EnemyWeakStateAttack::OnEnter()
{
	// アニメーションを変更する
	GetAnimator()->ChangeAnim(kAnimNameHeavyAttack1, false, 1.0f);

	// 武器の攻撃判定を生成する
	_attackCol->Enable();
	// 当たり判定はまだ行わない
	_attackCol->SetCollisionState(false);
}

void EnemyWeakStateAttack::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// 攻撃時の移動と
	// 攻撃判定の切り替えを行う

	const std::shared_ptr<AnimationModel::AnimData> animData = GetAnimator()->GetCurrentAnimData();
	// アニメーション進行度
	const float currentAnimFrameRate = animData->frame / animData->totalFrame;

	// 攻撃時の移動が行えるなら
	if (currentAnimFrameRate > kStartTransformFrameRate &&
		currentAnimFrameRate < kEndTransformFrameRate) {
		// 向いている方向へ前進する
		AddTransformForward(kEnterForwardVel);
	}

	// 攻撃判定を無効化しなければいけないなら
	if (GetEnemyManager().lock()->ShouldBeDisableAttackCol()) {
		// 攻撃判定が出ているなら
		if (_attackCol->GetCollisionState()) {
			// 攻撃判定を消す
			_attackCol->SetCollisionState(false);
		}
	}
	// 攻撃判定を出していいかつ
	// 攻撃判定が出ていないなら
	else if ((currentAnimFrameRate > kStartAttackColActiveFrameRate &&
		currentAnimFrameRate < kEndAttackColActiveFrameRate) &&
		!_attackCol->GetCollisionState()) {
		// 武器がまだ攻撃を行っていないなら
		if (!_attackCol->IsAlreadyAttack()) {
			// 攻撃判定を出す
			_attackCol->SetCollisionState(true);
			SoundManager::GetInstance().PlaySoundType(SEType::AttackSwing);
		}
	}
	// 攻撃判定を消すタイミングになっているかつ
	// 攻撃判定が出ているなら
	else if (currentAnimFrameRate > kEndAttackColActiveFrameRate &&
		_attackCol->GetCollisionState()) {
		// 攻撃判定を消す
		_attackCol->SetCollisionState(false);
	}
}

void EnemyWeakStateAttack::OnExit()
{
	// 攻撃判定を消す
	_attackCol->Disable();

	// 攻撃待機時間を設定
	SetAttackInterval(kAttackInterval);
}

std::shared_ptr<EnemyWeakStateBase> EnemyWeakStateAttack::CheckStateTransition()
{
	// アニメーションが終わっていなければreturn
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	// アニメーションが終わっていれば
	// 待機ステートに入る
	return std::make_shared<EnemyWeakStateIdle>(GetParentPtr());
}
