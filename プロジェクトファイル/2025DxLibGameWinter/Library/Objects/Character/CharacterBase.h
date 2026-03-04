#pragma once
#include "Library/Objects/GameObject.h"
#include "Library/Objects/AnimationModel.h"

class AttackableGameObject;

/// <summary>
/// キャラクターの基底クラス
/// </summary>
class CharacterBase abstract : public GameObject
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
	CharacterBase(
		std::weak_ptr<Physics> physics,
		PhysicsData::Priority priority,
		PhysicsData::GameObjectTag tag,
		PhysicsData::ColliderKind colliderKind,
		bool isTrigger,
		bool isCollision, 
		bool useGravity);
	virtual ~CharacterBase() = default;

	void SetColliderParent();

	/// <summary>
	/// ダメージを受ける処理
	/// </summary>
	/// <param name="attacker">攻撃を行った相手</param>
	virtual void TakeDamage(std::shared_ptr<AttackableGameObject> attacker) abstract;

	/// <summary>
	/// タイムスケールに応じて無敵時間更新
	/// </summary>
	void UpdateInvTime();
	/// <summary>
	/// ノックバックを移動に追加
	/// 地面についていれば移動量を減衰させる
	/// </summary>
	/// <param name="decayAmount">減衰量(0.0-1.0)</param>
	void UpdateKnockback(float decayAmount);
	/// <summary>
	/// ヒットストップを更新
	/// </summary>
	void UpdateHitStop();

	/// <summary>
	/// 現在の体力値を返す
	/// </summary>
	/// <returns></returns>
	float GetHitPoint() const { return _hitPoint; }
	/// <summary>
	/// 最大体力値を返す
	/// </summary>
	/// <returns></returns>
	virtual float GetMaxHitPoint() abstract;
	/// <summary>
	/// 生存状態を返す
	/// </summary>
	/// <returns></returns>
	virtual bool IsAlive() abstract;
	/// <summary>
	/// 無敵状態を返す
	/// </summary>
	/// <returns></returns>
	bool IsInv() const { return (_invincibilityTime > 0.0f); }
	/// <summary>
	/// 接地状態を返す
	/// </summary>
	/// <returns></returns>
	bool IsGround() const;
	
	/// <summary>
	/// <para> ノックバックベクトルを追加 </para>
	/// <para> リセットが呼ばれるまで適用 </para>
	/// <para> 地面に着いた場合減衰する </para>
	/// </summary>
	/// <param name="knockback"></param>
	/// <param name="dir"></param>
	void AddKnockback(Vector3 knockback, Vector3 dir);
	/// <summary>
	/// ノックバックベクトルをリセット
	/// </summary>
	void ResetKnockback();

protected:
	void SetHitPoint(float hitPoint);
	void Damage(float damage);

	float _hitPoint;	// 体力
	float _breakPoint;	// ノックバック耐久値
	Vector3 _knockbackVel;	// ノックバック時の進行量
	// ノックバックが登録されてから初めてのノックバックか
	// 地面判定競合の解決策
	bool _isFirstKnockback;

	float _invincibilityTime;	// 無敵時間
};

