#pragma once
#include "GameObject.h"
#include <list>

class CharacterBase;

/// <summary>
/// 攻撃可能なGameObject
/// </summary>
class AttackableGameObject : public GameObject
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="physics"></param>
	/// <param name="priority">位置補正の優先度</param>
	/// <param name="tag">タグ</param>
	/// <param name="colliderKind">当たり判定種別</param>
	/// <param name="isTrigger">トリガー</param>
	/// <param name="isCollision">当たり判定可否</param>
	/// <param name="useGravity">重力使用可否</param>
	AttackableGameObject(
		std::weak_ptr<Physics> physics,
		PhysicsData::Priority priority,
		PhysicsData::GameObjectTag tag,
		PhysicsData::ColliderKind colliderKind,
		bool isTrigger,
		bool isCollision,
		bool useGravity);
	virtual ~AttackableGameObject() = default;

	/// <summary>
	/// <para> 攻撃情報設定 </para>
	/// <para> Physicsへ登録も行う </para>
	/// </summary>
	/// <param name="attackPower">攻撃力</param>
	/// <param name="breakPower">削り値</param>
	/// <param name="knockbackVector">ノックバックベクトル</param>
	void EntryAttackStats(float attackPower, float breakPower, Vector3 knockbackVector);

	/// <summary>
	/// <para> 攻撃情報解放 </para>
	/// <para> Physicsの登録解除 </para>
	/// </summary>
	void ReleaseAttackStats();

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

	/// <summary>
	/// 武器の有効化
	/// </summary>
	/// <param name="attackPower">攻撃力</param>
	/// <param name="breakPower">削り値</param>
	/// <param name="knockbackVector">ノックバックベクトル</param>
	void Enable(float attackPower, float breakPower, Vector3 knockbackVector);
	/// <summary>
	/// 武器の無効化
	/// </summary>
	void Disable();

	/// <summary>
	/// <para> すでに攻撃していたか </para>
	/// <para> 攻撃済みのオブジェクトが0以上ならtrue </para>
	/// </summary>
	/// <returns></returns>
	bool IsAlreadyAttack() const;
	/// <summary>
	/// 攻撃履歴を消去
	/// </summary>
	void ClearAttackState();

	/// <summary>
	/// 所有者を設定する
	/// </summary>
	void SetOwnerStatus(std::weak_ptr<CharacterBase> owner);
	std::weak_ptr<CharacterBase> GetOwnerStatus() { return _owner; }

	void SetCollisionState(bool isCollision);
	bool GetCollisionState() const;

	float GetAttackPower() const { return _attackPower; }
	float GetBreakPower() const { return _breakPower; }
	Vector3 GetKnockbackForce() const { return _knockbackVector; }

protected:
	float _attackPower;			// 攻撃力
	float _breakPower;			// 削り値
	Vector3 _knockbackVector;	// ノックバックベクトル

	// 攻撃がヒットした相手のリスト
	std::list<std::weak_ptr<Collider>> _attackedColliders;

	std::weak_ptr<CharacterBase> _owner;		// 管理者
};

