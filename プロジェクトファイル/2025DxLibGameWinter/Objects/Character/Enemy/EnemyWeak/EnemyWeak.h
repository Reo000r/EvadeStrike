#pragma once
#include "Objects/Character/Enemy/EnemyBase.h"

class EnemyWeakStateBase;
class AttackCol;

class EnemyWeak : public EnemyBase
{
public:
	EnemyWeak(std::weak_ptr<Physics> physics, int modelHandle);
	virtual ~EnemyWeak();

	void Init() override;
	void Update() override;
	void Draw() const override;

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

	/// <summary>
	/// ダメージを受ける処理
	/// </summary>
	/// <param name="attacker">攻撃を行った相手</param>
	void TakeDamage(std::shared_ptr<AttackableGameObject> attacker) override;

	/// <summary>
	/// 最大体力値を返す
	/// </summary>
	/// <returns></returns>
	float GetMaxHitPoint() override;

	/// <summary>
	/// 生存状態を返す
	/// </summary>
	/// <returns></returns>
	bool IsAlive() override;
	/// <summary>
	/// 行動待機時間などを考慮した攻撃可否を返す
	/// </summary>
	/// <returns></returns>
	bool CanAttack() override;
	/// <summary>
	/// 攻撃判定を無効化する
	/// </summary>
	void DisableAttackCol() override;

private:
	/// <summary>
	/// 攻撃判定更新
	/// </summary>
	void UpdateAttackCol();
	/// <summary>
	/// ステート待機時間更新
	/// </summary>
	void UpdateStateTransitionTime();
	/// <summary>
	/// 攻撃待機時間更新
	/// </summary>
	void UpdateAttackInterval();

	// ステートが以下のメンバを変更可能にする
	friend EnemyWeakStateBase;

	/// <summary>
	/// ポインタを返す
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<EnemyWeak> GetParentPtr() { return std::static_pointer_cast<EnemyWeak>(shared_from_this()); }

	std::shared_ptr<EnemyWeakStateBase> _currentState;	// 現在のステート

	std::shared_ptr<AttackCol> _attackCol;				// 攻撃判定
};

