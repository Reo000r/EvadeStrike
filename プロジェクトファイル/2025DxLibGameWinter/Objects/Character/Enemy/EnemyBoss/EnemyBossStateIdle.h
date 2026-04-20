#pragma once
#include "EnemyBossStateBase.h"

class EffekseerEffect;

/// <summary>
/// ボス待機ステート
/// 行動を決定するハブステート
/// フェーズ・プレイヤー距離・攻撃権に応じて次の行動を選択する
/// </summary>
class EnemyBossStateIdle : public EnemyBossStateBase
{
public:
	EnemyBossStateIdle(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateIdle() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;

	// 攻撃予告エフェクト
	std::weak_ptr<EffekseerEffect> _attackNoticeEffect;
};
