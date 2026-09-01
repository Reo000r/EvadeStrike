#include "EnemyProjectile.h"
#include "Library/Physics/Collider.h"
#include "Objects/Character/Player/Player.h"

EnemyProjectile::EnemyProjectile(
    std::weak_ptr<Physics> physics,
    const Position3& startPos,
    const Vector3& direction,
    float speed,
    float lifeTime) :
    ProjectileBase(physics, PhysicsData::GameObjectTag::EnemyAttack,
        startPos, direction, speed, lifeTime)
{
}

void EnemyProjectile::SetupCollisionIgnoreTags()
{
    // 敵同士・他の攻撃判定とは当たらないようにする
    _collider->AddThroughTag(PhysicsData::GameObjectTag::EnemyMinion);
    _collider->AddThroughTag(PhysicsData::GameObjectTag::EnemyBoss);
    _collider->AddThroughTag(PhysicsData::GameObjectTag::EnemyAttack);
    _collider->AddThroughTag(PhysicsData::GameObjectTag::PlayerAttack);
}

bool EnemyProjectile::OnHitOther(const std::weak_ptr<Collider> collider)
{
    PhysicsData::GameObjectTag tag = collider.lock()->GetTag();
    // プレイヤー以外には反応しない
    if (tag != PhysicsData::GameObjectTag::Player) return false;

    // プレイヤーに当たった場合は
    // ダメージ依頼と消滅処理を行う
    std::shared_ptr<Player> player = std::static_pointer_cast<Player>(collider.lock()->GetParent());
    std::shared_ptr<AttackableGameObject> attackableObj =
        std::static_pointer_cast<AttackableGameObject>(shared_from_this());
    player->TakeDamage(attackableObj, false);

    // ダメージを与えた相手をリストに追加
    _attackedColliders.emplace_back(collider);

    // 一度ダメージを与えたら、連続ヒットを防ぐため当たり判定を無効にする
    SetCollisionState(false);

    // プレイヤーと当たらないようにする
    _collider->AddThroughTag(PhysicsData::GameObjectTag::Player);

    // ヒット後も弾自体は消滅させない
    return false;
}