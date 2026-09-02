#pragma once
#include "EnemyBossStateBase.h"

class EnemyBossBressProjectile;
class AttackRangeIndicator;

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

	/// <summary>
	/// 攻撃範囲を描画する
	/// </summary>
	void DrawAttackRangeIndicator() const override;

private:
	std::shared_ptr<EnemyBossStateBase> CheckStateTransition() override;

	void UpdateBress();

	/// <summary>
	/// 弾を1発発射した際にその弾用の攻撃範囲を生成する
	/// </summary>
	void SpawnIndicatorForProjectile(const std::weak_ptr<EnemyBossBressProjectile>&projectile, Vector3 fireDir);
	
	/// <summary>
	/// 発射済みの各攻撃範囲の不透明度を更新する
	/// </summary>
	void UpdateIndicators();

	// 発射した弾ごとの攻撃範囲
	struct BreathIndicatorEntry {
		std::shared_ptr<AttackRangeIndicator> indicator;
		float elapsedTime = 0.0f;
	};

	// 発射間隔タイマー
	float _fireTimer;
	// 残り発射回数
	int _remainFireCount;
	// 発射フェーズが終了したか
	bool _firingDone;
	// 発射方向
	Vector3 _fireDir;

	std::vector<std::weak_ptr<EnemyBossBressProjectile>> _projectileList;

	std::vector<BreathIndicatorEntry> _rangeIndicators;
};
