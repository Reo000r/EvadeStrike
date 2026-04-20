#pragma once
#include "EnemyBossStateBase.h"

class EffekseerEffect;

/// <summary>
/// ボス範囲攻撃ステート（Attack2: 叩きつけ）
/// 溜め動作の後、足元を中心とした広範囲の叩きつけを行う
/// 衝撃波エフェクトを足元に生成する
/// </summary>
class EnemyBossStateAttack2 : public EnemyBossStateBase
{
public:
	EnemyBossStateAttack2(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateAttack2() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;

	// 叩きつけ時の衝撃波エフェクト
	std::weak_ptr<EffekseerEffect> _currentEffect;

	// 衝撃波エフェクトが既に生成されたか
	bool _effectSpawned;
};
