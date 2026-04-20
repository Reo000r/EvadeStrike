#pragma once
#include "EnemyBoss.h"

// 各ステートで共有する定数
namespace
{
	// 追跡速度（EnemyWeakより遅く重厚感を出す）
	constexpr float kBossChaseSpeed = 3.5f;

	// 攻撃時の踏み込み量
	constexpr float kBossEnterForwardVel = 10.0f;

	// 距離閾値
	constexpr float kBossAttack1Range = 700.0f;	// 近接攻撃遷移距離
	constexpr float kBossAttack2Range = 1200.0f;	// 範囲攻撃遷移距離
	constexpr float kBossAttack3Range = 2000.0f;	// ブレス攻撃遷移距離
	constexpr float kBossIdleRange   = 1500.0f;	// 待機遷移距離
	constexpr float kBossChaseRange  = kBossIdleRange + 2000.0f;	// 追跡遷移距離

	// 回転速度（EnemyWeakより遅く）
	constexpr float kBossTurnSpeed = 0.08f;

	// ステート遷移待機時間
	constexpr float kBossStateTransitionTime = 0.8f;

	// 攻撃インターバル
	constexpr float kBossAttack1Interval = 2.5f;	// 近接: 短め
	constexpr float kBossAttack2Interval = 5.0f;	// 範囲: 長め
	constexpr float kBossAttack3Interval = 6.0f;	// ブレス: 最長

	// アニメーション速度
	constexpr float kBossAttackAnimSpeed  = 0.4f;
	constexpr float kBossDeathAnimSpeed   = 1.5f;

	// 被弾時タイムスケールを戻すフレーム
	constexpr int kReactDefaultScaleFrame = 4;

	// ブレス弾の発射間隔（秒）
	constexpr float kBressFireInterval = 0.25f;

	// ブレス弾の発射回数
	constexpr int kBressFireCount = 8;

	// フェーズ2の攻撃インターバル倍率（短くして連続攻撃を激しくする）
	constexpr float kPhase2IntervalRate = 0.6f;
}

class AttackCol;
class EnemyBossBressProjectile;

/// <summary>
/// ボスステートの基底クラス
/// EnemyWeakStateBase と同じ役割・設計を持つ、ボス専用の系統
/// </summary>
class EnemyBossStateBase
{
public:
	EnemyBossStateBase(std::weak_ptr<EnemyBoss> parent);
	virtual ~EnemyBossStateBase() = default;

	/// <summary>
	/// ステート開始直後の処理
	/// </summary>
	virtual void OnEnter() abstract;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() abstract;
	/// <summary>
	/// ステート終了直前の処理
	/// </summary>
	virtual void OnExit() abstract;

	/// <summary>
	/// ステート遷移確認を行う
	/// trueが帰ってきた場合、ステートが変わった後の為即returnすること
	/// </summary>
	virtual bool UpdateStateTransition();

	/// <summary>
	/// ステートの切り替えを行う
	/// 既存のステートは廃棄される
	/// </summary>
	void ChangeState(std::shared_ptr<EnemyBossStateBase> nextState);

protected:

	std::weak_ptr<Physics> GetPhysics() const;
	const std::shared_ptr<AnimationModel> GetAnimator() const;
	const std::weak_ptr<EnemyManager> GetEnemyManager() const;

	Matrix4x4 GetForwardMatrix() const;
	std::shared_ptr<AttackCol> GetAttackCol1() const;
	std::shared_ptr<AttackCol> GetAttackCol2() const;

	void ReleasePhysics();

	bool CanAttack() const;
	float GetAttackInterval() const;
	float GetStateTransitionTime() const;

	int GetPhase() const;

	void AddTransformForward(float add) const;
	void Stop() const;

	/// <summary>
	/// プレイヤーの方向を向く(速度制限なし)
	/// </summary>
	void RotateToPlayer() { GetParentPtr()->RotateToPlayer(); }

	/// <summary>
	/// プレイヤーの方向を向く（速度制限あり）
	/// </summary>
	void RotateToPlayer(float speed) { GetParentPtr()->RotateToPlayer(speed); }

	bool CanAttackRangeInPlayer(float range);
	bool IsNothingStateTransitionTime() const;
	bool IsNothingAttackInterval() const;
	void SetStateTransitionTime(float time);
	void SetAttackInterval(float interval);

	void FireBressProjectile();
	
	bool IsGround();

	std::shared_ptr<EnemyBoss> GetParentPtr() const { return _parent.lock(); }

protected:
	/// <summary>
	/// ステートの遷移条件を確認する
	/// 変更可能なステートがあればそのポインタを返す
	/// 変更がなければnullptrを返す
	/// </summary>
	virtual std::shared_ptr<EnemyBossStateBase> CheckStateTransition() abstract;

private:
	// baseからのみアクセス可能
	std::weak_ptr<EnemyBoss> _parent;
};
