#include "EventTrigger.h"
#include "EventManager.h"

EventTrigger::EventTrigger(
    std::weak_ptr<Physics> physics, 
    const ObjectData& data, 
    std::weak_ptr<ObjectHandleHolder> holder) :
    EventCollider(physics, data, holder, true)
{
    _isDraw = false;
}

void EventTrigger::OnCollide(const std::weak_ptr<Collider> collider)
{
    // プレイヤーと当たったらイベント発火
    if (collider.lock()->GetTag() == PhysicsData::GameObjectTag::Player) {
        if (_eventManager.lock()) {
            // 自身が保持するCallEventIdとEventTypeを通知
            _eventManager.lock()->CallEvent(_data.eventData.callEventId, _data.eventData.eventType);
        }
        // 一度発火したら役割終了
        _isExpired = true;
    }
}

void EventTrigger::OnCall(int id, const std::string& type)
{
    // 処理なし
}
