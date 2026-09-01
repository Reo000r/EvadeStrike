#include "EnemyBossBressProjectile.h"
#include "Library/Physics/Physics.h"
#include "Library/Physics/Collider.h"
#include "Library/Physics/ProjectSettings.h"
#include "Library/System/Statistics.h"
#include <DxLib.h>
#include <cassert>

namespace {
	// 弾の半径
	constexpr float kRadius = 60.0f;
	// 速度
	constexpr float kSpeed = 18.0f;
	// 最大寿命（秒）
	constexpr float kMaxLifeTime = 3.0f;
	// 攻撃力
	constexpr float kAttackPower = 15.0f;
	// ブレイク力定数
	constexpr float kBreakPower = 0.0f;
}

EnemyBossBressProjectile::EnemyBossBressProjectile(
	std::weak_ptr<Physics> physics,
	const Position3& startPos,
	const Vector3& direction) :
	AttackableGameObject(
		physics,
		PhysicsData::Priority::Low,
		PhysicsData::GameObjectTag::EnemyAttack,
		PhysicsData::ColliderKind::Sphere,
		true, true, false),
	_direction(direction),
	_canDelete(false),
	_lifeTime(kMaxLifeTime)
{
	// 方向ベクトルを正規化
	_direction.Normalized();
	SetPos(startPos);
}

EnemyBossBressProjectile::~EnemyBossBressProjectile()
{
}

void EnemyBossBressProjectile::Init()
{
	if (_physics.expired()) return;

	// 球体コライダーデータを設定する
	_collider->CreateColliderDataSphere(
		kRadius,
		false,	// isTrigger
		true	// isCollision
	);

	// 自分自身のポインタとコライダーを紐づける
	_collider->SetParent(shared_from_this());

	// Physicsへ登録する
	_physics.lock()->Entry(_collider);
}

void EnemyBossBressProjectile::Update()
{
	// 削除済みならスキップ
	if (_canDelete) return;

	// 寿命を減算する
	_lifeTime -= 1.0f / Statistics::kFPS;

	// 寿命切れなら消滅
	if (_lifeTime <= 0.0f) {
		_canDelete = true;
		ReleaseFromPhysics();
		return;
	}

	Position3 pos = GetPos();
	// 飛翔方向へ移動量を加算する
	pos += _direction * kSpeed;

	// コライダーの位置を更新する
	if (_collider) {
		_collider->SetPos(pos);
	}
}

void EnemyBossBressProjectile::OnCollide(const std::weak_ptr<Collider> collider)
{
	if (collider.expired()) return;
	if (_canDelete) return;

	// 衝突相手のタグを取得する
	PhysicsData::GameObjectTag tag = collider.lock()->GetTag();

	// ステージ（床・壁）に当たった場合は消滅する
	if (tag == PhysicsData::GameObjectTag::SystemWall ||
		tag == PhysicsData::GameObjectTag::StepGround)
	{
		_canDelete = true;
		ReleaseFromPhysics();
		return;
	}

	// プレイヤーに当たった場合も消滅する
	// ダメージ処理はプレイヤー側の OnCollide / TakeDamage で行われる
	if (tag == PhysicsData::GameObjectTag::Player)
	{
		_canDelete = true;
		ReleaseFromPhysics();
		return;
	}
}

void EnemyBossBressProjectile::ReleaseFromPhysics()
{
	if (_physics.expired()) return;
	if (!_collider) return;

	_physics.lock()->Release(_collider);
	_collider = nullptr;
}
