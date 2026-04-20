#pragma once
#include "Objects/Character/Enemy/EnemyBase.h"
#include <array>

class EnemyBossStateBase;
class AttackCol;
class EnemyBossBressProjectile;

/// <summary>
/// ボス敵クラス
/// EnemyBaseを継承し、3種の攻撃と怯みシステムを持つ
/// </summary>
class EnemyBoss : public EnemyBase
{
public:
	EnemyBoss(std::weak_ptr<Physics> physics, int modelHandle);
	virtual ~EnemyBoss();

	void Init() override;
	void Update() override;
	void Draw() const override;

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

	/// <summary>
	/// ダメージを受ける処理
	/// </summary>
	/// <param name="attacker">攻撃を行った相手</param>
	void TakeDamage(std::shared_ptr<AttackableGameObject> attacker) override;

	/// <summary>
	/// 最大体力値を返す
	/// </summary>
	float GetMaxHitPoint() override;

	/// <summary>
	/// 生存状態を返す
	/// </summary>
	bool IsAlive() override;

	/// <summary>
	/// 行動待機時間などを考慮した攻撃可否を返す
	/// </summary>
	bool CanAttack() override;

	/// <summary>
	/// 攻撃判定を無効化する（3種すべて）
	/// </summary>
	void DisableAttackCol() override;

	/// <summary>
	/// 現在のフェーズを返す（1 or 2）
	/// </summary>
	int GetPhase() const { return _phase; }

	/// <summary>
	/// 怯み状態かどうかを返す
	/// </summary>
	bool IsStagger() const;

	/// <summary>
	/// 怯み蓄積値をリセットする
	/// </summary>
	void ResetStaggerPoint();

	/// <summary>
	/// ブレス弾を発射する
	/// 頭部位置からプレイヤーへ向かう球体判定を生成する
	/// </summary>
	void FireBressProjectile();

	/// <summary>
	/// アクティブなブレス弾リストを返す
	/// </summary>
	const std::vector<std::shared_ptr<EnemyBossBressProjectile>>& GetBressProjectiles() const
	{
		return _bressProjectiles;
	}

private:
	/// <summary>
	/// フェーズ更新処理
	/// 体力割合に応じてフェーズを変化させる
	/// </summary>
	void UpdatePhase();

	/// <summary>
	/// 怯み蓄積値を自然回復させる
	/// </summary>
	void UpdateStaggerPoint();

	/// <summary>
	/// 攻撃判定の位置更新
	/// </summary>
	void UpdateAttackCol();

	/// <summary>
	/// ブレス弾の更新・削除
	/// </summary>
	void UpdateBressProjectiles();

	/// <summary>
	/// ステート待機時間更新
	/// </summary>
	void UpdateStateTransitionTime();

	/// <summary>
	/// 攻撃待機時間更新
	/// </summary>
	void UpdateAttackInterval();

	// ステートがメンバを変更可能にする
	friend EnemyBossStateBase;

	/// <summary>
	/// 自身のshared_ptrを返す
	/// </summary>
	std::shared_ptr<EnemyBoss> GetParentPtr()
	{
		return std::static_pointer_cast<EnemyBoss>(shared_from_this());
	}

	// 現在のステート
	std::shared_ptr<EnemyBossStateBase> _currentState;

	// 攻撃判定（Attack1: 近接, Attack2: 範囲叩きつけ）
	std::shared_ptr<AttackCol> _attackCol1;
	std::shared_ptr<AttackCol> _attackCol2;

	// ブレス弾リスト
	std::vector<std::shared_ptr<EnemyBossBressProjectile>> _bressProjectiles;

	// フェーズ（1 or 2）
	int _phase;

	// フェーズ2移行済みフラグ（1度だけ移行演出を出すため）
	bool _phaseChangedToTwo;

	// 怯み蓄積値
	float _staggerPoint;

	// 現在の怯み閾値（怯むたびに引き上げる）
	float _staggerThreshold;

	// 怯み蓄積値の自然回復速度（毎秒）
	static constexpr float kStaggerRecoveryPerSec = 5.0f;

	// フェーズ1の怯み閾値
	static constexpr float kStaggerThresholdPhase1 = 30.0f;

	// 怯みのたびに増加する閾値量
	static constexpr float kStaggerThresholdIncrement = 10.0f;
};
