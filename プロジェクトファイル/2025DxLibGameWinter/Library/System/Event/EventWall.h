#pragma once
#include "EventCollider.h"

/// <summary>
/// ƒCƒxƒ“ƒg•Ç
/// </summary>
class EventWall : public EventCollider {
public:
    EventWall(
        std::weak_ptr<Physics> physics,
        const ObjectData& data,
        std::weak_ptr<ObjectHandleHolder> holder);

    virtual void OnCollide(const std::weak_ptr<Collider> collider) override;

    virtual void OnCall(int id, const std::string& type) override;
};