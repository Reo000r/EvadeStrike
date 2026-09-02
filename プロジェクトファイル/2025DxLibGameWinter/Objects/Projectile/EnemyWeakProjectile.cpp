#include "EnemyWeakProjectile.h"

namespace {
    constexpr float kRadius = 60.0f;
    constexpr float kSpeed = 20.0f;
    constexpr float kMaxLifeTime = 4.0f;
    constexpr float kAttackPower = 5.0f;
    constexpr float kEffectPlaySpeed = 0.85f / kMaxLifeTime;
    constexpr float kEffectScale = kRadius * 0.02f;
}

EnemyWeakProjectile::EnemyWeakProjectile(
    std::weak_ptr<Physics> physics,
    std::weak_ptr<EnemyManager> enemyManager,
    const Position3& startPos,
    const Vector3& direction) :
    EnemyProjectile(physics, enemyManager, startPos, direction, kSpeed, kMaxLifeTime)
{
}

void EnemyWeakProjectile::Init()
{
    AttackColStats stats;
    stats.colRad = kRadius;
    stats.attackPower = kAttackPower;
    SetAttackData(stats);

    // エフェクト設定
    SetTrailEffect("AtkOmen_Salamander14.efkefc", kEffectPlaySpeed, kEffectScale);

    EnemyProjectile::Init();
}
