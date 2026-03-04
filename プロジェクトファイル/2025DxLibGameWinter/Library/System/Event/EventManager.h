#pragma once
#include "Loader/ObjectDataLoader.h"
#include <vector>
#include <memory>

class EnemyManager;
class EventCollider;
class Physics;
class ObjectHandleHolder;

class EventManager {
public:
    EventManager();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="physics"></param>
    void Init(
        std::weak_ptr<Physics> physics,
        std::weak_ptr<EnemyManager> enemyManager,
        std::vector<ObjectData> enemySpawnDataList);

    /// <summary>
    /// 更新
    /// </summary>
    void Update();
    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    /// <summary>
    /// IDとタイプを指定してイベントを呼ぶ
    /// </summary>
    /// <param name="eventId"></param>
    /// <param name="eventType"></param>
    void CallEvent(int eventId, const std::string& eventType);

    /// <summary>
    /// メンバにイベント判定を登録
    /// </summary>
    /// <param name=""></param>
    void RegisterEventCollider(std::shared_ptr<EventCollider> eventCollider);

    std::vector<std::shared_ptr<EventCollider>> GetEventColliders() { return _eventColList; }

    /// <summary>
    /// クリア可能か
    /// </summary>
    /// <returns></returns>
    bool IsClearable() const { return _isClearable; }

private:
    std::weak_ptr<Physics> _physics;
    std::weak_ptr<EnemyManager> _enemyManager;
    std::vector<std::shared_ptr<EventCollider>> _eventColList;
    std::vector<ObjectData> _enemySpawnDataList;

    // 実行済みリスト
    std::vector<std::pair<std::string, int>> _executedEventList;
    // クリアに必要な総イベント数
    int _requiredEventCount;
    // クリア可能フラグ
    bool _isClearable;
};
