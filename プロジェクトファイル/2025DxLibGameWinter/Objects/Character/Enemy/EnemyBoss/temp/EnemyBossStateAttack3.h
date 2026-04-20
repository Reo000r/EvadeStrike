#pragma once
#include "EnemyBossStateBase.h"

/// <summary>
/// ボスブレス攻撃ステート（Attack3）
/// 頭部からプレイヤーへ向けて球体の当たり判定（BressProjectile）を
/// 一定間隔で連続発射する
/// 一定回数発射後にアニメーション終了を待って待機ステートへ遷移する
/// </summary>
class EnemyBossStateAttack3 : public EnemyBossStateBase
{
public:
	EnemyBossStateAttack3(std::weak_ptr<EnemyBoss> parent);
	~EnemyBossStateAttack3() = default;

	void OnEnter() override;
	void Update() override;
	void OnExit() override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;

	// 発射間隔タイマー（秒）
	float _fireTimer;

	// 残り発射回数
	int _remainFireCount;

	// 発射フェーズが終了したか（発射完了 → アニメーション終了待ち）
	bool _firingDone;
};
