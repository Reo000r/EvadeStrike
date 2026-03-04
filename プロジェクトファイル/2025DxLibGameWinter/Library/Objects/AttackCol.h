#pragma once
#include "AttackableGameObject.h"
#include "Library/Geometry/Vector3.h"
#include "Library/Geometry/Matrix4x4.h"
#include "AttackColStats.h"

/// <summary>
/// 攻撃判定そのものを使いたい場合に用いる
/// </summary>
class AttackCol : public AttackableGameObject
{
public:
	AttackCol(std::weak_ptr<Physics> physics, 
		PhysicsData::GameObjectTag attackTag);
	virtual ~AttackCol();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const override;

	/// <summary>
	/// 攻撃判定データ初期化
	/// </summary>
	/// <param name="stats">攻撃判定データ</param>
	void SetAttackData(AttackColStats stats);
	/// <summary>
	/// 攻撃判定データを返す
	/// </summary>
	/// <returns></returns>
	AttackColStats GetAttackData() const { return _attackData; }
	/// <summary>
	/// 当たり判定データ初期化
	/// </summary>
	/// <param name="stats">当たり判定データ</param>
	void SetColliderData(std::shared_ptr<ColliderData> stats);
	/// <summary>
	/// 当たり判定データを返す
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<ColliderData> GetColliderData() const { return _collider->GetColData(); }

	/// <summary>
	/// 攻撃判定更新
	/// </summary>
	/// <param name="parentWorldMatrix">ワールド行列とみなす行列</param>
	void PositionUpdate(Matrix4x4 parentWorldMatrix);

	/// <summary>
	/// 攻撃判定の有効化
	/// </summary>
	void Enable();
	/// <summary>
	/// 攻撃判定の無効化
	/// </summary>
	void Disable();

private:
	AttackColStats _attackData;	// 攻撃判定データ
};

