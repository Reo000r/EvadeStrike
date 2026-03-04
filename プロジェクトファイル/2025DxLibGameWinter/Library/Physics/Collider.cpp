#include "Collider.h"
#include "Physics.h"
#include "Rigidbody.h"
#include "ColliderDataSphere.h"
#include "ColliderDataCapsule.h"
#include "ColliderDataPolygon.h"
#include "Library/Objects/GameObject.h"

#include <cassert>

Collider::Collider(PhysicsData::Priority priority, PhysicsData::GameObjectTag tag, 
	PhysicsData::ColliderKind colliderKind, bool isTrigger, bool isCollision, bool useGravity) :
	_priority(priority),
	_tag(tag),
	_parent(nullptr),
	_rigidbody(std::make_shared<Rigidbody>()),
	_colliderData(nullptr),
	_isFloor(false),
	_isWall(false)
{
	_rigidbody->Init(useGravity);
}

void Collider::EntryPhysics(std::weak_ptr<Physics> physics)
{
	if (_colliderData == nullptr) {
		assert(false && "colliderDataが作られていない");
		return;
	}

	_physics = physics;
	std::shared_ptr<Collider> thisptr = shared_from_this();
	// Physicsに自身を登録
	_physics.lock()->Entry(thisptr);
}

void Collider::ReleasePhysics()
{
	// Physicsから自身を登録解除
	_physics.lock()->Release(shared_from_this());
}

void Collider::OnCollide(const std::weak_ptr<Collider> collider)
{
	// 正常な親がいるなら
	if (_parent != nullptr) {
		// GameObjectのOnCollideを呼ぶ
		_parent->OnCollide(collider);
	}
}

Vector3 Collider::GetCenterPos() const
{
	PhysicsData::ColliderKind colKind = _colliderData->GetKind();
	Position3 retPos = _rigidbody->GetPos();

	switch (colKind) {
	case PhysicsData::ColliderKind::Sphere: {
		// 処理なし
		break;
	}
	case PhysicsData::ColliderKind::Capsule: {
		// 中点を返す
		std::shared_ptr<ColliderDataCapsule> colliderDataCapsule = std::static_pointer_cast<ColliderDataCapsule>(_colliderData);
		Position3 startPos = colliderDataCapsule->GetStartPos(retPos);
		Position3 endPos = colliderDataCapsule->GetEndPos(retPos);
		retPos = (startPos + endPos) * 0.5f;
		break;
	}
	case PhysicsData::ColliderKind::Polygon: {
		// 処理なし
		break;
	}
	default: {
		assert(false && "不明な当たり判定種別");
		break;
	}
	}


	return retPos;
}

Vector3 Collider::CalcNextPos() const
{
	// タイムスケール
	float timeScale = 1.0f;	// デフォルト値
	// 正常な親がいればタイムスケールを取得
	if (_parent != nullptr) {
		timeScale = _parent->GetCurrentTimeScale();
	}

	return _rigidbody->CalcNextPos(timeScale);
}

void Collider::AddThroughTag(PhysicsData::GameObjectTag tag)
{
	_colliderData->AddThroughTag(tag);
}

void Collider::RemoveThroughTag(PhysicsData::GameObjectTag tag)
{
	_colliderData->RemoveThroughTag(tag);
}

void Collider::CreateColliderDataSphere(float radius, bool isTrigger, bool isCollision)
{
	if (_colliderData != nullptr) {
		assert(false && "colliderDataは既に作られている");
		return;
	}

	_colliderData = std::make_shared<ColliderDataSphere>(
		isTrigger, isCollision, radius);
}

void Collider::CreateColliderDataCapsule(float radius, Vector3 startToEnd, bool isTrigger, bool isCollision)
{
	if (_colliderData != nullptr) {
		assert(false && "colliderDataは既に作られている");
		return;
	}

	_colliderData = std::make_shared<ColliderDataCapsule>(
		isTrigger, isCollision, radius, startToEnd);
}

void Collider::CreateColliderDataPolygon(int modelHandle, bool isTrigger, bool isCollision)
{
	if (_colliderData != nullptr) {
		assert(false && "colliderDataは既に作られている");
		return;
	}

	_colliderData = std::make_shared<ColliderDataPolygon>(
		isTrigger, isCollision, modelHandle);
}

void Collider::SetColliderDataSphere(float radius, bool isTrigger, bool isCollision)
{
	if (_colliderData == nullptr) {
		assert(false && "colliderDataが作られていない");
		return;
	}

	// 共通のデータを変更
	_colliderData->_isTrigger = isTrigger;
	_colliderData->_isCollision = isCollision;

	std::shared_ptr<ColliderDataSphere> colliderDataSphere = std::static_pointer_cast<ColliderDataSphere>(_colliderData);
	colliderDataSphere->_radius = radius;
	_colliderData = colliderDataSphere;
}

void Collider::SetColliderDataCapsule(float radius, Vector3 startToEnd, bool isTrigger, bool isCollision)
{
	if (_colliderData == nullptr) {
		assert(false && "colliderDataが作られていない");
		return;
	}

	// 共通のデータを変更
	_colliderData->_isTrigger = isTrigger;
	_colliderData->_isCollision = isCollision;

	std::shared_ptr<ColliderDataCapsule> colliderDataCapsule = std::static_pointer_cast<ColliderDataCapsule>(_colliderData);
	colliderDataCapsule->_radius = radius;
	colliderDataCapsule->_startToEnd = startToEnd;
	_colliderData = colliderDataCapsule;
}

void Collider::SetColliderDataPolygon(int modelHandle, bool isTrigger, bool isCollision)
{
	if (_colliderData == nullptr) {
		assert(false && "colliderDataが作られていない");
		return;
	}

	// 共通のデータを変更
	_colliderData->_isTrigger = isTrigger;
	_colliderData->_isCollision = isCollision;

	std::shared_ptr<ColliderDataPolygon> colliderDataPolygon = std::static_pointer_cast<ColliderDataPolygon>(_colliderData);
	colliderDataPolygon->_modelHandle = modelHandle;
	_colliderData = colliderDataPolygon;
}
