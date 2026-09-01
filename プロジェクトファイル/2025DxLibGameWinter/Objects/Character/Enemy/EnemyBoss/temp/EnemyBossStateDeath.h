#pragma once
#include "EnemyBossStateBase.h"

class EffekseerEffect;

/// <summary>
/// ボス死亡ステート
/// 死亡アニメーションとフェードアウトを行い、
/// ボス討伐イベントを通知する
/// </summary>
class EnemyBossStateDeath : public EnemyBossStateBase
{
public:
	EnemyBossStateDeath(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateDeath() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;

	// 死亡時エフェクト
	std::weak_ptr<EffekseerEffect> _defeatEffect;
};
