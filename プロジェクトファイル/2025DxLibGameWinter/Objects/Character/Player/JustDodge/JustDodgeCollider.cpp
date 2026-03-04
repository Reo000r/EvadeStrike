#include "JustDodgeCollider.h"
#include "Library/Physics/Rigidbody.h"
#include "Library/Geometry/Matrix4x4.h"
#include "Library/Physics/Physics.h"
#include "Library/Physics/ColliderData.h"
#include "Library/Physics/ColliderDataSphere.h"

JustDodgeCollider::JustDodgeCollider(std::weak_ptr<Physics> physics) :
    GameObject(physics, PhysicsData::Priority::Static, 
        PhysicsData::GameObjectTag::PlayerJustDodge, PhysicsData::ColliderKind::Sphere,
        true, false, false),
    _isHit(false),
    _posOffset(Vector3Zero())
{
}

JustDodgeCollider::~JustDodgeCollider()
{
}

void JustDodgeCollider::Init()
{
}

void JustDodgeCollider::Update()
{
}

void JustDodgeCollider::Draw() const
{
}

void JustDodgeCollider::OnCollide(const std::weak_ptr<Collider> collider)
{
	// 相手が不明な場合は何もしない
	if (collider.expired()) {
		return;
	}

	std::shared_ptr<Collider> other = collider.lock();

	// 敵の攻撃出ないなら何もしない
	if (other->GetTag() != PhysicsData::GameObjectTag::EnemyAttack) {
		return;
	}

	// 敵の攻撃判定だったので
	// メンバをtrueにする
	_isHit = true;
}

void JustDodgeCollider::UpdatePosition(Position3 pos)
{
    _collider->SetPos(pos + _posOffset);
}

void JustDodgeCollider::Enable()
{
	// colliderが既に登録されていれば
	if (_physics.lock()->IsRegistered(_collider)) {
		// 古いcolliderは登録解除する
		ReleasePhysics();
	}
	// 登録
	EntryPhysics(_physics);

	// Colliderと自身(GameObject)を紐づける
	_collider->SetParent(shared_from_this());
	// 攻撃判定を有効化
	_collider->SetCollisionState(true);

	_isHit = false;
}

void JustDodgeCollider::Disable()
{
	// 攻撃判定を有効化
	_collider->SetCollisionState(false);

	// colliderが有効でなければreturn
	if (!_collider) return;
	// colliderが登録されていなければreturn
	if (!_physics.lock()->IsRegistered(_collider)) return;
	// 登録解除
	ReleasePhysics();

	_isHit = false;
}

bool JustDodgeCollider::GetCollisionState() const
{
	return _collider->IsCollision();
}

void JustDodgeCollider::SetData(float rad, Vector3 offset)
{
	// 当たり判定データ設定
	_collider->CreateColliderDataSphere(
		rad,	// 半径
		true,	// isTrigger
		false	// isCollision
	);

	SetPosOffset(offset);
}

void JustDodgeCollider::SetPosOffset(Vector3 offset)
{
    _posOffset = offset;
}

bool JustDodgeCollider::IsHit() const
{
	return _isHit;
}
