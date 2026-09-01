#include "EnemyBossStateStagger.h"
#include "EnemyBossStateIdle.h"
#include "Objects/Character/Player/PlayerAnimationData.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"

namespace
{
	// スタンエフェクトの頭部オフセット
	const Vector3 kStaggerEffectOffset = Vector3(0, 450, 0);

	// スタンエフェクトのスケール
	constexpr float kStaggerEffectScale = 1.5f;

	// 怯み後の遷移待機時間（復帰後すぐ攻撃させない）
	constexpr float kStaggerRecoveryTransitionTime = 1.5f;
}

EnemyBossStateStagger::EnemyBossStateStagger(std::weak_ptr<EnemyBoss> parent) :
	EnemyBossStateBase(parent)
{
}

void EnemyBossStateStagger::OnEnter()
{
	// 移動量をゼロにする
	Stop();

	// 怯み（ダウン）アニメーションを再生する
	// ボス専用の怯みアニメーション名を使用することが望ましい
	// （ここでは kAnimNameReact を流用する）
	GetAnimator()->ChangeAnim(kAnimNameReact, false, 1.0f);
	GetAnimator()->ResetCurrentAnimProgress();

	// 3種すべての攻撃判定を無効化する
	GetParentPtr()->DisableAttackCol();

	// 怯み蓄積値をリセットする
	GetParentPtr()->ResetStaggerPoint();

	printf("Boss Stagger!\n");

	// スタンエフェクトを頭上に生成する
	Position3 effectPos = GetParentPtr()->GetPos() + kStaggerEffectOffset;
	_staggerEffect = EffectManager::GetInstance().GenerateEffect(
		"Stagger.efkefc", effectPos);
	if (_staggerEffect.lock())
	{
		_staggerEffect.lock()->SetScale(
			Vector3(kStaggerEffectScale, kStaggerEffectScale, kStaggerEffectScale));
		_staggerEffect.lock()->SetPlaySpeed(0.6f);
	}
}

void EnemyBossStateStagger::Update()
{
	// ステートが変わっていた場合return
	if (UpdateStateTransition()) return;

	// スタンエフェクトの位置をボスに追従させる
	if (_staggerEffect.lock() && _staggerEffect.lock()->IsPlaying())
	{
		_staggerEffect.lock()->SetPos(
			GetParentPtr()->GetPos() + kStaggerEffectOffset);
	}
}

void EnemyBossStateStagger::OnExit()
{
	// スタンエフェクトを停止する
	if (_staggerEffect.lock())
	{
		_staggerEffect.lock()->Stop();
	}

	// 怯みから復帰後、すぐ攻撃させないための遷移待機時間を設定する
	SetStateTransitionTime(kStaggerRecoveryTransitionTime);
}

std::shared_ptr<EnemyBossStateBase> EnemyBossStateStagger::CheckStateTransition()
{
	// 怯みアニメーションが終了していれば待機ステートへ
	if (!GetAnimator()->GetCurrentAnimData()->isEnd) return nullptr;

	return std::make_shared<EnemyBossStateIdle>(GetParentPtr());
}
