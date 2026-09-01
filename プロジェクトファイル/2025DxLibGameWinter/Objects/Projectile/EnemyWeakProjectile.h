#pragma once
#include "Objects/Projectile/EnemyProjectile.h"

/// <summary>
/// G‹›“G(‘Ş”ğ+ËŒ‚s“®)‚ª”­Ë‚·‚é’e
/// </summary>
class EnemyWeakProjectile : public EnemyProjectile
{
public:
    EnemyWeakProjectile(
        std::weak_ptr<Physics> physics,
        const Position3& startPos,
        const Vector3& direction);
    ~EnemyWeakProjectile() override = default;

    void Init() override;
};