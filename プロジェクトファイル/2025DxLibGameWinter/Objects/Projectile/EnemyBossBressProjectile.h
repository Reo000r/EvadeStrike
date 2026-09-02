#pragma once
#include "EnemyProjectile.h"

class EnemyManager;

class EnemyBossBressProjectile : public EnemyProjectile
{
public:
    EnemyBossBressProjectile(
        std::weak_ptr<Physics> physics,
        std::weak_ptr<EnemyManager> enemyManager,
        const Position3& startPos,
        const Vector3& direction);
    ~EnemyBossBressProjectile() override = default;

    /// <summary>
    /// èâä˙âª
    /// </summary>
    void Init() override;
};