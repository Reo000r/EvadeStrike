#pragma once
#include "EnemyBossStateBase.h"

class AttackCol;

/// <summary>
/// ボス通常攻撃ステート（Attack1: 近接）
/// アニメーション進行度に応じて踏み込みと攻撃判定を制御する
/// </summary>
class EnemyBossStateAttack1 : public EnemyBossStateBase
{
public:
	EnemyBossStateAttack1(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateAttack1() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;
};
