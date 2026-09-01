#include "ProjectileBase.h"
#include "Library/Physics/Physics.h"
#include "Library/Physics/Collider.h"
#include "Library/Physics/ProjectSettings.h"
#include "Library/System/Statistics.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Effect/EffekseerEffect.h"

ProjectileBase::ProjectileBase(
    std::weak_ptr<Physics> physics,
    PhysicsData::GameObjectTag attackTag,
    const Position3& startPos,
    const Vector3& direction,
    float speed,
    float lifeTime) :
    AttackCol(physics, attackTag),
    _direction(direction),
    _speed(speed),
    _canDelete(false),
    _lifeTime(lifeTime),
    _effectName(),
    _effectPlaySpeed(1.0f),
    _effectScale(1.0f),
    _hasTrailEffect(false)
{
    _direction.Normalized();
    SetPos(startPos);
}

ProjectileBase::~ProjectileBase()
{
}

void ProjectileBase::Init()
{
    if (_physics.expired()) return;

    // 自分自身のポインタとコライダーを紐づける
    _collider->SetParent(shared_from_this());

    // 派生クラスが無視したいタグを設定する
    SetupCollisionIgnoreTags();

    // 当たり判定を活性化
    Enable();

    // 追従エフェクトが設定されていれば生成する
    if (_hasTrailEffect) {
        _currentEffect = EffectManager::GetInstance().GenerateEffect(_effectName, GetPos());
        if (!_currentEffect.expired()) {
            float timeScale = GetCurrentTimeScale();
            _currentEffect.lock()->SetPlaySpeed(_effectPlaySpeed * timeScale);
            _currentEffect.lock()->SetScale(Vector3(_effectScale, _effectScale, _effectScale));
        }
    }
}

void ProjectileBase::Update()
{
    if (_canDelete) return;

    // timescaleを加味して生存時間を減らす
    float timeScale = GetCurrentTimeScale();
    _lifeTime -= 1.0f / Statistics::kFPS * timeScale;

    // 生存時間がないならば
    if (_lifeTime <= 0.0f) {
        _canDelete = true;
        // 当たり判定を解放する
        ReleaseFromPhysics();
        // エフェクトを解放する
        if (!_currentEffect.expired()) _currentEffect.lock()->DeleteEffect();
        return;
    }

    // 移動
    Position3 pos = GetPos();
    pos += _direction * _speed * timeScale;
    if (_collider) {
        _collider->SetPos(pos);
    }

    // エフェクト移動
    if (!_currentEffect.expired() && _currentEffect.lock()->IsPlaying()) {
        _currentEffect.lock()->SetPos(GetPos());
        _currentEffect.lock()->SetPlaySpeed(_effectPlaySpeed * timeScale);
    }
}

void ProjectileBase::OnCollide(const std::weak_ptr<Collider> collider)
{
    if (collider.expired()) return;
    if (_canDelete) return;

    PhysicsData::GameObjectTag tag = collider.lock()->GetTag();

    // 地形に当たった場合は弾の種類に関わらず消滅する
    if (tag == PhysicsData::GameObjectTag::SystemWall ||
        tag == PhysicsData::GameObjectTag::StepGround) {
        _canDelete = true;
        ReleaseFromPhysics();
        if (!_currentEffect.expired()) _currentEffect.lock()->SetPlaySpeed(_effectPlaySpeed);
        return;
    }

    // それ以外の相手への対応は派生クラスに委ねる
    if (OnHitOther(collider)) {
        _canDelete = true;
        ReleaseFromPhysics();
    }
}

void ProjectileBase::ReleaseFromPhysics()
{
    if (_physics.expired()) return;
    if (!_collider) return;

    _physics.lock()->Release(_collider);
    _collider = nullptr;
}

void ProjectileBase::SetTrailEffect(const std::string& effectName, float playSpeed, float scale)
{
    _effectName = effectName;
    _effectPlaySpeed = playSpeed;
    _effectScale = scale;
    _hasTrailEffect = true;
}