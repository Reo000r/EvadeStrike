#include "EventWall.h"
#include "Library/System/Effect/EffekseerEffect.h"
#include "Library/System/Effect/EffectManager.h"
#include "Library/System/Statistics.h"
#include "Library/Geometry/Easing.h"
#include "Library/Geometry/Matrix4x4.h"

namespace {
    constexpr float kStageScale = 1.75f;
    const Vector3 kEffectPosOffset =
        Vector3(0, 500 * kStageScale, 0);
    constexpr float kEffectScale = 5.0f * kStageScale;
    constexpr float kDefaultEffectSpeed = 1.0f;
    constexpr float kTotalAnimTime = 1.0f;
    constexpr float kAnimSpeed = 1.0f;
}

EventWall::EventWall(
    std::weak_ptr<Physics> physics,
    const ObjectData& data,
    std::weak_ptr<ObjectHandleHolder> holder) :
    EventCollider(physics, data, holder, false),
    _isPlayAnim(false),
    _animTime(0.0f)
{
}

void EventWall::Init()
{
    EventCollider::Init();

    _effect = EffectManager::GetInstance().GenerateEffect(
        "Wall_magic_circle.efkefc", GetPos() + kEffectPosOffset);
    _effect.lock()->SetScale(Vector3(kEffectScale, kEffectScale, kEffectScale));
    _effect.lock()->SetPlaySpeed(kDefaultEffectSpeed);
    _effect.lock()->SetRot(_data.transData.rot);

    // 正常なエフェクトであれば
    if (!_effect.expired()) {
        // 仮壁表示を切る
        _isDraw = false;
    }
}

void EventWall::Update()
{
    if (!_isPlayAnim) return;

    // アニメーションが終わっていれば
    if (_animTime <= 0.0f) {
        _effect.lock()->DeleteEffect();
        EventCollider::Expire();
    }
    // アニメーション時間の更新
    _animTime -= kAnimSpeed / Statistics::kFPS;

    // エフェクトの更新
    float prog = _animTime / kTotalAnimTime;
    prog = Easing::Get(prog, EasingType::EaseOutBack);
    float scale = kEffectScale * prog;
    _effect.lock()->SetScale(Vector3(scale, scale, scale));
}

void EventWall::OnCollide(const std::weak_ptr<Collider> collider)
{
    // 処理なし
}

void EventWall::OnCall(int id, const std::string& type)
{
    // 処理なし
}

void EventWall::Expire()
{
    if (_isPlayAnim) return;
    _animTime = kTotalAnimTime;
    _isPlayAnim = true;
}
