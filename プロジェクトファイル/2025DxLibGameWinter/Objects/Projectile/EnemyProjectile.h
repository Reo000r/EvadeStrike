#pragma once
#include "ProjectileBase.h"

class EnemyManager;

/// <summary>
/// 敵側が発射する、プレイヤーに向かって直進する弾の共通クラス
/// 敵同士の攻撃とはすり抜ける、プレイヤーに当たったらダメージを与える処理を共通化する
/// </summary>
class EnemyProjectile : public ProjectileBase
{
public:
    EnemyProjectile(
        std::weak_ptr<Physics> physics,
        std::weak_ptr<EnemyManager> enemyManager,
        const Position3& startPos,
        const Vector3& direction,
        float speed,
        float lifeTime);
    virtual ~EnemyProjectile() = default;

    void Update() override;

protected:
    void SetupCollisionIgnoreTags() override;
    bool OnHitOther(const std::weak_ptr<Collider> collider) override;

private:
    std::weak_ptr<EnemyManager> _enemyManager;
};