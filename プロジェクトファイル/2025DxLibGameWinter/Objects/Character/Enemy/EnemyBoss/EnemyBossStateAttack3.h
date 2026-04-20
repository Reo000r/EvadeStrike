#pragma once
#include "EnemyBossStateBase.h"

class EnemyBossBressProjectile;

/// <summary>
/// ボスブレス攻撃ステート
/// プレイヤーへ向けてブレスを発射する
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

	void UpdateBress();

	// 発射間隔タイマー
	float _fireTimer;
	// 残り発射回数
	int _remainFireCount;
	// 発射フェーズが終了したか
	bool _firingDone;
	// 発射方向
	Vector3 _fireDir;

	std::vector<std::weak_ptr<EnemyBossBressProjectile>> _projectileList;
};
