#pragma once
#include "Library/Objects/AttackCol.h"
#include "Library/Geometry/Vector3.h"
#include <string>
#include <memory>

class EffekseerEffect;

/// <summary>
/// 直進する弾の共通処理をまとめた基底クラス
/// 移動・寿命管理・地形との衝突判定・追従エフェクトの生成/更新のみを担当する
/// 誰に当たったらどうなるかは派生クラスに委ねる
/// </summary>
class ProjectileBase : public AttackCol
{
public:
    ProjectileBase(
        std::weak_ptr<Physics> physics,
        PhysicsData::GameObjectTag attackTag,
        const Position3& startPos,
        const Vector3& direction,
        float speed,
        float lifeTime);
    virtual ~ProjectileBase();

    void Init() override;
    void Update() override;
    void Draw() const override {}

    /// <summary>
    /// 衝突したときに呼ばれる
    /// </summary>
    /// <param name="colider"></param>
    void OnCollide(const std::weak_ptr<Collider> collider) override;

    bool CanDelete() const { return _canDelete; }
    void ReleaseFromPhysics();

protected:
    /// <summary>
    /// 当たり判定を無視したい相手のタグを設定
    /// </summary>
    virtual void SetupCollisionIgnoreTags() abstract;

    /// <summary>
    /// 地形以外の相手に当たった際の処理
    /// </summary>
    /// <param name="collider">衝突相手</param>
    /// <returns>弾を消滅させる場合はtrue</returns>
    virtual bool OnHitOther(const std::weak_ptr<Collider> collider) abstract;

    /// <summary>
    /// 追従エフェクトを設定する
    /// </summary>
    void SetTrailEffect(const std::string& effectName, float playSpeed, float scale);

protected:
    Vector3 _direction;
    float _speed;
    bool _canDelete;
    float _lifeTime;
    std::weak_ptr<EffekseerEffect> _currentEffect;

private:
    std::string _effectName;
    float _effectPlaySpeed;
    float _effectScale;
    bool _hasTrailEffect;
};