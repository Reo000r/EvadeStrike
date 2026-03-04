#include "AttackableGameObject.h"
#include "Library/Physics/Physics.h"
#include "Objects/Character/Player/Player.h"
#include "Objects/Character/Enemy/EnemyBase.h"
#include <cassert>

AttackableGameObject::AttackableGameObject(
	std::weak_ptr<Physics> physics,
	PhysicsData::Priority priority,
	PhysicsData::GameObjectTag tag,
	PhysicsData::ColliderKind colliderKind,
	bool isTrigger,
	bool isCollision,
	bool useGravity) :
	GameObject(physics, priority, tag, colliderKind, 
		isTrigger, isCollision, useGravity),
	_attackPower(),
	_breakPower(),
	_knockbackVector()
{
}

void AttackableGameObject::EntryAttackStats(float attackPower, float breakPower, Vector3 knockbackVector)
{
	_attackPower = attackPower;
	_breakPower = breakPower;
	_knockbackVector = knockbackVector;

	// colliderが既に登録されていれば
	if (_physics.lock()->IsRegistered(_collider)) {
		// 古いcolliderは登録解除する
		ReleasePhysics();
	}

	// 登録
	EntryPhysics(_physics);
	// 攻撃履歴を消去
	ClearAttackState();
}

void AttackableGameObject::ReleaseAttackStats()
{
	// colliderが有効でなければreturn
	if (!_collider) return;
	// colliderが登録されていなければreturn
	if (!_physics.lock()->IsRegistered(_collider)) return;
	// 登録解除
	ReleasePhysics();
}

void AttackableGameObject::OnCollide(const std::weak_ptr<Collider> collider)
{
	// 相手や所有者が不明な場合は何もしない
	if (collider.expired() || _owner.expired()) {
		return;
	}

	std::shared_ptr<Collider> other = collider.lock();
	std::shared_ptr<CharacterBase> owner = _owner.lock();

	// 自身がプレイヤーの攻撃であれば
	if (_collider->GetTag() == PhysicsData::GameObjectTag::PlayerAttack) {

		// 敵でないなら攻撃しない
		if (other->GetTag() != PhysicsData::GameObjectTag::Enemy) {
			return;
		}

		// 既に攻撃済みの相手なら何もしない
		auto it = std::find_if(
			_attackedColliders.begin(), _attackedColliders.end(),
			[&collider](const std::weak_ptr<Collider>& wptr) {
				return !wptr.expired() && !collider.expired() &&
					wptr.lock() == collider.lock();
			});

		// 見つかったなら攻撃済み
		if (it != _attackedColliders.end()) {
			return;
		}

		std::shared_ptr<EnemyBase> enemy = std::static_pointer_cast<EnemyBase>(other->GetParent());
		std::shared_ptr<AttackableGameObject> attackableObj = 
			std::static_pointer_cast<AttackableGameObject>(shared_from_this());
		// 相手にダメージ処理を依頼する
		enemy->TakeDamage(attackableObj);

		// ダメージを与えた相手をリストに追加
		_attackedColliders.emplace_back(collider);
	}
	// 自身が敵の攻撃であれば
	else if (_collider->GetTag() == PhysicsData::GameObjectTag::EnemyAttack) {

		// プレイヤーでないなら攻撃しない
		if (other->GetTag() != PhysicsData::GameObjectTag::Player) {
			return;
		}

		std::shared_ptr<Player> player = std::static_pointer_cast<Player>(other->GetParent());
		std::shared_ptr<AttackableGameObject> attackableObj = 
			std::static_pointer_cast<AttackableGameObject>(shared_from_this());
		// 相手にダメージ処理を依頼する
		player->TakeDamage(attackableObj);

		// ダメージを与えた相手をリストに追加
		_attackedColliders.emplace_back(collider);
		
		// 所有者がPlayerでない場合は
		// 一度ダメージを与えたら、連続ヒットを防ぐため当たり判定を無効にする
		SetCollisionState(false);
	}
	else {
		// 不明な攻撃元
		assert(false && "不明な攻撃元");
	}
}

void AttackableGameObject::Enable(float attackPower, float breakPower, Vector3 knockbackVector)
{
	// 攻撃情報を設定
	EntryAttackStats(attackPower, breakPower, knockbackVector);
	// Colliderと自身(GameObject)を紐づける
	_collider->SetParent(shared_from_this());
	// 攻撃判定を有効化
	SetCollisionState(true);
}

void AttackableGameObject::Disable()
{
	// 攻撃判定の無効化
	SetCollisionState(false);
	// 攻撃情報の削除
	ClearAttackState();
	ReleaseAttackStats();
}

void AttackableGameObject::SetCollisionState(bool isCollision)
{
	_collider->SetCollisionState(isCollision);
}

bool AttackableGameObject::GetCollisionState() const
{
	return _collider->IsCollision();
}

bool AttackableGameObject::IsAlreadyAttack() const
{
	return (_attackedColliders.size() > 0);
}

void AttackableGameObject::ClearAttackState()
{
	_attackedColliders.clear();
}

void AttackableGameObject::SetOwnerStatus(std::weak_ptr<CharacterBase> owner)
{
	_owner = owner;
}
