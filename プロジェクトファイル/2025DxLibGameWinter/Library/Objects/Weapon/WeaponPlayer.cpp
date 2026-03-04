#include "WeaponPlayer.h"
#include "Objects/Character/Player/Player.h"
//#include "Objects/Character/Enemy/EnemyBase.h"

WeaponPlayer::WeaponPlayer(std::weak_ptr<Physics> physics) :
	Weapon(physics, PhysicsData::GameObjectTag::PlayerAttack)
{
    // 処理なし
}

void WeaponPlayer::OnCollide(const std::weak_ptr<Collider> collider)
{
    // 相手や所有者が不明な場合は何もしない
    if (collider.expired() || _owner.expired()) {
        return;
    }

    auto other = collider.lock();
    auto owner = _owner.lock();

    // 敵でないなら攻撃しない
    if (other->GetTag() != PhysicsData::GameObjectTag::Enemy) {
        return;
    }

    //auto enemy = std::static_pointer_cast<EnemyBase>(other);
    //auto attackableObj = std::static_pointer_cast<AttackableGameObject>(shared_from_this());
    //// 相手にダメージ処理を依頼する
    //enemy->TakeDamage(attackableObj);

    //// ダメージを与えた相手をリストに追加
    //_attackedColliders.emplace_back(collider);
}

