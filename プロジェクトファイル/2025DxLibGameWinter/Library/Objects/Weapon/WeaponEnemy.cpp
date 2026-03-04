#include "WeaponEnemy.h"
#include "Objects/Character/Player/Player.h"

WeaponEnemy::WeaponEnemy(std::weak_ptr<Physics> physics) :
	Weapon(physics, PhysicsData::GameObjectTag::EnemyAttack)
{
    // 処理なし
}
