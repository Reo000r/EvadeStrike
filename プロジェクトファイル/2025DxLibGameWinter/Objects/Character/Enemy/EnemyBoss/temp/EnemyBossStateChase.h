#pragma once
#include "EnemyBossStateBase.h"

/// <summary>
/// ボス追跡ステート
/// プレイヤーを追いかける
/// </summary>
class EnemyBossStateChase : public EnemyBossStateBase
{
public:
	EnemyBossStateChase(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateChase() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;
};
