#pragma once
#include "EnemyBossStateBase.h"

class EffekseerEffect;

/// <summary>
/// ボス怯みステート
/// 怯み蓄積値が閾値を超えたときに移行する
/// ダウンアニメーションを再生し、終了後に待機ステートへ遷移する
/// </summary>
class EnemyBossStateStagger : public EnemyBossStateBase
{
public:
	EnemyBossStateStagger(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateStagger() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;

	// スタンエフェクト
	std::weak_ptr<EffekseerEffect> _staggerEffect;
};
