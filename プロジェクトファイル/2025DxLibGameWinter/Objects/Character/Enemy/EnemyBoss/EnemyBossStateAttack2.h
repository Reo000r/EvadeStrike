#pragma once
#include "EnemyBossStateBase.h"

class EffekseerEffect;
class AttackRangeIndicator;

/// <summary>
/// ボス範囲攻撃ステート
/// </summary>
class EnemyBossStateAttack2 : public EnemyBossStateBase
{
public:
	EnemyBossStateAttack2(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateAttack2() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

	/// <summary>
	/// 攻撃範囲インジケーターを描画する
	/// </summary>
	void DrawAttackRangeIndicator() const override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;

	// エフェクト
	std::weak_ptr<EffekseerEffect> _currentEffect;

	// エフェクトが既に生成されたか
	bool _effectSpawned;

	// 攻撃範囲
	std::shared_ptr<AttackRangeIndicator> _rangeIndicator;
};
