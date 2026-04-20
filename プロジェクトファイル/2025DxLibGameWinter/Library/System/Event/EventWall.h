#pragma once
#include "EventCollider.h"

class EffekseerEffect;

/// <summary>
/// イベント壁
/// </summary>
class EventWall : public EventCollider {
public:
    EventWall(
        std::weak_ptr<Physics> physics,
        const ObjectData& data,
        std::weak_ptr<ObjectHandleHolder> holder);

    void Init() override;
    void Update() override;

    void OnCollide(const std::weak_ptr<Collider> collider) override;

    void OnCall(int id, const std::string& type) override;

    /// <summary>
    /// 削除対象にする
    /// </summary>
    void Expire() override;

    // 死亡時エフェクト
    std::weak_ptr<EffekseerEffect> _effect;

    bool _isPlayAnim;
    float _animTime;
};