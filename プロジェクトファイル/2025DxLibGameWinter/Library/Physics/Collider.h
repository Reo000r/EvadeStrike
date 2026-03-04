#pragma once
#include <memory>
#include "Library/Geometry/Vector3.h"
#include "ProjectSettings.h"
#include "Library/Physics/Rigidbody.h"
#include "Library/Physics/ColliderData.h"

class Physics;
class ColliderData;
class GameObject;

/// <summary>
/// 当たり判定
/// </summary>
class Collider : public std::enable_shared_from_this<Collider> {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="priority">位置補正の優先度</param>
	/// <param name="tag">タグ</param>
	/// <param name="colliderKind">当たり判定種別</param>
	/// <param name="isTrigger">トリガー</param>
	/// <param name="isCollision">当たり判定可否</param>
	/// <param name="useGravity">重力使用可否</param>
	Collider(
		PhysicsData::Priority priority, 
		PhysicsData::GameObjectTag tag, 
		PhysicsData::ColliderKind colliderKind, 
		bool isTrigger,
		bool isCollision, 
		bool useGravity);

	~Collider() = default;
	void EntryPhysics(std::weak_ptr<Physics> physics);
	void ReleasePhysics();

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider);

	PhysicsData::GameObjectTag GetTag() const	{ return _tag; }
	// 位置補正優先度情報を返す
	PhysicsData::Priority GetPriority() const	{ return _priority; }
	
	Vector3 GetPos() const { return _rigidbody->GetPos(); }
	Vector3 GetCenterPos() const;
	Vector3 CalcNextPos() const;
	Vector3 GetVel() const { return _rigidbody->GetVel(); }
	Vector3 GetDir() const { return _rigidbody->GetDir(); }
	std::shared_ptr<ColliderData> GetColData() const { return _colliderData; }
	bool UseGravity() const { return _rigidbody->UseGravity(); }
	float GetGravityScale() const { return _rigidbody->GetGravityScale(); }
	bool IsCollision() const { return _colliderData->IsCollision(); }
	std::shared_ptr<GameObject> GetParent() const { return _parent; }
	
	void SetPos(const Position3& set) const { _rigidbody->SetPos(set); }
	void SetVel(const Vector3& set) const { _rigidbody->SetVel(set); }
	void SetPosY(const float set) const { _rigidbody->SetPosY(set); }
	void SetVelY(const float set) const { _rigidbody->SetVelY(set); }
	void AddPos(const Position3& add) const { _rigidbody->AddPos(add); }
	void AddVel(const Vector3& add) const { _rigidbody->AddVel(add); }
	void SetColData(std::shared_ptr<ColliderData> data) { _colliderData = data; }
	void SetUseGravity(bool set) const { _rigidbody->SetUseGravity(set); }
	void SetGravityScale(float scale) const { _rigidbody->SetGravityScale(scale); }
	void SetParent(std::shared_ptr<GameObject> parent) { _parent = parent; }
	void SetCollisionState(bool isCollision) { _colliderData->SetCollisionState(isCollision); }
	
	bool IsFloor() const { return _isFloor; };
	bool IsWall() const { return _isWall; };
	void SetIsFloor(bool isFloor) { _isFloor = isFloor; };
	void SetIsWall(bool isWall) { _isWall = isWall; };

	/// <summary>
	/// 当たり判定を無視するタグの追加
	/// </summary>
	/// <param name="tag"></param>
	void AddThroughTag(PhysicsData::GameObjectTag tag);
	/// <summary>
	/// 当たり判定を無視するタグの削除
	/// </summary>
	/// <param name="tag"></param>
	void RemoveThroughTag(PhysicsData::GameObjectTag tag);

	/// <summary>
	/// 球用の当たり判定を作成する
	/// </summary>
	/// <returns></returns>
	void CreateColliderDataSphere(
		float radius, 
		bool isTrigger, bool isCollision);
	/// <summary>
	/// カプセル用の当たり判定を作成する
	/// </summary>
	/// <returns></returns>
	void CreateColliderDataCapsule(
		float radius, Vector3 startToEnd, 
		bool isTrigger, bool isCollision);
	/// <summary>
	/// ポリゴン用の当たり判定を作成する
	/// </summary>
	/// <returns></returns>
	void CreateColliderDataPolygon(int modelHandle, 
		bool isTrigger, bool isCollision);

	/// <summary>
	/// 球用の当たり判定を編集する
	/// </summary>
	/// <returns></returns>
	void SetColliderDataSphere(
		float radius, 
		bool isTrigger, bool isCollision);
	/// <summary>
	/// カプセル用の当たり判定を編集する
	/// </summary>
	/// <returns></returns>
	void SetColliderDataCapsule(
		float radius, Vector3 startToEnd, 
		bool isTrigger, bool isCollision);
	/// <summary>
	/// ポリゴン用の当たり判定を編集する
	/// </summary>
	/// <returns></returns>
	void SetColliderDataPolygon(int modelHandle,
		bool isTrigger, bool isCollision);

private:
	// 以下の変数はこのクラスからアクセス可能
	friend Physics;

	std::shared_ptr<GameObject> _parent;	// 自身の管理者
	std::shared_ptr<Rigidbody> _rigidbody;
	std::shared_ptr<ColliderData> _colliderData;

	std::weak_ptr<Physics> _physics;

	PhysicsData::GameObjectTag	_tag;
	// 位置補正優先度情報
	PhysicsData::Priority _priority;

private:

	bool _isFloor;	// 床に触れているか
	bool _isWall;	// 壁に触れているか
	//Position3 _nextPos;
};

