#pragma once
#include "EnemyBossStateBase.h"

class EffekseerEffect;

/// <summary>
/// ボス出現ステート
/// 登場アニメーションを再生し、終了後に待機ステートへ遷移する
/// </summary>
class EnemyBossStateSpawn : public EnemyBossStateBase
{
public:
	EnemyBossStateSpawn(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateSpawn() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;

	// 出現時エフェクト
	std::weak_ptr<EffekseerEffect> _spawnEffect;

	float _targetScale;
};
