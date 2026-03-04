#pragma once
#include "EventCollider.h"

class EventManager;

/// <summary>
/// イベント判定
/// プレイヤーが触れると対応したイベントが起こる
/// </summary>
class EventTrigger : public EventCollider {
public:
    EventTrigger(std::weak_ptr<Physics> physics, const ObjectData& data, std::weak_ptr<ObjectHandleHolder> holder);

    virtual void OnCollide(const std::weak_ptr<Collider> collider) override;

    virtual void OnCall(int id, const std::string& type) override;
};
