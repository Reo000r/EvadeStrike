#pragma once
#include "EnemyProjectile.h"

class EnemyBossBressProjectile : public EnemyProjectile
{
public:
    EnemyBossBressProjectile(
        std::weak_ptr<Physics> physics,
        const Position3& startPos,
        const Vector3& direction);
    ~EnemyBossBressProjectile() override = default;

    /// <summary>
    /// èâä˙âª
    /// </summary>
    void Init() override;
};