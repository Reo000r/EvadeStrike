#pragma once
#include "EnemyBoss.h"

// 各ステートで共有する定数
namespace
{
	// 追跡速度
	constexpr float kBossChaseSpeed = 3.5f;

	// 攻撃時の踏み込み量
	constexpr float kBossEnterForwardVel = 11.0f;

	// 距離閾値
	constexpr float kBossAttack1Range = 3000.0f;	// 近接攻撃遷移距離
	constexpr float kBossAttack2Range = 800.0f;		// 範囲攻撃遷移距離
	constexpr float kBossAttack3Range = 1700.0f;	// ブレス攻撃遷移距離
	constexpr float kBossIdleRange = 2000.0f;	// 待機遷移距離
	constexpr float kBossChaseRange = kBossIdleRange + 2000.0f;	// 追跡遷移距離

	// 回転速度
	constexpr float kBossTurnSpeed = 0.05f;

	// ステート遷移待機時間
	constexpr float kBossStateTransitionTime = 1.2f;

	// 攻撃インターバル
	constexpr float kBossAttack1Interval = 2.0f;	// 近接
	constexpr float kBossAttack2Interval = 2.0f;	// 範囲
	constexpr float kBossAttack3Interval = 3.0f;	// ブレス

	// 被弾時タイムスケールを戻すフレーム
	constexpr int kReactDefaultScaleFrame = 4;

	// フェーズ2の攻撃インターバル倍率
	constexpr float kPhase2IntervalRate = 0.6f;
}

class AttackCol;
class EnemyBossBressProjectile;

/// <summary>
/// <para> ボスステートの基底クラス </para>
/// <para> EnemyWeakStateBase と同じ役割・設計を持つ、ボス専用の系統 </para>
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
	/// <para> ステート遷移確認を行う </para>
	/// <para> trueが帰ってきた場合、ステートが変わった後の為即returnすること </para>
	/// </summary>
	virtual bool UpdateStateTransition();

	/// <summary>
	/// <para> ステートの切り替えを行う </para>
	/// <para> 既存のステートは廃棄される </para>
	/// </summary>
	void ChangeState(std::shared_ptr<EnemyBossStateBase> nextState);

protected:

	std::weak_ptr<Physics> GetPhysics() const;
	const std::shared_ptr<AnimationModel> GetAnimator() const;
	const std::weak_ptr<EnemyManager> GetEnemyManager() const;

	Matrix4x4 GetForwardMatrix() const;
	std::shared_ptr<AttackCol> GetAttackColLight() const;
	std::shared_ptr<AttackCol> GetAttackColHeavy() const;

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

	std::weak_ptr<EnemyBossBressProjectile> FireBressProjectile(Vector3 dir);
	Vector3 GetMomentBressDir();

	bool IsGround();

	std::shared_ptr<EnemyBoss> GetParentPtr() const { return _parent.lock(); }

protected:
	/// <summary>
	/// <para> ステートの遷移条件を確認する </para>
	/// <para> 変更可能なステートがあればそのポインタを返す </para>
	/// <para> 変更がなければnullptrを返す </para>
	/// </summary>
	virtual std::shared_ptr<EnemyBossStateBase> CheckStateTransition() abstract;

private:
	// baseからのみアクセス可能
	std::weak_ptr<EnemyBoss> _parent;
};
