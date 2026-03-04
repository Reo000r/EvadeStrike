#pragma once
#include "Library/Objects/GameObject.h"
#include "Loader/ObjectDataLoader.h"

class ObjectHandleHolder;
class EventManager;

class EventCollider : public GameObject {
public:
    EventCollider(
        std::weak_ptr<Physics> physics, 
        const ObjectData& data, 
        std::weak_ptr<ObjectHandleHolder> holder, 
        bool isTrigger);
    virtual ~EventCollider();

    void Init() override;
    void Update() override;
    void Draw() const override;

    void SetEventManager(std::weak_ptr<EventManager> eventManager);

    /// <summary>
    /// イベントが呼ばれた際の反応
    /// </summary>
    /// <param name="id"></param>
    /// <param name="type"></param>
    virtual void OnCall(int id, const std::string & type) abstract;

    /// <summary>
    /// 削除対象になったか
    /// </summary>
    /// <returns></returns>
    bool IsExpired() const { return _isExpired; }

    /// <summary>
    /// 削除対象にする
    /// </summary>
    void Expire();

    // イベント情報を取得
    const ObjectData& GetObjectData() const { return _data; }

    std::shared_ptr<Collider> GetCollider() { return _collider; }

protected:
    std::weak_ptr<ObjectHandleHolder> _handleHolder;
    std::weak_ptr<EventManager> _eventManager;

    ObjectData _data;
    int _modelHandle;
    bool _isDraw;

    bool _isExpired;
};