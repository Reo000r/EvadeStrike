#pragma once
#include "EnemyBossStateBase.h"

class AttackCol;
class AttackRangeIndicator;

/// <summary>
/// ボス突進攻撃ステート
/// </summary>
class EnemyBossStateAttack1 : public EnemyBossStateBase
{
public:
	EnemyBossStateAttack1(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateAttack1() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

	/// <summary>
	/// 攻撃範囲インジケーターを描画する
	/// </summary>
	void DrawAttackRangeIndicator() const override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;

	// 攻撃範囲
	std::shared_ptr<AttackRangeIndicator> _rangeIndicator;
};
