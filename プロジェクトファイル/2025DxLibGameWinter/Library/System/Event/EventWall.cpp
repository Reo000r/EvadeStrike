#include "EventWall.h"

EventWall::EventWall(
    std::weak_ptr<Physics> physics,
    const ObjectData& data,
    std::weak_ptr<ObjectHandleHolder> holder) :
    EventCollider(physics, data, holder, false)
{
}

void EventWall::OnCollide(const std::weak_ptr<Collider> collider)
{
    // ˆ—‚È‚µ
}

void EventWall::OnCall(int id, const std::string& type)
{
    // ˆ—‚È‚µ
}
